/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "markroomasunreadjobtest.h"
#include "ruqola_restapi_helper.h"
#include "subscriptions/markroomasunreadjob.h"
#include <QJsonDocument>
QTEST_GUILESS_MAIN(MarkRoomAsUnReadJobTest)
using namespace RocketChatRestApi;
MarkRoomAsUnReadJobTest::MarkRoomAsUnReadJobTest(QObject *parent)
    : QObject(parent)
{
}

void MarkRoomAsUnReadJobTest::shouldHaveDefaultValue()
{
    MarkRoomAsUnReadJob job;
    verifyDefaultValue(&job);
    QVERIFY(job.requireHttpAuthentication());
    QVERIFY(job.objectId().isEmpty());
    QCOMPARE(job.unReadObject(), MarkRoomAsUnReadJob::MarkAsUnReadObject::Unknown);
    QVERIFY(!job.hasQueryParameterSupport());
}

void MarkRoomAsUnReadJobTest::shouldGenerateRequest()
{
    MarkRoomAsUnReadJob job;
    QNetworkRequest request = QNetworkRequest(QUrl());
    verifyAuthentication(&job, request);
    QCOMPARE(request.url(), QUrl(QStringLiteral("http://www.kde.org/api/v1/subscriptions.unread")));
    QCOMPARE(request.header(QNetworkRequest::ContentTypeHeader).toString(), QStringLiteral("application/json"));
}

void MarkRoomAsUnReadJobTest::shouldGenerateJson()
{
    MarkRoomAsUnReadJob job;
    const QString roomId = QStringLiteral("foo1");
    job.setObjectId(roomId.toLatin1());
    job.setUnReadObject(MarkRoomAsUnReadJob::MarkAsUnReadObject::Room);
    QCOMPARE(job.json().toJson(QJsonDocument::Compact), QStringLiteral(R"({"roomId":"%1"})").arg(roomId).toLatin1());
    job.setUnReadObject(MarkRoomAsUnReadJob::MarkAsUnReadObject::FromMessage);
    QCOMPARE(job.json().toJson(QJsonDocument::Compact), QStringLiteral(R"({"firstUnreadMessage":{"_id":"foo1"}})").arg(roomId).toLatin1());
}

void MarkRoomAsUnReadJobTest::shouldNotStarting()
{
    MarkRoomAsUnReadJob job;

    RestApiMethod method;
    method.setServerUrl(QStringLiteral("http://www.kde.org"));
    job.setRestApiMethod(&method);

    QNetworkAccessManager mNetworkAccessManager;
    job.setNetworkAccessManager(&mNetworkAccessManager);
    QVERIFY(!job.canStart());
    const QString auth = QStringLiteral("foo");
    const QString userId = QStringLiteral("foo");
    job.setAuthToken(auth);
    QVERIFY(!job.canStart());
    job.setUserId(userId);
    QVERIFY(!job.canStart());
    const QByteArray roomId = "foo1"_ba;
    job.setObjectId(roomId);
    QVERIFY(!job.canStart());
    job.setUnReadObject(MarkRoomAsUnReadJob::MarkAsUnReadObject::FromMessage);
    QVERIFY(job.canStart());
}

#include "moc_markroomasunreadjobtest.cpp"
