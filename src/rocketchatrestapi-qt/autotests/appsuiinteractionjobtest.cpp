/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "appsuiinteractionjobtest.h"
#include "misc/methodcalljob.h"
#include "ruqola_restapi_helper.h"
#include <QJsonDocument>
QTEST_GUILESS_MAIN(AppsUiInteractionJobTest)
using namespace RocketChatRestApi;
AppsUiInteractionJobTest::AppsUiInteractionJobTest(QObject *parent)
    : QObject(parent)
{
}

void AppsUiInteractionJobTest::shouldHaveDefaultValue()
{
    MethodCallJob job;
    verifyDefaultValue(&job);
    QVERIFY(job.requireHttpAuthentication());
    QVERIFY(!job.hasQueryParameterSupport());
    MethodCallJob::MethodCallJobInfo info;
    info.anonymous = true;
    job.setMethodCallJobInfo(info);
    QVERIFY(!job.requireHttpAuthentication());
}

void AppsUiInteractionJobTest::shouldGenerateRequest()
{
    MethodCallJob job;
    QNetworkRequest request = QNetworkRequest(QUrl());
    verifyAuthentication(&job, request);
    QCOMPARE(request.url(), QUrl(QStringLiteral("http://www.kde.org/api/v1/method.call")));
    QCOMPARE(request.header(QNetworkRequest::ContentTypeHeader).toString(), QStringLiteral("application/json"));

    MethodCallJob::MethodCallJobInfo info;
    info.methodName = QStringLiteral("login");
    info.anonymous = true;
    job.setMethodCallJobInfo(info);
    verifyAuthentication(&job, request);
    QCOMPARE(request.url(), QUrl(QStringLiteral("http://www.kde.org/api/v1/method.callAnon/login")));
}

void AppsUiInteractionJobTest::shouldGenerateJson()
{
    MethodCallJob job;
    MethodCallJob::MethodCallJobInfo info;
    info.methodName = QStringLiteral("login");
    info.anonymous = true;
    job.setMethodCallJobInfo(info);

    QCOMPARE(job.json().toJson(QJsonDocument::Compact), QStringLiteral(R"({"message":"{}"})").toLatin1());

    QVariantMap map;
    map.insert(QStringLiteral("msg"), QStringLiteral("method"));
    map.insert(QStringLiteral("id"), QStringLiteral("52"));
    map.insert(QStringLiteral("method"), QStringLiteral("login"));
    info.messageObj = QJsonObject::fromVariantMap(map);
    info.methodName = QStringLiteral("login");
    info.anonymous = true;
    job.setMethodCallJobInfo(info);
    QCOMPARE(job.json().toJson(QJsonDocument::Compact),
             QStringLiteral("{\"message\":\"{\\\"id\\\":\\\"52\\\",\\\"method\\\":\\\"login\\\",\\\"msg\\\":\\\"method\\\"}\"}").toLatin1());
}

void AppsUiInteractionJobTest::shouldNotStarting()
{
    MethodCallJob job;

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
    // TODO
}

#include "moc_appsuiinteractionjobtest.cpp"
