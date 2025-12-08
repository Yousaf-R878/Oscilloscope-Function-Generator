#include "FTDViewTerminal.h"
#include "FTDDataSource.h"
#include "../Controller/FTDController.h"
#include "FTDFTDISource.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#ifdef Q_OS_UNIX
#include <QSocketNotifier>
#include <unistd.h>
#endif
#include <algorithm>
#include <cmath>

FTDViewTerminal::FTDViewTerminal(QObject* parent)
    : QObject(parent),
      m_out(stdout),
      m_in(stdin) {
    m_timer.setInterval(100); // 10 FPS
    QObject::connect(&m_timer, &QTimer::timeout, this, &FTDViewTerminal::onTick);

#ifdef Q_OS_UNIX
    m_stdinNotifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
    connect(m_stdinNotifier, &QSocketNotifier::activated, this, &FTDViewTerminal::onStdinActivity);
#endif

    m_useAnsi = !qEnvironmentVariableIsEmpty("TERM");
    printHelp();
}

void FTDViewTerminal::setSource(FTDDataSource* src) {
    m_source = src;
}

void FTDViewTerminal::setController(FTDController* controller) {
    m_controller = controller;
}

void FTDViewTerminal::start() {
    m_timer.start();
    if (m_source && !m_source->isActive()) m_source->start();
}

void FTDViewTerminal::stop() {
    m_timer.stop();
    if (m_source && m_source->isActive()) m_source->stop();
}

void FTDViewTerminal::onTick() {
    if (!m_source || !m_source->isActive() || m_source->sampleRate() <= 0) {
        m_out << "[TerminalView] No active source. Type 'start' or 'help'." << Qt::endl;
        return;
    }
    const int sr = m_source->sampleRate();
    const int needed = std::max(100, (int)std::ceil(sr * m_timeWindowSec));
    QVector<float> s;
    int got = m_source->copyRecentSamples(needed, s);
    if (got > 0) printFrame(s);
}

void FTDViewTerminal::printHelp() {
    m_out << "Oscilloscope Terminal View (commands):" << Qt::endl;
    m_out << "  start | stop | quit" << Qt::endl;
    m_out << "  timebase_ms <per_div_ms>   (e.g., 50)" << Qt::endl;
    m_out << "  scale <units_per_div>      (e.g., 1.0)" << Qt::endl;
    m_out << "  scope start                (start FTDI oscilloscope)" << Qt::endl;
    m_out << "  scope stop                 (stop FTDI oscilloscope)" << Qt::endl;
    m_out << "  scope sampleTime=<value><unit>  (e.g., scope sampleTime=1ms)" << Qt::endl;
    m_out << "  scope sampleFor=<value><unit>   (e.g., scope sampleFor=10s)" << Qt::endl;
    m_out << Qt::endl;
}

void FTDViewTerminal::printFrame(const QVector<float>& samples) {
    const int width = 80;
    const int height = 20;
    const float unitsPerScreen = m_unitsPerDiv * 8.0f; // 8 divs vertically

    // Prepare a buffer of spaces
    std::vector<char> grid(width * height, ' ');

    auto toYrow = [&](float v) {
        float normalized = (v / (unitsPerScreen/2.0f)); // -1..1 across half-screen
        float y = (height/2.0f) - normalized * (height/2.0f);
        int row = std::clamp((int)std::round(y), 0, height-1);
        return row;
    };

    // Center line
    int mid = height/2;
    for (int x=0;x<width;++x) grid[mid*width + x] = '-';

    // Decimate to columns by min/max envelope
    const int N = samples.size();
    const int step = std::max(1, N / width);
    for (int x=0; x<width; ++x) {
        int start = x * step;
        int end = std::min(N, start + step);
        if (start >= end) break;
        float vmin =  1e9f, vmax = -1e9f;
        for (int i = start; i < end; ++i) {
            float v = samples[i] * (1.0f / m_unitsPerDiv);
            vmin = std::min(vmin, v);
            vmax = std::max(vmax, v);
        }
        int r1 = toYrow(vmin);
        int r2 = toYrow(vmax);
        if (r1 > r2) std::swap(r1, r2);
        for (int r=r1; r<=r2; ++r) {
            grid[r*width + x] = '*';
        }
    }

    if (m_useAnsi) {
        m_out << "\x1b[2J\x1b[H"; // clear + home
    }
    m_out << "Time/div: " << QString::number((m_timeWindowSec/10.0) * 1000.0, 'f', 1)
          << " ms    Units/div: " << QString::number(m_unitsPerDiv, 'f', 2)
          << "    " << QDateTime::currentDateTime().toString("HH:mm:ss")
          << Qt::endl;

    for (int r=0; r<height; ++r) {
        m_out << QString::fromLatin1(grid.data() + r*width, width) << Qt::endl;
    }
    m_out.flush();
}

