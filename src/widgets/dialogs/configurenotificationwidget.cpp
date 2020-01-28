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

#include "configurenotificationwidget.h"
#include <QVBoxLayout>
#include <KLocalizedString>
#include <QFormLayout>
#include <QCheckBox>
#include <QGroupBox>

ConfigureNotificationWidget::ConfigureNotificationWidget(QWidget *parent)
    : QWidget(parent)
{
    QFormLayout *mainLayout = new QFormLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mDisableNotification = new QCheckBox(this);
    mDisableNotification->setObjectName(QStringLiteral("mDisableNotification"));
    mainLayout->addRow(i18n("Disable Notification:"), mDisableNotification);

    mHideUnreadRoomStatus = new QCheckBox(this);
    mHideUnreadRoomStatus->setObjectName(QStringLiteral("mHideUnreadRoomStatus"));
    mainLayout->addRow(i18n("Hide Unread Room Status:"), mHideUnreadRoomStatus);

    mMuteGroupMentions = new QCheckBox(this);
    mMuteGroupMentions->setObjectName(QStringLiteral("mMuteGroupMentions"));
    mainLayout->addRow(i18n("Mute Group Mentions:"), mMuteGroupMentions);

    QGroupBox *desktopGroupBox = new QGroupBox(i18n("Desktop"), this);
    desktopGroupBox->setObjectName(QStringLiteral("desktopGroupBox"));
    mainLayout->addWidget(desktopGroupBox);

    QFormLayout *desktopGroupBoxLayout = new QFormLayout(desktopGroupBox);

}

ConfigureNotificationWidget::~ConfigureNotificationWidget()
{
}
