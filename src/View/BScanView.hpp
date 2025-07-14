#pragma once

#include <IScanView.hpp>
#include <QQuickPaintedItem>

namespace Union::Model {
    class DefectListModel;
}

namespace Union::View {

    struct DefectItem {
        qreal   hor_len = {};
        qreal   ver_len = {};
        double  max_amp = {}; // 最高波 %
        QPointF pos     = {}; // 最高波位置
    };

    class BScanView : public IScanView {
        friend Model::DefectListModel;

    public:
    private:
        Q_OBJECT
        QML_ELEMENT

        int m_dataCursor = {};

    public:
        BScanView();
        ~BScanView() override;

        Q_INVOKABLE void pushDefectItem(QRect region, double max_amp, QPoint pos);
        Q_INVOKABLE void clearDefectList();

        void replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size = true) noexcept;

        int  dataCursor() const;
        void setDataCursor(int newDataCursor);

    signals:

        void dataCursorChanged();
        void defectItemPushed();

    protected:
        virtual void paint(QPainter* painter) override;
        virtual void draw_image(const QImage& img, QPainter* painter) override;

    private:
        std::vector<DefectItem> defect_list_ = {}; // 缺陷列表
        std::optional<QImage>   defect_mask_ = {};

        void draw_box_defect(QPainter* painter) const; // 绘制缺陷矩形

        // 更新缺陷掩膜图像的大小
        void update_defect_mask_size();

        // 控件上像素点的位置转换为缺陷掩膜上像素点的位置
        std::optional<QPoint> local_pos_to_defect_mask_pos(QPoint pt) const;
        // 原始图像上的像素点的位置转换为缺陷掩膜上像素点的位置
        std::optional<QPoint> raw_image_pos_to_defect_mask_pos(QPoint pt) const;

        Q_PROPERTY(int dataCursor READ dataCursor WRITE setDataCursor NOTIFY dataCursorChanged FINAL)
    };
} // namespace Union::View

QDebug operator<<(QDebug debug, const ::Union::View::DefectItem& defect);
