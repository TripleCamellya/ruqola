/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librocketchatrestapi-qt5_export.h"
#include "restapiabstractjob.h"
#include <QDebug>

namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT5_EXPORT UsersSetPreferencesJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    struct LIBROCKETCHATRESTAPI_QT5_EXPORT UsersSetPreferencesInfo {
        Q_REQUIRED_RESULT bool isValid() const;
        QString userId;
        QString newRoomNotification;
        QString newMessageNotification;
        QString desktopNotifications;
        QString mobileNotifications;
        QString emailNotificationMode;
        QStringList highlights;
        bool useEmoji = true;
        bool convertAsciiToEmoji = true;
        bool hideRoles = false;
        bool hideAvatars = false;
        // Add more
    };

    explicit UsersSetPreferencesJob(QObject *parent = nullptr);
    ~UsersSetPreferencesJob() override;

    Q_REQUIRED_RESULT bool start() override;
    Q_REQUIRED_RESULT bool requireHttpAuthentication() const override;
    Q_REQUIRED_RESULT bool canStart() const override;
    Q_REQUIRED_RESULT QNetworkRequest request() const override;

    Q_REQUIRED_RESULT QJsonDocument json() const;

    Q_REQUIRED_RESULT UsersSetPreferencesInfo usersSetPreferencesInfo() const;
    void setUsersSetPreferencesInfo(const UsersSetPreferencesInfo &usersSetPreferencesInfo);

Q_SIGNALS:
    void usersSetPreferencesDone(const QJsonObject &replyObject);

private:
    Q_DISABLE_COPY(UsersSetPreferencesJob)
    void onPostRequestResponse(const QJsonDocument &replyJson) override;
    UsersSetPreferencesInfo mUsersSetPreferencesInfo;
};
}
Q_DECLARE_TYPEINFO(RocketChatRestApi::UsersSetPreferencesJob::UsersSetPreferencesInfo, Q_MOVABLE_TYPE);
LIBROCKETCHATRESTAPI_QT5_EXPORT QDebug operator<<(QDebug d, const RocketChatRestApi::UsersSetPreferencesJob::UsersSetPreferencesInfo &t);
