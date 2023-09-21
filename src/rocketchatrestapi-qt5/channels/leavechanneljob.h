/*
   SPDX-FileCopyrightText: 2018-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "channelgroupbasejob.h"
#include "librestapi_private_export.h"
namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT5_TESTS_EXPORT LeaveChannelJob : public ChannelGroupBaseJob
{
    Q_OBJECT
public:
    explicit LeaveChannelJob(QObject *parent = nullptr);
    ~LeaveChannelJob() override;

    [[nodiscard]] bool start() override;
    [[nodiscard]] bool requireHttpAuthentication() const override;
    [[nodiscard]] bool canStart() const override;

    [[nodiscard]] QNetworkRequest request() const override;

    [[nodiscard]] QJsonDocument json() const;

Q_SIGNALS:
    void leaveChannelDone();

private:
    Q_DISABLE_COPY(LeaveChannelJob)
    void onPostRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
};
}
