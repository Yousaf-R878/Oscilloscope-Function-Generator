#include "FTDMainWindow.h"
#include "FTDFTDISource.h"
#include "../Controller/FTDController.h"
#include "../Model/FTDReader.h"
#include "../Model/FTDShift.h"
#include "../Model/FTDScale.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QLabel>
#include <QGroupBox>
#include <QTabWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QThread>
#include <sstream>
#include <iomanip>
#include <iostream>

static const struct { const char* label; double sec; } kTimebases[] = {
    {"5 ms/div", 0.050}, {"10 ms/div", 0.100}, {"20 ms/div", 0.200},
    {"50 ms/div", 0.500}, {"100 ms/div", 1.000}, {"200 ms/div", 2.000}
};

static const struct { const char* label; float units; } kScales[] = {
    {"0.2 units/div", 0.2f}, {"0.5 units/div", 0.5f}, {"1.0 units/div", 1.0f},
    {"2.0 units/div", 2.0f}, {"5.0 units/div", 5.0f}
};

FTDMainWindow::FTDMainWindow(FTDController* controller, QWidget* parent)
    : QMainWindow(parent), m_controller(controller) {
    buildUi();
}

FTDMainWindow::~FTDMainWindow() {
    if (m_ftdiSource) {
        m_ftdiSource->stop();
    }
}

void FTDMainWindow::buildUi() {
    m_view = new FTDScopeView(this);
    m_view->setMinimumHeight(400);  // Set minimum height for graph area

    // Create tab widget
    m_tabs = new QTabWidget(this);
    
    // Create tabs
    QWidget* oscilloscopeTab = new QWidget();
    QWidget* ioTab = new QWidget();
    QWidget* filtersTab = new QWidget();
    QWidget* fileTab = new QWidget();
    
    buildOscilloscopeTab(oscilloscopeTab);
    buildIOTab(ioTab);
    buildFiltersTab(filtersTab);
    buildFileTab(fileTab);
    
    m_tabs->addTab(oscilloscopeTab, "Oscilloscope");
    m_tabs->addTab(ioTab, "I/O Operations");
    m_tabs->addTab(filtersTab, "Filters");
    m_tabs->addTab(fileTab, "File Operations");

    auto* central = new QWidget(this);
    auto* mainVl = new QVBoxLayout(central);
    mainVl->setContentsMargins(0,0,0,0);
    mainVl->addWidget(m_view, /*stretch*/3);  // Increased stretch factor to make graph area bigger
    mainVl->addWidget(m_tabs, /*stretch*/0);
    setCentralWidget(central);

    // Create FTDI source if controller is provided
    if (m_controller) {
        m_ftdiSource = new FTDFTDISource(m_controller, this);
        m_view->setSource(m_ftdiSource);
        m_current = m_ftdiSource;
    }

    onTimebaseChanged(m_timebaseCombo->currentIndex());
    onScaleChanged(m_scaleCombo->currentIndex());

    m_status = new QLabel(this);
    statusBar()->addPermanentWidget(m_status);
    m_status->setText("Ready");
}

