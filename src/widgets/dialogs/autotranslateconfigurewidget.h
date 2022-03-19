/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>

#include "libruqolawidgets_private_export.h"
class QCheckBox;
class QComboBox;
class Room;
class RocketChatAccount;
class LIBRUQOLAWIDGETS_TESTS_EXPORT AutoTranslateConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AutoTranslateConfigureWidget(RocketChatAccount *account, QWidget *parent = nullptr);
    ~AutoTranslateConfigureWidget() override;

    Q_REQUIRED_RESULT Room *room() const;
    void setRoom(Room *room);

private:
    void slotAutoTranslateChanged();
    void slotChangeAutoTranslate(bool status);
    void slotLanguageChanged(int index);
    void slotAutoTranslateLanguageChanged();
    QCheckBox *const mAutoTranslate;
    QComboBox *const mLanguage;
    Room *mRoom = nullptr;
    RocketChatAccount *const mRocketChatAccount;
};
