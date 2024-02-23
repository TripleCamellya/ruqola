/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "personalaccesstokenpluginutil.h"
#include "plugins/pluginauthenticationconfigurewidget.h"
class QLineEdit;
class PersonalAccessTokenAuthenticationConfigWidget : public PluginAuthenticationConfigureWidget
{
    Q_OBJECT
public:
    explicit PersonalAccessTokenAuthenticationConfigWidget(QWidget *parent = nullptr);
    ~PersonalAccessTokenAuthenticationConfigWidget() override;

    [[nodiscard]] PersonalAccessTokenPluginUtil::PersonalAccessTokenPluginInfo info() const;
    void setInfo(const PersonalAccessTokenPluginUtil::PersonalAccessTokenPluginInfo &info);

Q_SIGNALS:
    void enableOkButton(bool enabled);

private:
    void slotEnableOkButton();
    QLineEdit *const mServerNameLineEdit;
    QLineEdit *const mAccountNameLineEdit;
    QLineEdit *const mUserLineEdit;
    QLineEdit *const mPersonalAccessTokenLineEdit;
};
