/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "showlistmessagebasewidget.h"
#include "misc/lineeditcatchreturnkey.h"
#include "model/listmessagesmodelfilterproxymodel.h"
#include "room/messagelistview.h"

#include <KLocalizedString>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

ShowListMessageBaseWidget::ShowListMessageBaseWidget(RocketChatAccount *account, QWidget *parent)
    : QWidget(parent)
    , mSearchMessageLineEdit(new QLineEdit(this))
    , mMessageListInfo(new QLabel(this))
    , mMessageListView(new MessageListView(account, MessageListView::Mode::Viewing, this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mSearchMessageLineEdit->setObjectName(QStringLiteral("mSearchMessageLineEdit"));
    mSearchMessageLineEdit->setClearButtonEnabled(true);
    new LineEditCatchReturnKey(mSearchMessageLineEdit, this);
    mSearchMessageLineEdit->setPlaceholderText(i18n("Search messages..."));
    connect(mSearchMessageLineEdit, &QLineEdit::textChanged, this, &ShowListMessageBaseWidget::slotSearchMessageTextChanged);
    mainLayout->addWidget(mSearchMessageLineEdit);

    mMessageListInfo->setObjectName(QStringLiteral("mMessageListInfo"));
    mMessageListInfo->setTextFormat(Qt::RichText);
    mMessageListInfo->setContextMenuPolicy(Qt::NoContextMenu);
    QFont labFont = mMessageListInfo->font();
    labFont.setBold(true);
    mMessageListInfo->setFont(labFont);
    connect(mMessageListInfo, &QLabel::linkActivated, this, &ShowListMessageBaseWidget::loadMoreElements);
    mainLayout->addWidget(mMessageListInfo);

    mMessageListView->setObjectName(QStringLiteral("mMessageListView"));

    mainLayout->addWidget(mMessageListView);
    connect(mMessageListView, &MessageListView::modelChanged, this, &ShowListMessageBaseWidget::updateLabel);
}

ShowListMessageBaseWidget::~ShowListMessageBaseWidget() = default;

void ShowListMessageBaseWidget::setModel(ListMessagesModelFilterProxyModel *model)
{
    mModel = model;
    mMessageListView->setModel(model);
    mModel->setFilterString({});
    connect(mModel, &ListMessagesModelFilterProxyModel::hasFullListChanged, this, &ShowListMessageBaseWidget::updateLabel);
    connect(mModel, &ListMessagesModelFilterProxyModel::loadingInProgressChanged, this, &ShowListMessageBaseWidget::updateLabel);
    updateLabel();
}

MessageListView *ShowListMessageBaseWidget::messageListView() const
{
    return mMessageListView;
}

void ShowListMessageBaseWidget::setRoom(Room *room)
{
    mMessageListView->setRoom(room);
}

void ShowListMessageBaseWidget::updateLabel()
{
    if (mModel->loadMoreListMessagesInProgress()) {
        mMessageListInfo->setText(i18n("Loading..."));
    } else {
        mMessageListInfo->setText(mModel->numberOfMessages() == 0 ? i18n("No Message found") : displayShowMessageInRoom());
    }
}

QString ShowListMessageBaseWidget::displayShowMessageInRoom() const
{
    QString displayMessageStr = i18np("%1 Message in room (Total: %2)", "%1 Messages in room (Total: %2)", mModel->numberOfMessages(), mModel->total());
    if (!mModel->hasFullList()) {
        displayMessageStr += QStringLiteral(" <a href=\"loadmoreelement\">%1</a>").arg(i18n("(Click here for Loading more...)"));
    }
    return displayMessageStr;
}

void ShowListMessageBaseWidget::slotSearchMessageTextChanged(const QString &str)
{
    mMessageListView->setSearchText(str);
    mModel->setFilterString(str);
}
