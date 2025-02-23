/*
   SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolacore_export.h"
#include <QJsonObject>

// TODO make it private export symbol ?
class LIBRUQOLACORE_EXPORT AutoGenerateInteractionUiView
{
public:
    AutoGenerateInteractionUiView();
    ~AutoGenerateInteractionUiView();

    void parseView(const QJsonObject &json);

    [[nodiscard]] QByteArray id() const;
    void setId(const QByteArray &newId);

    [[nodiscard]] bool operator==(const AutoGenerateInteractionUiView &other) const;

private:
    QByteArray mId;
};

Q_DECLARE_METATYPE(AutoGenerateInteractionUiView)
Q_DECLARE_TYPEINFO(AutoGenerateInteractionUiView, Q_RELOCATABLE_TYPE);
LIBRUQOLACORE_EXPORT QDebug operator<<(QDebug d, const AutoGenerateInteractionUiView &t);
