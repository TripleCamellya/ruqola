/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "whatsnewwidget.h"
#include "whatsnew/whatsnewtranslation.h"

#include <QTextBrowser>
#include <QVBoxLayout>

WhatsNewWidget::WhatsNewWidget(QWidget *parent)
    : QWidget{parent}
    , mLabelInfo(new QTextBrowser(this))
    , mWhatsNewComboBoxWidget(new WhatsNewComboBoxWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mWhatsNewComboBoxWidget->setObjectName(QStringLiteral("mWhatsNewComboBoxWidget"));
    mainLayout->addWidget(mWhatsNewComboBoxWidget);

    mLabelInfo->setObjectName(QStringLiteral("mLabelInfo"));
    mLabelInfo->setReadOnly(true);
    mLabelInfo->setOpenExternalLinks(true);
    mLabelInfo->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    connect(mWhatsNewComboBoxWidget, &WhatsNewComboBoxWidget::versionChanged, this, &WhatsNewWidget::slotVersionChanged);
    mWhatsNewComboBoxWidget->initializeVersion(currentVersion());
    mainLayout->addWidget(mLabelInfo);
    fillTranslations();
    fillComboBox();
}

WhatsNewWidget::~WhatsNewWidget() = default;

void WhatsNewWidget::fillComboBox()
{
    mWhatsNewComboBoxWidget->addVersion(i18n("All Version"), -1);
    int index = 0;
    for (const WhatsNewInfo &info : std::as_const(mWhatsNewInfo)) {
        mWhatsNewComboBoxWidget->addVersion(i18n("Version %1", info.version()), index);
        index++;
    }
}

void WhatsNewWidget::fillTranslations()
{
    mWhatsNewInfo = WhatsNewTranslations::createWhatsNewInfo();
}

int WhatsNewWidget::currentVersion() const
{
    return mWhatsNewInfo.count() - 1;
}

void WhatsNewWidget::updateInformations()
{
    slotVersionChanged(currentVersion());
}

QString WhatsNewWidget::generateStartEndHtml(const QString &str) const
{
    QString message = QStringLiteral("<qt>");
    message += str;
    message += QStringLiteral("</qt>");
    return message;
}

void WhatsNewWidget::slotVersionChanged(int type)
{
    if (type == -1) { // All
        QString message;
        int index = 0;
        for (const WhatsNewInfo &info : std::as_const(mWhatsNewInfo)) {
            message += generateVersionHeader(index);
            message += createVersionInformation(info);
            index++;
        }
        mLabelInfo->setHtml(generateStartEndHtml(message));
    } else {
        const QString message = generateStartEndHtml(createVersionInformation(mWhatsNewInfo.at(type)));
        mLabelInfo->setHtml(message);
    }
}

QString WhatsNewWidget::importantChangeStr() const
{
    return QStringLiteral("<b>") + i18n("Important changes since last version:") + QStringLiteral("</b>");
}

QString WhatsNewWidget::featuresChangeStr() const
{
    return QStringLiteral("<b>") + i18n("Some of the new features in this release of Ruqola include:") + QStringLiteral("</b>");
}

QString WhatsNewWidget::bugFixingChangeStr() const
{
    return QStringLiteral("<b>") + i18n("Some bug fixing:") + QStringLiteral("</b>");
}

QString WhatsNewWidget::createVersionInformation(const WhatsNewInfo &info)
{
    QString message;
    if (!info.changes().isEmpty()) {
        message += importantChangeStr();
        message += QStringLiteral("<ul>");
        for (int i = 0, total = info.changes().count(); i < total; ++i) {
            message += QStringLiteral("<li>%1</li>").arg(info.changes().at(i));
        }
        message += QStringLiteral("</ul>");
    }

    if (!info.newFeatures().isEmpty()) {
        message += featuresChangeStr();
        message += QStringLiteral("<ul>");
        for (int i = 0, total = info.newFeatures().count(); i < total; ++i) {
            message += QStringLiteral("<li>%1</li>").arg(info.newFeatures().at(i));
        }
        message += QStringLiteral("</ul>");
    }

    if (!info.bugFixings().isEmpty()) {
        message += bugFixingChangeStr();
        message += QStringLiteral("<ul>");
        for (int i = 0, total = info.bugFixings().count(); i < total; ++i) {
            message += QStringLiteral("<li>%1</li>").arg(info.bugFixings().at(i));
        }
        message += QStringLiteral("</ul>");
    }
    return message;
}

QString WhatsNewWidget::generateVersionHeader(int type) const
{
    if (type != -1) {
        return QStringLiteral("<h1><i> Version %1 </i></h1><hr/><br>").arg(mWhatsNewInfo.at(type).version());
    }
    return {};
}

#include "moc_whatsnewwidget.cpp"
