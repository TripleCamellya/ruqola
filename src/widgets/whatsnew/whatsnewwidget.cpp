/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "whatsnewwidget.h"

#include <KLazyLocalizedString>

#include <QCryptographicHash>
#include <QVBoxLayout>

// enter items for the "Important changes" list here:
static const KLazyLocalizedString ruqolaChanges[] = {
    kli18n("Store Message in Local Database"),
    // TODO
};
static const int numRuqolaChanges = sizeof ruqolaChanges / sizeof *ruqolaChanges;

// enter items for the "new features" list here, so the main body of
// the welcome page can be left untouched (probably much easier for
// the translators). Note that the <li>...</li> tags are added
// automatically below:
static const KLazyLocalizedString ruqolaNewFeatures[] = {
    kli18n("Download Server Icon from Server"),
    kli18n("Show Server Error Info in Specific DialogBox"),
    kli18n("Allow to Copy Image in Clipboard"),
    // TODO
};
static const int numRuqolaNewFeatures = sizeof ruqolaNewFeatures / sizeof *ruqolaNewFeatures;

WhatsNewWidget::WhatsNewWidget(QWidget *parent)
    : QWidget{parent}
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});
}

WhatsNewWidget::~WhatsNewWidget() = default;

// static
QString WhatsNewWidget::newFeaturesMD5()
{
    QByteArray str;
    for (int i = 0; i < numRuqolaChanges; ++i) {
        str += ruqolaChanges[i].untranslatedText();
    }
    for (int i = 0; i < numRuqolaNewFeatures; ++i) {
        str += ruqolaNewFeatures[i].untranslatedText();
    }
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(str);
    return QLatin1String(md5.result().toBase64());
}
