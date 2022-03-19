/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class PermissionsListAllJobTest : public QObject
{
    Q_OBJECT
public:
    explicit PermissionsListAllJobTest(QObject *parent = nullptr);
    ~PermissionsListAllJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldGenerateRequest();
};
