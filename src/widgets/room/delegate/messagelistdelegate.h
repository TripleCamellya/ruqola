/*
   SPDX-FileCopyrightText: 2020 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include "messagelistlayout/messagelistlayoutbase.h"
#include "room.h"
#include "utils.h"

#include "misc/pixmapcache.h"

#include <QItemDelegate>
#include <QScopedPointer>

class QListView;
class RocketChatAccount;
class Message;
class MessageDelegateHelperBase;
class MessageDelegateHelperText;
class MessageAttachmentDelegateHelperImage;
class MessageAttachmentDelegateHelperFile;
class MessageDelegateHelperReactions;
class MessageAttachmentDelegateHelperVideo;
class MessageAttachmentDelegateHelperSound;
class MessageAttachmentDelegateHelperText;
class MessageAttachment;
class AvatarCacheManager;
class TextSelectionImpl;
class MessageListLayoutBase;

class LIBRUQOLAWIDGETS_TESTS_EXPORT MessageListDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    struct MenuInfo {
        Room::RoomType roomType = Room::RoomType::Unknown;
        QPoint globalPos;
        QPoint pos;
        bool editMode = false;
    };

    explicit MessageListDelegate(RocketChatAccount *account, QListView *view);
    ~MessageListDelegate() override;

    void setRocketChatAccount(RocketChatAccount *rcAccount);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool mouseEvent(QEvent *event, const QStyleOptionViewItem &option, const QModelIndex &index);
    bool maybeStartDrag(QMouseEvent *event, const QStyleOptionViewItem &option, const QModelIndex &index);

    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    void drawDate(QPainter *painter, const QModelIndex &index, const QStyleOptionViewItem &option, bool drawLastSeenLine) const;

    void setShowThreadContext(bool b);

    Q_REQUIRED_RESULT bool hasSelection() const;
    Q_REQUIRED_RESULT QString selectedText() const;

    void selectAll(const QStyleOptionViewItem &option, const QModelIndex &index);
    void clearTextDocumentCache();

    void clearSelection();

    Q_REQUIRED_RESULT QString urlAt(const QStyleOptionViewItem &option, const QModelIndex &index, QPoint pos) const;

    Q_REQUIRED_RESULT bool contextMenu(const QStyleOptionViewItem &option, const QModelIndex &index, const MessageListDelegate::MenuInfo &info);

    Q_REQUIRED_RESULT const QString &searchText() const;
    void setSearchText(const QString &newSearchText);

    Q_REQUIRED_RESULT QPixmap makeAvatarPixmap(const QWidget *widget, const QModelIndex &index, int maxHeight) const;

    /// @note Ownership is not transferred
    Q_REQUIRED_RESULT MessageDelegateHelperBase *attachmentsHelper(const MessageAttachment &msgAttach) const;

    Q_REQUIRED_RESULT MessageDelegateHelperText *helperText() const;

    Q_REQUIRED_RESULT MessageDelegateHelperReactions *helperReactions() const;

    void removeMessageCache(const QString &messageId, const QStringList &attachmentIdList);

Q_SIGNALS:
    void showUserInfo(const QString &userName);
    void startPrivateConversation(const QString &userName);
    void updateView();

private:
    void slotUpdateColors();
    void switchMessageLayout();

    Q_REQUIRED_RESULT MessageListLayoutBase::Layout doLayout(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawLastSeenLine(QPainter *painter, qint64 displayLastSeenY, const QStyleOptionViewItem &option) const;
    Q_REQUIRED_RESULT bool isSystemMessage(const Message *message) const;

    friend class MessageListDelegateTest;

    QString mSearchText;
    const QIcon mEditedIcon;
    const QIcon mRolesIcon;
    const QIcon mAddReactionIcon;
    const QIcon mFavoriteIcon;
    const QIcon mFollowingIcon;
    const QIcon mPinIcon;
    const QIcon mTranslatedIcon;
    QColor mEditColorMode;
    QColor mThreadedMessageBackgroundColor;
    QColor mOpenDiscussionColorMode;
    QColor mReplyThreadColorMode;
    RocketChatAccount *mRocketChatAccount = nullptr;
    QListView *const mListView;
    TextSelectionImpl *const mTextSelectionImpl;

    QScopedPointer<MessageDelegateHelperText> mHelperText;
    QScopedPointer<MessageAttachmentDelegateHelperImage> mHelperAttachmentImage;
    QScopedPointer<MessageAttachmentDelegateHelperFile> mHelperAttachmentFile;
    QScopedPointer<MessageDelegateHelperReactions> mHelperReactions;
    QScopedPointer<MessageAttachmentDelegateHelperVideo> mHelperAttachmentVideo;
    QScopedPointer<MessageAttachmentDelegateHelperSound> mHelperAttachmentSound;
    QScopedPointer<MessageAttachmentDelegateHelperText> mHelperAttachmentText;
    AvatarCacheManager *const mAvatarCacheManager;
    MessageListLayoutBase *mMessageListLayoutBase = nullptr;
};
