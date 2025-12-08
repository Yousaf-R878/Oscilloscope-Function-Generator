#pragma once
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QTextStream>
#include <QElapsedTimer>
#include "FTDView.h"

// Forward declaration
class FTDController;

class FTDViewTerminal : public QObject, public FTDView {
    Q_OBJECT
public:
    explicit FTDViewTerminal(QObject* parent=nullptr);
    void setSource(class FTDDataSource* src) override;
    void setController(FTDController* controller);  // For scope commands
    void setTotalTimeWindowSec(double sec10Div) override { m_timeWindowSec = sec10Div; }
    void setVerticalScale(float unitsPerDiv) override { m_unitsPerDiv = unitsPerDiv; }

    void start();
    void stop();

private slots:
    void onTick();
    void onStdinActivity();

private:
    void printFrame(const QVector<float>& samples);
    void printHelp();
    int parseDurationMicros(const QString& durationStr);

    class FTDDataSource* m_source = nullptr;
    FTDController* m_controller = nullptr;
    QTimer m_timer;
    double m_timeWindowSec = 0.5; // 500ms across screen
    float m_unitsPerDiv = 1.0f;
    bool m_useAnsi = false;
    QTextStream m_out;
    QTextStream m_in;

#ifdef Q_OS_UNIX
    class QSocketNotifier* m_stdinNotifier = nullptr;
#endif
};

