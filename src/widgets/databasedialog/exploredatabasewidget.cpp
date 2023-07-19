/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "exploredatabasewidget.h"
#include "misc/lineeditcatchreturnkey.h"
#include "model/messagemodel.h"
#include "rocketchataccount.h"
#include "room/messagelistview.h"
#include <KMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

// debug dialogbox => don't translate it
ExploreDatabaseWidget::ExploreDatabaseWidget(RocketChatAccount *account, QWidget *parent)
    : QWidget{parent}
    , mRocketChatAccount(account)
    , mMessageListView(new MessageListView(account, MessageListView::Mode::Viewing, this))
    , mLocalMessageDatabase(new LocalMessageDatabase())
    , mRoomName(new QLineEdit(this))
    , mNumberOfMessages(new QSpinBox(this))
    , mMessageModel(new MessageModel(QStringLiteral("no_room"), account, nullptr, this))
{
    mNumberOfMessages->setObjectName(QStringLiteral("mNumberOfMessages"));
    mNumberOfMessages->setRange(-1, 9999);
    mNumberOfMessages->setValue(-1);
    mNumberOfMessages->setSpecialValueText(QStringLiteral("All messages"));

    mMessageListView->setObjectName(QStringLiteral("mMessageListView"));
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    auto hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins({});
    hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
    mainLayout->addLayout(hboxLayout);

    mRoomName->setObjectName(QStringLiteral("mRoomName"));
    mRoomName->setClearButtonEnabled(true);
    new LineEditCatchReturnKey(mRoomName, this);
    auto label = new QLabel(QStringLiteral("Room name:"), this);
    label->setObjectName(QStringLiteral("label"));
    hboxLayout->addWidget(label);
    hboxLayout->addWidget(mRoomName);
    hboxLayout->addWidget(new QLabel(QStringLiteral("Number Of Elements"), this));
    hboxLayout->addWidget(mNumberOfMessages);

    auto pushButton = new QPushButton(QStringLiteral("Load"), this);
    pushButton->setObjectName(QStringLiteral("pushButton"));
    hboxLayout->addWidget(pushButton);
    connect(pushButton, &QPushButton::clicked, this, &ExploreDatabaseWidget::slotLoad);
    connect(mRoomName, &QLineEdit::returnPressed, this, &ExploreDatabaseWidget::slotLoad);

    mMessageListView->setModel(mMessageModel);
    mainLayout->addWidget(mMessageListView);
}

ExploreDatabaseWidget::~ExploreDatabaseWidget() = default;

void ExploreDatabaseWidget::slotLoad()
{
    const QString roomName = mRoomName->text().trimmed();
    if (!roomName.isEmpty()) {
        const QVector<Message> listMessages =
            mLocalMessageDatabase->loadMessages(mRocketChatAccount->accountName(), roomName, -1, -1, mNumberOfMessages->value());
        mMessageModel->clear();
        mMessageModel->addMessages(listMessages);
    } else {
        KMessageBox::error(this, QStringLiteral("Room '%1' does not have database").arg(roomName), QStringLiteral("Database empty"));
    }
}

#include "moc_exploredatabasewidget.cpp"
