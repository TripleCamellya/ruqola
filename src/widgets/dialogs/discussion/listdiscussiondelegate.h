/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include "lrucache.h"
#include <QItemDelegate>
#include <QTextDocument>
class LIBRUQOLAWIDGETS_TESTS_EXPORT ListDiscussionDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ListDiscussionDelegate(QObject *parent = nullptr);
    ~ListDiscussionDelegate() override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    Q_REQUIRED_RESULT bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    Q_REQUIRED_RESULT QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

Q_SIGNALS:
    void openDiscussion(const QString &discussionRoomId);

private:
    struct Layout {
        // Attachment name
        QString text;
        qreal textY = 0;

        // TimeStamp
        QString lastMessageTimeText;
        qreal lastMessageTimeY = 0;

        // Number of message
        int numberOfMessages = 0;

        // OpenDiscussionText
        QString openDiscussionText;
        qreal openDiscussionTextY = 0;
        //
        QRect usableRect;
    };
    Q_REQUIRED_RESULT ListDiscussionDelegate::Layout doLayout(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    mutable LRUCache<QString, std::unique_ptr<QTextDocument>, 32> mDocumentCache;
    Q_REQUIRED_RESULT QTextDocument *documentForIndex(const QModelIndex &index, int width) const;
    Q_REQUIRED_RESULT QSize textSizeHint(const QModelIndex &index, int maxWidth, const QStyleOptionViewItem &option, qreal *pBaseLine) const;
};
