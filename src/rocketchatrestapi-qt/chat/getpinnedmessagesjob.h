/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librocketchatrestapi-qt_export.h"
#include "restapiabstractjob.h"

namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT_EXPORT GetPinnedMessagesJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    explicit GetPinnedMessagesJob(QObject *parent = nullptr);
    ~GetPinnedMessagesJob() override;

    [[nodiscard]] bool canStart() const override;

    [[nodiscard]] bool requireHttpAuthentication() const override;

    [[nodiscard]] bool start() override;

    [[nodiscard]] QNetworkRequest request() const override;

    [[nodiscard]] QString roomId() const;
    void setRoomId(const QString &roomId);

    [[nodiscard]] bool hasQueryParameterSupport() const override;

Q_SIGNALS:
    void getPinnedMessagesDone(const QJsonObject &obj, const QString &roomId);

private:
    Q_DISABLE_COPY(GetPinnedMessagesJob)
    void onGetRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
    QString mRoomId;
};
}
