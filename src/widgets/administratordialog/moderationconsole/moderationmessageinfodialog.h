/*
   SPDX-FileCopyrightText: 2023-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include "moderation/moderationreportinfos.h"
#include <QDialog>

class ModerationMessageInfoWidget;
class RocketChatAccount;
class LIBRUQOLAWIDGETS_TESTS_EXPORT ModerationMessageInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ModerationMessageInfoDialog(RocketChatAccount *account, QWidget *parent = nullptr);
    ~ModerationMessageInfoDialog() override;

    void setReportInfos(const ModerationReportInfos &info);

private:
    LIBRUQOLAWIDGETS_NO_EXPORT void readConfig();
    LIBRUQOLAWIDGETS_NO_EXPORT void writeConfig();
    ModerationMessageInfoWidget *const mModerationMessageInfoWidget;
};
