/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ModerationUserReportedMessagesJobTest : public QObject
{
    Q_OBJECT
public:
    explicit ModerationUserReportedMessagesJobTest(QObject *parent = nullptr);
    ~ModerationUserReportedMessagesJobTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldGenerateRequest();
    void shouldNotStarting();
};
