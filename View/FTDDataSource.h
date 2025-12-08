#pragma once
#include <QObject>
#include <QVector>
#include <QMutex>

// Abstract base class for oscilloscope data sources
class FTDDataSource : public QObject {
    Q_OBJECT
public:
    explicit FTDDataSource(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~FTDDataSource() = default;

    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isActive() const = 0;
    virtual int sampleRate() const = 0;

    // Copies up to 'count' most-recent samples into 'out' (mono, normalized -1..1).
    virtual int copyRecentSamples(int count, QVector<float>& out) = 0;
signals:
    void stateChanged(bool running);
};
