/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "appcategoriesjob.h"

#include "restapimethod.h"
#include "rocketchatqtrestapi_debug.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>

using namespace Qt::Literals::StringLiterals;
using namespace RocketChatRestApi;
AppCategoriesJob::AppCategoriesJob(QObject *parent)
    : RestApiAbstractJob(parent)
{
}

AppCategoriesJob::~AppCategoriesJob() = default;

bool AppCategoriesJob::requireHttpAuthentication() const
{
    return true;
}

bool AppCategoriesJob::start()
{
    if (!canStart()) {
        qCWarning(ROCKETCHATQTRESTAPI_LOG) << "Impossible to start get app count job";
        deleteLater();
        return false;
    }
    submitGetRequest();

    addStartRestApiInfo(QByteArrayLiteral("AppCategoriesJob: get report info starting"));
    return true;
}

void AppCategoriesJob::onGetRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson)
{
    Q_UNUSED(replyErrorString)
    const QJsonArray replyArray = replyJson.array();
    Q_EMIT appCategoriesDone(replyArray);
#if 0
    const QJsonObject replyObject = replyJson.object();
    if (replyObject["success"_L1].toBool()) {
        addLoggerInfo(QByteArrayLiteral("AppCategoriesJob: success: ") + replyJson.toJson(QJsonDocument::Indented));
        Q_EMIT appCategoriesDone(replyObject);
    } else {
        emitFailedMessage(replyErrorString, replyObject);
        addLoggerWarning(QByteArrayLiteral("AppCategoriesJob: Problem when we tried to get app count info : ") + replyJson.toJson(QJsonDocument::Indented));
    }
#endif
}

QNetworkRequest AppCategoriesJob::request() const
{
    const QUrl url = mRestApiMethod->generateUrl(RestApiUtil::RestApiUrlType::CategoriesApps, RestApiUtil::RestApiUrlExtensionType::Apps);
    QNetworkRequest request(url);
    addAuthRawHeader(request);
    addRequestAttribute(request);

    return request;
}

#include "moc_appcategoriesjob.cpp"
