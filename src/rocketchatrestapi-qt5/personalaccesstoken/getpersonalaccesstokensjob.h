/*
   SPDX-FileCopyrightText: 2022-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "restapiabstractjob.h"

#include "librocketchatrestapi-qt5_export.h"

#include <QNetworkRequest>
namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT5_EXPORT GetPersonalAccessTokensJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    explicit GetPersonalAccessTokensJob(QObject *parent = nullptr);
    ~GetPersonalAccessTokensJob() override;

    [[nodiscard]] bool start() override;
    [[nodiscard]] bool requireHttpAuthentication() const override;
    [[nodiscard]] QNetworkRequest request() const override;
    [[nodiscard]] bool hasQueryParameterSupport() const override;

Q_SIGNALS:
    void getPersonalAccessTokensDone(const QJsonObject &obj);

private:
    Q_DISABLE_COPY(GetPersonalAccessTokensJob)
    LIBROCKETCHATRESTAPI_QT5_NO_EXPORT void onGetRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
};
}
