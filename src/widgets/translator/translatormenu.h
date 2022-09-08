/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include <QObject>
class QMenu;
class LIBRUQOLAWIDGETS_TESTS_EXPORT TranslatorMenu : public QObject
{
    Q_OBJECT
public:
    explicit TranslatorMenu(QObject *parent = nullptr);
    ~TranslatorMenu();

    Q_REQUIRED_RESULT QMenu *menu() const;

    Q_REQUIRED_RESULT bool isEmpty() const;
Q_SIGNALS:
    void translate(const QString &from, const QString &to);

private:
    Q_REQUIRED_RESULT static QString searchI18nFromLanguage(const QVector<QPair<QString, QString>> &languagesList, const QString &lang);
    void updateMenu();
    QMenu *const mMenu;
};
