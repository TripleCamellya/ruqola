/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../githubauthenticationjob.h"
#include <QCommandLineParser>
#include <QGuiApplication>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    QCommandLineParser parser;

    const QCommandLineOption clientId(QStringList() << QStringLiteral("i") << QStringLiteral("client-id"),
                                      QStringLiteral("Specifies the application client id"),
                                      QStringLiteral("client_id"));

    parser.addOptions({clientId});
    parser.process(app);

    if (parser.isSet(clientId)) {
        const int value = parser.value(clientId).toInt();
        auto job = new GitHubAuthenticationJob();
        GitHubAuthenticationJob::GitHubInfo info;
        // info.url;
        // job->setHubLabInfo();
        job->start();
        app.exec();
    } else {
        parser.showHelp();
    }
    return 0;
}
