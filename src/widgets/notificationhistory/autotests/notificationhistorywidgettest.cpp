/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "notificationhistorywidgettest.h"
#include "notificationhistory/notificationhistorywidget.h"
#include <QListView>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(NotificationHistoryWidgetTest)

NotificationHistoryWidgetTest::NotificationHistoryWidgetTest(QObject *parent)
    : QObject{parent}
{
}

void NotificationHistoryWidgetTest::shouldHaveDefaultValues()
{
    NotificationHistoryWidget w;

    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins{});

    auto mListNotifications = w.findChild<QListView *>(QStringLiteral("mListNotifications"));
    QVERIFY(mListNotifications);
}