void FTDMainWindow::buildOscilloscopeTab(QWidget* parent) {
    auto* vl = new QVBoxLayout(parent);
    vl->setContentsMargins(8,8,8,8);
    vl->setSpacing(8);

    // Top row: Source and Start/Stop
    auto* hl1 = new QHBoxLayout();
    hl1->setSpacing(8);

    m_sourceCombo = new QComboBox(parent);
    m_sourceCombo->addItem("FTDI Oscilloscope");
    connect(m_sourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FTDMainWindow::onSourceChanged);

    m_startStop = new QPushButton("Start", parent);
    connect(m_startStop, &QPushButton::clicked, this, &FTDMainWindow::onStartStop);

    hl1->addWidget(new QLabel("Source:", parent));
    hl1->addWidget(m_sourceCombo);
    hl1->addWidget(m_startStop);
    hl1->addStretch(1);

    // Second row: Timebase and Scale
    auto* hl2 = new QHBoxLayout();
    hl2->setSpacing(8);

    m_timebaseCombo = new QComboBox(parent);
    for (auto t : kTimebases) m_timebaseCombo->addItem(t.label);
    m_timebaseCombo->setCurrentIndex(3); // 50 ms/div default
    connect(m_timebaseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FTDMainWindow::onTimebaseChanged);

    m_scaleCombo = new QComboBox(parent);
    for (auto s : kScales) m_scaleCombo->addItem(s.label);
    m_scaleCombo->setCurrentIndex(2);
    connect(m_scaleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FTDMainWindow::onScaleChanged);

    hl2->addWidget(new QLabel("Timebase:", parent));
    hl2->addWidget(m_timebaseCombo);
    hl2->addSpacing(12);
    hl2->addWidget(new QLabel("Vertical:", parent));
    hl2->addWidget(m_scaleCombo);
    hl2->addStretch(1);

    // Third row: Sample Time and Duration (for FTDI scope)
    auto* hl3 = new QHBoxLayout();
    hl3->setSpacing(8);

    m_sampleTime = new QDoubleSpinBox(parent);
    m_sampleTime->setRange(0.001, 1000.0);
    m_sampleTime->setDecimals(3);
    m_sampleTime->setValue(1.0);
    m_sampleTime->setSingleStep(0.1);
    connect(m_sampleTime, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FTDMainWindow::onSampleTimeChanged);

    m_sampleTimeUnit = new QComboBox(parent);
    m_sampleTimeUnit->addItem("us");
    m_sampleTimeUnit->addItem("ms");
    m_sampleTimeUnit->addItem("s");
    m_sampleTimeUnit->setCurrentIndex(1);  // Default to ms
    connect(m_sampleTimeUnit, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FTDMainWindow::onSampleTimeChanged);

    m_sampleDuration = new QDoubleSpinBox(parent);
    m_sampleDuration->setRange(0.001, 3600.0);
    m_sampleDuration->setDecimals(3);
    m_sampleDuration->setValue(10.0);
    m_sampleDuration->setSingleStep(0.1);
    connect(m_sampleDuration, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FTDMainWindow::onSampleDurationChanged);

    m_sampleDurationUnit = new QComboBox(parent);
    m_sampleDurationUnit->addItem("us");
    m_sampleDurationUnit->addItem("ms");
    m_sampleDurationUnit->addItem("s");
    m_sampleDurationUnit->setCurrentIndex(2);  // Default to s
    connect(m_sampleDurationUnit, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FTDMainWindow::onSampleDurationChanged);

    hl3->addWidget(new QLabel("Sample Time:", parent));
    hl3->addWidget(m_sampleTime);
    hl3->addWidget(m_sampleTimeUnit);
    hl3->addSpacing(12);
    hl3->addWidget(new QLabel("Sample Duration:", parent));
    hl3->addWidget(m_sampleDuration);
    hl3->addWidget(m_sampleDurationUnit);
    hl3->addStretch(1);

    // Fourth row: Wave Test
    auto* hl4 = new QHBoxLayout();
    hl4->setSpacing(8);

    m_waveTestSamples = new QSpinBox(parent);
    m_waveTestSamples->setRange(10, 10000);
    m_waveTestSamples->setValue(100);
    m_waveTestSamples->setSuffix(" samples");

    m_waveTestAmplitude = new QSpinBox(parent);
    m_waveTestAmplitude->setRange(0, 127);
    m_waveTestAmplitude->setValue(127);
    m_waveTestAmplitude->setSuffix(" amplitude");

    m_waveTestButton = new QPushButton("Generate Wave Test", parent);
    connect(m_waveTestButton, &QPushButton::clicked, this, &FTDMainWindow::onWaveTest);

    hl4->addWidget(new QLabel("Wave Test:", parent));
    hl4->addWidget(m_waveTestSamples);
    hl4->addWidget(m_waveTestAmplitude);
    hl4->addWidget(m_waveTestButton);
    hl4->addStretch(1);

    vl->addLayout(hl1);
    vl->addLayout(hl2);
    vl->addLayout(hl3);
    vl->addLayout(hl4);
    vl->addStretch(1);
}

