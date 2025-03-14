/*
   SPDX-FileCopyrightText: 2023-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
class AuthenticationOauthWidget;
class AuthenticationOauthTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AuthenticationOauthTestWidget(QWidget *parent = nullptr);
    ~AuthenticationOauthTestWidget() override = default;

private:
    AuthenticationOauthWidget *const mAuthenticationOauthWidget;
};
