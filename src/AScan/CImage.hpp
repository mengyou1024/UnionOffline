#ifndef CIMAGE_HPP
#define CIMAGE_HPP
#include <QImage>
#include <QQuickPaintedItem>
namespace AScan {
    class CImage : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT
    public:
        CImage();
        Q_INVOKABLE void setImage(QImage img, bool resize_if_small = false);
        Q_INVOKABLE void setImageAndResize(QImage img);
        Q_INVOKABLE void setImageAndZoomIn(QImage img, int factor = 2);

        int  getMaxWidth() const;
        void setMaxWidth(int newMaxWidth);

        int  getMaxHeight() const;
        void setMaxHeight(int newMaxHeight);

    signals:
        void maxWidthChanged();

        void maxHeightChanged();

    protected:
        void paint(QPainter* painter) override;

    private:
        std::optional<QImage> m_image   = std::nullopt;
        int                   maxWidth  = -1;
        int                   maxHeight = -1;
        Q_PROPERTY(int maxWidth READ getMaxWidth WRITE setMaxWidth NOTIFY maxWidthChanged FINAL)
        Q_PROPERTY(int maxHeight READ getMaxHeight WRITE setMaxHeight NOTIFY maxHeightChanged FINAL)
    };
} // namespace AScan

#endif // CIMAGE_HPP