void FTDMainWindow::buildIOTab(QWidget* parent) {
    auto* vl = new QVBoxLayout(parent);
    vl->setContentsMargins(8,8,8,8);
    vl->setSpacing(8);

    // Read section
    auto* readGroup = new QGroupBox("Read Bytes", parent);
    auto* readLayout = new QVBoxLayout(readGroup);
    
    auto* readHl = new QHBoxLayout();
    readHl->addWidget(new QLabel("Count:", readGroup));
    m_readCount = new QSpinBox(readGroup);
    m_readCount->setRange(1, 10000);
    m_readCount->setValue(1);
    readHl->addWidget(m_readCount);
    m_readButton = new QPushButton("Read Bytes", readGroup);
    connect(m_readButton, &QPushButton::clicked, this, &FTDMainWindow::onReadBytes);
    readHl->addWidget(m_readButton);
    readHl->addStretch(1);
    
    readLayout->addLayout(readHl);
    
    m_readDisplay = new QTextEdit(readGroup);
    m_readDisplay->setReadOnly(true);
    m_readDisplay->setMaximumHeight(150);
    readLayout->addWidget(m_readDisplay);
    
    vl->addWidget(readGroup);

    // Write section
    auto* writeGroup = new QGroupBox("Write Bytes", parent);
    auto* writeLayout = new QVBoxLayout(writeGroup);
    
    auto* writeHl1 = new QHBoxLayout();
    writeHl1->addWidget(new QLabel("Byte Value (0-255 or 0xXX):", writeGroup));
    m_writeByteValue = new QLineEdit(writeGroup);
    m_writeByteValue->setText("0");
    writeHl1->addWidget(m_writeByteValue);
    writeLayout->addLayout(writeHl1);
    
    auto* writeHl2 = new QHBoxLayout();
    writeHl2->addWidget(new QLabel("Count:", writeGroup));
    m_writeCount = new QSpinBox(writeGroup);
    m_writeCount->setRange(1, 1000);
    m_writeCount->setValue(1);
    writeHl2->addWidget(m_writeCount);
    m_writeButton = new QPushButton("Write Bytes", writeGroup);
    connect(m_writeButton, &QPushButton::clicked, this, &FTDMainWindow::onWriteBytes);
    writeHl2->addWidget(m_writeButton);
    writeHl2->addStretch(1);
    writeLayout->addLayout(writeHl2);
    
    vl->addWidget(writeGroup);
    vl->addStretch(1);
}

