/*
   SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "appupdateinfojobtest.h"
#include "apps/appupdateinfojob.h"
#include "ruqola_restapi_helper.h"
QTEST_GUILESS_MAIN(AppUpdateInfoJobTest)
using namespace RocketChatRestApi;
AppUpdateInfoJobTest::AppUpdateInfoJobTest(QObject *parent)
    : QObject(parent)
{
}

void AppUpdateInfoJobTest::shouldHaveDefaultValue()
{
    AppUpdateInfoJob job;
    verifyDefaultValue(&job);
    QVERIFY(job.requireHttpAuthentication());
    QVERIFY(!job.hasQueryParameterSupport());
}

void AppUpdateInfoJobTest::shouldGenerateRequest()
{
    {
        AppUpdateInfoJob job;
        job.setAppMode(AppUpdateInfoJob::AppMode::Delete);
        job.setAppsId(QByteArrayLiteral("foo"));
        QNetworkRequest request = QNetworkRequest(QUrl());
        verifyAuthentication(&job, request);
        QCOMPARE(request.url(), QUrl(QStringLiteral("http://www.kde.org/api/apps/foo")));
    }

    {
        AppUpdateInfoJob job;
        job.setAppMode(AppUpdateInfoJob::AppMode::Post);
        job.setAppInfoType(AppUpdateInfoJob::AppInfoType::Settings);
        job.setAppsId(QByteArrayLiteral("foo"));
        QNetworkRequest request = QNetworkRequest(QUrl());
        verifyAuthentication(&job, request);
        QCOMPARE(request.url(), QUrl(QStringLiteral("http://www.kde.org/api/apps/foo/settings")));
    }
}

#include "moc_appupdateinfojobtest.cpp"
