/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "roomsexportjob.h"
using namespace Qt::Literals::StringLiterals;

#include "restapimethod.h"
#include "rocketchatqtrestapi_debug.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
using namespace RocketChatRestApi;
RoomsExportJob::RoomsExportJob(QObject *parent)
    : RestApiAbstractJob(parent)
{
}

RoomsExportJob::~RoomsExportJob() = default;

bool RoomsExportJob::start()
{
    if (!canStart()) {
        deleteLater();
        return false;
    }
    addStartRestApiInfo("RoomsExportJob::start");
    submitPostRequest(json());

    return true;
}

void RoomsExportJob::onPostRequestResponse(const QString &replyErrorString, const QJsonDocument &replyJson)
{
    const QJsonObject replyObject = replyJson.object();
    if (replyObject["success"_L1].toBool()) {
        addLoggerInfo(QByteArrayLiteral("RoomsExportJob: success: ") + replyJson.toJson(QJsonDocument::Indented));
        Q_EMIT roomExportDone();
    } else {
        emitFailedMessage(replyErrorString, replyObject);
        addLoggerWarning(QByteArrayLiteral("RoomsExportJob: problem: ") + replyJson.toJson(QJsonDocument::Indented));
    }
}

bool RoomsExportJob::requireHttpAuthentication() const
{
    return true;
}

bool RoomsExportJob::canStart() const
{
    if (!mRoomExportInfo.isValid()) {
        qCWarning(ROCKETCHATQTRESTAPI_LOG) << "mRoomExportInfo: mRoomExportInfo is not valid.";
        return false;
    }

    if (!RestApiAbstractJob::canStart()) {
        return false;
    }
    return true;
}

QJsonDocument RoomsExportJob::json() const
{
    QJsonObject jsonObj;

    jsonObj["rid"_L1] = QLatin1StringView(mRoomExportInfo.roomId);
    switch (mRoomExportInfo.exportAs) {
    case RoomsExportInfo::ExportAs::Unknown:
        // Nothing it's a bug here.
        break;
    case RoomsExportInfo::ExportAs::File:
        jsonObj["type"_L1] = QStringLiteral("file");
        createJsonForFile(jsonObj);
        break;
    case RoomsExportInfo::ExportAs::Email:
        jsonObj["type"_L1] = QStringLiteral("email");
        createJsonForEmail(jsonObj);
        break;
    }
    const QJsonDocument postData = QJsonDocument(jsonObj);
    // qDebug() << " postData**************** " << postData;
    return postData;
}

void RoomsExportJob::createJsonForFile(QJsonObject &jsonObj) const
{
    switch (mRoomExportInfo.fileFormat) {
    case RoomsExportInfo::FileFormat::Unknown:
        // It's a bug
        break;
    case RoomsExportInfo::FileFormat::Html:
        jsonObj["format"_L1] = QStringLiteral("html");
        break;
    case RoomsExportInfo::FileFormat::Json:
        jsonObj["format"_L1] = QStringLiteral("json");
        break;
    }
    jsonObj["dateTo"_L1] = mRoomExportInfo.dateTo.toString(Qt::ISODateWithMs);
    jsonObj["dateFrom"_L1] = mRoomExportInfo.dateFrom.toString(Qt::ISODateWithMs);
}

void RoomsExportJob::createJsonForEmail(QJsonObject &jsonObj) const
{
    jsonObj["toUsers"_L1] = QJsonArray::fromStringList(mRoomExportInfo.toUsers);
    jsonObj["toEmails"_L1] = QJsonArray::fromStringList(mRoomExportInfo.toEmails);
    jsonObj["subject"_L1] = mRoomExportInfo.subject;
    jsonObj["messages"_L1] = mRoomExportInfo.messages;
}

RoomsExportJob::RoomsExportInfo RoomsExportJob::roomExportInfo() const
{
    return mRoomExportInfo;
}

void RoomsExportJob::setRoomExportInfo(const RoomsExportJob::RoomsExportInfo &roomExportInfo)
{
    mRoomExportInfo = roomExportInfo;
}

QNetworkRequest RoomsExportJob::request() const
{
    const QUrl url = mRestApiMethod->generateUrl(RestApiUtil::RestApiUrlType::RoomsExport);
    QNetworkRequest request(url);
    addAuthRawHeader(request);
    addRequestAttribute(request);
    return request;
}

QDebug operator<<(QDebug d, const RocketChatRestApi::RoomsExportJob::RoomsExportInfo &t)
{
    d << "roomId " << t.roomId;
    d << "toUsers " << t.toUsers;
    d << "toEmails " << t.toEmails;
    d << "subject " << t.subject;
    d << "messages " << t.messages;
    d << "dateFrom " << t.dateFrom;
    d << "dateTo " << t.dateTo;
    d << "fileFormat " << static_cast<int>(t.fileFormat);
    d << "exportAs " << static_cast<int>(t.exportAs);
    return d;
}

bool RoomsExportJob::RoomsExportInfo::isValid() const
{
    // Check from exportas type
    return fileFormat != RoomsExportInfo::FileFormat::Unknown && exportAs != RoomsExportInfo::ExportAs::Unknown && !roomId.isEmpty();
}

#include "moc_roomsexportjob.cpp"
