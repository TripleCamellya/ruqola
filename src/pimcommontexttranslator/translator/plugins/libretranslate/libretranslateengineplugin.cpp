/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "libretranslateengineplugin.h"

#include <KConfigGroup>
#include <KSharedConfig>
#include <PimCommonTextTranslator/TranslatorEngineAccessManager>
#include <QJsonDocument>
#include <QJsonObject>

LibreTranslateEnginePlugin::LibreTranslateEnginePlugin(QObject *parent)
    : PimCommonTextTranslator::TranslatorEnginePlugin(parent)
{
    loadSettings();
}

LibreTranslateEnginePlugin::~LibreTranslateEnginePlugin() = default;

void LibreTranslateEnginePlugin::translate()
{
    if (verifyFromAndToLanguage()) {
        return;
    }
    translateText();
}

void LibreTranslateEnginePlugin::translateText()
{
    clear();

    QByteArray postData = "&q=" + QUrl::toPercentEncoding(inputText()) + "&source=" + from().toUtf8() + "&target=" + to().toUtf8();
    if (!mApiKey.isEmpty()) {
        postData += "&api_key=" + mApiKey.toUtf8();
    }
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(QUrl(QStringLiteral("%1/translate").arg(mServerUrl)));

    QNetworkReply *reply = PimCommonTextTranslator::TranslatorEngineAccessManager::self()->networkManager()->post(request, postData);
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply](QNetworkReply::NetworkError error) {
        slotError(error);
        reply->deleteLater();
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        parseTranslation(reply);
    });
}

void LibreTranslateEnginePlugin::parseTranslation(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        Q_EMIT translateFailed(false, reply->errorString());
        reply->deleteLater();
        return;
    }
    const QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
    if (hasDebug()) {
        setJsonDebug(QString::fromUtf8(jsonResponse.toJson(QJsonDocument::Indented)));
    }

    const QJsonObject responseObject = jsonResponse.object();
    setResult(responseObject.value(QStringLiteral("translatedText")).toString());
    reply->deleteLater();
    Q_EMIT translateDone();
}

void LibreTranslateEnginePlugin::loadSettings()
{
    KConfigGroup myGroup(KSharedConfig::openConfig(), QStringLiteral("LibreTranslateTranslator"));
    mServerUrl = myGroup.readEntry(QStringLiteral("ServerUrl"), QString());
}
