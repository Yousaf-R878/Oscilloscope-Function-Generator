#pragma once
#include "FTDDataSource.h"
#include <QTimer>
#include <QVector>
#include <QMutex>
#include <QThread>
#include <vector>
#include <atomic>

// Forward declaration
class FTDController;

class FTDFTDISource : public FTDDataSource {
    Q_OBJECT
public:
    explicit FTDFTDISource(FTDController* controller, QObject* parent = nullptr);
    ~FTDFTDISource() override;

    bool start() override;
    void stop() override;
    bool isActive() const override;
    int sampleRate() const override;
    int copyRecentSamples(int count, QVector<float>& out) override;

    // Configuration methods
    void setSampleInterval(int intervalMicros);
    void setCollectionDuration(int durationMicros);

private slots:
    void onUpdateTimer();

private:
    FTDController* m_controller;
    QTimer* m_updateTimer;
    QThread* m_collectionThread;
    QVector<float> m_sampleBuffer;
    QMutex m_bufferMutex;
    
    int m_sampleIntervalMicros;
    int m_collectionDurationMicros;
    std::atomic<bool> m_running;
    std::atomic<bool> m_shouldStop;
    int m_sampleRate;
    
    void updateBuffer();
    void collectionThreadFunction();
    float byteToFloat(unsigned char byte);
};

