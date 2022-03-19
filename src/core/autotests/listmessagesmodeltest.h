/*
   SPDX-FileCopyrightText: 2019-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ListMessagesModelTest : public QObject
{
    Q_OBJECT
public:
    explicit ListMessagesModelTest(QObject *parent = nullptr);
    ~ListMessagesModelTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
