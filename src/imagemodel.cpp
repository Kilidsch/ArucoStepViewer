#include "imagemodel.h"

#include <QImage>
#include <iostream>

ImageModel::ImageModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ImageModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_tabs.size());
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)
    if (!index.isValid())
    {
        std::cerr << "Invalid indexy?!?!\n";
        return QVariant();
    }

    const auto &tab = m_tabs[index.row()];
    if (role == ImageRoles::ImageRole)
    {
        auto &img = tab.imgs[0];
        QImage show_img{img.data, img.cols, img.rows, QImage::Format::Format_BGR888};
        return QVariant(show_img);
    }
    else if (role == ImageRoles::TabName)
    {
        return QVariant(QString::fromStdString(tab.name));
    }
    else
    {
        return QVariant();
    }
}

QHash<int, QByteArray> ImageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[TabName] = "name";
    return roles;
}
