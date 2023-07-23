/*
   SPDX-FileCopyrightText: 2020-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "channels/channelhistoryjob.h"
#include "libruqolawidgets_private_export.h"
#include "messages/message.h"
#include "room.h"
#include "roomheaderwidget.h"
#include <QPointer>
#include <QWidget>

class RoomHeaderWidget;
class RocketChatAccount;
class UsersInRoomFlowWidget;
class RoomCounterInfoWidget;
class Room;
class RoomWidgetBase;
class ReconnectInfoWidget;
class OtrWidget;
class OffLineWidget;

namespace TextEditTextToSpeech
{
class TextToSpeechContainerWidget;
}

class LIBRUQOLAWIDGETS_TESTS_EXPORT RoomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RoomWidget(QWidget *parent = nullptr);
    ~RoomWidget() override;

    Q_REQUIRED_RESULT QString roomId() const;
    void setRoomId(const QString &roomId);

    void setCurrentRocketChatAccount(RocketChatAccount *account);
    Q_REQUIRED_RESULT Room::RoomType roomType() const;

    Room *room() const;

    void slotShowListOfUsersInRoom(bool checked);
    void setChannelSelected(const QString &roomId, Room::RoomType roomType);
    void storeRoomSettings();

    void setLayoutSpacing(int spacing);
    void scrollToMessageId(const QString &messageId);

    void forceLineEditFocus();

Q_SIGNALS:
    void selectChannelRequested(const QString &channelId);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void slotClearNotification();
    void updateRoomHeader();
    void connectRoom();
    void slotChangeFavorite(bool b);
    void setRoomType(Room::RoomType roomType);
    void slotEncryptedChanged(bool b);
    void slotGoBackToRoom();
    void slotCreateNewDiscussion(const QString &messageId, const QString &originalMessage);
    void slotCreatePrivateDiscussion(const QString &userName);
    void slotSearchMessages();
    void slotActionRequested(RoomHeaderWidget::ChannelActionType type);
    void slotShowDiscussions();
    void slotShowThreads();
    void slotShowFileAttachments();
    void slotStarredMessages();
    void slotPinnedMessages();
    void slotShowMentions();
    void slotConfigureNotification();
    void slotLoadHistory();
    void slotOpenThreadRequested(const QString &threadMessageId, const QString &threadMessagePreview, bool threadIsFollowing, const Message &threadMessage);
    void slotConfigureAutoTranslate();
    void slotChannelInfoRequested();
    void slotInviteUsers();
    void slotAddUsersInRoom();
    void updateListView();
    void slotVideoChat();
    void slotPruneMessages();
    void slotUpdateRoomCounterInfoWidget();
    void slotExportMessages();
    void clearBeforeSwitching();
    void slotTeamChannelsRequested();
    void slotJumpToUnreadMessage(qint64 numberOfMessage);
    void slotGotoMessage(const QString &messageId, const QString &messageDateTimeUtc);
    void slotOpenTeamRequested(const QString &teamId);
    void slotTryReconnect();
    void slotDisplayReconnectWidget(int seconds);
    void slotLoginStatusChanged();
    void slotCloseOtr();
    void slotRefreshOtrKeys();
    void slotCallRequested();
    void slotTextToSpeech(const QString &messageText);

    Room::RoomType mRoomType = Room::RoomType::Unknown;

    RoomWidgetBase *const mRoomWidgetBase;

    RoomHeaderWidget *const mRoomHeaderWidget;
    QPointer<Room> mRoom;
    UsersInRoomFlowWidget *const mUsersInRoomFlowWidget;
    RoomCounterInfoWidget *const mRoomCounterInfoWidget;
    ReconnectInfoWidget *const mRoomReconnectInfoWidget;
    OtrWidget *const mOtrWidget;
    OffLineWidget *const mOffLineWidget;
    TextEditTextToSpeech::TextToSpeechContainerWidget *const mTextToSpeechWidget = nullptr;
    QPointer<RocketChatAccount> mCurrentRocketChatAccount;
};
