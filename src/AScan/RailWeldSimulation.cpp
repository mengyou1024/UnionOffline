#include "RailWeldSimulation.hpp"
#include <AScanType.hpp>
#include <MDATType.hpp>
#include <QBrush>
#include <QLoggingCategory>
#include <QPainter>
#include <QtMath>
#include <Specialization.hpp>

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "Union.RailWeldSimulation");

[[maybe_unused]] constexpr auto LIST_SIMULATION_POSITON = {
    "轨头踏面",
    "轨距角",
    "轨头侧面",
    "轨头下颚",
    "轨腰",
    "轨底上表面",
    "轨底角",
    "轨底下表面",
};

[[maybe_unused]] constexpr std::array LIST_RAIL_TYPES           = {"43kg/m", "50kg/m", "60kg/m", "75kg/m"};
[[maybe_unused]] constexpr std::array LIST_RAIL_PROBE_DIRECTION = {"左向", "右向", "平行向"};
[[maybe_unused]] constexpr std::array LIST_CHANNEL              = {"K2.5轨头", "K2.5轨底", "K1", "K0", "双K1", "双K0.8"};
[[maybe_unused]] constexpr auto       NUM_RAIL_PT               = 127;
[[maybe_unused]] constexpr auto       SCALE_FACTOR              = 1.7f;
[[maybe_unused]] constexpr auto       OFFSET_X                  = 22.5f; // 调整到中心位置
[[maybe_unused]] constexpr auto       OFFSET_Y                  = 50.5f; // 调整到中心位置
[[maybe_unused]] constexpr auto       MAX_SEGMENT_LENGTH        = 10.00F;
[[maybe_unused]] constexpr auto       SIMULATION_LINE_COLOR     = Qt::red;

// clang-format off
constexpr float RAIL_PT[NUM_RAIL_PT][2]  = {
    {148.00F, 0.00F  }, {148.31F, 0.02F  }, {148.62F, 0.10F  }, {148.91F, 0.22F  }, {149.18F, 0.38F  },
    {149.41F, 0.59F  }, {149.62F, 0.82F  }, {149.78F, 1.09F  }, {149.90F, 1.38F  }, {149.98F, 1.69F  },
    {150.00F, 2.00F  }, {150.00F, 8.02F  }, {149.95F, 8.67F  }, {149.79F, 9.30F  }, {149.54F, 9.90F  },
    {149.19F, 10.44F }, {148.75F, 10.92F }, {148.25F, 11.33F }, {147.69F, 11.65F }, {147.08F, 11.88F },
    {146.44F, 12.00F }, {126.93F, 14.17F }, {122.81F, 14.85F }, {118.77F, 15.95F }, {98.80F , 22.57F },
    {95.77F , 23.85F }, {93.00F , 25.62F }, {90.55F , 27.82F }, {88.50F,  30.39F }, {86.89F , 33.26F },
    {85.78F , 36.35F }, {85.18F , 39.59F }, {83.44F , 66.48F }, {83.51F,  93.42F }, {85.40F , 120.30F},
    {85.96F , 123.54F}, {86.93F , 126.69F}, {88.31F , 129.68F}, {88.97F,  130.68F}, {89.76F , 131.58F},
    {90.68F , 132.35F}, {91.70F , 132.97F}, {92.81F , 133.44F}, {110.07F, 139.19F}, {110.36F, 139.32F},
    {110.64F, 139.49F}, {110.88F, 139.71F}, {111.08F, 139.96F}, {111.25F, 140.24F}, {111.36F, 140.54F},
    {111.42F, 140.86F}, {111.43F, 141.19F}, {110.40F, 161.80F}, {110.13F, 163.89F}, {109.52F, 165.89F},
    {108.60F, 167.78F}, {107.39F, 169.49F}, {105.91F, 170.99F}, {104.22F, 172.23F}, {102.35F, 173.18F},
    {100.35F, 173.82F}, {92.70F , 175.20F}, {84.95F , 175.83F}, {65.05F,  175.83F}, {57.30F , 175.20F},
    {49.65F , 173.82F}, {47.65F , 173.18F}, {45.78F , 172.23F}, {44.09F,  170.99F}, {42.61F , 169.49F},
    {41.40F , 167.78F}, {40.48F , 165.89F}, {39.87F , 163.89F}, {39.60F,  161.80F}, {38.57F , 141.19F},
    {38.58F , 140.86F}, {38.64F , 140.54F}, {38.75F , 140.24F}, {38.92F,  139.96F}, {39.12F , 139.71F},
    {39.36F , 139.49F}, {39.64F , 139.32F}, {39.93F , 139.19F}, {57.19F,  133.44F}, {58.30F , 132.97F},
    {59.32F , 132.35F}, {60.24F , 131.58F}, {61.03F , 130.68F}, {61.69F,  129.68F}, {63.07F , 126.69F},
    {64.04F , 123.54F}, {64.60F , 120.30F}, {66.49F , 93.42F }, {66.56F,  66.48F }, {64.82F , 39.59F },
    {64.22F , 36.35F }, {63.11F , 33.26F }, {61.50F , 30.39F }, {59.45F,  27.82F }, {57.00F , 25.62F },
    {54.23F , 23.85F }, {51.20F , 22.57F }, {31.23F , 15.95F }, {27.19F,  14.85F }, {23.07F , 14.17F },
    {3.56F  , 12.00F }, {2.92F  , 11.88F }, {2.31F  , 11.65F }, {1.75F,   11.33F }, {1.25F  , 10.92F },
    {0.81F  , 10.44F }, {0.46F  , 9.90F  }, {0.21F  , 9.30F  }, {0.05F,   8.67F  }, {0.00F  , 8.02F  },
    {0.00F  , 2.00F  }, {0.02F  , 1.69F  }, {0.10F  , 1.38F  }, {0.22F,   1.09F  }, {0.38F  , 0.82F  },
    {0.59F  , 0.59F  }, {0.82F  , 0.38F  }, {1.09F  , 0.22F  }, {1.38F,   0.10F  }, {1.69F  , 0.02F  },
    {2.00F  , 0.00F  }, {148.00F, 0.00F  },
};
// clang-format on

