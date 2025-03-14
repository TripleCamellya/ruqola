/*
   SPDX-FileCopyrightText: 2021 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include "messages/block/block.h"
#include "messages/messageattachment.h"
#include "messages/messageurl.h"

#include <QObject>
#include <QPersistentModelIndex>

class QTextCursor;
class QTextDocument;
class Message;

class LIBRUQOLAWIDGETS_TESTS_EXPORT DocumentFactoryInterface
{
public:
    virtual ~DocumentFactoryInterface();
    /**
     * Creates (or retrieves from a cache) the QTextDocument for a given @p index.
     * @param width The width for layouting that QTextDocument. -1 if no layouting is desired (e.g. for converting to text or HTML)
     * @return the QTextDocument. Ownership remains with the cache, don't delete it.
     */
    [[nodiscard]] virtual QTextDocument *documentForIndex(const QModelIndex &index) const
    {
        Q_UNUSED(index);
        Q_ASSERT(false);
        return nullptr;
    }
    [[nodiscard]] virtual QTextDocument *documentForAttachement(const MessageAttachment &msgAttach) const
    {
        Q_UNUSED(msgAttach);
        Q_ASSERT(false);
        return nullptr;
    }
    [[nodiscard]] virtual QTextDocument *documentForBlock(const Block &block) const
    {
        Q_UNUSED(block);
        Q_ASSERT(false);
        return nullptr;
    }
    [[nodiscard]] virtual QTextDocument *documentForUrlPreview(const MessageUrl &messageUrl) const
    {
        Q_UNUSED(messageUrl);
        Q_ASSERT(false);
        return nullptr;
    }
};

class LIBRUQOLAWIDGETS_TESTS_EXPORT TextSelection : public QObject
{
    Q_OBJECT
public:
    TextSelection();
    [[nodiscard]] bool hasSelection() const;
    enum class Format : uint8_t {
        Text,
        Html,
    };
    [[nodiscard]] QString selectedText(Format format) const;
    [[nodiscard]] bool contains(const QModelIndex &index, int charPos, const MessageAttachment &att = {}) const;
    [[nodiscard]] QTextCursor
    selectionForIndex(const QModelIndex &index, QTextDocument *doc, const MessageAttachment &att = {}, const MessageUrl &msgUrl = {}) const;

    void clear();
    void setTextSelectionStart(const QModelIndex &index, int charPos);
    void setTextSelectionEnd(const QModelIndex &index, int charPos);
    void setAttachmentTextSelectionEnd(const QModelIndex &index, int charPos, const MessageAttachment &msgAttach);
    void selectWordUnderCursor(const QModelIndex &index, int charPos, DocumentFactoryInterface *factory);
    void selectWordUnderCursor(const QModelIndex &index, int charPos, DocumentFactoryInterface *factory, const MessageAttachment &msgAttach);
    void selectWordUnderCursor(const QModelIndex &index, int charPos, DocumentFactoryInterface *factory, const MessageUrl &msgUrl);
    void selectMessage(const QModelIndex &index);

    void setTextHelperFactory(DocumentFactoryInterface *newTextHelperFactory);
    [[nodiscard]] DocumentFactoryInterface *textHelperFactory() const;

    void setAttachmentFactories(const QList<DocumentFactoryInterface *> &newAttachmentFactories);

    [[nodiscard]] const QList<DocumentFactoryInterface *> &attachmentFactories() const;

    [[nodiscard]] DocumentFactoryInterface *messageUrlHelperFactory() const;
    void setMessageUrlHelperFactory(DocumentFactoryInterface *newMessageUrlHelperFactory);

    void setPreviewUrlTextSelectionEnd(const QModelIndex &index, int charPos, const MessageUrl &msgUrl);

    void setAttachmentTextSelectionStart(const QModelIndex &index, int charPos, const MessageAttachment &msgAttach);

    void setPreviewUrlTextSelectionStart(const QModelIndex &index, int charPos, const MessageUrl &msgUrl);
Q_SIGNALS:
    void repaintNeeded(const QModelIndex &index);

private:
    LIBRUQOLAWIDGETS_NO_EXPORT void selectWord(const QModelIndex &index, int charPos, QTextDocument *doc);
    struct OrderedPositions {
        int fromRow;
        int fromCharPos;
        int toRow;
        int toCharPos;
    };

    struct AttachmentSelection {
        MessageAttachment attachment;
        int fromCharPos = 0;
        int toCharPos = 0;
    };

    struct MessageUrlSelection {
        MessageUrl messageUrl;
        int fromCharPos = 0;
        int toCharPos = 0;
    };

    [[nodiscard]] LIBRUQOLAWIDGETS_NO_EXPORT OrderedPositions orderedPositions() const;
    LIBRUQOLAWIDGETS_NO_EXPORT void selectionText(const OrderedPositions ordered,
                                                  Format format,
                                                  int row,
                                                  const QModelIndex &index,
                                                  QTextDocument *doc,
                                                  QString &str,
                                                  const MessageAttachment &att = {},
                                                  const MessageUrl &messageUrl = {}) const;

    QPersistentModelIndex mStartIndex;
    QPersistentModelIndex mEndIndex;
    QList<AttachmentSelection> mAttachmentSelection;
    QList<MessageUrlSelection> mMessageUrlSelection;
    int mStartPos = -1; // first selected character in start row
    int mEndPos = -1; // last selected character in end row

    DocumentFactoryInterface *mTextHelperFactory = nullptr;
    DocumentFactoryInterface *mMessageUrlHelperFactory = nullptr;
    QList<DocumentFactoryInterface *> mAttachmentFactories;
};
