/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class LoadRecentHistoryManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit LoadRecentHistoryManagerTest(QObject *parent = nullptr);
    ~LoadRecentHistoryManagerTest() override = default;
private Q_SLOTS:
    void shouldIncreaseTimer();
};
