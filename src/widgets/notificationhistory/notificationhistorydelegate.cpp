/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "notificationhistorydelegate.h"
#include "accountmanager.h"
#include "common/delegatepaintutil.h"
#include "delegateutils/messagedelegateutils.h"
#include "delegateutils/textselectionimpl.h"
#include "model/notificationhistorymodel.h"
#include "rocketchataccount.h"
#include "ruqola.h"
#include "ruqolawidgets_selection_debug.h"
#include "textconverter.h"
#include <QAbstractItemView>
#include <QPainter>
#include <QTextBlock>
#include <QToolTip>

NotificationHistoryDelegate::NotificationHistoryDelegate(QListView *view, QObject *parent)
    : MessageListDelegateBase{view, parent}
{
}

NotificationHistoryDelegate::~NotificationHistoryDelegate()
{
}

void NotificationHistoryDelegate::drawAccountRoomInfo(QPainter *painter, const QModelIndex &index, const QStyleOptionViewItem &option) const
{
    const QPen origPen = painter->pen();
    const qreal margin = MessageDelegateUtils::basicMargin();
    const QString accountName = index.data(NotificationHistoryModel::AccountName).toString();
    const QString roomName = index.data(NotificationHistoryModel::RoomName).toString();
    const QString infoStr = QStringLiteral("%1 - %2").arg(accountName, roomName);
    const QSize infoSize = option.fontMetrics.size(Qt::TextSingleLine, infoStr);
    const QRect infoAreaRect(option.rect.x(), option.rect.y(), option.rect.width(), infoSize.height()); // the whole row
    const QRect infoTextRect = QStyle::alignedRect(Qt::LayoutDirectionAuto, Qt::AlignCenter, infoSize, infoAreaRect);
    painter->drawText(infoTextRect, infoStr);
    const int lineY = (infoAreaRect.top() + infoAreaRect.bottom()) / 2;
    QColor lightColor(painter->pen().color());
    lightColor.setAlpha(60);
    painter->setPen(lightColor);
    painter->drawLine(infoAreaRect.left(), lineY, infoTextRect.left() - margin, lineY);
    painter->drawLine(infoTextRect.right() + margin, lineY, infoAreaRect.right(), lineY);
    painter->setPen(origPen);
}

void NotificationHistoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    drawBackground(painter, option, index);

    const Layout layout = doLayout(option, index);

    drawAccountRoomInfo(painter, index, option);

    // Draw the pixmap
    if (!layout.avatarPixmap.isNull()) {
        painter->drawPixmap(layout.avatarPos, layout.avatarPixmap);
    }

    // Draw the sender
    const QFont oldFont = painter->font();
    painter->setFont(layout.senderFont);
    painter->drawText(layout.senderRect.x(), layout.baseLine, layout.senderText);
    painter->setFont(oldFont);

    // Draw Text
    if (layout.textRect.isValid()) {
        auto *doc = documentForModelIndex(index, layout.textRect.width());
        if (doc) {
            MessageDelegateUtils::drawSelection(doc,
                                                layout.textRect,
                                                layout.textRect.top(),
                                                painter,
                                                index,
                                                option,
                                                mTextSelectionImpl->textSelection(),
                                                {},
                                                false);
        }
    }

    // Timestamp
    DelegatePaintUtil::drawLighterText(painter, layout.timeStampText, layout.timeStampPos);

    // debug (TODO remove it for release)
    // painter->drawRect(option.rect.adjusted(0, 0, -1, -1));

    painter->restore();
}

QSize NotificationHistoryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Note: option.rect in this method is huge (as big as the viewport)
    const Layout layout = doLayout(option, index);

    int additionalHeight = 0;
    // A little bit of margin below the very last item, it just looks better
    if (index.row() == index.model()->rowCount() - 1) {
        additionalHeight += 4;
    }

    // contents is date + text
    const int contentsHeight = layout.textRect.y() + layout.textRect.height() - option.rect.y();
    const int senderAndAvatarHeight = qMax<int>(layout.senderRect.y() + layout.senderRect.height() - option.rect.y(),
                                                layout.avatarPos.y() + MessageDelegateUtils::dprAwareSize(layout.avatarPixmap).height() - option.rect.y());

    //    qDebug() << "senderAndAvatarHeight" << senderAndAvatarHeight << "text" << layout.textRect.height() << "total contents" << contentsHeight;
    //    qDebug() << "=> returning" << qMax(senderAndAvatarHeight, contentsHeight) + additionalHeight;

    return {option.rect.width(), qMax(senderAndAvatarHeight, contentsHeight) + additionalHeight};
}

