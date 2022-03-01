/*
   SPDX-FileCopyrightText: 2021-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include "misc/searchwithdelaylineedit.h"

#include <QJsonDocument>
class RocketChatAccount;
class LIBRUQOLAWIDGETS_TESTS_EXPORT ChannelNameValidLineEdit : public SearchWithDelayLineEdit
{
    Q_OBJECT
public:
    enum ChannelNameStatus {
        Unknown = 0,
        Valid = 1,
        AlreadyExistingName = 2,
        InvalidCharacters = 3,
    };
    explicit ChannelNameValidLineEdit(RocketChatAccount *account, QWidget *parent = nullptr);
    ~ChannelNameValidLineEdit() override;
Q_SIGNALS:
    void channelIsValid(ChannelNameValidLineEdit::ChannelNameStatus status);

private:
    void updateStyleSheet(bool state);
    void slotSearchDone(quint64 id, const QJsonDocument &result);
    void slotSearchChannelRequested(const QString &str);
    void clearLineEdit();
    void emitIsValid(bool state);
    QString mNegativeBackground;
    quint64 mDdpIdentifier = 0;
    RocketChatAccount *const mRocketChatAccount;
};
