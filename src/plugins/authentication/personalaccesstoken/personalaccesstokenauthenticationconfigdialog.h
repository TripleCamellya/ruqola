/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "personalaccesstokenpluginutil.h"

#include <QDialog>
class PersonalAccessTokenAuthenticationConfigWidget;
class PersonalAccessTokenAuthenticationConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PersonalAccessTokenAuthenticationConfigDialog(QWidget *parent = nullptr);
    ~PersonalAccessTokenAuthenticationConfigDialog() override;

    [[nodiscard]] PersonalAccessTokenPluginUtil::PersonalAccessTokenPluginInfo info() const;
    void setInfo(const PersonalAccessTokenPluginUtil::PersonalAccessTokenPluginInfo &info);

private:
    PersonalAccessTokenAuthenticationConfigWidget *const mPersonalAccessTokenAuthenticationConfigWidget;
};
