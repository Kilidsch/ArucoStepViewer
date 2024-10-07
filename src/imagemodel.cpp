#include "imagemodel.h"

#include <QImage>

ImageModel::ImageModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ImageModel::rowCount(const QModelIndex &parent) const
{
    // std::unique_lock lock(m_tab_mut);
    //   For list models only the root node (an invalid parent) should return the list's size. For all
    //   other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_tabs[m_current_tab.load()].size());
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
    // std::unique_lock lock(m_tab_mut);

    Q_UNUSED(role)
    if (!index.isValid())
    {
        return QVariant();
    }

    const auto &tab = m_tabs[m_current_tab.load()][index.row()];
    if (role == ImageRoles::ImageRole)
    {
        auto &img = tab.imgs[0];
        QImage show_img{img.data, img.cols, img.rows, QImage::Format::Format_BGR888};
        return QVariant(show_img.copy());
    }
    else if (role == ImageRoles::TabName)
    {
        return QVariant(QString::fromStdString(tab.name));
    }
    else if (role == ImageRoles::ImageListRole)
    {
        QVariantList image_list;
        for (const auto &cv_img : tab.imgs)
        {
            QImage show_img{cv_img.data, cv_img.cols, cv_img.rows, QImage::Format::Format_BGR888};
            image_list.push_back(show_img.copy());
        }
        return QVariant(image_list);
    }
    else
    {
        return QVariant();
    }
}

void ImageModel::setTabs(const std::vector<Tab> &new_tabs)
{
    {
        std::unique_lock lock(m_tab_mut);
        // Note: we know that the number of tabs/rows never changes
        // and the name of the tabs does not change either
        // So emit dataChanged signal instead of beginModelReset...
        if (m_current_tab.load() == 0)
        {
            m_tabs[1] = new_tabs;
            m_current_tab.store(1);
        }
        else
        {
            m_tabs[0] = new_tabs;
            m_current_tab.store(0);
        }
    }
    // emit dataChanged(index(0), index(static_cast<int>(1)), {ImageListRole, ImageRole});
    emit dataChanged(index(0), index(static_cast<int>(m_tabs[m_current_tab.load()].size() - 1)),
                     {ImageListRole, ImageRole});
}

QHash<int, QByteArray> ImageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[TabName] = "name";
    roles[ImageListRole] = "image_list";
    return roles;
}