void FTDMainWindow::buildFiltersTab(QWidget* parent) {
    auto* vl = new QVBoxLayout(parent);
    vl->setContentsMargins(8,8,8,8);
    vl->setSpacing(8);

    // Shift filter section
    auto* shiftGroup = new QGroupBox("Shift Filter", parent);
    auto* shiftLayout = new QHBoxLayout(shiftGroup);
    shiftLayout->addWidget(new QLabel("Offset (-128 to 127):", shiftGroup));
    m_shiftOffset = new QSpinBox(shiftGroup);
    m_shiftOffset->setRange(-128, 127);
    m_shiftOffset->setValue(0);
    shiftLayout->addWidget(m_shiftOffset);
    m_addShiftButton = new QPushButton("Add Shift Filter", shiftGroup);
    connect(m_addShiftButton, &QPushButton::clicked, this, &FTDMainWindow::onAddShiftFilter);
    shiftLayout->addWidget(m_addShiftButton);
    shiftLayout->addStretch(1);
    vl->addWidget(shiftGroup);

    // Scale filter section
    auto* scaleGroup = new QGroupBox("Scale Filter", parent);
    auto* scaleLayout = new QHBoxLayout(scaleGroup);
    scaleLayout->addWidget(new QLabel("Factor (1-255):", scaleGroup));
    m_scaleFactor = new QSpinBox(scaleGroup);
    m_scaleFactor->setRange(1, 255);
    m_scaleFactor->setValue(1);
    scaleLayout->addWidget(m_scaleFactor);
    m_addScaleButton = new QPushButton("Add Scale Filter", scaleGroup);
    connect(m_addScaleButton, &QPushButton::clicked, this, &FTDMainWindow::onAddScaleFilter);
    scaleLayout->addWidget(m_addScaleButton);
    scaleLayout->addStretch(1);
    vl->addWidget(scaleGroup);

    // Active filters list
    auto* filtersListGroup = new QGroupBox("Active Filters", parent);
    auto* filtersListLayout = new QVBoxLayout(filtersListGroup);
    m_filtersList = new QListWidget(filtersListGroup);
    m_filtersList->setMaximumHeight(150);
    filtersListLayout->addWidget(m_filtersList);
    vl->addWidget(filtersListGroup);

    // Pipe controls
    auto* pipeGroup = new QGroupBox("Pipe Execution", parent);
    auto* pipeLayout = new QHBoxLayout(pipeGroup);
    m_executePipeButton = new QPushButton("Execute Pipe", pipeGroup);
    connect(m_executePipeButton, &QPushButton::clicked, this, &FTDMainWindow::onExecutePipe);
    m_clearFiltersButton = new QPushButton("Clear Filters", pipeGroup);
    connect(m_clearFiltersButton, &QPushButton::clicked, this, &FTDMainWindow::onClearFilters);
    pipeLayout->addWidget(m_executePipeButton);
    pipeLayout->addWidget(m_clearFiltersButton);
    pipeLayout->addStretch(1);
    vl->addWidget(pipeGroup);
    vl->addStretch(1);
}

void FTDMainWindow::buildFileTab(QWidget* parent) {
    auto* vl = new QVBoxLayout(parent);
    vl->setContentsMargins(8,8,8,8);
    vl->setSpacing(8);

    // Read file section
    auto* readFileGroup = new QGroupBox("Read File", parent);
    auto* readFileLayout = new QVBoxLayout(readFileGroup);
    
    auto* readFileHl = new QHBoxLayout();
    readFileHl->addWidget(new QLabel("Filename:", readFileGroup));
    m_readFileName = new QLineEdit(readFileGroup);
    m_readFileName->setText("Input.txt");
    readFileHl->addWidget(m_readFileName);
    m_browseReadButton = new QPushButton("Browse...", readFileGroup);
    connect(m_browseReadButton, &QPushButton::clicked, this, &FTDMainWindow::onBrowseReadFile);
    readFileHl->addWidget(m_browseReadButton);
    readFileLayout->addLayout(readFileHl);
    
    m_readFileButton = new QPushButton("Read File", readFileGroup);
    connect(m_readFileButton, &QPushButton::clicked, this, &FTDMainWindow::onReadFile);
    readFileLayout->addWidget(m_readFileButton);
    
    vl->addWidget(readFileGroup);

    // Write file section
    auto* writeFileGroup = new QGroupBox("Write File", parent);
    auto* writeFileLayout = new QVBoxLayout(writeFileGroup);
    
    auto* writeFileHl = new QHBoxLayout();
    writeFileHl->addWidget(new QLabel("Filename:", writeFileGroup));
    m_writeFileName = new QLineEdit(writeFileGroup);
    m_writeFileName->setText("output.txt");
    writeFileHl->addWidget(m_writeFileName);
    m_browseWriteButton = new QPushButton("Browse...", writeFileGroup);
    connect(m_browseWriteButton, &QPushButton::clicked, this, &FTDMainWindow::onBrowseWriteFile);
    writeFileHl->addWidget(m_browseWriteButton);
    writeFileLayout->addLayout(writeFileHl);
    
    m_writeFileButton = new QPushButton("Write File", writeFileGroup);
    connect(m_writeFileButton, &QPushButton::clicked, this, &FTDMainWindow::onWriteFile);
    writeFileLayout->addWidget(m_writeFileButton);
    
    vl->addWidget(writeFileGroup);
    vl->addStretch(1);
}

