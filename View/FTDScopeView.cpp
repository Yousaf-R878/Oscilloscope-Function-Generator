#include "FTDScopeView.h"
#include <QPainter>
#include <QPaintEvent>
#include <QFontMetrics>
#include <algorithm>
#include <cmath>

FTDScopeView::FTDScopeView(QWidget* parent)
    : QWidget(parent) {
    setAutoFillBackground(true);
    m_timer.setInterval(16); // ~60 FPS
    connect(&m_timer, &QTimer::timeout, this, &FTDScopeView::onRefresh);
    m_timer.start();
}

void FTDScopeView::setSource(FTDDataSource* src) {
    m_source = src;
}

void FTDScopeView::setTotalTimeWindowSec(double sec10Div) {
    m_timeWindowSec = sec10Div;
}

void FTDScopeView::setVerticalScale(float unitsPerDiv) {
    m_unitsPerDiv = unitsPerDiv;
}

void FTDScopeView::onRefresh() {
    update();
}

void FTDScopeView::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);
    QPainter p(this);
    p.fillRect(rect(), palette().base());
    drawGrid(p);

    if (!m_source || !m_source->isActive() || m_source->sampleRate() <= 0) {
        p.setPen(Qt::DashLine);
        p.drawText(rect().adjusted(10,10,-10,-10), Qt::AlignLeft | Qt::AlignTop, "No active source");
        return;
    }

    const int sr = m_source->sampleRate();
    const int needed = std::max(100, static_cast<int>(std::ceil(sr * m_timeWindowSec)));
    QVector<float> samples;
    const int got = m_source->copyRecentSamples(needed, samples);
    if (got > 0) {
        drawWave(p, samples);
    }
}

void FTDScopeView::drawGrid(QPainter& p) {
    const QRect r = rect().adjusted(8, 8, -8, -8);
    const int divsX = 10;
    const int divsY = 8;

    // Outer frame
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(r);

    // Grid lines
    for (int i = 1; i < divsX; ++i) {
        int x = r.left() + (i * r.width()) / divsX;
        p.drawLine(x, r.top(), x, r.bottom());
    }
    for (int j = 1; j < divsY; ++j) {
        int y = r.top() + (j * r.height()) / divsY;
        p.drawLine(r.left(), y, r.right(), y);
    }

    // Center line thicker
    p.setPen(QPen(Qt::black, 2));
    int yMid = r.center().y();
    p.drawLine(r.left(), yMid, r.right(), yMid);

    // Labels
    p.setPen(Qt::black);
    const QString tLabel = QString("Time/div: %1 ms").arg((m_timeWindowSec / 10.0) * 1000.0, 0, 'f', 2);
    const QString vLabel = QString("Units/div: %1").arg(m_unitsPerDiv, 0, 'f', 2);
    p.drawText(r.adjusted(4, 4, -4, -4), Qt::AlignLeft | Qt::AlignTop, tLabel + "    " + vLabel);
}

void FTDScopeView::drawWave(QPainter& p, const QVector<float>& samples) {
    const QRect r = rect().adjusted(8, 8, -8, -8);
    if (r.width() <= 1 || r.height() <= 1) return;

    // Map samples -> pixels using min/max envelope per pixel to reduce aliasing
    const int N = samples.size();
    const int W = r.width();
    const int step = std::max(1, N / W);

    QVector<QPointF> poly;
    poly.reserve(2 * (N / step));

    auto clamp = [](float v){ return std::max(-10.0f, std::min(10.0f, v)); };

    float unitsPerDiv = m_unitsPerDiv;
    float unitsPerScreen = unitsPerDiv * 8; // 8 vertical divisions
    for (int x = 0; x < W; ++x) {
        int start = x * step;
        int end = std::min(N, start + step);
        if (start >= end) break;
        float vmin =  1e9f;
        float vmax = -1e9f;
        for (int i = start; i < end; ++i) {
            float v = clamp(samples[i]) * (1.0f / unitsPerDiv); // scale by units/div (arbitrary)
            vmin = std::min(vmin, v);
            vmax = std::max(vmax, v);
        }
        // Convert to pixel Y (0 at top)
        auto toY = [&](float v) {
            float normalized = (v / (unitsPerScreen / 2.0f)); // -1..1 across half screen
            float y = r.center().y() - normalized * (r.height() / 2.0f);
            return y;
        };
        poly.push_back(QPointF(r.left() + x, toY(vmin)));
        poly.push_back(QPointF(r.left() + x, toY(vmax)));
    }

    p.setPen(QPen(Qt::darkGreen, 1));
    for (int i = 0; i + 1 < poly.size(); i += 2) {
        p.drawLine(poly[i], poly[i+1]);
    }
}

