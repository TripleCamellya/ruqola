/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "offlinewidget.h"
#include <KLocalizedString>

OffLineWidget::OffLineWidget(QWidget *parent)
    : KMessageWidget{parent}
{
    setVisible(false);
    setCloseButtonVisible(false);
    setMessageType(Information);
    setText(i18n("Network is offline mode."));
}

OffLineWidget::~OffLineWidget() = default;

#include "moc_offlinewidget.cpp"