void FTDMainWindow::onSourceChanged(int idx) {
    if (m_running) onStartStop(); // stop current
    
    if (idx == 0 && m_ftdiSource) {
        m_view->setSource(m_ftdiSource);
        m_current = m_ftdiSource;
        m_status->setText("Source: FTDI Oscilloscope");
    }
}

void FTDMainWindow::onStartStop() {
    if (!m_current) {
        m_status->setText("No source available");
        return;
    }

    if (!m_running) {
        if (m_current->start()) {
            m_running = true;
            m_startStop->setText("Stop");
            m_status->setText("Running");
        } else {
            m_status->setText("Failed to start source");
        }
    } else {
        if (m_current) m_current->stop();
        m_running = false;
        m_startStop->setText("Start");
        m_status->setText("Stopped");
    }
}

void FTDMainWindow::onTimebaseChanged(int idx) {
    if (idx < 0) return;
    double totalSec = kTimebases[idx].sec * 10.0; // 10 divisions across
    m_view->setTotalTimeWindowSec(totalSec);
}

void FTDMainWindow::onScaleChanged(int idx) {
    if (idx < 0) return;
    m_view->setVerticalScale(kScales[idx].units);
}

void FTDMainWindow::onSampleTimeChanged() {
    if (!m_controller || !m_ftdiSource) return;
    
    double value = m_sampleTime->value();
    QString unit = m_sampleTimeUnit->currentText();
    
    int intervalMicros = 0;
    if (unit == "s") {
        intervalMicros = static_cast<int>(value * 1000000);
    } else if (unit == "ms") {
        intervalMicros = static_cast<int>(value * 1000);
    } else if (unit == "us") {
        intervalMicros = static_cast<int>(value);
    }
    
    if (intervalMicros > 0) {
        // Configure sampling with a large number of samples for continuous operation
        int numberOfSamples = 1000000;  // Large number for continuous
        m_controller->configureSampling(numberOfSamples, intervalMicros);
        m_ftdiSource->setSampleInterval(intervalMicros);
    }
}

void FTDMainWindow::onSampleDurationChanged() {
    if (!m_controller || !m_ftdiSource) return;
    
    double value = m_sampleDuration->value();
    QString unit = m_sampleDurationUnit->currentText();
    
    int durationMicros = 0;
    if (unit == "s") {
        durationMicros = static_cast<int>(value * 1000000);
    } else if (unit == "ms") {
        durationMicros = static_cast<int>(value * 1000);
    } else if (unit == "us") {
        durationMicros = static_cast<int>(value);
    }
    
    if (durationMicros > 0) {
        m_ftdiSource->setCollectionDuration(durationMicros);
        
        // If running, restart with new duration
        if (m_running) {
            int sampleIntervalMicros = 1000000 / m_ftdiSource->sampleRate();
            if (sampleIntervalMicros > 0) {
                m_controller->runScopeWithWait(sampleIntervalMicros, durationMicros);
            }
        }
    }
}

