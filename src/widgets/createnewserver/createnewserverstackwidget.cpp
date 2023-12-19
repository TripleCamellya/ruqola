/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "createnewserverstackwidget.h"
#include "createnewservercheckurlwidget.h"
#include "createnewserverwidget.h"

CreateNewServerStackWidget::CreateNewServerStackWidget(QWidget *parent)
    : QStackedWidget(parent)
    , mCreateNewServerCheckUrlWidget(new CreateNewServerCheckUrlWidget(this))
    , mCreateNewServerWidget(new CreateNewServerWidget(this))
{
    mCreateNewServerCheckUrlWidget->setObjectName(QStringLiteral("mCreateNewServerCheckUrlWidget"));
    addWidget(mCreateNewServerCheckUrlWidget);

    mCreateNewServerWidget->setObjectName(QStringLiteral("mCreateNewServerWidget"));
    addWidget(mCreateNewServerWidget);
    setCurrentWidget(mCreateNewServerCheckUrlWidget);
    connect(mCreateNewServerCheckUrlWidget,
            &CreateNewServerCheckUrlWidget::serverUrlFound,
            this,
            [this](const QString &serverUrl, const QVector<AuthenticationInfo> &authenticationInfos) {
                setCurrentWidget(mCreateNewServerWidget);
                AccountManager::AccountManagerInfo info;
                info.serverUrl = serverUrl;
                mCreateNewServerWidget->setAccountInfo(std::move(info));
                mCreateNewServerWidget->setAuthenticationInfos(authenticationInfos);
            });
    connect(mCreateNewServerWidget, &CreateNewServerWidget::updateOkButton, this, &CreateNewServerStackWidget::updateOkButton);
}

CreateNewServerStackWidget::~CreateNewServerStackWidget() = default;

void CreateNewServerStackWidget::setExistingAccountName(const QStringList &lst)
{
    mCreateNewServerWidget->setExistingAccountName(lst);
}

AccountManager::AccountManagerInfo CreateNewServerStackWidget::accountInfo() const
{
    const AccountManager::AccountManagerInfo info = mCreateNewServerWidget->accountInfo();
    return info;
}

void CreateNewServerStackWidget::setAccountInfo(const AccountManager::AccountManagerInfo &info)
{
    mCreateNewServerWidget->setAccountInfo(info);
}
#include "moc_createnewserverstackwidget.cpp"
