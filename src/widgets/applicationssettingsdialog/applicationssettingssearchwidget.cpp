/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "applicationssettingssearchwidget.h"
#include "applicationssettingscategoriescombobox.h"
#include "applicationssettingspricecombobox.h"
#include "applicationssettingsstatuscombobox.h"
#include "rocketchataccount.h"

#include <KLineEditEventHandler>
#include <QHBoxLayout>
#include <QLineEdit>
using namespace Qt::Literals::StringLiterals;
ApplicationsSettingsSearchWidget::ApplicationsSettingsSearchWidget(RocketChatAccount *account, QWidget *parent)
    : QWidget{parent}
    , mSearchLineEdit(new QLineEdit(this))
    , mApplicationsSettingsComboBox(new ApplicationsSettingsCategoriesComboBox(account, this))
    , mApplicationsSettingsPriceComboBox(new ApplicationsSettingsPriceComboBox(this))
    , mApplicationsSettingsStatusComboBox(new ApplicationsSettingsStatusComboBox(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins({});
    mainLayout->setObjectName("mainLayout"_L1);

    mSearchLineEdit->setObjectName("mSearchLineEdit"_L1);
    mainLayout->addWidget(mSearchLineEdit);
    KLineEditEventHandler::catchReturnKey(mSearchLineEdit);
    mSearchLineEdit->setClearButtonEnabled(true);

    mApplicationsSettingsComboBox->setObjectName("mApplicationsSettingsComboBox"_L1);
    mainLayout->addWidget(mApplicationsSettingsComboBox);

    mApplicationsSettingsPriceComboBox->setObjectName("mApplicationsSettingsPriceComboBox"_L1);
    mainLayout->addWidget(mApplicationsSettingsPriceComboBox);

    mApplicationsSettingsStatusComboBox->setObjectName("mApplicationsSettingsStatusComboBox"_L1);
    mainLayout->addWidget(mApplicationsSettingsStatusComboBox);
    connect(mSearchLineEdit, &QLineEdit::textChanged, this, &ApplicationsSettingsSearchWidget::filterChanged);
    connect(mApplicationsSettingsComboBox, &ApplicationsSettingsCategoriesComboBox::activated, this, &ApplicationsSettingsSearchWidget::filterChanged);
    connect(mApplicationsSettingsPriceComboBox, &ApplicationsSettingsPriceComboBox::activated, this, &ApplicationsSettingsSearchWidget::filterChanged);
    connect(mApplicationsSettingsStatusComboBox, &ApplicationsSettingsStatusComboBox::activated, this, &ApplicationsSettingsSearchWidget::filterChanged);
}

ApplicationsSettingsSearchWidget::~ApplicationsSettingsSearchWidget() = default;

AppsMarketPlaceFilterProxyModel::FilterInfo ApplicationsSettingsSearchWidget::filterInfo() const
{
    AppsMarketPlaceFilterProxyModel::FilterInfo info;
    return info;
}

#include "moc_applicationssettingssearchwidget.cpp"
