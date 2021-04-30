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

#include "teamselectdeletedroomwidget.h"
#include "misc/lineeditcatchreturnkey.h"
#include "model/teamroomsmodel.h"
#include "restapirequest.h"
#include "rocketchataccount.h"
#include "ruqola.h"
#include "ruqolawidgets_debug.h"
#include "teams/teamslistroomsjob.h"
#include "teamselectdeletedroomfilterproxymodel.h"
#include <KLocalizedString>
#include <QLineEdit>
#include <QListView>
#include <QVBoxLayout>

TeamSelectDeletedRoomWidget::TeamSelectDeletedRoomWidget(QWidget *parent)
    : QWidget(parent)
    , mListView(new QListView(this))
    , mSearchLineEdit(new QLineEdit(this))
    , mTeamRoomsModel(new TeamRoomsModel(this))
    , mTeamSelectProxyModel(new TeamSelectDeletedRoomFilterProxyModel(mTeamRoomsModel, this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mSearchLineEdit->setObjectName(QStringLiteral("mSearchLineEdit"));
    connect(mSearchLineEdit, &QLineEdit::textChanged, this, &TeamSelectDeletedRoomWidget::slotTextChanged);

    mainLayout->addWidget(mSearchLineEdit);

    mSearchLineEdit->setPlaceholderText(i18n("Search Room..."));
    mSearchLineEdit->setClearButtonEnabled(true);
    new LineEditCatchReturnKey(mSearchLineEdit, this);

    mListView->setObjectName(QStringLiteral("mListView"));
    mainLayout->addWidget(mListView);

    mTeamRoomsModel->setIsCheckable(true);
    mListView->setModel(mTeamSelectProxyModel);
}

TeamSelectDeletedRoomWidget::~TeamSelectDeletedRoomWidget()
{
}

void TeamSelectDeletedRoomWidget::slotTextChanged(const QString &str)
{
    mTeamSelectProxyModel->setFilterString(str);
}

void TeamSelectDeletedRoomWidget::initializeTeamRoomsList()
{
    auto *rcAccount = Ruqola::self()->rocketChatAccount();
    auto job = new RocketChatRestApi::TeamsListRoomsJob(this);
    job->setTeamId(mTeamId);
    rcAccount->restApi()->initializeRestApiJob(job);
    connect(job, &RocketChatRestApi::TeamsListRoomsJob::teamListRoomsDone, this, &TeamSelectDeletedRoomWidget::slotTeamListRoomsDone);
    if (!job->start()) {
        qCWarning(RUQOLAWIDGETS_LOG) << "Impossible to start TeamsListRoomsJob job";
    }
}

void TeamSelectDeletedRoomWidget::slotTeamListRoomsDone(const QJsonObject &obj)
{
    QVector<TeamRoom> teamRooms;
    const QJsonArray rooms = obj.value(QLatin1String("rooms")).toArray();
    for (int i = 0, total = rooms.count(); i < total; ++i) {
        const QJsonObject r = rooms.at(i).toObject();
        TeamRoom teamRoom;
        teamRoom.parse(r);
        teamRooms.append(teamRoom);
        // qDebug() << "TeamRoom  " << teamRoom;
    }
    mTeamRoomsModel->setTeamRooms(teamRooms);
}

QString TeamSelectDeletedRoomWidget::teamId() const
{
    return mTeamId;
}

void TeamSelectDeletedRoomWidget::setTeamId(const QString &teamId)
{
    mTeamId = teamId;
    initializeTeamRoomsList();
}

QStringList TeamSelectDeletedRoomWidget::roomsId() const
{
    QStringList lst;
    for (int i = 0, cnt = mTeamRoomsModel->rowCount(); i < cnt; ++i) {
        const QModelIndex idx = mTeamRoomsModel->index(i, 0, QModelIndex());
        if (mTeamRoomsModel->data(idx, Qt::CheckStateRole).toInt() == Qt::Checked) {
            lst.append(mTeamRoomsModel->data(idx, TeamRoomsModel::Identifier).toString());
        }
    }
    return lst;
}
