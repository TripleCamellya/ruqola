/*
   SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librocketchatrestapi-qt_export.h"
#include "restapiabstractjob.h"

namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT_EXPORT GetUsersInRoleJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    explicit GetUsersInRoleJob(QObject *parent = nullptr);
    ~GetUsersInRoleJob() override;

    [[nodiscard]] bool requireHttpAuthentication() const override;

    [[nodiscard]] bool start() override;

    [[nodiscard]] QNetworkRequest request() const override;

    [[nodiscard]] bool hasQueryParameterSupport() const override;

    [[nodiscard]] const QString &roleId() const;
    void setRoleId(const QString &newRoleId);

    [[nodiscard]] bool canStart() const override;

Q_SIGNALS:
    void getUsersInRoleDone(const QJsonObject &obj);

private:
    LIBROCKETCHATRESTAPI_QT_NO_EXPORT void onGetRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
    LIBROCKETCHATRESTAPI_QT_NO_EXPORT void initialUrlParameters(QUrlQuery &urlQuery) const;
    QString mRoleId;
};
}
