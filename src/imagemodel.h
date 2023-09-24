#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include "testimages.h"
#include <QAbstractListModel>

class ImageModel : public QAbstractListModel
{
    Q_OBJECT

    enum ImageRoles
    {
        ImageRole = Qt::UserRole + 1,
        TabName
    };

  public:
    explicit ImageModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setTabs(const std::vector<Tab> &new_tabs)
    {
        emit layoutAboutToBeChanged();
        m_tabs = new_tabs;
        emit layoutChanged();
    }

  private:
    std::vector<Tab> m_tabs;

    // QAbstractItemModel interface
  public:
    QHash<int, QByteArray> roleNames() const;
};

#endif // IMAGEMODEL_H
