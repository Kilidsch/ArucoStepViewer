#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include "testimages.h"
#include <QAbstractListModel>
#include <mutex>

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

    void setTabs(const std::vector<Tab> &new_tabs);

  private:
    std::array<std::vector<Tab>, 2> m_tabs;
    std::atomic<size_t> m_current_tab = 0;
    std::mutex m_tab_mut;

    // QAbstractItemModel interface
  public:
    QHash<int, QByteArray> roleNames() const override;
};

#endif // IMAGEMODEL_H
