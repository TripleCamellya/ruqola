/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "activitiesmanagertest.h"
#include "activities/activitiesmanager.h"
#include <QTest>
QTEST_GUILESS_MAIN(ActivitiesManagerTest)
ActivitiesManagerTest::ActivitiesManagerTest(QObject *parent)
    : QObject{parent}
{
}

void ActivitiesManagerTest::shouldHaveDefaultValues()
{
    ActivitiesManager w;
    QVERIFY(!w.enabled());
}
