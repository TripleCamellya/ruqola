/*
   Copyright (c) 2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License or
   ( at your option ) version 3 or, at the discretion of KDE e.V.
   ( which shall act as a proxy as in section 14 of the GPLv3 ), any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "channelcleanhistoryjob.h"

#include "rocketchatqtrestapi_debug.h"
#include "restapimethod.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
using namespace RocketChatRestApi;
ChannelCleanHistoryJob::ChannelCleanHistoryJob(QObject *parent)
    : ChannelBaseJob(parent)
{
}

ChannelCleanHistoryJob::~ChannelCleanHistoryJob()
{
}

bool ChannelCleanHistoryJob::start()
{
    if (!canStart()) {
        deleteLater();
        return false;
    }
    addStartRestApiInfo("ChannelCleanHistoryJob::start");
    QNetworkReply *reply = submitPostRequest(json());
    connect(reply, &QNetworkReply::finished, this, &ChannelCleanHistoryJob::slotCleanHistoryFinished);
    return true;
}

void ChannelCleanHistoryJob::slotCleanHistoryFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply) {
        const QByteArray data = reply->readAll();
        const QJsonDocument replyJson = QJsonDocument::fromJson(data);
        const QJsonObject replyObject = replyJson.object();

        if (replyObject[QStringLiteral("success")].toBool()) {
            addLoggerInfo(QByteArrayLiteral("ChannelCleanHistoryJob: success: ") + replyJson.toJson(QJsonDocument::Indented));
            Q_EMIT cleanHistoryDone();
        } else {
            emitFailedMessage(replyObject, reply);
            addLoggerWarning(QByteArrayLiteral("ChannelCleanHistoryJob: problem: ") + replyJson.toJson(QJsonDocument::Indented));
        }
        reply->deleteLater();
    }
    deleteLater();
}

ChannelCleanHistoryJob::CleanHistoryInfo ChannelCleanHistoryJob::cleanHistoryInfo() const
{
    return mCleanHistoryInfo;
}

void ChannelCleanHistoryJob::setCleanHistoryInfo(const CleanHistoryInfo &cleanHistoryInfo)
{
    mCleanHistoryInfo = cleanHistoryInfo;
}

bool ChannelCleanHistoryJob::requireHttpAuthentication() const
{
    return true;
}

bool ChannelCleanHistoryJob::canStart() const
{
    if (!hasRoomIdentifier()) {
        qCWarning(ROCKETCHATQTRESTAPI_LOG) << "ChannelCleanHistoryJob: RoomId and RoomName are empty";
        return false;
    }
    if (!RestApiAbstractJob::canStart()) {
        return false;
    }
    return true;
}

QJsonDocument ChannelCleanHistoryJob::json() const
{
    QJsonObject jsonObj;
    generateJSon(jsonObj);

    const QJsonDocument postData = QJsonDocument(jsonObj);
    return postData;
}

QNetworkRequest ChannelCleanHistoryJob::request() const
{
    QUrl url = mRestApiMethod->generateUrl(RestApiUtil::RestApiUrlType::ChannelsCleanHistory);
    //TODO
    QNetworkRequest request(url);
    addAuthRawHeader(request);
    addRequestAttribute(request);
    return request;
}

bool ChannelCleanHistoryJob::CleanHistoryInfo::isValid() const
{
    return lastest.isValid() && oldest.isValid() && !roomId.isEmpty();
    return false;
}
