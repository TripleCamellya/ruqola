/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class AdministratorModerationReportedMessageConsoleTreeWidget : public QObject
{
    Q_OBJECT
public:
    explicit AdministratorModerationReportedMessageConsoleTreeWidget(QObject *parent = nullptr);
    ~AdministratorModerationReportedMessageConsoleTreeWidget() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
};