void FTDMainWindow::onWaveTest() {
    if (!m_controller) {
        m_status->setText("Controller not available");
        return;
    }
    
    int samples = m_waveTestSamples->value();
    int amplitude = m_waveTestAmplitude->value();
    
    // Get the sample interval from the UI to match oscilloscope timing
    int intervalMicros = 1000;  // Default 1ms
    if (m_sampleTime) {
        double value = m_sampleTime->value();
        QString unit = m_sampleTimeUnit->currentText();
        if (unit == "s") {
            intervalMicros = static_cast<int>(value * 1000000);
        } else if (unit == "ms") {
            intervalMicros = static_cast<int>(value * 1000);
        } else if (unit == "us") {
            intervalMicros = static_cast<int>(value);
        }
    }
    
    try {
        m_waveTestButton->setEnabled(false);
        m_status->setText(QString("Generating wave test: %1 samples...").arg(samples));
        
        // Generate wave test in a separate thread to avoid blocking UI
        QThread* waveThread = QThread::create([this, samples, amplitude, intervalMicros]() {
            try {
                m_controller->generateWaveTest(samples, amplitude, intervalMicros);
                QMetaObject::invokeMethod(this, [this, samples]() {
                    m_waveTestButton->setEnabled(true);
                    m_status->setText(QString("Wave test completed: %1 samples generated").arg(samples));
                }, Qt::QueuedConnection);
            } catch (const std::exception& e) {
                QString errorMsg = e.what();
                QMetaObject::invokeMethod(this, [this, errorMsg]() {
                    m_waveTestButton->setEnabled(true);
                    m_status->setText(QString("Wave test failed: %1").arg(errorMsg));
                }, Qt::QueuedConnection);
            }
        });
        waveThread->start();
        
    } catch (const std::exception& e) {
        m_waveTestButton->setEnabled(true);
        m_status->setText(QString("Wave test failed: %1").arg(e.what()));
        QMessageBox::warning(this, "Wave Test Error", QString("Error: %1").arg(e.what()));
    }
}

void FTDMainWindow::onReadBytes() {
    if (!m_controller) {
        m_status->setText("Controller not available");
        return;
    }
    
    int count = m_readCount->value();
    try {
        // Call the controller method (same as command line)
        m_controller->readBytes(count);
        
        // Get the actual read data from the reader
        FTDReader* reader = m_controller->getReader();
        if (reader) {
            const std::vector<unsigned char>& buffer = reader->getBuffer();
            DWORD bytesRead = reader->getBytesRead();
            
            // Display the actual values
            QString displayText = QString("Read %1 byte(s): ").arg(bytesRead);
            for (DWORD i = 0; i < bytesRead && i < buffer.size(); ++i) {
                displayText += QString("0x%1 ").arg(static_cast<int>(buffer[i]), 2, 16, QChar('0'));
            }
            displayText += "\n";
            
            m_readDisplay->append(displayText);
            m_status->setText(QString("Read %1 bytes").arg(bytesRead));
        } else {
            m_readDisplay->append(QString("Read %1 bytes (no reader available)").arg(count));
            m_status->setText(QString("Read %1 bytes").arg(count));
        }
    } catch (const std::exception& e) {
        m_readDisplay->append(QString("Error: %1").arg(e.what()));
        m_status->setText(QString("Read failed: %1").arg(e.what()));
    }
}

void FTDMainWindow::onWriteBytes() {
    if (!m_controller) {
        m_status->setText("Controller not available");
        return;
    }
    
    QString byteStr = m_writeByteValue->text().trimmed();
    int byteValue = 0;
    
    // Parse byte value (support hex 0xXX or decimal)
    if (byteStr.startsWith("0x", Qt::CaseInsensitive)) {
        bool ok;
        byteValue = byteStr.mid(2).toInt(&ok, 16);
        if (!ok) {
            m_status->setText("Invalid hex byte value");
            return;
        }
    } else {
        bool ok;
        byteValue = byteStr.toInt(&ok);
        if (!ok || byteValue < 0 || byteValue > 255) {
            m_status->setText("Invalid byte value (0-255)");
            return;
        }
    }
    
    int count = m_writeCount->value();
    try {
        m_controller->writeBytes(static_cast<unsigned char>(byteValue), count);
        m_status->setText(QString("Wrote %1 byte(s) (0x%2)").arg(count).arg(byteValue, 2, 16, QChar('0')));
    } catch (const std::exception& e) {
        m_status->setText(QString("Write failed: %1").arg(e.what()));
    }
}

