/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DirectChannelInfoWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit DirectChannelInfoWidgetTest(QObject *parent = nullptr);
    ~DirectChannelInfoWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
