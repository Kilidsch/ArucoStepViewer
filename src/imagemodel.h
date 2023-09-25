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
        TabName,
        ImageListRole
    };

  public:
    explicit ImageModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setTabs(const std::vector<Tab> &new_tabs)
    {
        // Note: we know that the number of tabs/rows never changes
        // and the name of the tabs does not change either
        // So emit dataChanged signal instead of beginModelReset...
        m_tabs = new_tabs;
        emit dataChanged(index(0), index(static_cast<int>(m_tabs.size() - 1)), {ImageListRole, ImageRole});
    }

  private:
    std::vector<Tab> m_tabs;

    // QAbstractItemModel interface
  public:
    QHash<int, QByteArray> roleNames() const override;
};

#endif // IMAGEMODEL_H
