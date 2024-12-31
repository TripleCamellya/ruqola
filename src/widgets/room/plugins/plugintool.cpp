/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugintool.h"

PluginTool::PluginTool(QObject *parent)
    : QObject{parent}
{
}

PluginTool::~PluginTool() = default;

#include "moc_plugintool.cpp"
