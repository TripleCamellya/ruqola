/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ddpauthenticationmanagerutils.h"
#include "utils.h"

#include <QJsonObject>

QJsonArray DDPAuthenticationManagerUtils::loginResume(const QString &token)
{
    QJsonArray array;
    QJsonObject resumeObject;
    resumeObject[QStringLiteral("resume")] = token;
    array.append(resumeObject);
    return array;
}

QJsonArray DDPAuthenticationManagerUtils::loginLdap(const QString &user, const QString &password)
{
    QJsonArray array;
    QJsonObject ldapObject;
    ldapObject[QStringLiteral("ldap")] = true;
    ldapObject[QStringLiteral("username")] = user;
    ldapObject[QStringLiteral("ldapPass")] = password;
    ldapObject[QStringLiteral("ldapOptions")] = QJsonObject();
    array.append(ldapObject);
    return array;
}

QJsonArray DDPAuthenticationManagerUtils::loginOAuth(const QString &credentialToken, const QString &credentialSecret)
{
    QJsonArray array;
    QJsonObject oauthObject;

    QJsonObject oauthCredentialObject;
    oauthCredentialObject[QStringLiteral("credentialToken")] = credentialToken;
    oauthCredentialObject[QStringLiteral("credentialSecret")] = credentialSecret;
    oauthObject[QStringLiteral("oauth")] = oauthCredentialObject;
    array.append(oauthObject);
    return array;
}

QJsonArray DDPAuthenticationManagerUtils::login(const QString &user, const QString &password)
{
    // TODO: need to support login with email too ("email": "address" instead of "username": "user")

    QJsonArray array;
    QJsonObject loginObject;

    const QByteArray sha256pw = Utils::convertSha256Password(password);
    QJsonObject userObject;
    userObject[QStringLiteral("username")] = user;
    loginObject[QStringLiteral("user")] = userObject;

    QJsonObject passwordObject;
    passwordObject[QStringLiteral("algorithm")] = QStringLiteral("sha-256");
    passwordObject[QStringLiteral("digest")] = QString::fromLatin1(sha256pw);
    loginObject[QStringLiteral("password")] = passwordObject;

    array.append(loginObject);
    return array;
}
