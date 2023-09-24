#include "imageitem.h"

#include <QPainter>
#include <iostream>

void ImageItem::paint(QPainter *painter)
{
    std::cerr << "Soll paintedskjhdsf!\n";
    QRect target(0, 0, width(), height());
    painter->drawImage(target, m_img);
}

QImage ImageItem::img() const
{
    return m_img;
}

void ImageItem::setImg(const QImage &newImg)
{
    std::cerr << "Got a new image!!!\n";
    if (newImg == m_img)
    {
        std::cerr << "Got a NOTOTOTOTO\n";

        return;
    }
    m_img = newImg;
    emit imgChanged(newImg);
    std::cerr << "Nice\n";
}
