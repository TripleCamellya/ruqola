/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "retentionpolicysettingswidget.h"

#include <KLocalizedString>

#include <QCheckBox>
#include <QFormLayout>

RetentionPolicySettingsWidget::RetentionPolicySettingsWidget(RocketChatAccount *account, QWidget *parent)
    : SettingsWidgetBase(account, parent)
    , mEnabled(new QCheckBox(i18n("Enabled"), this))
    , mUseAdvancedRetentionPolicyConfiguration(new QCheckBox(i18n("Use Advanced Retention Policy configuration"), this))
    , mDontPruneThreads(new QCheckBox(i18n("Do not prune Threads"), this))
    , mDontPruneDiscussion(new QCheckBox(i18n("Do not prune Discussion message"), this))
{
    auto mainLayout = new QFormLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mEnabled->setObjectName(QStringLiteral("mEnabled"));
    mainLayout->addWidget(mEnabled);
    connectCheckBox(mEnabled, QStringLiteral("RetentionPolicy_Enabled"));

    mUseAdvancedRetentionPolicyConfiguration->setObjectName(QStringLiteral("mUseAdvancedRetentionPolicyConfiguration"));
    mainLayout->addWidget(mUseAdvancedRetentionPolicyConfiguration);
    connectCheckBox(mUseAdvancedRetentionPolicyConfiguration, QStringLiteral("RetentionPolicy_Advanced_Precision"));

    mDontPruneThreads->setObjectName(QStringLiteral("mDontPruneThreads"));
    mainLayout->addWidget(mDontPruneThreads);
    connectCheckBox(mDontPruneThreads, QStringLiteral("RetentionPolicy_DoNotPruneThreads"));

    mDontPruneDiscussion->setObjectName(QStringLiteral("mDontPruneDiscussion"));
    mainLayout->addWidget(mDontPruneDiscussion);
    connectCheckBox(mDontPruneDiscussion, QStringLiteral("RetentionPolicy_DoNotPruneDiscussion"));
}

RetentionPolicySettingsWidget::~RetentionPolicySettingsWidget() = default;

void RetentionPolicySettingsWidget::initialize()
{
    // TODO
}
