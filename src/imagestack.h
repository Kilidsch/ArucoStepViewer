#ifndef IMAGESTACK_H
#define IMAGESTACK_H

#include <QQuickPaintedItem>
#include <QVariantList>

class ImageStack : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QVariantList imgList WRITE setImgList NOTIFY imgListChanged)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)

    QVariantList m_imgList;
    int m_index = 0;

public:
    ImageStack() = default;

    void paint(QPainter *painter) override;

    const QVariantList &imgList() const;
    void setImgList(const QVariantList &newImgList);

    int index() const;
    void setIndex(int newIndex);

signals:
    void imgListChanged();
    void indexChanged();
};

#endif // IMAGESTACK_H