namespace Union::AScan::RailWeld {

    int RailWeldSimulation::simulationProbePosition() const {
        return m_simulationProbePosition;
    }

    void RailWeldSimulation::setSimulationProbePosition(int newSimulationProbePosition) {
        if (m_simulationProbePosition == newSimulationProbePosition)
            return;
        m_simulationProbePosition = newSimulationProbePosition;
        emit simulationProbePositionChanged();
    }

    int RailWeldSimulation::simulationProbePoint() const {
        return m_simulationProbePoint;
    }

    void RailWeldSimulation::setSimulationProbePoint(int newSimulationProbePoint) {
        if (m_simulationProbePoint == newSimulationProbePoint)
            return;
        m_simulationProbePoint = newSimulationProbePoint;
        emit simulationProbePointChanged();
    }

    int RailWeldSimulation::simulationProbeDirection() const {
        return m_simulationProbeDirection;
    }

    void RailWeldSimulation::setSimulationProbeDirection(int newSimulationProbeDirection) {
        if (m_simulationProbeDirection == newSimulationProbeDirection)
            return;
        m_simulationProbeDirection = newSimulationProbeDirection;
        emit simulationProbeDirectionChanged();
    }

    int RailWeldSimulation::simulationRailType() const {
        return m_simulationRailType;
    }

    void RailWeldSimulation::setSimulationRailType(int newSimulationRailType) {
        if (m_simulationRailType == newSimulationRailType)
            return;
        m_simulationRailType = newSimulationRailType;
        emit simulationRailTypeChanged();
    }

    qreal RailWeldSimulation::probeAngle() const {
        return m_probeAngle;
    }

    void RailWeldSimulation::setProbeAngle(qreal newProbeAngle) {
        if (qFuzzyCompare(m_probeAngle, newProbeAngle))
            return;
        m_probeAngle = newProbeAngle;
        emit probeAngleChanged();
    }

    qreal RailWeldSimulation::actualSoundDistance() const {
        return m_actualSoundDistance;
    }

    void RailWeldSimulation::setActualSoundDistance(qreal newActualSoundDistance) {
        if (qFuzzyCompare(m_actualSoundDistance, newActualSoundDistance))
            return;
        m_actualSoundDistance = newActualSoundDistance;
        emit actualSoundDistanceChanged();
    }

    QVariant RailWeldSimulation::ascanIntf() const {
        return m_ascanIntf;
    }

    void RailWeldSimulation::setAscanIntf(const QVariant& newAscanIntf) {
        if (m_ascanIntf == newAscanIntf)
            return;
        m_ascanIntf = newAscanIntf;
        emit ascanIntfChanged();
    }

