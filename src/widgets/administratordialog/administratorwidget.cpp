/*
   Copyright (c) 2020-2021 Laurent Montel <montel@kde.org>

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

#include "administratorwidget.h"
#include "customsounds/administratorcustomsoundswidget.h"
#include "customuserstatus/administratorcustomuserstatuswidget.h"
#include "permissionmanager.h"
#include "rocketchataccount.h"
#include "rooms/administratorroomswidget.h"
#include "ruqola.h"
#include "serverinfo/administratorserverinfowidget.h"
#include "users/administratoruserswidget.h"
#include <KLocalizedString>
#include <QTabWidget>
#include <QVBoxLayout>

AdministratorWidget::AdministratorWidget(QWidget *parent)
    : QWidget(parent)
    , mTabWidget(new QTabWidget(this))
    , mAdministratorRoomsWidget(new AdministratorRoomsWidget(this))
    , mAdministratorCustomUserStatusWidget(new AdministratorCustomUserStatusWidget(this))
    , mAdministratorCustomSoundsWidget(new AdministratorCustomSoundsWidget(this))
    , mAdministratorServerInfoWidget(new AdministratorServerInfoWidget(this))
    , mAdministratorUsersWidget(new AdministratorUsersWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mTabWidget->setObjectName(QStringLiteral("mTabWidget"));
    mainLayout->addWidget(mTabWidget);

    mAdministratorRoomsWidget->setObjectName(QStringLiteral("mAdministratorRoomsWidget"));
    mTabWidget->addTab(mAdministratorRoomsWidget, i18n("Rooms"));

    mAdministratorCustomUserStatusWidget->setObjectName(QStringLiteral("mAdministratorCustomUserStatusWidget"));
    mTabWidget->addTab(mAdministratorCustomUserStatusWidget, i18n("Custom User Status"));

    if (Ruqola::self()->rocketChatAccount()->hasPermission(QStringLiteral("manage-sounds"))) {
        mAdministratorCustomSoundsWidget->setObjectName(QStringLiteral("mAdministratorCustomSoundsWidget"));
        mTabWidget->addTab(mAdministratorCustomSoundsWidget, i18n("Custom Sounds"));
    }

    if (Ruqola::self()->rocketChatAccount()->hasPermission(QStringLiteral("view-statistics"))) {
        mAdministratorServerInfoWidget->setObjectName(QStringLiteral("mAdministratorServerInfoWidget"));
        mTabWidget->addTab(mAdministratorServerInfoWidget, i18n("Server Info"));
    }
    mAdministratorUsersWidget->setObjectName(QStringLiteral("mAdministratorUsersWidget"));
    mTabWidget->addTab(mAdministratorUsersWidget, i18n("Users"));
}

AdministratorWidget::~AdministratorWidget()
{
}
