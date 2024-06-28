/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "libruqolawidgets_private_export.h"
#include <QWidget>

class LIBRUQOLAWIDGETS_TESTS_EXPORT ApplicationsSettingsInProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ApplicationsSettingsInProgressWidget(QWidget *parent = nullptr);
    ~ApplicationsSettingsInProgressWidget() override;
};
