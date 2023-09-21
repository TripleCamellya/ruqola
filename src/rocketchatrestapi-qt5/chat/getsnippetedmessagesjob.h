/*
   SPDX-FileCopyrightText: 2019-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librestapi_private_export.h"
#include "restapiabstractjob.h"

#include <QNetworkRequest>
namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT5_TESTS_EXPORT GetSnippetedMessagesJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    explicit GetSnippetedMessagesJob(QObject *parent = nullptr);
    ~GetSnippetedMessagesJob() override;

    [[nodiscard]] bool canStart() const override;

    [[nodiscard]] bool requireHttpAuthentication() const override;

    [[nodiscard]] bool start() override;

    [[nodiscard]] QNetworkRequest request() const override;

    [[nodiscard]] QString roomId() const;
    void setRoomId(const QString &roomId);

    [[nodiscard]] bool hasQueryParameterSupport() const override;

Q_SIGNALS:
    void getSnippetedMessagesDone(const QJsonObject &obj, const QString &roomId);

private:
    Q_DISABLE_COPY(GetSnippetedMessagesJob)
    void onGetRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
    QString mRoomId;
};
}
