/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librocketchatrestapi-qt_export.h"
#include "restapiabstractjob.h"
#include <QDateTime>
#include <QDebug>
namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT_EXPORT RoomsUnmuteUserJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    explicit RoomsUnmuteUserJob(QObject *parent = nullptr);
    ~RoomsUnmuteUserJob() override;

    [[nodiscard]] bool start() override;
    [[nodiscard]] bool requireHttpAuthentication() const override;
    [[nodiscard]] bool canStart() const override;

    [[nodiscard]] QNetworkRequest request() const override;

    [[nodiscard]] QJsonDocument json() const;

Q_SIGNALS:
    void cleanHistoryDone();

private:
    LIBROCKETCHATRESTAPI_QT_NO_EXPORT void onPostRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
};
}
