#include "imagestack.h"

#include <QPainter>

void ImageStack::paint(QPainter *painter)
{
    QRect target(0, 0, width(), height());
    if (m_imgList.isEmpty())
    {
        return;
    }
    else
    {
        assert(m_index >= 0);
        if(m_index >= m_imgList.size()){
            m_index = m_imgList.size() - 1;
        }
        painter->drawImage(target, m_imgList.at(m_index).value<QImage>());
    }
}

const QVariantList &ImageStack::imgList() const
{
    return m_imgList;
}

void ImageStack::setImgList(const QVariantList &newImgList)
{
    if (m_imgList == newImgList)
        return;
    m_imgList = newImgList;
    update();
    emit imgListChanged();
}

int ImageStack::index() const
{
    return m_index;
}

void ImageStack::setIndex(int newIndex)
{
    if (m_index == newIndex)
        return;
    m_index = newIndex;
    update();
    emit indexChanged();
}
