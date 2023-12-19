#include "imageitem.h"

#include <QPainter>

void ImageItem::paint(QPainter *painter)
{
    QRect target(0, 0, width(), height());
    painter->drawImage(target, m_img);
}

QImage ImageItem::img() const
{
    return m_img;
}

void ImageItem::setImg(const QImage &newImg)
{
    if (newImg == m_img)
    {
        return;
    }
    m_img = newImg;
    emit imgChanged(m_img);
}
