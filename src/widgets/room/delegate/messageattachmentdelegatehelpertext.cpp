/*
   Copyright (c) 2020 Laurent Montel <montel@kde.org>

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

#include "messageattachmentdelegatehelpertext.h"
#include "messagedelegateutils.h"
#include "ruqolawidgets_debug.h"
#include "ruqola.h"
#include "rocketchataccount.h"
#include "dialogs/showvideodialog.h"
#include "common/delegatepaintutil.h"
#include "common/delegateutil.h"
#include "textconverter.h"

#include <KLocalizedString>

#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QTextBlock>
#include <QStyleOptionViewItem>

MessageAttachmentDelegateHelperText::~MessageAttachmentDelegateHelperText()
{
}

void MessageAttachmentDelegateHelperText::draw(const MessageAttachment &msgAttach, QPainter *painter, QRect messageRect, const QModelIndex &index, const QStyleOptionViewItem &option) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);

    //painter->drawRect(messageRect);

    const TextLayout layout = layoutText(msgAttach, option, messageRect.width(), messageRect.height());
    int nextY = messageRect.y();
    if (!layout.title.isEmpty()) {
        painter->drawText(messageRect.x(), messageRect.y() + option.fontMetrics.ascent(), layout.title);
        const QIcon hideShowIcon = QIcon::fromTheme(layout.isShown ? QStringLiteral("visibility") : QStringLiteral("hint"));
        hideShowIcon.paint(painter, layout.hideShowButtonRect.translated(messageRect.topLeft()));
        nextY += layout.titleSize.height();
    }
    if (layout.isShown) {
        auto *doc = documentForIndex(msgAttach, messageRect.width());
        if (!doc) {
            return;
        }
    #if 0
        QVector<QAbstractTextDocumentLayout::Selection> selections;
        if (index == mCurrentIndex) {
            QTextCharFormat selectionFormat;
            selectionFormat.setBackground(option.palette.brush(QPalette::Highlight));
            selectionFormat.setForeground(option.palette.brush(QPalette::HighlightedText));
            selections.append({mCurrentTextCursor, selectionFormat});
        }
        if (useItalicsForMessage(index)) {
            QTextCursor cursor(doc);
            cursor.select(QTextCursor::Document);
            QTextCharFormat format;
            format.setForeground(Qt::gray); //TODO use color from theme.
            cursor.mergeCharFormat(format);
        }
    #endif

        painter->save();
        painter->translate(messageRect.left(), nextY);
        const QRect clip(0, 0, messageRect.width(), messageRect.height());

        // Same as pDoc->drawContents(painter, clip) but we also set selections
        QAbstractTextDocumentLayout::PaintContext ctx;
        //FIXME ctx.selections = selections;
        if (clip.isValid()) {
            painter->setClipRect(clip);
            ctx.clip = clip;
        }
        doc->documentLayout()->draw(painter, ctx);
        painter->restore();
        //TODO add fields
    }

}

QSize MessageAttachmentDelegateHelperText::sizeHint(const MessageAttachment &msgAttach, const QModelIndex &index, int maxWidth, const QStyleOptionViewItem &option) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    const TextLayout layout = layoutText(msgAttach, option, maxWidth, -1);
    int height = layout.titleSize.height() + DelegatePaintUtil::margin();
    if (layout.isShown && !layout.title.isEmpty()) {
        height += layout.textSize.height();
    }
    return QSize(qMax(layout.titleSize.width(), maxWidth),
                 height);
}

bool MessageAttachmentDelegateHelperText::handleMouseEvent(const MessageAttachment &msgAttach, QMouseEvent *mouseEvent, QRect attachmentsRect, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(attachmentsRect);
    Q_UNUSED(option);
    Q_UNUSED(index);
    Q_UNUSED(msgAttach);
    if (mouseEvent->type() == QEvent::MouseButtonRelease) {
        //TODO ???
    }
    //Implement click on url
    return false;
}

MessageAttachmentDelegateHelperText::TextLayout MessageAttachmentDelegateHelperText::layoutText(const MessageAttachment &msgAttach, const QStyleOptionViewItem &option, int attachmentsWidth, int attachmentsHeight) const
{
    TextLayout layout;
    layout.title = msgAttach.title();
    if (!layout.title.isEmpty()) {
        layout.titleSize = layout.title.isEmpty() ? QSize() : option.fontMetrics.size(Qt::TextSingleLine, layout.title);
        const int iconSize = option.widget->style()->pixelMetric(QStyle::PM_ButtonIconSize);
        layout.hideShowButtonRect = QRect(layout.titleSize.width() + DelegatePaintUtil::margin(), 0, iconSize, iconSize);
    }
    auto *doc = documentForIndex(msgAttach, attachmentsWidth);
    layout.textSize = doc ? QSize(doc->idealWidth(), doc->size().height()) : QSize();
    return layout;
}

//Draw text + title as in image
QTextDocument *MessageAttachmentDelegateHelperText::documentForIndex(const MessageAttachment &msgAttach, int width) const
{
    const QString attachmentId = msgAttach.attachementId();
    auto it = mDocumentCache.find(attachmentId);
    if (it != mDocumentCache.end()) {
        auto ret = it->value.get();
        if (ret->textWidth() != width) {
            ret->setTextWidth(width);
        }
        return ret;
    }

    const QString text = msgAttach.text();

    if (text.isEmpty()) {
        return nullptr;
    }
    // Use TextConverter in case it starts with a [](URL) reply marker
    auto *rcAccount = Ruqola::self()->rocketChatAccount();
    TextConverter textConverter(rcAccount->emojiManager());
    const QString contextString = textConverter.convertMessageText(text, rcAccount->userName(), {});
    auto doc = MessageDelegateUtils::createTextDocument(false, contextString, width);
    auto ret = doc.get();
    mDocumentCache.insert(attachmentId, std::move(doc));
    return ret;
}
