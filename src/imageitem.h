#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QImage>
#include <QQuickItem>
#include <QQuickPaintedItem>

class ImageItem : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QImage img READ img WRITE setImg NOTIFY imgChanged)
  public:
    ImageItem() = default;
    ImageItem(QImage img) : m_img(img)
    {
    }

    // QQuickPaintedItem interface
  public:
    void paint(QPainter *painter) override;
    QImage img() const;
    void setImg(const QImage &newImg);

  signals:
    void imgChanged(const QImage &newImg);

  private:
    QImage m_img;
};

#endif // IMAGEITEM_H
