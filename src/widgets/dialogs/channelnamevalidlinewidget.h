/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include <QWidget>
class ChannelNameValidLineEdit;
class RocketChatAccount;
class QLabel;
class LIBRUQOLAWIDGETS_TESTS_EXPORT ChannelNameValidLineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChannelNameValidLineWidget(RocketChatAccount *account, QWidget *parent = nullptr);
    ~ChannelNameValidLineWidget() override;

    Q_REQUIRED_RESULT QString text() const;
Q_SIGNALS:
    void channelIsValid(bool valid);

private:
    void slotChannelIsValid(bool isValid);
    ChannelNameValidLineEdit *const mChannelNameValidLineEdit;
    QLabel *const mChannelNameLabel;
};
