/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagelinewidgettest.h"
#include "room/messagelinewidget.h"
#include "room/messagetextedit.h"
#include <QHBoxLayout>
#include <QTest>
#include <QToolButton>
QTEST_MAIN(MessageLineWidgetTest)

MessageLineWidgetTest::MessageLineWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void MessageLineWidgetTest::shouldHaveDefaultValues()
{
    MessageLineWidget w;
    QCOMPARE(w.mode(), MessageLineWidget::EditingMode::NewMessage);
    QVERIFY(w.threadMessageId().isEmpty());

    auto mainLayout = w.findChild<QHBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins{});
    QCOMPARE(mainLayout->spacing(), 0);

    auto mMessageTextEdit = w.findChild<MessageTextEdit *>(QStringLiteral("mMessageTextEdit"));
    QVERIFY(mMessageTextEdit);

    auto mSendFile = w.findChild<QToolButton *>(QStringLiteral("mSendFile"));
    QVERIFY(mSendFile);
    QVERIFY(!mSendFile->icon().isNull());
    QVERIFY(mSendFile->autoRaise());

    auto mEmoticonButton = w.findChild<QToolButton *>(QStringLiteral("mEmoticonButton"));
    QVERIFY(mEmoticonButton);
    QVERIFY(!mEmoticonButton->icon().isNull());
    QCOMPARE(mEmoticonButton->popupMode(), QToolButton::InstantPopup);
    QVERIFY(mEmoticonButton->autoRaise());

    auto mSendMessageButton = w.findChild<QToolButton *>(QStringLiteral("mSendMessageButton"));
    QVERIFY(mSendMessageButton);
    QVERIFY(!mSendMessageButton->icon().isNull());
    QVERIFY(mSendMessageButton->autoRaise());

    QVERIFY(w.roomId().isEmpty());
    QVERIFY(w.messageIdBeingEdited().isEmpty());
}

// TODO test signal/slot
