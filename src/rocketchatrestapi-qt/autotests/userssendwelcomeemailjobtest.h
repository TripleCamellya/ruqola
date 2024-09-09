/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class UsersSendWelcomeEmailJobTest : public QObject
{
    Q_OBJECT
public:
    explicit UsersSendWelcomeEmailJobTest(QObject *parent = nullptr);
    ~UsersSendWelcomeEmailJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldGenerateRequest();
    void shouldGenerateJson();
    void shouldNotStarting();
};
