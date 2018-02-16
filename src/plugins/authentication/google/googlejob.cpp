/*
 * Copyright 2016 Riccardo Iaconelli <riccardo@kde.org>
 * Copyright 2018 Veluri Mithun <velurimithun38@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#include "googlejob.h"
#include "ruqola_debug.h"
#include "googleauthenticationplugin_debug.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QMetaEnum>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

#include <o2/o0globals.h>
#include <o2/o0settingsstore.h>
#include <o2/o2google.h>

Google::Google(QObject *parent)
    : QObject(parent)
{
    p_o2Google = new O2Google(this);

    getDataFromJson();

    p_o2Google->setClientId(m_clientID);
    p_o2Google->setClientSecret(m_clientSecret);
    p_o2Google->setLocalPort(8888); //it is from redirect url(http://127.0.0.1:8888/)
    p_o2Google->setRequestUrl(m_authUri);  // Use the desktop login UI
    p_o2Google->setScope(QStringLiteral("email"));

    // Create a store object for writing the received tokens
    O0SettingsStore *store = new O0SettingsStore(QLatin1String(O2_ENCRYPTION_KEY));
    store->setGroupKey(QStringLiteral("Google"));
    p_o2Google->setStore(store);

    connect(p_o2Google, &O2Google::linkedChanged, this, &Google::onLinkedChanged);
    connect(p_o2Google, &O2Google::linkingFailed, this, &Google::linkingFailed);
    connect(p_o2Google, &O2Google::linkingSucceeded, this, &Google::onLinkingSucceeded);
    connect(p_o2Google, &O2Google::openBrowser, this, &Google::onOpenBrowser);
    connect(p_o2Google, &O2Google::closeBrowser, this, &Google::onCloseBrowser);
    connect(p_o2Google, &O2Google::linkingSucceeded, this, &Google::OAuthLoginMethodParameter);
}

void Google::getDataFromJson()
{
    QFile f(QStringLiteral(":/client_secret.json"));

    QString val;
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        val = QString::fromLatin1(f.readAll());
    } else {
        qCWarning(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << "Impossible to read client_secret.json";
        //TODO exit ?
    }

    //******github*******
    //38a607244195a0d7af8 > clientID
    //bb617841568d7c1e0c0888f292cf69b7b11d327e3 > clientSecret
    //https://github.com/login/oauth/authorize
    //https://github.com/login/oauth/access_token
    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject object = document.object();
    const auto settingsObject = object[QStringLiteral("web")].toObject();
    const auto authUri(settingsObject[QStringLiteral("auth_uri")].toString());
    const auto clientID = settingsObject[QStringLiteral("client_id")].toString();
    const auto clientSecret(settingsObject[QStringLiteral("client_secret")].toString());

    m_clientID = clientID;
    m_clientSecret = clientSecret;
    m_authUri = authUri;
    m_tokenUri = QStringLiteral("https://accounts.google.com/o/oauth2/token");
}

void Google::doOAuth(O2::GrantFlow grantFlowType)
{
    qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("Starting OAuth 2 with grant flow type:") << QStringLiteral("Authorization Grant Flow")
                                                    << QStringLiteral("...");
    p_o2Google->setGrantFlow(grantFlowType);
    p_o2Google->unlink();

    //TODO: refresh the token if it is expired(not valid)
    validateToken();
    if (m_isValidToken) {
        OAuthLoginMethodParameter();
    } else {
        p_o2Google->link();
    }
}

//currently not used
void Google::validateToken()
{
    if (!p_o2Google->linked()) {
        qCWarning(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << "ERROR: Application is not linked!";
        Q_EMIT linkingFailed();
        return;
    }

    QString accessToken = p_o2Google->token();
    QString debugUrlStr = QString(m_tokenUri).arg(accessToken);
    QNetworkRequest request = QNetworkRequest(QUrl(debugUrlStr));
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkReply *reply = mgr->get(request);
    connect(reply, &QNetworkReply::finished, this, &Google::onFinished);
    qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("Validating user token. Please wait...");
}

void Google::onOpenBrowser(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

void Google::onCloseBrowser()
{
    //TODO: close the browser
}

void Google::onLinkedChanged()
{
    qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("Link changed!");
}

void Google::onLinkingSucceeded()
{
    O2Google *o1t = qobject_cast<O2Google *>(sender());
    if (o1t == nullptr || !o1t->linked()) {
        return;
    }
    m_accessToken = o1t->token();
    QVariantMap extraTokens = o1t->extraTokens();
    if (!extraTokens.isEmpty()) {
        Q_EMIT extraTokensReady(extraTokens);
        qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("Extra tokens in response:");
        foreach (const QString &key, extraTokens.keys()) {
            qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << key << QStringLiteral(":")
                                                            << (extraTokens.value(key).toString().left(3) + QStringLiteral("..."));
        }
    }
}

//currently not used
void Google::onFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        qCWarning(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("NULL reply!");
        Q_EMIT linkingFailed();
        return;
    }

    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("Reply error:") << reply->error();
        qCWarning(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("Reason:") << reply->errorString();
        Q_EMIT linkingFailed();
        return;
    }

    const QByteArray replyData = reply->readAll();
    bool valid = !replyData.contains("error");
    if (valid) {
        qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("Token is valid");
        Q_EMIT linkingSucceeded();
        m_isValidToken = true;
    } else {
        qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << QStringLiteral("Token is invalid");
        Q_EMIT linkingFailed();
    }
}

void Google::OAuthLoginMethodParameter()
{
    QJsonObject auth;
    QJsonObject authKeys;//
    authKeys[QStringLiteral("credentialToken")] = m_accessToken;
    authKeys[QStringLiteral("credentialSecret")] = m_clientSecret;

    auth[QStringLiteral("oauth")] = authKeys;
    qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << "-------------------------";
    qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << "-------------------------";
    qCDebug(RUQOLA_GOOGLEAUTHENTICATION_PLUGIN_LOG) << "OAuth Json" << auth;

    //oauthLoginJob = Ruqola::self()->rocketChatAccount()->ddp()->method(QStringLiteral("login"), QJsonDocument(auth));
    Q_EMIT loginMethodCalled();
}
