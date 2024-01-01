/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ChannelCounterInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit ChannelCounterInfoTest(QObject *parent = nullptr);
    ~ChannelCounterInfoTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldLoadChannelCounter_data();
    void shouldLoadChannelCounter();
};
