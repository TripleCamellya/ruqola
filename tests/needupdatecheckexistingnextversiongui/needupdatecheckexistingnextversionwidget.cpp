/*
   SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "needupdateversion/needupdatecheckexistingnewversionjob.h"
#include "needupdateversion/needupdateversionutils.h"

#include "needupdatecheckexistingnextversionwidget.h"

NeedUpdateCheckExistingNextVersionWidget::NeedUpdateCheckExistingNextVersionWidget(QWidget *parent)
    : QWidget{parent}
{
    auto mainLayout = new QVBoxLayout(this);
    auto lineEdit = new QLineEdit(this);
    mainLayout->addWidget(lineEdit);

    auto pushButton = new QPushButton(QStringLiteral("Download"), this);
    mainLayout->addWidget(pushButton);

    auto plainTextEdit = new QPlainTextEdit(this);
    mainLayout->addWidget(plainTextEdit);
    connect(pushButton, &QPushButton::clicked, this, [this, lineEdit, plainTextEdit]() {
        if (!lineEdit->text().isEmpty()) {
            auto job = new NeedUpdateCheckExistingNewVersionJob(this);
            job->setUrl(lineEdit->text());
            job->setCompileDate(NeedUpdateVersionUtils::compileDate());
            job->start();
        }
    });
    qDebug() << " compile date " << NeedUpdateVersionUtils::compileDate();
}

#include "moc_needupdatecheckexistingnextversionwidget.cpp"
