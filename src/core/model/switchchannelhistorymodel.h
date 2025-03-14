/*
   SPDX-FileCopyrightText: 2022-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolacore_export.h"
#include "utils.h"
#include <QAbstractListModel>

class LIBRUQOLACORE_EXPORT SwitchChannelHistoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum SwitchChannelHistoryRoles {
        Name = Qt::UserRole + 1,
        Identifier,
        AvatarInfo,
    };
    Q_ENUM(SwitchChannelHistoryRoles)

    struct SwitchChannelInfo {
        SwitchChannelInfo() = default;
        SwitchChannelInfo(const QString &name, const QByteArray &identifier, const Utils::AvatarInfo &info)
            : mName(name)
            , mIdentifier(identifier)
            , mAvatarInfo(info)
        {
        }
        [[nodiscard]] bool operator==(const SwitchChannelInfo &other) const;
        QString mName;
        QByteArray mIdentifier;
        Utils::AvatarInfo mAvatarInfo;
    };

    explicit SwitchChannelHistoryModel(QObject *parent = nullptr);
    ~SwitchChannelHistoryModel() override;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    void addHistory(const SwitchChannelInfo &info);

private:
    QList<SwitchChannelInfo> mSwichChannelsInfo;
};

Q_DECLARE_METATYPE(SwitchChannelHistoryModel::SwitchChannelInfo)
Q_DECLARE_TYPEINFO(SwitchChannelHistoryModel::SwitchChannelInfo, Q_RELOCATABLE_TYPE);