void FTDMainWindow::onAddShiftFilter() {
    if (!m_controller) {
        m_status->setText("Controller not available");
        return;
    }
    
    int offset = m_shiftOffset->value();
    try {
        m_controller->addProcess(std::make_shared<Shift>(offset));
        m_filtersList->addItem(QString("Shift(%1)").arg(offset));
        m_status->setText(QString("Added Shift filter (offset: %1)").arg(offset));
    } catch (const std::exception& e) {
        m_status->setText(QString("Failed to add filter: %1").arg(e.what()));
    }
}

void FTDMainWindow::onAddScaleFilter() {
    if (!m_controller) {
        m_status->setText("Controller not available");
        return;
    }
    
    int factor = m_scaleFactor->value();
    try {
        m_controller->addProcess(std::make_shared<Scale>(factor));
        m_filtersList->addItem(QString("Scale(%1)").arg(factor));
        m_status->setText(QString("Added Scale filter (factor: %1)").arg(factor));
    } catch (const std::exception& e) {
        m_status->setText(QString("Failed to add filter: %1").arg(e.what()));
    }
}

void FTDMainWindow::onExecutePipe() {
    if (!m_controller) {
        m_status->setText("Controller not available");
        return;
    }
    
    try {
        m_controller->executePipe();
        m_status->setText("Pipe executed successfully");
        QMessageBox::information(this, "Pipe Execution", "Pipe executed successfully. Check console for details.");
    } catch (const std::exception& e) {
        m_status->setText(QString("Pipe execution failed: %1").arg(e.what()));
        QMessageBox::warning(this, "Pipe Error", QString("Error: %1").arg(e.what()));
    }
}

void FTDMainWindow::onClearFilters() {
    if (!m_controller) {
        return;
    }
    
    m_controller->clearPipe();
    m_filtersList->clear();
    m_status->setText("Filters cleared");
}

void FTDMainWindow::onBrowseReadFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Select File to Read", ".", "All Files (*.*)");
    if (!filename.isEmpty()) {
        m_readFileName->setText(filename);
    }
}

void FTDMainWindow::onBrowseWriteFile() {
    QString filename = QFileDialog::getSaveFileName(this, "Select File to Write", ".", "All Files (*.*)");
    if (!filename.isEmpty()) {
        m_writeFileName->setText(filename);
    }
}

void FTDMainWindow::onReadFile() {
    if (!m_controller) {
        m_status->setText("Controller not available");
        return;
    }
    
    QString filename = m_readFileName->text().trimmed();
    if (filename.isEmpty()) {
        m_status->setText("Please specify a filename");
        return;
    }
    
    try {
        m_controller->readFromFile(filename.toStdString());
        m_status->setText(QString("Read from file: %1").arg(filename));
        QMessageBox::information(this, "Read File", QString("Successfully read from file: %1\nCheck console for details.").arg(filename));
    } catch (const std::exception& e) {
        m_status->setText(QString("Read file failed: %1").arg(e.what()));
        QMessageBox::warning(this, "Read File Error", QString("Error: %1").arg(e.what()));
    }
}

void FTDMainWindow::onWriteFile() {
    if (!m_controller) {
        m_status->setText("Controller not available");
        return;
    }
    
    QString filename = m_writeFileName->text().trimmed();
    if (filename.isEmpty()) {
        m_status->setText("Please specify a filename");
        return;
    }
    
    try {
        m_controller->writeToFile(filename.toStdString());
        m_status->setText(QString("Wrote to file: %1").arg(filename));
        QMessageBox::information(this, "Write File", QString("Successfully wrote to file: %1\nCheck console for details.").arg(filename));
    } catch (const std::exception& e) {
        m_status->setText(QString("Write file failed: %1").arg(e.what()));
        QMessageBox::warning(this, "Write File Error", QString("Error: %1").arg(e.what()));
    }
}

