/*
   SPDX-FileCopyrightText: 2022-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librocketchatrestapi-qt5_export.h"
#include "restapiabstractjob.h"

#include <QNetworkRequest>
namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT5_EXPORT VideoConferenceProvidersJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    explicit VideoConferenceProvidersJob(QObject *parent = nullptr);
    ~VideoConferenceProvidersJob() override;

    Q_REQUIRED_RESULT bool requireHttpAuthentication() const override;

    Q_REQUIRED_RESULT bool start() override;

    Q_REQUIRED_RESULT QNetworkRequest request() const override;

Q_SIGNALS:
    void videoConferenceProvidersDone(const QJsonObject &obj);

private:
    Q_DISABLE_COPY(VideoConferenceProvidersJob)
    LIBROCKETCHATRESTAPI_QT5_NO_EXPORT void onGetRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
};
}
