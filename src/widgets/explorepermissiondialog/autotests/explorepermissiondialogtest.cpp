/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "explorepermissiondialogtest.h"
#include "explorepermissiondialog/explorepermissiondialog.h"
#include "explorepermissiondialog/explorepermissionwidget.h"
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QTest>
#include <QVBoxLayout>

QTEST_MAIN(ExplorePermissionDialogTest)
ExplorePermissionDialogTest::ExplorePermissionDialogTest(QObject *parent)
    : QObject{parent}
{
    QStandardPaths::setTestModeEnabled(true);
}

void ExplorePermissionDialogTest::shouldHaveDefaultValues()
{
    ExplorePermissionDialog d;
    QVERIFY(!d.windowTitle().isEmpty());

    auto mainLayout = d.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);

    auto mExporePermissionWidget = d.findChild<ExplorePermissionWidget *>(QStringLiteral("mExporePermissionWidget"));
    QVERIFY(mExporePermissionWidget);

    auto button = d.findChild<QDialogButtonBox *>(QStringLiteral("button"));
    QVERIFY(button);
}
