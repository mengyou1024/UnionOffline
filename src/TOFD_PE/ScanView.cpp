#include "ScanView.hpp"
#include "../common/common.hpp"
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCharts>
#include <QtCore>
#include <Yo/File>
#include <span>

static Q_LOGGING_CATEGORY(TAG, "TOFD_PE.VIEW");

namespace TOFD_PE {
    TofdPeInteractor* ScanView::intr() const {
        return m_intr;
    }

    void ScanView::setIntr(TofdPeInteractor* newIntr) {
        if (m_intr == newIntr)
            return;
        m_intr = newIntr;
        emit intrChanged();
    }

    bool ScanView::isPe() const {
        return m_isPe;
    }

    void ScanView::setIsPe(bool newIsPe) {
        if (m_isPe == newIsPe)
            return;
        m_isPe = newIsPe;
        emit isPeChanged();
    }

    int ScanView::softGain() const {
        return m_softGain;
    }

    void ScanView::setSoftGain(int newSoftGain) {
        if (m_softGain == newSoftGain)
            return;
        m_softGain = newSoftGain;
        emit softGainChanged();
        update();
        updateChartView();
    }

    int ScanView::cursor() const {
        return m_cursor;
    }

    void ScanView::setCursor(int newCursor) {
        qDebug(TAG) << __FUNCTION__;
        qDebug(TAG) << "lines:" << ((intr() != nullptr) ? intr()->getLines() : -1) << "cursor:" << newCursor;
        if (!intr() || newCursor < 0 || newCursor >= intr()->getLines()) {
            return;
        }

        if (m_cursor == newCursor)
            return;
        m_cursor = newCursor;
        emit cursorChanged();
        updateChartView();
    }

    QPointF ScanView::abscissaRange() const {
        return m_abscissaRange;
    }

    void ScanView::setAbscissaRange(QPointF newAbscissaRange) {
        if (m_abscissaRange == newAbscissaRange)
            return;
        m_abscissaRange = newAbscissaRange;
        emit abscissaRangeChanged();
    }

    QPointF ScanView::ordinateRange() const {
        return m_ordinateRange;
    }

    void ScanView::setOrdinateRange(QPointF newOrdinateRange) {
        if (m_ordinateRange == newOrdinateRange)
            return;
        m_ordinateRange = newOrdinateRange;
        emit ordinateRangeChanged();
    }

    ScanView::ScanView() {}

    ScanView::~ScanView() {}

    void ScanView::paint(QPainter* painter) {
        MOROSE_TEST_TIME_QUICK("painter");
        painter->fillRect(QRect(0, 0, width(), height()), QBrush(Qt::black));
        drawAxis(painter);
        if (isPe()) {
            drawSecondDScan(painter);
        } else {
            drawDScan(painter);
        }
    }

    QRect ScanView::getDrawable() const {
        return QRect(AXIS_WIDTH, 0, width() - AXIS_WIDTH, height() - AXIS_WIDTH);
    }

