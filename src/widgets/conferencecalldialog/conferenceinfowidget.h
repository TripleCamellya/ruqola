/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include <QWidget>
class RocketChatAccount;
class LIBRUQOLAWIDGETS_TESTS_EXPORT ConferenceInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConferenceInfoWidget(RocketChatAccount *account, QWidget *parent = nullptr);
    ~ConferenceInfoWidget() override;

    void initializeInfo();

    Q_REQUIRED_RESULT QString conferenceId() const;
    void setConferenceId(const QString &newConferenceId);

private:
    QString mConferenceId;
    RocketChatAccount *const mRocketChatAccount;
};