int FTDViewTerminal::parseDurationMicros(const QString& durationStr) {
    // Parse duration string like "1ms", "10s", "1000us"
    QRegularExpression re(R"((\d+(?:\.\d+)?)\s*(us|ms|s)?)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(durationStr.trimmed());
    
    if (!match.hasMatch()) {
        return -1;
    }
    
    double value = match.captured(1).toDouble();
    QString unit = match.captured(2).toLower();
    
    if (unit == "s") {
        return static_cast<int>(value * 1000000);
    } else if (unit == "ms") {
        return static_cast<int>(value * 1000);
    } else if (unit == "us" || unit.isEmpty()) {
        return static_cast<int>(value);
    }
    
    return -1;
}

void FTDViewTerminal::onStdinActivity() {
#ifdef Q_OS_UNIX
    QString line = m_in.readLine().trimmed();
    if (line.isEmpty()) return;
    const QString cmd = line.section(' ', 0, 0).toLower();
    const QString arg = line.section(' ', 1);
    const QString fullArg = line.section(' ', 1);  // Full argument including =value

    if (cmd == "quit" || cmd == "exit") {
        QCoreApplication::quit();
        return;
    } else if (cmd == "start") {
        if (m_source && !m_source->isActive()) m_source->start();
    } else if (cmd == "stop") {
        if (m_source && m_source->isActive()) m_source->stop();
    } else if (cmd == "timebase_ms") {
        bool ok=false; double msdiv = arg.toDouble(&ok);
        if (ok && msdiv > 0.0) setTotalTimeWindowSec((msdiv/1000.0)*10.0);
    } else if (cmd == "scale") {
        bool ok=false; double u = arg.toDouble(&ok);
        if (ok && u > 0.0) setVerticalScale((float)u);
    } else if (cmd == "scope") {
        // Handle scope commands
        if (!m_controller) {
            m_out << "Error: Controller not set. Cannot execute scope commands." << Qt::endl;
            return;
        }
        
        QString scopeCmd = fullArg.section(' ', 0, 0).toLower();
        QString scopeArg = fullArg.section(' ', 1);
        
        if (scopeCmd == "start") {
            m_controller->startOscilloscope();
            if (m_source && !m_source->isActive()) {
                m_source->start();
            }
            m_out << "Oscilloscope started." << Qt::endl;
        } else if (scopeCmd == "stop") {
            m_controller->stopOscilloscope();
            if (m_source && m_source->isActive()) {
                m_source->stop();
            }
            m_out << "Oscilloscope stopped." << Qt::endl;
        } else if (scopeCmd.startsWith("sampletime=")) {
            QString valueStr = scopeCmd.mid(11);  // Remove "sampletime="
            int intervalMicros = parseDurationMicros(valueStr);
            if (intervalMicros > 0) {
                // Configure sampling - need number of samples, use a default large number for continuous
                int numberOfSamples = 1000000;  // Large number for continuous sampling
                m_controller->configureSampling(numberOfSamples, intervalMicros);
                
                // Update the source if it's FTDI source
                FTDFTDISource* ftdiSource = qobject_cast<FTDFTDISource*>(m_source);
                if (ftdiSource) {
                    ftdiSource->setSampleInterval(intervalMicros);
                }
                m_out << "Sample time set to " << valueStr << Qt::endl;
            } else {
                m_out << "Invalid sample time format. Use: scope sampleTime=<value><unit> (e.g., 1ms, 10us, 0.5s)" << Qt::endl;
            }
        } else if (scopeCmd.startsWith("samplefor=")) {
            QString valueStr = scopeCmd.mid(10);  // Remove "samplefor="
            int durationMicros = parseDurationMicros(valueStr);
            if (durationMicros > 0) {
                // Get current sample interval
                int sampleIntervalMicros = 1000;  // Default 1ms
                FTDFTDISource* ftdiSource = qobject_cast<FTDFTDISource*>(m_source);
                if (ftdiSource) {
                    sampleIntervalMicros = 1000000 / ftdiSource->sampleRate();  // Convert Hz to microseconds
                }
                
                // Run scope with wait
                m_controller->runScopeWithWait(sampleIntervalMicros, durationMicros);
                m_out << "Sampling for " << valueStr << Qt::endl;
            } else {
                m_out << "Invalid duration format. Use: scope sampleFor=<value><unit> (e.g., 10s, 100ms)" << Qt::endl;
            }
        } else {
            m_out << "Unknown scope command: " << scopeCmd << ". Type 'help' for available commands." << Qt::endl;
        }
    } else if (cmd == "help") {
        printHelp();
    } else {
        m_out << "Unknown command. Type 'help'." << Qt::endl;
    }
#endif
}

