/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class VideoConferenceInfosTest : public QObject
{
    Q_OBJECT
public:
    explicit VideoConferenceInfosTest(QObject *parent = nullptr);
    ~VideoConferenceInfosTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldLoadDeviceInfos_data();
    void shouldLoadDeviceInfos();
};
