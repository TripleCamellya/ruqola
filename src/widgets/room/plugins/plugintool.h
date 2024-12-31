/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "libruqolawidgets_export.h"
#include <QObject>

class LIBRUQOLAWIDGETS_EXPORT PluginTool : public QObject
{
    Q_OBJECT
public:
    explicit PluginTool(QObject *parent = nullptr);
    ~PluginTool() override;

Q_SIGNALS:
    void activated();
};
