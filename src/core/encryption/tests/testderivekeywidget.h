/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QWidget>

class TestDeriveKeyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestDeriveKeyWidget(QWidget *parent = nullptr);
    ~TestDeriveKeyWidget() override = default;
};
