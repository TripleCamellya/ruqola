/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "librocketchatrestapi-qt_export.h"
#include "restapiabstractjob.h"

namespace RocketChatRestApi
{
class LIBROCKETCHATRESTAPI_QT_EXPORT GetThreadsJob : public RestApiAbstractJob
{
    Q_OBJECT
public:
    enum class TheadSearchType : uint8_t {
        All = 0,
        Following,
        Unread,
    };
    explicit GetThreadsJob(QObject *parent = nullptr);
    ~GetThreadsJob() override;

    [[nodiscard]] bool canStart() const override;

    [[nodiscard]] bool requireHttpAuthentication() const override;

    [[nodiscard]] bool start() override;

    [[nodiscard]] QNetworkRequest request() const override;

    [[nodiscard]] QString roomId() const;
    void setRoomId(const QString &roomId);
    [[nodiscard]] bool hasQueryParameterSupport() const override;

    [[nodiscard]] TheadSearchType searchType() const;
    void setSearchType(TheadSearchType newSearchType);

    void setUseSyntaxRc70(bool state);
Q_SIGNALS:
    void getThreadsDone(const QJsonObject &obj, const QString &roomId);

private:
    LIBROCKETCHATRESTAPI_QT_NO_EXPORT void onGetRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson) override;
    TheadSearchType mSearchType = TheadSearchType::All;
    QString mRoomId;
    bool mUseSyntaxRc70 = false;
};
}
