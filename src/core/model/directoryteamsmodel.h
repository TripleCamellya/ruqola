/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "custombasemodel.h"
#include "libruqolacore_export.h"
#include "roominfo/roomsinfo.h"

class LIBRUQOLACORE_EXPORT DirectoryTeamsModel : public CustomBaseModel
{
    Q_OBJECT
public:
    enum DirectoryTeamsRoles {
        TeamName,
        RoomsCount,
        TeamIdentifier,
        CreateTeamDateTime,
        CreateTeam,
        RoomIdentifier,
        LastColumn = RoomIdentifier,
    };
    Q_ENUM(DirectoryTeamsRoles)
    explicit DirectoryTeamsModel(QObject *parent = nullptr);
    ~DirectoryTeamsModel() override;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = {}) const override;

    void parseElements(const QJsonObject &obj) override;
    void addMoreElements(const QJsonObject &obj) override;

    [[nodiscard]] int total() const override;

protected:
    [[nodiscard]] QList<int> hideColumns() const override;

private:
    LIBRUQOLACORE_NO_EXPORT void checkFullList() override;
    RoomsInfo mRoomsInfo;
};
