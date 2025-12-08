#pragma once
#include <QWidget>
#include <QTimer>
#include <QPen>
#include "FTDView.h"
#include "FTDDataSource.h"

class FTDScopeView : public QWidget, public FTDView {
    Q_OBJECT
public:
    explicit FTDScopeView(QWidget* parent = nullptr);

    void setSource(FTDDataSource* src) override;
    void setTotalTimeWindowSec(double sec10Div) override; // total time across the screen (10 divisions)
    void setVerticalScale(float unitsPerDiv) override;

protected:
    void paintEvent(QPaintEvent* e) override;

private slots:
    void onRefresh();

private:
    void drawGrid(class QPainter& p);
    void drawWave(class QPainter& p, const QVector<float>& samples);

    FTDDataSource* m_source = nullptr;
    QTimer m_timer;
    double m_timeWindowSec = 0.1; // default 100ms across screen
    float m_unitsPerDiv = 1.0f;   // arbitrary vertical units
};

