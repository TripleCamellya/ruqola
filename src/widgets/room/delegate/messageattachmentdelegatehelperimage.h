/*
   SPDX-FileCopyrightText: 2020 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagedelegatehelperbase.h"
#include "misc/pixmapcache.h"
#include "runninganimatedimage.h"

#include <QModelIndex>
#include <QPixmap>
#include <vector>

class LIBRUQOLAWIDGETS_TESTS_EXPORT MessageAttachmentDelegateHelperImage : public MessageDelegateHelperBase
{
public:
    MessageAttachmentDelegateHelperImage() = default;
    ~MessageAttachmentDelegateHelperImage() override = default;
    MessageAttachmentDelegateHelperImage(const MessageAttachmentDelegateHelperImage &) = delete;
    MessageAttachmentDelegateHelperImage &operator=(const MessageAttachmentDelegateHelperImage &) = delete;
    void
    draw(const MessageAttachment &msgAttach, QPainter *painter, QRect messageRect, const QModelIndex &index, const QStyleOptionViewItem &option) const override;
    QSize sizeHint(const MessageAttachment &msgAttach, const QModelIndex &index, int maxWidth, const QStyleOptionViewItem &option) const override;
    bool handleMouseEvent(const MessageAttachment &msgAttach,
                          QMouseEvent *mouseEvent,
                          QRect attachmentsRect,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) override;

private:
    friend class MessageDelegateHelperImageTest;

    struct ImageLayout {
        QPixmap pixmap;
        QString imagePreviewPath;
        QString imageBigPath;
        QString title;
        QString description;
        QSize titleSize;
        QSize imageSize;
        QSize descriptionSize;
        QRect hideShowButtonRect;
        QRect downloadButtonRect;
        bool isShown = true;
        bool isAnimatedImage = false;
    };
    Q_REQUIRED_RESULT ImageLayout layoutImage(const MessageAttachment &msgAttach,
                                              const QStyleOptionViewItem &option,
                                              int attachmentsWidth,
                                              int attachmentsHeight) const;

    std::vector<RunningAnimatedImage>::iterator findRunningAnimatedImage(const QModelIndex &index) const;
    void removeRunningAnimatedImage(const QModelIndex &index) const;

private:
    mutable PixmapCache mPixmapCache;
    mutable std::vector<RunningAnimatedImage> mRunningAnimatedImages; // not a hash or map, since QPersistentModelIndex changes value
};
