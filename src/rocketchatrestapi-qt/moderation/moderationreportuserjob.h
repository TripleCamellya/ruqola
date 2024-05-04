/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librocketchatrestapi-qt_export.h"
#include "restapiabstractjob.h"
namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT_EXPORT ModerationReportUserJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    explicit ModerationReportUserJob(QObject *parent = nullptr);
    ~ModerationReportUserJob() override;

    [[nodiscard]] bool start() override;
    [[nodiscard]] bool requireHttpAuthentication() const override;
    [[nodiscard]] bool canStart() const override;
    [[nodiscard]] QNetworkRequest request() const override;

    [[nodiscard]] QJsonDocument json() const;

    [[nodiscard]] QByteArray userIdForMessages() const;
    void setUserIdForMessages(const QByteArray &newUserIdForMessages);

    [[nodiscard]] QByteArray messageId() const;
    void setMessageId(const QByteArray &newMessageId);

Q_SIGNALS:
    void moderationDismissReportsDone();

private:
    Q_DISABLE_COPY(ModerationReportUserJob)
    LIBROCKETCHATRESTAPI_QT_NO_EXPORT void onPostRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
    QByteArray mUserIdForMessages;
    QByteArray mMessageId;
};
}
