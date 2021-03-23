/*
   Copyright (c) 2021 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License or
   ( at your option ) version 3 or, at the discretion of KDE e.V.
   ( which shall act as a proxy as in section 14 of the GPLv3 ), any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include "rooms/saveroomsettingsjob.h"

#include <QWidget>

class QCheckBox;
class QPushButton;
class Room;
class ChannelInfoPruneWidget;
class KPasswordLineEdit;
class SystemMessagesComboBox;
class QLineEdit;
class LIBRUQOLAWIDGETS_TESTS_EXPORT ChannelInfoEditableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChannelInfoEditableWidget(QWidget *parent = nullptr);
    ~ChannelInfoEditableWidget() override;
    void setRoom(Room *room);
    Q_REQUIRED_RESULT RocketChatRestApi::SaveRoomSettingsJob::SaveRoomSettingsInfo saveRoomSettingsInfo() const;

    void updateEditableChannelInfo();
    void connectEditableWidget();
    void updateRetentionValue();
    void updateUiFromPermission();
Q_SIGNALS:
    void channelDeleted();
    void fnameChanged(const QString &fname);

private:
    void joinCodeChanged();
    Room *mRoom = nullptr;

    QLineEdit *const mName;
    QLineEdit *const mComment;
    QLineEdit *const mAnnouncement;
    QLineEdit *const mDescription;
    KPasswordLineEdit *const mPasswordLineEdit;
    QCheckBox *const mReadOnly;
    QCheckBox *const mArchive;
    QCheckBox *const mPrivate;
    QCheckBox *const mEncrypted;
    QPushButton *const mDeleteChannel;
    QWidget *mEncryptedLabel = nullptr;
    ChannelInfoPruneWidget *const mChannelInfoPruneWidget;
    SystemMessagesComboBox *const mSystemMessageCombox;
};
