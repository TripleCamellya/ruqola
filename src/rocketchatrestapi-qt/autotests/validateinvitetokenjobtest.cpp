/*
   SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "validateinvitetokenjobtest.h"
#include "invite/sendinvitationemailjob.h"
#include "ruqola_restapi_helper.h"
#include <QJsonDocument>
QTEST_GUILESS_MAIN(ValidateInviteTokenJobTest)
using namespace RocketChatRestApi;
ValidateInviteTokenJobTest::ValidateInviteTokenJobTest(QObject *parent)
    : QObject(parent)
{
}

void ValidateInviteTokenJobTest::shouldHaveDefaultValue()
{
    SendInvitationEmailJob job;
    verifyDefaultValue(&job);
    QVERIFY(job.requireHttpAuthentication());
    QVERIFY(!job.hasQueryParameterSupport());
    QVERIFY(!job.requireTwoFactorAuthentication());
    QVERIFY(job.emails().isEmpty());
}

void ValidateInviteTokenJobTest::shouldGenerateRequest()
{
    SendInvitationEmailJob job;
    QNetworkRequest request = QNetworkRequest(QUrl());
    verifyAuthentication(&job, request);
    QCOMPARE(request.url(), QUrl(QStringLiteral("http://www.kde.org/api/v1/sendInvitationEmail")));
    QCOMPARE(request.header(QNetworkRequest::ContentTypeHeader).toString(), QStringLiteral("application/json"));
}

void ValidateInviteTokenJobTest::shouldGenerateJson()
{
    SendInvitationEmailJob job;
    QStringList emails;
    emails.append(QStringLiteral("bla@kde.org"));
    emails.append(QStringLiteral("bli@kde.org"));
    job.setEmails(emails);
    QCOMPARE(job.json().toJson(QJsonDocument::Compact), QStringLiteral(R"({"emails":["bla@kde.org","bli@kde.org"]})").toLatin1());
}

void ValidateInviteTokenJobTest::shouldNotStarting()
{
    SendInvitationEmailJob job;

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
    const QStringList emails{QStringLiteral("foo"), QStringLiteral("bla")};
    job.setEmails(emails);
    QVERIFY(job.canStart());
}

#include "moc_validateinvitetokenjobtest.cpp"