    void ScanView::drawAxis(QPainter* painter) const {
        const auto drawAble = getDrawable();
        painter->fillRect(QRect(0, 0, AXIS_WIDTH, height()), QBrush(0xafeeee));
        painter->fillRect(QRect(AXIS_WIDTH, drawAble.bottom() + 1, drawAble.width(), AXIS_WIDTH), QBrush(0xafeeee));
        // 水平
        for (int i = 0; i <= 50; i++) {
            QLineF       li;
            qreal        x  = std::round(drawAble.x() + ((drawAble.width() - 1) / 50.0) * i);
            qreal        y  = drawAble.bottom() + 1;
            QColor       c  = QColor(0x00cc00);
            QString      s  = QString::number(m_abscissaRange.x() + m_abscissaRange.y() / 50.0 * i, 'f', 0);
            QFontMetrics fm = painter->fontMetrics();
            painter->setPen(c);
            if (i % 10 == 0) {
                li = {x, y, x, y + 15};
                if (i == 0) {
                    c = QColor(255, 0, 0);
                    painter->setPen(c);
                } else {
                    c = QColor(0, 0, 0);
                    painter->setPen(c);
                }
                QRect fmRect   = fm.boundingRect(s);
                QRect drawRect = {};
                if (i != 50) {
                    drawRect = QRect(QPoint(li.p2().x() + 5, li.p2().y() - 5), QSize(fmRect.width(), fmRect.height()));
                } else {
                    drawRect = QRect(QPoint(li.p2().x() - fmRect.width() - 5, li.p2().y() - 5), QSize(fmRect.width(), fmRect.height()));
                }
                painter->drawText(drawRect, Qt::AlignCenter, s);
            } else if (i % 5 == 0) {
                li = {x, y, x, y + 10};
            } else {
                li = {x, y, x, y + 5};
            }
            painter->drawLine(li);
        }
        // 垂直
        for (int i = 0; i <= 50; i++) {
            QLineF       li;
            qreal        x  = drawAble.left() - 1;
            qreal        y  = std::round((drawAble.height() - 1) / 50.0 * (50 - i));
            QColor       c  = QColor(0x00cc00);
            QString      s  = QString::number(m_ordinateRange.x() + m_ordinateRange.y() / 50.0 * (50 - i), 'f', 1);
            QFontMetrics fm = painter->fontMetrics();
            painter->setPen(c);
            if (i % 10 == 0) {
                li = {x - 15, y, x, y};
                if (i == 50) {
                    c = QColor(255, 0, 0);
                    painter->setPen(c);
                } else {
                    c = QColor(0, 0, 0);
                    painter->setPen(c);
                }
                painter->save();
                QRect fmRect   = fm.boundingRect(s);
                QRect drawRect = QRect(QPoint(0, 0), QSize(fmRect.width(), fmRect.height()));
                if (i != 0) {
                    painter->translate(li.p1().x() + 5, li.p1().y() + 5);
                } else {
                    painter->translate(li.p1().x() + 5, li.p1().y() + 5);
                }
                painter->rotate(90);
                painter->drawText(drawRect, Qt::AlignCenter, s);
                painter->restore();
            } else if (i % 5 == 0) {
                li = {x - 10, y, x, y};
            } else {
                li = {x - 5, y, x, y};
            }
            painter->drawLine(li);
        }
    }

    void ScanView::drawDScan(QPainter* painter) const {
        if (intr() == nullptr) {
            return;
        }
        const auto maxLines = intr()->getMaxLines();
        QImage     dScan(maxLines, 500, QImage::Format_RGB32);
        for (uint32_t x = 0; std::cmp_less(x, maxLines); x++) {
            for (uint32_t y = 0; std::cmp_less(y, 500); y++) {
                if (std::cmp_less(x, intr()->getLines())) {
                    auto bias    = static_cast<int>(intr()->getTofdData()[x * 500 + y]) - 128;
                    auto newBias = Union::CalculateGainOutput((double)bias, softGain());
                    if (newBias > 72) {
                        newBias = 72;
                    } else if (newBias < -128) {
                        newBias = -128;
                    }
                    auto     gray  = static_cast<uint8_t>(newBias + 128);
                    uint32_t color = gray | gray << 8 | gray << 16;
                    dScan.setPixel(QPoint(x, y), color);
                } else {
                    dScan.setPixel(QPoint(x, y), {});
                }
            }
        }
        painter->drawImage(getDrawable(), dScan);
    }

    void ScanView::drawSecondDScan(QPainter* painter) const {
        if (intr() == nullptr) {
            return;
        }
        const auto maxLines = intr()->getMaxLines();
        QImage     dScan(maxLines, 500, QImage::Format_RGB32);
        for (uint32_t x = 0; std::cmp_less(x, maxLines); x++) {
            for (uint32_t y = 0; std::cmp_less(y, 500); y++) {
                if (std::cmp_less(x, intr()->getSubLines())) {
                    auto    _t   = Union::CalculateGainOutput((double)intr()->getPeData()[x * 500 + y], softGain());
                    uint8_t gray = 0;
                    if (_t > 255.0) {
                        gray = 255;
                    } else if (_t < 0.0) {
                        gray = 0;
                    } else {
                        gray = static_cast<uint8_t>(_t);
                    }
                    uint32_t color = gray | gray << 8 | gray << 16;
                    dScan.setPixel(QPoint(x, y), color);
                } else {
                    dScan.setPixel(QPoint(x, y), {});
                }
            }
        }
        painter->drawImage(getDrawable(), dScan);
    }

