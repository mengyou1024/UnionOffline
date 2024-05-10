#include "CImage.hpp"
#include <QLoggingCategory>
#include <QPainter>
#include <QRect>

namespace AScan {
    CImage::CImage() {}

    void CImage::setImage(QImage img, bool resize_if_small) {
        if (resize_if_small) {
            if (maxWidth > 0 && img.width() < maxWidth) {
                setWidth(img.width());
            } else {
                setWidth(maxWidth);
            }

            if (maxHeight > 0 && img.height() < maxHeight) {
                setHeight(img.height());
            } else {
                setHeight(maxHeight);
            }
        } else {
            setWidth(img.width());
            setHeight(img.height());
        }
        m_image = img;
        update();
    }

    void CImage::setImageAndResize(QImage img) {
        setWidth(img.width());
        setHeight(img.height());
        m_image = img;
        update();
    }

    void CImage::setImageAndZoomIn(QImage img, int factor) {
        QSize img_size(img.width() * factor, img.height() * factor);
        setWidth(img.width() * factor);
        setHeight(img.height() * factor);
        m_image = img.scaled(img_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        update();
    }

    void CImage::paint(QPainter* painter) {
        painter->fillRect(QRect(0, 0, width(), height()), QBrush(Qt::transparent));
        if (m_image.has_value()) {
            painter->drawImage(QRect(0, 0, width(), height()), *m_image);
        }
    }

    int CImage::getMaxHeight() const {
        return maxHeight;
    }

    void CImage::setMaxHeight(int newMaxHeight) {
        if (maxHeight == newMaxHeight)
            return;
        maxHeight = newMaxHeight;
        emit maxHeightChanged();
    }

    int CImage::getMaxWidth() const {
        return maxWidth;
    }

    void CImage::setMaxWidth(int newMaxWidth) {
        if (maxWidth == newMaxWidth)
            return;
        maxWidth = newMaxWidth;
        emit maxWidthChanged();
    }
} // namespace AScan
