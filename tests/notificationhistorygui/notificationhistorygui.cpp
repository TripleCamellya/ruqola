/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "notificationhistorygui.h"
#include "notificationhistory/notificationhistorydialog.h"
#include "notificationhistorymanager.h"
#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QVBoxLayout>

NotificationHistoryGui::NotificationHistoryGui(QWidget *parent)
    : QWidget{parent}
    , mTextEdit(new QTextEdit(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mTextEdit);
    auto pushButton = new QPushButton(QStringLiteral("Send Notification"), this);
    mainLayout->addWidget(pushButton);
    connect(pushButton, &QPushButton::clicked, this, &NotificationHistoryGui::slotSendNotification);

    auto dlg = new NotificationHistoryDialog(this);
    QList<AccountManager::AccountDisplayInfo> infos;
    AccountManager::AccountDisplayInfo serverInfo;
    serverInfo.name = QStringLiteral("accountName");
    infos.append(std::move(serverInfo));
    dlg->addServerList(infos);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void NotificationHistoryGui::slotSendNotification()
{
    const QString plainText = mTextEdit->toPlainText();
    if (!plainText.isEmpty()) {
        const QByteArray content = plainText.toUtf8();
        QJsonParseError error;
        const auto doc = QJsonDocument::fromJson(content, &error);
        if (error.error) {
            qDebug() << " error " << error.errorString();
            return;
        }
        NotificationInfo info;
        info.setAccountName(QStringLiteral("accountName")); // TODO
        info.setDateTime(QDateTime::currentDateTime());
        info.parseNotification(doc.array());
        qDebug() << " info " << info;
        NotificationHistoryManager::self()->addNotification(info);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);

    NotificationHistoryGui w;
    w.show();
    return app.exec();
}

#include "moc_notificationhistorygui.cpp"
