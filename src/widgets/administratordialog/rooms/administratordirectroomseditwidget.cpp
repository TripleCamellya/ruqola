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

#include "administratordirectroomseditwidget.h"
#include <KLocalizedString>
#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>

AdministratorDirectRoomsEditWidget::AdministratorDirectRoomsEditWidget(QWidget *parent)
    : AdministratorRoomsEditBaseWidget{parent}
    , mLineEdit(new QLineEdit(this))
    , mDefaultCheckBox(new QCheckBox(i18n("Default"), this))
    , mFavoriteCheckBox(new QCheckBox(i18n("Favorite"), this))
    , mFeaturedCheckBox(new QCheckBox(i18n("Featured"), this))
{
    auto mainLayout = new QFormLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});
    mLineEdit->setObjectName(QStringLiteral("mLineEdit"));
    mainLayout->addRow(i18n("Name:"), mLineEdit);
    mLineEdit->setReadOnly(true);

    mDefaultCheckBox->setObjectName(QStringLiteral("mDefaultCheckBox"));
    mFavoriteCheckBox->setObjectName(QStringLiteral("mFavoriteCheckBox"));
    mFeaturedCheckBox->setObjectName(QStringLiteral("mFeaturedCheckBox"));

    mainLayout->addRow(mDefaultCheckBox);
    mainLayout->addRow(mFavoriteCheckBox);
    mainLayout->addRow(mFeaturedCheckBox);
}

AdministratorDirectRoomsEditWidget::~AdministratorDirectRoomsEditWidget()
{
}

AdministratorRoomsEditBaseWidget::RoomEditInfo AdministratorDirectRoomsEditWidget::roomEditInfo() const
{
    AdministratorRoomsEditBaseWidget::RoomEditInfo info;
    info.name = mLineEdit->text();
    info.featured = mFeaturedCheckBox->isChecked();
    info.defaultRoom = mDefaultCheckBox->isChecked();
    info.favorite = mFavoriteCheckBox->isChecked();
    return info;
}

void AdministratorDirectRoomsEditWidget::setRoomEditInfo(const RoomEditInfo &newRoomEditInfo)
{
    mLineEdit->setText(newRoomEditInfo.name);
    mFeaturedCheckBox->setChecked(newRoomEditInfo.featured);
    mDefaultCheckBox->setChecked(newRoomEditInfo.defaultRoom);
    mFavoriteCheckBox->setChecked(newRoomEditInfo.favorite);
}
