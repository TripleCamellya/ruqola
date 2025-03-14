/*
   SPDX-FileCopyrightText: 2020-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "deleteuserjob.h"

#include "restapimethod.h"
#include "rocketchatqtrestapi_debug.h"

#include <QJsonDocument>
#include <QJsonObject>
using namespace Qt::Literals::StringLiterals;
using namespace RocketChatRestApi;
DeleteUserJob::DeleteUserJob(QObject *parent)
    : UserBaseJob(parent)
{
}

DeleteUserJob::~DeleteUserJob() = default;

bool DeleteUserJob::start()
{
    if (!canStart()) {
        deleteLater();
        return false;
    }
    addStartRestApiInfo("DeleteUserJob::start");
    submitPostRequest(json());
    return true;
}

void DeleteUserJob::onPostRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson)
{
    const QJsonObject replyObject = replyJson.object();
    qDebug() << "replyObject " << replyObject;

    if (replyObject["success"_L1].toBool()) {
        addLoggerInfo("DeleteUserJob: success: "_ba + replyJson.toJson(QJsonDocument::Indented));
        Q_EMIT deleteUserDone();
    } else {
        emitFailedMessage(replyErrorString, replyObject);
        addLoggerWarning("DeleteUserJob: Problem: "_ba + replyJson.toJson(QJsonDocument::Indented));
    }
}

bool DeleteUserJob::requireHttpAuthentication() const
{
    return true;
}

bool DeleteUserJob::canStart() const
{
    if (!RestApiAbstractJob::canStart()) {
        return false;
    }
    if (!hasUserIdentifier()) {
        qCWarning(ROCKETCHATQTRESTAPI_LOG) << "DeleteUserJob: identifier is empty";
        return false;
    }
    return true;
}

QNetworkRequest DeleteUserJob::request() const
{
    const QUrl url = mRestApiMethod->generateUrl(RestApiUtil::RestApiUrlType::UsersDelete);
    QNetworkRequest request(url);
    addAuthRawHeader(request);
    addRequestAttribute(request);
    return request;
}

QJsonDocument DeleteUserJob::json() const
{
    QJsonObject jsonObj;
    generateJson(jsonObj);
    const QJsonDocument postData = QJsonDocument(jsonObj);
    return postData;
}

#include "moc_deleteuserjob.cpp"
