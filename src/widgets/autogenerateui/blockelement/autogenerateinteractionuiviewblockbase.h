/*
   SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include <QDebug>
#include <QJsonObject>
#include <QString>

class LIBRUQOLAWIDGETS_TESTS_EXPORT AutoGenerateInteractionUiViewBlockBase
{
public:
    AutoGenerateInteractionUiViewBlockBase();
    virtual ~AutoGenerateInteractionUiViewBlockBase();
    [[nodiscard]] QString appId() const;
    void setAppId(const QString &newAppId);

    [[nodiscard]] QString blockId() const;
    void setBlockId(const QString &newBlockId);

    [[nodiscard]] bool operator==(const AutoGenerateInteractionUiViewBlockBase &other) const;

    virtual void parse(const QJsonObject &json);

    virtual void generateWidget(QWidget *parent);

private:
    QString mAppId;
    QString mBlockId;
};
Q_DECLARE_METATYPE(AutoGenerateInteractionUiViewBlockBase)
Q_DECLARE_TYPEINFO(AutoGenerateInteractionUiViewBlockBase, Q_RELOCATABLE_TYPE);
LIBRUQOLAWIDGETS_NO_EXPORT QDebug operator<<(QDebug d, const AutoGenerateInteractionUiViewBlockBase &t);
