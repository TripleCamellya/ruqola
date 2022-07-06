/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QAbstractListModel>

#include "bannerinfo/bannerinfos.h"
#include "libruqolacore_export.h"
#include <QVector>

class LIBRUQOLACORE_EXPORT BannerInfosModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum BannerInfosRoles {
        AccountName = Qt::UserRole + 1,
    };
    Q_ENUM(BannerInfosRoles)

    explicit BannerInfosModel(QObject *parent = nullptr);
    ~BannerInfosModel() override;

    Q_REQUIRED_RESULT int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_REQUIRED_RESULT QVariant data(const QModelIndex &index, int role) const override;

    void clear();

    void insertBannerInfos(const BannerInfos &infos);

private:
    BannerInfos mBannerInfos;
};
