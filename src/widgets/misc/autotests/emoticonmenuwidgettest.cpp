/*
   Copyright (c) 2020-2021 Laurent Montel <montel@kde.org>

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

#include "emoticonmenuwidgettest.h"
#include "misc/emoticonmenuwidget.h"
#include "misc/emoticonrecentusedfilterproxymodel.h"
#include "misc/recentusedemoticonview.h"
#include "model/emoticoncustommodelfilterproxymodel.h"
#include "model/emoticonmodelfilterproxymodel.h"
#include <QLineEdit>
#include <QTabWidget>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(EmoticonMenuWidgetTest)
EmoticonMenuWidgetTest::EmoticonMenuWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void EmoticonMenuWidgetTest::shouldHaveDefaultValues()
{
    EmoticonMenuWidget w;
    auto layout = w.findChild<QVBoxLayout *>(QStringLiteral("layout"));
    QVERIFY(layout);
    QCOMPARE(layout->contentsMargins(), {});

    auto mTabWidget = w.findChild<QTabWidget *>(QStringLiteral("mTabWidget"));
    QVERIFY(mTabWidget);

    auto mSearchLineEdit = w.findChild<QLineEdit *>(QStringLiteral("mSearchLineEdit"));
    QVERIFY(mSearchLineEdit);
    QVERIFY(mSearchLineEdit->isClearButtonEnabled());
    QVERIFY(!mSearchLineEdit->placeholderText().isEmpty());

    auto mRecentUsedFilterProxyModel = w.findChild<EmoticonRecentUsedFilterProxyModel *>(QStringLiteral("mRecentUsedFilterProxyModel"));
    QVERIFY(mRecentUsedFilterProxyModel);
    auto mEmoticonFilterProxyModel = w.findChild<EmoticonModelFilterProxyModel *>(QStringLiteral("mEmoticonFilterProxyModel"));
    QVERIFY(mEmoticonFilterProxyModel);
    auto mEmoticonCustomFilterProxyModel = w.findChild<EmoticonCustomModelFilterProxyModel *>(QStringLiteral("mEmoticonCustomFilterProxyModel"));
    QVERIFY(mEmoticonCustomFilterProxyModel);

    auto mSearchEmojisView = w.findChild<QListView *>(QStringLiteral("mSearchEmojisView"));
    QVERIFY(mSearchEmojisView);
    auto mRecentUsedEmoticonView = w.findChild<RecentUsedEmoticonView *>(QStringLiteral("mRecentUsedEmoticonView"));
    QVERIFY(mRecentUsedEmoticonView);
    auto mCustomEmojiView = w.findChild<QListView *>(QStringLiteral("mCustomEmojiView"));
    QVERIFY(mCustomEmojiView);
}