    void RailWeldSimulation::cursorChanged(int idx, double soft_gain) {
        using namespace Union::__390N_T8::MDATType;
        auto ascan_intf = ascanIntf().value<std::shared_ptr<Union::AScan::AScanIntf>>();
        auto intf       = std::dynamic_pointer_cast<UnType>(ascan_intf);
        if (intf == nullptr || !intf->isSpecial001Enabled(idx)) {
            qCWarning(TAG) << "intf is nullptr or feature CMP001 is not enable";
            return;
        }
        const auto& data   = std::get<UnType::ID_ASCAN_DATA>(intf->m_data.second.at(idx)).m_data;
        const auto& cmp001 = std::dynamic_pointer_cast<CMP001>(std::get<UnType::ID_CHANNEL_PARAM>(intf->m_data.second.at(idx)));
        if (!cmp001) {
            qCWarning(TAG) << "convert to cmp001 error";
            return;
        }
        m_thicknessData.resize(std::ssize(data));
        std::ranges::transform(data.begin(), data.end(), m_thicknessData.begin(), [soft_gain](auto it) -> double {
            auto ret = Union::CalculateGainOutput(it, soft_gain);
            if (ret > 255) {
                ret = 255;
            }
            return ret;
        });
        setSimulationProbePoint(cmp001->simulation_point);
        setSimulationProbePosition(cmp001->simulation_position);
        setSimulationProbeDirection(cmp001->simulation_direction);
        setSimulationRailType(cmp001->railtypes % 4);
        setProbeAngle(cmp001->angle);
        setActualSoundDistance(cmp001->simulation_length);
        m_channelName = LIST_CHANNEL[cmp001->channel % std::ssize(LIST_CHANNEL)];
        update();
    }

    RailWeldSimulation::RailWeldSimulation() {
        QVector<QPointF> points;
        for (int i = 0; i < NUM_RAIL_PT; i++) {
            points.append(QPointF(RAIL_PT[i][0] * SCALE_FACTOR + OFFSET_X, RAIL_PT[i][1] * SCALE_FACTOR + OFFSET_Y));
        }
        m_pointsRefine = generateRefinePoints(points, MAX_SEGMENT_LENGTH);
    }

    void RailWeldSimulation::paint(QPainter* painter) {
        painter->fillRect(QRect(0, 0, width(), height()), QBrush(Qt::transparent));

        auto font = painter->font();
        font.setPointSize(10);
        painter->setFont(font);
        QFontMetricsF font_metrics = painter->fontMetrics();
        QRectF        fm_rect      = font_metrics.boundingRect(m_channelName);
        QRectF        draw_rect    = QRectF(QPointF((width() - fm_rect.width()) / 2.0, 0), QSizeF(fm_rect.width() + 10, fm_rect.height() + 10));

        painter->drawText(draw_rect, Qt::AlignCenter, m_channelName);

        auto scale_factor = std::min(width() / 300.0, height() / 400.0);
        if (scale_factor > 1) {
            scale_factor = 1;
        }

        // 设置坐标变换，使原点在左下角，y轴向上
        QTransform transform;
        transform.translate(10, height());
        transform.scale(scale_factor, -scale_factor);
        painter->setTransform(transform);

        // 画钢轨图像
        drawRailImage(painter);
        // 画探头位置
        drawProbePosition(painter, simulationProbePoint());

        auto points = calculateLinePoints();
        drawVariableThicknessLine(painter, points);
    }

    QVector<QPointF> RailWeldSimulation::generateRefinePoints(const QVector<QPointF>& points, float maxSegmentLength) {
        QVector<QPointF> newPoints;

        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF p1 = points[i];
            QPointF p2 = points[i + 1];

            newPoints.append(p1);

            float dx            = p2.x() - p1.x();
            float dy            = p2.y() - p1.y();
            float segmentLength = std::sqrt(dx * dx + dy * dy);

            if (segmentLength > maxSegmentLength) {
                int   numSegments = std::ceil(segmentLength / maxSegmentLength);
                float stepX       = dx / numSegments;
                float stepY       = dy / numSegments;

                for (int j = 1; j < numSegments; ++j) {
                    newPoints.append(QPointF(p1.x() + j * stepX, p1.y() + j * stepY));
                }
            }
        }

