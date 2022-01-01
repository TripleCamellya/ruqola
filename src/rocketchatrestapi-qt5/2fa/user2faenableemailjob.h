/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librestapi_private_export.h"
#include "restapiabstractjob.h"
namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT5_TESTS_EXPORT User2FAEnableEmailJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    explicit User2FAEnableEmailJob(QObject *parent = nullptr);
    ~User2FAEnableEmailJob() override;

    Q_REQUIRED_RESULT bool start() override;
    Q_REQUIRED_RESULT bool requireHttpAuthentication() const override;
    Q_REQUIRED_RESULT bool canStart() const override;
    Q_REQUIRED_RESULT QNetworkRequest request() const override;

    Q_REQUIRED_RESULT QJsonDocument json() const;

Q_SIGNALS:
    void enableEmailDone();

protected:
    void onPostRequestResponse(const QJsonDocument &replyJson) override;

private:
    Q_DISABLE_COPY(User2FAEnableEmailJob)
    void slotEnabledEmail();
};
}
