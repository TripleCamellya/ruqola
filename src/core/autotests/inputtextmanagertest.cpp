/*
   Copyright (c) 2018-2021 Laurent Montel <montel@kde.org>

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

#include "inputtextmanagertest.h"
#include "inputtextmanager.h"
#include "model/inputcompletermodel.h"
#include <QSignalSpy>
#include <QTest>
QTEST_GUILESS_MAIN(InputTextManagerTest)

InputTextManagerTest::InputTextManagerTest(QObject *parent)
    : QObject(parent)
{
}

void InputTextManagerTest::shouldHaveDefaultValue()
{
    InputTextManager manager(nullptr, nullptr);
    QVERIFY(manager.inputCompleterModel());
    QCOMPARE(manager.inputCompleterModel()->rowCount(), 0);
    // TODO
}

void InputTextManagerTest::shouldReplaceWord_data()
{
    QTest::addColumn<QString>("newword");
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("position");
    QTest::addColumn<QString>("result");
    QTest::addColumn<int>("expectedPosition");
    QTest::newRow("empty") << QString() << QString() << 5 << QString() << 5;
    QTest::newRow("replace1") << QStringLiteral("bla") << QStringLiteral("foo @d") << 5 << QStringLiteral("foo @bla") << 8;
    QTest::newRow("replace2") << QStringLiteral("bla") << QStringLiteral("foo @daaaa") << 5 << QStringLiteral("foo @bla") << 8;
    QTest::newRow("replace3") << QStringLiteral("bla") << QStringLiteral("@daaaa foo") << 1 << QStringLiteral("@bla foo") << 4;
    QTest::newRow("buildbot") << QStringLiteral("buildbot ") << QStringLiteral("@bu") << 3 << QStringLiteral("@buildbot ") << 10;
    QTest::newRow("trailing_space_already_there") << QStringLiteral("bla ") << QStringLiteral("@daaaa foo") << 1 << QStringLiteral("@bla foo") << 5;
}

void InputTextManagerTest::shouldReplaceWord()
{
    QFETCH(QString, newword);
    QFETCH(QString, text);
    QFETCH(int, position);
    QFETCH(int, expectedPosition);
    QFETCH(QString, result);

    InputTextManager manager(nullptr, nullptr);

    // Widgets
    QCOMPARE(manager.applyCompletion(newword, text, &position), result);
    QCOMPARE(position, expectedPosition);
}

void InputTextManagerTest::shouldSearchWord_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("position");
    QTest::addColumn<QString>("result");
    QTest::newRow("empty") << QString() << 5 << QString();
    QTest::newRow("one_letter") << QStringLiteral("l") << 1 << QStringLiteral("l");
    QTest::newRow("at") << QStringLiteral("@") << 1 << QStringLiteral("@");
    QTest::newRow("at_letter_1") << QStringLiteral("@a") << 1 << QStringLiteral("@a");
    QTest::newRow("at_letter") << QStringLiteral("@a") << 2 << QStringLiteral("@a");
    QTest::newRow("at_word_space") << QStringLiteral("@abc ") << 5 << QString();
}

void InputTextManagerTest::shouldSearchWord()
{
    QFETCH(QString, text);
    QFETCH(int, position);
    QFETCH(QString, result);

    InputTextManager manager(nullptr, nullptr);
    int start; // TODO test the output value
    QCOMPARE(manager.searchWord(text, position, start), result);
}

void InputTextManagerTest::shouldEmitCompletionRequestSignals()
{
    InputTextManager manager(nullptr, nullptr);
    QSignalSpy typeChangedSpy(&manager, &InputTextManager::completionTypeChanged);
    QSignalSpy requestSpy(&manager, &InputTextManager::completionRequested);

    manager.setInputTextChanged(QStringLiteral("a @"), 3);
    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(typeChangedSpy.at(0).at(0).value<InputTextManager::CompletionForType>(), InputTextManager::User);
    QCOMPARE(requestSpy.count(), 0); // We don't send signal when we have only "@" now.
    typeChangedSpy.clear();
    requestSpy.clear();

    manager.setInputTextChanged(QStringLiteral("a :"), 3);
    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(typeChangedSpy.at(0).at(0).value<InputTextManager::CompletionForType>(), InputTextManager::Emoji);
    typeChangedSpy.clear();
    QCOMPARE(requestSpy.count(), 0); // emoji completion doesn't use this signal
    requestSpy.clear();

    manager.setInputTextChanged(QStringLiteral("a "), 2);
    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(typeChangedSpy.at(0).at(0).value<InputTextManager::CompletionForType>(), InputTextManager::None);
    typeChangedSpy.clear();

    manager.setInputTextChanged(QStringLiteral("a #c"), 4);
    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(typeChangedSpy.at(0).at(0).value<InputTextManager::CompletionForType>(), InputTextManager::Channel);
    QCOMPARE(requestSpy.count(), 1);
    QCOMPARE(requestSpy.at(0).at(0).toString(), QStringLiteral("c"));
    typeChangedSpy.clear();
    requestSpy.clear();

    manager.setInputTextChanged(QStringLiteral("hello @foo"), 10);
    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(typeChangedSpy.at(0).at(0).value<InputTextManager::CompletionForType>(), InputTextManager::User);
    QCOMPARE(requestSpy.count(), 1);
    QCOMPARE(requestSpy.at(0).at(0).toString(), QStringLiteral("foo"));
    requestSpy.clear();
    typeChangedSpy.clear();

    manager.setInputTextChanged(QStringLiteral("@foo hello"), 4);
    QCOMPARE(typeChangedSpy.count(), 0); // User again
    QCOMPARE(requestSpy.count(), 1);
    QCOMPARE(requestSpy.at(0).at(0).toString(), QStringLiteral("foo"));
    requestSpy.clear();
    typeChangedSpy.clear();

    manager.setInputTextChanged(QStringLiteral("a :heart:"), 9);
    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(typeChangedSpy.at(0).at(0).value<InputTextManager::CompletionForType>(), InputTextManager::Emoji);
    QCOMPARE(requestSpy.count(), 0);
    typeChangedSpy.clear();

    // If the cursor isn't at the end of the word, don't trigger completion
    manager.setInputTextChanged(QStringLiteral("@foo"), 2);
    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(typeChangedSpy.at(0).at(0).value<InputTextManager::CompletionForType>(), InputTextManager::None);
    QCOMPARE(requestSpy.count(), 0);
    typeChangedSpy.clear();
}
