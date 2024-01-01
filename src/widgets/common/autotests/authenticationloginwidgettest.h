/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class AuthenticationLoginWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit AuthenticationLoginWidgetTest(QObject *parent = nullptr);
    ~AuthenticationLoginWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