        newPoints.append(points.last());
        return newPoints;
    }

    void RailWeldSimulation::drawRailImage(QPainter* painter) const noexcept {
        QPen pen(Qt::black, 2);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(pen);
        for (int i = 0; i < m_pointsRefine.size() - 1; i++) {
            painter->drawLine(m_pointsRefine[i], m_pointsRefine[i + 1]);
        }
    }

    void RailWeldSimulation::drawProbePosition(QPainter* painter, int point) const noexcept {
        if (point < 0 || point >= m_pointsRefine.size() - 1) {
            return;
        }

        // 获取滑动条对应的线段的两个点
        const auto& currentLeftPoint  = m_pointsRefine[point];
        const auto& currentRightPoint = m_pointsRefine[point + 1];

        // 计算线段的中心点
        QPointF centerPoint = (currentLeftPoint + currentRightPoint) / 2.0;

        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(QBrush(QColor(SIMULATION_LINE_COLOR)));
        painter->setPen(Qt::NoPen); // 不需要边框

        // 绘制圆形
        painter->drawEllipse(QPointF(centerPoint.x(), centerPoint.y()), 7.5, 7.5);
    }

    bool RailWeldSimulation::findIntersection(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4, QPointF& intersectionPoint) const {
        float x1 = p1.x(), y1 = p1.y();
        float x2 = p2.x(), y2 = p2.y();
        float x3 = p3.x(), y3 = p3.y();
        float x4 = p4.x(), y4 = p4.y();

        // 计算线段的方向向量
        float d1x = x2 - x1;
        float d1y = y2 - y1;
        float d2x = x4 - x3;
        float d2y = y4 - y3;

        // 计算叉积（也可以视作行列式的值）
        float denominator = d1x * d2y - d1y * d2x;

        // 如果叉积接近零，线段平行或共线
        if (std::fabs(denominator) < 1e-7) {
            // 判断是否共线
            if (std::fabs((x3 - x1) * d1y - (y3 - y1) * d1x) < 1e-7) {
                // 共线的情况
                // 检查是否存在重叠部分
                if (std::max(std::min(x1, x2), std::min(x3, x4)) <= std::min(std::max(x1, x2), std::max(x3, x4)) &&
                    std::max(std::min(y1, y2), std::min(y3, y4)) <= std::min(std::max(y1, y2), std::max(y3, y4))) {
                    // 共线且重叠，取一个合理的交点
                    intersectionPoint.setX(std::max(std::min(x1, x2), std::min(x3, x4)));
                    intersectionPoint.setY(std::max(std::min(y1, y2), std::min(y3, y4)));
                    return true;
                }
            }
            return false; // 平行且不重叠
        }

        // 计算 t 和 u 参数，用于确定交点是否在两条线段之内
        float t = ((x3 - x1) * d2y - (y3 - y1) * d2x) / denominator;
        float u = ((x3 - x1) * d1y - (y3 - y1) * d1x) / denominator;

        // 忽略t无限接近0的情况
        if (std::fabs(t) < 1e-6) {
            return false;
        }

        // 检查 t 和 u 是否在 [0, 1] 的范围内，包括端点
        if (t >= 1e-4 && t <= 1.0f + 1e-4 && u >= -1e-4 && u <= 1.0f + 1e-4) {
            intersectionPoint.setX(x1 + t * d1x);
            intersectionPoint.setY(y1 + t * d1y);
            return true;
        }

        return false;
    }

    void RailWeldSimulation::drawVariableThicknessLine(QPainter* painter, const QVector<QPointF>& points) {
        int             segmentCount = m_thicknessData.size();
        double          totalLength  = 0.0;
        QVector<double> segmentLengths;

        // 计算每个线段的长度
        for (int i = 0; i < points.size() - 1; ++i) {
            double length = std::sqrt(std::pow(points[i + 1].x() - points[i].x(), 2) +
                                      std::pow(points[i + 1].y() - points[i].y(), 2));
            segmentLengths.append(length);
            totalLength += length;
        }

        double  segmentLength     = totalLength / segmentCount;
        QPointF currentPoint      = points[0];
        double  accumulatedLength = 0.0;
        int     thicknessIndex    = 0;

        for (int i = 0; i < points.size() - 1 && thicknessIndex < segmentCount; ++i) {
            double  currentSegmentLength = segmentLengths[i];
            QPointF nextPoint            = points[i + 1];

            while (accumulatedLength + currentSegmentLength >= segmentLength) {
                double  remainingLength   = segmentLength - accumulatedLength;
                QPointF interpolatedPoint = currentPoint + remainingLength / currentSegmentLength * (nextPoint - currentPoint);

                if (0 < m_thicknessData[thicknessIndex]) {
                    QPen pen(QColor(SIMULATION_LINE_COLOR), 0.02 * m_thicknessData[thicknessIndex]);
                    painter->setPen(pen);
                    painter->drawLine(currentPoint, interpolatedPoint);
                }

                currentPoint      = interpolatedPoint;
                accumulatedLength = 0.0;
                currentSegmentLength -= remainingLength;
                thicknessIndex++;
            }

            accumulatedLength += currentSegmentLength;
            currentPoint = nextPoint;
        }
    }

    QVector<QPointF> RailWeldSimulation::calculateLinePoints() const noexcept {
        constexpr std::array SCALE_LENGTH_TABLE = {
            299.0 / 140.0,
            299.0 / 152.0,
            299.0 / 176.0,
            299.0 / 185.0,
        };
        QVector<QPointF> ret_points        = {};
        const auto&      currentLeftPoint  = m_pointsRefine[simulationProbePoint()];
        const auto&      currentRightPoint = m_pointsRefine[simulationProbePoint() + 1];

        const auto& scale_len      = SCALE_LENGTH_TABLE.at(simulationRailType());
        QPointF     centerPoint    = (currentLeftPoint + currentRightPoint) / 2.0;
        float       sound_distance = 0;
        float       probe_angle    = 90.0 - probeAngle();

        if (0 == simulationProbeDirection()) { // dir left
            probe_angle = 180.0 - probe_angle;
        } else if (2 == simulationProbeDirection()) { // dir Parallel
            probe_angle = 90.0;
        }

        if (simulationProbeDirection() == 0 || simulationProbeDirection() == 1) {
            sound_distance = actualSoundDistance() * scale_len;
        } else {
            sound_distance = actualSoundDistance() * scale_len * cos(qDegreesToRadians(probeAngle()));
        }

        float   dx         = currentRightPoint.x() - currentLeftPoint.x();
        float   dy         = currentRightPoint.y() - currentLeftPoint.y();
        float   angle      = std::atan2(dy, dx);                 // 当前线段的角度（相对于水平线）
        float   emitAngle  = angle + probe_angle * M_PI / 180.0; // set angle
        bool    First_in   = true;
        QPointF startPoint = centerPoint;

        while (sound_distance > 0) {
            QPointF endPoint = QPointF(startPoint.x() + sound_distance * std::cos(emitAngle),
                                       startPoint.y() + sound_distance * std::sin(emitAngle));

            // 初始化最短距离和最接近的交点
            float   minDistance = std::numeric_limits<float>::max();
            QPointF closestIntersectionPoint;
            int     closestSegmentIndex = -1;

            // 检查发射线段与图形中的其他线段是否有交点
            for (int i = 0; i < m_pointsRefine.size() - 1; ++i) {
                QPointF p3 = m_pointsRefine[i];
                QPointF p4 = m_pointsRefine[i + 1];

                QPointF currentIntersectionPoint;
                if (findIntersection(startPoint, endPoint, p3, p4, currentIntersectionPoint)) {
                    // 计算当前交点与发射点之间的距离
                    float distance = std::sqrt(std::pow(currentIntersectionPoint.x() - startPoint.x(), 2) + std::pow(currentIntersectionPoint.y() - startPoint.y(), 2));

                    // 如果该交点距离比当前记录的最短距离小，则更新
                    if (distance < minDistance) {
                        minDistance              = distance;
                        closestIntersectionPoint = currentIntersectionPoint;
                        closestSegmentIndex      = i;
                    }
                }
            }

            if (closestSegmentIndex != -1) {
                float CurrLength = std::sqrt(std::pow(closestIntersectionPoint.x() - startPoint.x(), 2) + std::pow(closestIntersectionPoint.y() - startPoint.y(), 2));
                if (First_in) {
                    ret_points.append(startPoint);
                    ret_points.append(closestIntersectionPoint);
                    First_in = false;
                } else
                    ret_points.append(closestIntersectionPoint);

                // 减去已绘制的部分长度
                sound_distance -= CurrLength;

                // 计算线段的角度
                QPointF p3           = m_pointsRefine[closestSegmentIndex];
                QPointF p4           = m_pointsRefine[closestSegmentIndex + 1];
                float   segmentAngle = std::atan2(p4.y() - p3.y(), p4.x() - p3.x());

                // 计算入射角相对于线段的角度
                float incidentAngle = emitAngle - segmentAngle;

                // 计算反射角
                float reflectionAngle = -incidentAngle; // 保持在同一侧

                // 将反射角转换回全局坐标系
                emitAngle = segmentAngle + reflectionAngle;

                // 更新起点
                startPoint = closestIntersectionPoint;
            } else {
                // 没有交点
                if (First_in) {
                    ret_points.append(startPoint);
                    ret_points.append(endPoint);
                } else {
                    ret_points.append(endPoint);
                }
                break;
            }
        }
        return ret_points;
    }

} // namespace Union::AScan::RailWeld