// text AccountName/room
// [margin] <pixmap> [margin] <sender> [margin] <text message> [margin] <date/time> [margin]
NotificationHistoryDelegate::Layout NotificationHistoryDelegate::doLayout(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    NotificationHistoryDelegate::Layout layout;
    const QString userName = index.data(NotificationHistoryModel::SenderUserName).toString();
    const int margin = MessageDelegateUtils::basicMargin();
    layout.senderText = QLatin1Char('@') + userName;
    layout.senderFont = option.font;
    layout.senderFont.setBold(true);


    // Timestamp
    layout.timeStampText = index.data(NotificationHistoryModel::DateTime).toString();

    // Message (using the rest of the available width)
    const int iconSize = option.widget->style()->pixelMetric(QStyle::PM_ButtonIconSize);
    const QFontMetricsF senderFontMetrics(layout.senderFont);
    const qreal senderAscent = senderFontMetrics.ascent();
    const QSizeF senderTextSize = senderFontMetrics.size(Qt::TextSingleLine, layout.senderText);
    // Resize pixmap TODO cache ?
    const QPixmap pix = index.data(NotificationHistoryModel::Pixmap).value<QPixmap>();
    if (!pix.isNull()) {
        const QPixmap scaledPixmap = pix.scaled(senderTextSize.height(), senderTextSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        layout.avatarPixmap = scaledPixmap;
    }

    const int senderX = option.rect.x() + MessageDelegateUtils::dprAwareSize(layout.avatarPixmap).width() + 2 * margin;

    const int textLeft = senderX + senderTextSize.width() + margin;
    const QSize timeSize = MessageDelegateUtils::timeStampSize(layout.timeStampText, option);
    const int widthAfterMessage = iconSize + margin + timeSize.width() + margin / 2;
    const int maxWidth = qMax(30, option.rect.width() - textLeft - widthAfterMessage);

    layout.baseLine = 0;
    const QSize textSize = textSizeHint(index, maxWidth, option, &layout.baseLine);

    const int textVMargin = 3; // adjust this for "compactness"
    QRect usableRect = option.rect;
    // Add area for account/room info
    usableRect.setTop(usableRect.top() + option.fontMetrics.height());

    layout.textRect = QRect(textLeft, usableRect.top() + textVMargin, maxWidth, textSize.height() + textVMargin);
    layout.baseLine += layout.textRect.top(); // make it absolute

    layout.timeStampPos = QPoint(option.rect.width() - timeSize.width() - margin / 2, layout.baseLine);

    layout.senderRect = QRectF(senderX, layout.baseLine - senderAscent, senderTextSize.width(), senderTextSize.height());
    // Align top of avatar with top of sender rect
    layout.avatarPos = QPointF(option.rect.x() + margin, layout.senderRect.y());

    return layout;
}

QTextDocument *NotificationHistoryDelegate::documentForModelIndex(const QModelIndex &index, int width) const
{
    Q_ASSERT(index.isValid());
    const QString messageId = index.data(NotificationHistoryModel::MessageId).toString();
    Q_ASSERT(!messageId.isEmpty());

    auto it = mDocumentCache.find(messageId);
    if (it != mDocumentCache.end()) {
        auto ret = it->value.get();
        if (width != -1 && !qFuzzyCompare(ret->textWidth(), width)) {
            ret->setTextWidth(width);
        }
        return ret;
    }

    const QString messageStr = index.data(NotificationHistoryModel::MessageStr).toString();

    if (messageStr.isEmpty()) {
        return nullptr;
    }
    auto *rcAccount = rocketChatAccount(index);
    // Use TextConverter in case it starts with a [](URL) reply marker
    QString needUpdateMessageId; // TODO use it ?
    const QString contextString = TextConverter::convertMessageText(messageStr,
                                                                    rcAccount ? rcAccount->userName() : QString(),
                                                                    {},
                                                                    rcAccount ? rcAccount->highlightWords() : QStringList(),
                                                                    rcAccount ? rcAccount->emojiManager() : nullptr,
                                                                    rcAccount ? rcAccount->messageCache() : nullptr,
                                                                    needUpdateMessageId,
                                                                    {},
                                                                    {},
                                                                    mSearchText);
    auto doc = MessageDelegateUtils::createTextDocument(false, contextString, width);
    auto ret = doc.get();
    mDocumentCache.insert(messageId, std::move(doc));
    return ret;
}

bool NotificationHistoryDelegate::helpEvent(QHelpEvent *helpEvent, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (!helpEvent || !view || !index.isValid()) {
        return QItemDelegate::helpEvent(helpEvent, view, option, index);
    }

    if (helpEvent->type() != QEvent::ToolTip) {
        return false;
    }

    const Layout layout = doLayout(option, index);
    const auto *doc = documentForModelIndex(index, layout.textRect.width());
    if (!doc) {
        return false;
    }

    const QPoint relativePos = adaptMousePosition(helpEvent->pos(), layout.textRect, option);
    QString formattedTooltip;
    if (MessageDelegateUtils::generateToolTip(doc, relativePos, formattedTooltip)) {
        QToolTip::showText(helpEvent->globalPos(), formattedTooltip, view);
        return true;
    }
    return true;
}

QPoint NotificationHistoryDelegate::adaptMousePosition(const QPoint &pos, QRect textRect, const QStyleOptionViewItem &option)
{
    Q_UNUSED(option);
    const QPoint relativePos = pos - textRect.topLeft();
    return relativePos;
}

bool NotificationHistoryDelegate::mouseEvent(QEvent *event, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    const QEvent::Type eventType = event->type();
    if (eventType == QEvent::MouseButtonRelease) {
        auto mev = static_cast<QMouseEvent *>(event);
        const Layout layout = doLayout(option, index);
        if (handleMouseEvent(mev, layout.textRect, option, index)) {
            return true;
        }
    } else if (eventType == QEvent::MouseButtonPress || eventType == QEvent::MouseMove || eventType == QEvent::MouseButtonDblClick) {
        auto mev = static_cast<QMouseEvent *>(event);
        if (mev->buttons() & Qt::LeftButton) {
            const Layout layout = doLayout(option, index);
            if (handleMouseEvent(mev, layout.textRect, option, index)) {
                return true;
            }
        }
    }
    return false;
}

bool NotificationHistoryDelegate::maybeStartDrag(QMouseEvent *event, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    const Layout layout = doLayout(option, index);
    if (MessageListDelegateBase::maybeStartDrag(event, layout.textRect, option, index)) {
        return true;
    }
    return false;
}

RocketChatAccount *NotificationHistoryDelegate::rocketChatAccount(const QModelIndex &index) const
{
    const QString accountName = index.data(NotificationHistoryModel::AccountName).toString();
    auto *rcAccount = Ruqola::self()->accountManager()->accountFromName(accountName);
    return rcAccount;
}
