/*
   SPDX-FileCopyrightText: 2020-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librocketchatrestapi-qt_export.h"
#include "restapiabstractjob.h"
#include <QDateTime>
#include <QDebug>
namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT_EXPORT RoomsCleanHistoryJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    struct LIBROCKETCHATRESTAPI_QT_EXPORT CleanHistoryInfo {
        [[nodiscard]] bool isValid() const;
        QDateTime latest; // required
        QDateTime oldest; // required
        QByteArray roomId; // required
        QStringList users;
        bool inclusive = false;
        bool excludePinned = false;
        bool filesOnly = false;
        bool ignoreThreads = false;
        bool ignoreDiscussion = false;
    };
    explicit RoomsCleanHistoryJob(QObject *parent = nullptr);
    ~RoomsCleanHistoryJob() override;

    [[nodiscard]] bool start() override;
    [[nodiscard]] bool requireHttpAuthentication() const override;
    [[nodiscard]] bool canStart() const override;

    [[nodiscard]] QNetworkRequest request() const override;

    [[nodiscard]] QJsonDocument json() const;

    [[nodiscard]] CleanHistoryInfo cleanHistoryInfo() const;
    void setCleanHistoryInfo(const CleanHistoryInfo &cleanHistoryInfo);

Q_SIGNALS:
    void cleanHistoryDone();

private:
    LIBROCKETCHATRESTAPI_QT_NO_EXPORT void onPostRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
    CleanHistoryInfo mCleanHistoryInfo;
};
}
Q_DECLARE_TYPEINFO(RocketChatRestApi::RoomsCleanHistoryJob::CleanHistoryInfo, Q_RELOCATABLE_TYPE);
LIBROCKETCHATRESTAPI_QT_EXPORT QDebug operator<<(QDebug d, const RocketChatRestApi::RoomsCleanHistoryJob::CleanHistoryInfo &t);
