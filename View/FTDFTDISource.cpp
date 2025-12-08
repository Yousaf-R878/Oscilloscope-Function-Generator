#include "FTDFTDISource.h"
#include "../Controller/FTDController.h"
#include "../Model/FTDReader.h"
#include "../Model/FTDOscilloscopeThreaded.h"
#include "../ftd2xx.h"
#include <QDebug>
#include <QThread>
#include <algorithm>

FTDFTDISource::FTDFTDISource(FTDController* controller, QObject* parent)
    : FTDDataSource(parent),
      m_controller(controller),
      m_updateTimer(new QTimer(this)),
      m_collectionThread(nullptr),
      m_sampleIntervalMicros(1000),  // Default 1ms
      m_collectionDurationMicros(10000000),  // Default 10s
      m_running(false),
      m_shouldStop(false),
      m_sampleRate(1000) {  // Default 1kHz
    
    if (!m_controller) {
        qWarning() << "FTDFTDISource: controller is null";
    }
    
    connect(m_updateTimer, &QTimer::timeout, this, &FTDFTDISource::onUpdateTimer);
    m_updateTimer->setInterval(16);  // ~60 FPS update rate
}

FTDFTDISource::~FTDFTDISource() {
    stop();
}

bool FTDFTDISource::start() {
    if (!m_controller) {
        qWarning() << "FTDFTDISource: Cannot start, controller is null";
        return false;
    }
    
    if (m_running.load()) {
        return true;
    }
    
    m_shouldStop = false;
    m_running = true;
    
    // Start the update timer to read from threaded oscilloscope
    m_updateTimer->start();
    
    // Start threaded collection in a separate thread
    m_collectionThread = QThread::create([this]() {
        this->collectionThreadFunction();
    });
    m_collectionThread->start();
    
    emit stateChanged(true);
    return true;
}

void FTDFTDISource::stop() {
    if (!m_running.load()) {
        return;
    }
    
    m_shouldStop = true;
    m_running = false;
    
    m_updateTimer->stop();
    
    if (m_controller) {
        m_controller->stopOscilloscope();
    }
    
    // Wait for collection thread to finish
    if (m_collectionThread) {
        m_collectionThread->wait(1000);  // Wait up to 1 second
        if (m_collectionThread->isRunning()) {
            m_collectionThread->terminate();
            m_collectionThread->wait();
        }
        delete m_collectionThread;
        m_collectionThread = nullptr;
    }
    
    {
        QMutexLocker lock(&m_bufferMutex);
        m_sampleBuffer.clear();
    }
    
    emit stateChanged(false);
}

bool FTDFTDISource::isActive() const {
    return m_running.load();
}

int FTDFTDISource::sampleRate() const {
    if (m_sampleIntervalMicros > 0) {
        return 1000000 / m_sampleIntervalMicros;  // Convert microseconds to Hz
    }
    return m_sampleRate;
}

int FTDFTDISource::copyRecentSamples(int count, QVector<float>& out) {
    QMutexLocker lock(&m_bufferMutex);
    
    out.clear();
    if (m_sampleBuffer.isEmpty()) {
        return 0;
    }
    
    // Get the most recent 'count' samples
    int startIdx = std::max(0, static_cast<int>(m_sampleBuffer.size() - count));
    int actualCount = static_cast<int>(m_sampleBuffer.size() - startIdx);
    
    out.reserve(actualCount);
    for (int i = startIdx; i < m_sampleBuffer.size(); ++i) {
        out.append(m_sampleBuffer[i]);
    }
    
    return actualCount;
}

void FTDFTDISource::setSampleInterval(int intervalMicros) {
    m_sampleIntervalMicros = intervalMicros;
    if (m_sampleIntervalMicros > 0) {
        m_sampleRate = 1000000 / m_sampleIntervalMicros;
    }
}

void FTDFTDISource::setCollectionDuration(int durationMicros) {
    m_collectionDurationMicros = durationMicros;
}

void FTDFTDISource::onUpdateTimer() {
    if (!m_running.load() || !m_controller) {
        return;
    }
    
    updateBuffer();
}

void FTDFTDISource::collectionThreadFunction() {
    // Start the threaded oscilloscope and let it run continuously
    FTDOscilloscopeThreaded* scopeThreaded = m_controller->getOscilloscopeThreaded();
    if (!scopeThreaded) {
        qWarning() << "FTDFTDISource: Threaded oscilloscope not available";
        return;
    }
    
    // Start the oscilloscope
    scopeThreaded->start();
    
    // Continuously collect data
    while (!m_shouldStop.load() && m_running.load()) {
        if (m_sampleIntervalMicros > 0 && m_collectionDurationMicros > 0) {
            static std::mutex out_mx;
            // Run collection for the specified duration
            // This will block until collection is done, then we restart for continuous operation
            scopeThreaded->collectDataWithWait(m_sampleIntervalMicros, m_collectionDurationMicros, out_mx);
            scopeThreaded->join();
            
            // If still running, restart collection for continuous operation
            if (m_running.load() && !m_shouldStop.load()) {
                scopeThreaded->start();
            } else {
                break;
            }
        } else {
            // If not configured, wait a bit and check again
            QThread::msleep(100);
        }
    }
    
    // Stop the oscilloscope when done
    scopeThreaded->stop();
    scopeThreaded->join();
}

void FTDFTDISource::updateBuffer() {
    if (!m_controller) {
        return;
    }
    
    // Read from the threaded oscilloscope's collected data
    FTDOscilloscopeThreaded* scopeThreaded = m_controller->getOscilloscopeThreaded();
    if (!scopeThreaded) {
        return;
    }
    
    // Get the collected data from the threaded oscilloscope
    const std::vector<unsigned char>& collectedData = scopeThreaded->getCollectedData();
    if (collectedData.empty()) {
        return;
    }
    
    // Convert bytes to float samples and add to buffer
    QMutexLocker lock(&m_bufferMutex);
    
    // Keep buffer size reasonable (e.g., last 10000 samples)
    const int maxBufferSize = 10000;
    
    // Get the most recent samples (avoid duplicates by tracking what we've already added)
    static size_t lastProcessedSize = 0;
    size_t currentSize = collectedData.size();
    
    // If size decreased, collection was reset, so reset our tracking
    if (currentSize < lastProcessedSize) {
        lastProcessedSize = 0;
    }
    
    if (currentSize > lastProcessedSize) {
        // Only process new data
        for (size_t i = lastProcessedSize; i < currentSize; ++i) {
            float sample = byteToFloat(collectedData[i]);
            m_sampleBuffer.append(sample);
            
            // Trim buffer if it gets too large
            if (m_sampleBuffer.size() > maxBufferSize) {
                m_sampleBuffer.removeFirst();
            }
        }
        lastProcessedSize = currentSize;
    }
}

float FTDFTDISource::byteToFloat(unsigned char byte) {
    // Convert unsigned char (0-255) to float (-1.0 to 1.0)
    return (static_cast<float>(byte) - 127.5f) / 127.5f;
}