    void ScanView::updateChartView() const {
        constexpr auto CHARTVIEW_SERIES_NAME = "AScan";
        auto           CreateSeries          = [this](QAbstractSeries::SeriesType type, QString name, QAbstractAxis* axisX, QAbstractAxis* axisY) -> QAbstractSeries* {
            QAbstractSeries* ret = nullptr;

            bool res = QMetaObject::invokeMethod(m_chartView, "createSeries", Q_RETURN_ARG(QAbstractSeries*, ret),
                                                                    Q_ARG(int, type),
                                                                    Q_ARG(QString, name),
                                                                    Q_ARG(QAbstractAxis*, axisX), Q_ARG(QAbstractAxis*, axisY));
            if (!res) {
                qCritical(TAG) << "invok method `createSeries` error with parameter:" << type << name << axisX << axisY;
            }
            if (ret) {
                ret->setUseOpenGL(true);
            }
            return ret;
        };

        auto CreateAScanAxis = [](qreal min = 0.0, qreal max = 1.0) {
            QValueAxis* axis = new QValueAxis;
            axis->setMin(min);
            axis->setMax(max);
            axis->setTickCount(10);
            axis->setGridLinePen(QPen(QColor(0xAEAEAE), 1, Qt::DashLine));
            return axis;
        };

        auto CreateAScanSeries = [&](QPointF pt, QSizeF sz) {
            QValueAxis* axisX = CreateAScanAxis(pt.x(), pt.x() + sz.width());
            QValueAxis* axisY = CreateAScanAxis(pt.y(), pt.y() + sz.height());
            auto        ret   = (QLineSeries*)CreateSeries(QAbstractSeries::SeriesTypeLine, CHARTVIEW_SERIES_NAME, axisX, axisY);
            auto        pen   = ret->pen();
            pen.setColor(QColor(0x336666));
            ret->setPen(pen);
            return ret;
        };

        auto FindSeries = [this](QString name) {
            QAbstractSeries* ret = nullptr;
            QMetaObject::invokeMethod(m_chartView, "series", Q_RETURN_ARG(QAbstractSeries*, ret), Q_ARG(QString, name));
            return ret;
        };
        auto series = (QLineSeries*)FindSeries(CHARTVIEW_SERIES_NAME);
        if (!series) {
            series   = CreateAScanSeries({0.0, 0.0}, {255.0, 500.0});
            auto pen = series->pen();
            pen.setWidthF(1);
            pen.setColor(Qt::yellow);
            series->setPen(pen);
        }

        auto GetData = [&]() -> std::optional<QList<QPointF>> {
            QList<QPointF> data = {};
            if (m_cursor < 0 || intr() == nullptr || m_cursor > intr()->getLines() - 1) {
                return std::nullopt;
            }
            const std::span data_ptr(intr()->getTofdData().begin() + (500 * m_cursor), 500);
            qDebug(TAG) << "data size:" << intr()->getTofdData().size();
            for (auto i = 0; std::cmp_less(i, 500); i++) {
                auto temp    = data_ptr[i];
                auto bias    = static_cast<int>(temp) - 128;
                auto newBias = Union::CalculateGainOutput((double)bias, softGain());
                if (newBias > 72) {
                    newBias = 72;
                } else if (newBias < -128) {
                    newBias = -128;
                }
                auto    result = static_cast<uint8_t>(newBias + 128);
                QPointF pt     = {static_cast<qreal>(result), 499.0 - i};
                data.push_back(pt);
            }
            return data;
        };

        auto GetSecondData = [&]() -> std::optional<QList<QPointF>> {
            QList<QPointF> data = {};
            if (m_cursor < 0 || intr() == nullptr || m_cursor > intr()->getSubLines() - 1) {
                return std::nullopt;
            }
            const std::span data_ptr(intr()->getPeData().begin() + (500 * m_cursor), 500);
            for (auto i = 0; std::cmp_less(i, 500); i++) {
                auto  raw    = data_ptr[i];
                auto  _t     = Union::CalculateGainOutput((double)raw, softGain());
                qreal result = 0;
                if (_t > 255.0) {
                    result = 255.0;
                } else if (_t < 0.0) {
                    result = 0.0;
                } else {
                    result = _t;
                }
                QPointF pt = {result, 499.0 - i};
                data.push_back(pt);
            }
            return data;
        };

        QList<QPointF> data = {};
        if (!isPe()) {
            series->replace(GetData().value_or(data));
        } else {
            series->replace(GetSecondData().value_or(data));
        }
    }

    QQuickItem* ScanView::chartView() const {
        return m_chartView;
    }

    void ScanView::setChartView(QQuickItem* newChartView) {
        if (m_chartView == newChartView)
            return;
        m_chartView = newChartView;
        emit chartViewChanged();
    }
} // namespace TOFD_PE
