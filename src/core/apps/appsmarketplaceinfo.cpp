/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "appsmarketplaceinfo.h"
#include "utils.h"
#include <KLocalizedString>

#include <QJsonArray>
using namespace Qt::Literals::StringLiterals;
AppsMarketPlaceInfo::AppsMarketPlaceInfo() = default;

AppsMarketPlaceInfo::~AppsMarketPlaceInfo() = default;

QDebug operator<<(QDebug d, const AppsMarketPlaceInfo &t)
{
    d.nospace() << "appId " << t.appId();
    d.nospace() << "isEnterpriseOnly " << t.isEnterpriseOnly();
    d.nospace() << "appName " << t.appName();
    d.nospace() << "categories " << t.categories();
    d.nospace() << "documentationUrl " << t.documentationUrl();
    d.nospace() << "purchaseType " << t.purchaseType();
    d.nospace() << "description " << t.description();
    d.nospace() << "price " << t.price();
    d.nospace() << "version " << t.version();
    d.nospace() << "shortDescription " << t.shortDescription();
    d.nospace() << "pixmap is valid " << !t.pixmap().isNull();
    d.nospace() << "modifiedDate " << t.modifiedDate();
    d.nospace() << "isPaid " << t.isPaid();
    d.nospace() << "pricePlan " << t.pricePlan();
    d.nospace() << "isPrivate " << t.isPrivate();
    return d;
}

QDebug operator<<(QDebug d, const AppsMarketPlaceInfo::PricePlan &t)
{
    d.nospace() << "price " << t.price;
    d.nospace() << "trialDays " << t.trialDays;
    d.nospace() << "enabled " << t.enabled;
    d.nospace() << "strategy " << t.strategy;
    return d;
}

void AppsMarketPlaceInfo::parsePrincingPlan(const QJsonArray &array)
{
    mPricePlan.clear();
    for (const QJsonValue &current : array) {
        PricePlan price;
        price.price = current["price"_L1].toInt();
        price.trialDays = current["trialDays"_L1].toInt();
        price.enabled = current["enabled"_L1].toBool();
        price.strategy = price.convertStringToStrategy(current["strategy"_L1].toString());
        mPricePlan.append(price);
    }
}

bool AppsMarketPlaceInfo::isPrivate() const
{
    return mIsPrivate;
}

void AppsMarketPlaceInfo::setIsPrivate(bool newIsPrivate)
{
    mIsPrivate = newIsPrivate;
}

bool AppsMarketPlaceInfo::PricePlan::operator==(const AppsMarketPlaceInfo::PricePlan &other) const
{
    return price == other.price && trialDays == other.trialDays && strategy == other.strategy && enabled == other.enabled;
}

AppsMarketPlaceInfo::PricePlan::Strategy AppsMarketPlaceInfo::PricePlan::convertStringToStrategy(const QString &str) const
{
    if (str == "monthly"_L1) {
        return AppsMarketPlaceInfo::PricePlan::Strategy::Monthly;
    }
    return AppsMarketPlaceInfo::PricePlan::Strategy::Unknown;
}

void AppsMarketPlaceInfo::parseAppsMarketPlaceInfo(const QJsonObject &replyObject)
{
    qDebug() << " replyObject " << replyObject;
    mAppId = replyObject["appId"_L1].toString().toLatin1();
    mIsEnterpriseOnly = replyObject["isEnterpriseOnly"_L1].toBool();
    mPurchaseType = replyObject["purchaseType"_L1].toString();

    mModifiedDate = Utils::parseIsoDate("modifiedAt"_L1, replyObject);

    parsePrincingPlan(replyObject["pricingPlans"_L1].toArray());
    // TODO implement plans support
    mPrice = replyObject["price"_L1].toInt();

    const QJsonObject latestObj = replyObject["latest"_L1].toObject();
    const QJsonArray categoriesArray = latestObj["categories"_L1].toArray();

    QStringList lst;
    lst.reserve(categoriesArray.count());
    for (const QJsonValue &current : categoriesArray) {
        lst.append(current.toString());
    }
    mCategories = lst;

    mDescription = latestObj["description"_L1].toString();
    mShortDescription = latestObj["shortDescription"_L1].toString();
    mVersion = latestObj["version"_L1].toString();
    mAppName = latestObj["name"_L1].toString();
    mDocumentationUrl = latestObj["documentationUrl"_L1].toString();
    const QByteArray baImageBase64 = latestObj["iconFileData"_L1].toString().toLatin1();
    mPixmap.loadFromData(QByteArray::fromBase64(baImageBase64), "PNG");
}

QByteArray AppsMarketPlaceInfo::appId() const
{
    return mAppId;
}

void AppsMarketPlaceInfo::setAppId(const QByteArray &newAppId)
{
    mAppId = newAppId;
}

bool AppsMarketPlaceInfo::isEnterpriseOnly() const
{
    return mIsEnterpriseOnly;
}

void AppsMarketPlaceInfo::setIsEnterpriseOnly(bool newIsEnterpriseOnly)
{
    mIsEnterpriseOnly = newIsEnterpriseOnly;
}

QString AppsMarketPlaceInfo::appName() const
{
    return mAppName;
}

void AppsMarketPlaceInfo::setAppName(const QString &newAppName)
{
    mAppName = newAppName;
}

QStringList AppsMarketPlaceInfo::categories() const
{
    return mCategories;
}

void AppsMarketPlaceInfo::setCategories(const QStringList &newCategories)
{
    mCategories = newCategories;
}

bool AppsMarketPlaceInfo::isValid() const
{
    // TODO
    return !mAppId.isEmpty();
}

QString AppsMarketPlaceInfo::documentationUrl() const
{
    return mDocumentationUrl;
}

void AppsMarketPlaceInfo::setDocumentationUrl(const QString &newDocumentationUrl)
{
    mDocumentationUrl = newDocumentationUrl;
}

QString AppsMarketPlaceInfo::purchaseType() const
{
    return mPurchaseType;
}

void AppsMarketPlaceInfo::setPurchaseType(const QString &newPurchaseType)
{
    mPurchaseType = newPurchaseType;
}

int AppsMarketPlaceInfo::price() const
{
    return mPrice;
}

void AppsMarketPlaceInfo::setPrice(int newPrice)
{
    mPrice = newPrice;
}

QString AppsMarketPlaceInfo::description() const
{
    return mDescription;
}

void AppsMarketPlaceInfo::setDescription(const QString &newDescription)
{
    mDescription = newDescription;
}

QPixmap AppsMarketPlaceInfo::pixmap() const
{
    return mPixmap;
}

void AppsMarketPlaceInfo::setPixmap(const QPixmap &newIcon)
{
    mPixmap = newIcon;
}

QString AppsMarketPlaceInfo::version() const
{
    return mVersion;
}

void AppsMarketPlaceInfo::setVersion(const QString &newVersion)
{
    mVersion = newVersion;
}

QString AppsMarketPlaceInfo::shortDescription() const
{
    return mShortDescription;
}

void AppsMarketPlaceInfo::setShortDescription(const QString &newShortDescription)
{
    mShortDescription = newShortDescription;
}

bool AppsMarketPlaceInfo::operator==(const AppsMarketPlaceInfo &other) const
{
    return mCategories == other.mCategories && mAppId == other.mAppId && mAppName == other.mAppName && mDescription == other.mDescription
        && mDocumentationUrl == other.mDocumentationUrl && mPurchaseType == other.mPurchaseType && mVersion == other.mVersion
        && mShortDescription == other.mShortDescription /*&& mPixmap.isNull() == other.mPixmap.isNull()*/ && mPrice == other.mPrice
        && mIsEnterpriseOnly == other.mIsEnterpriseOnly && mModifiedDate == other.mModifiedDate && mPricePlan == other.mPricePlan;
}

qint64 AppsMarketPlaceInfo::modifiedDate() const
{
    return mModifiedDate;
}

void AppsMarketPlaceInfo::setModifiedDate(qint64 newModifiedDate)
{
    mModifiedDate = newModifiedDate;
}

QString AppsMarketPlaceInfo::applicationInformations() const
{
    QString str = QStringLiteral("<b>%1</b><br/>").arg(mAppName);
    str += mShortDescription + QStringLiteral("<br/><br/>");

    QString newDescription = mDescription;
    newDescription.replace(QLatin1Char('\n'), QStringLiteral("<br/>"));

    str += QStringLiteral("<b>%1</b><br/>").arg(i18n("Description")) + newDescription + QStringLiteral("<br/><br/>");

    str += QStringLiteral("<b>%1</b><br/>").arg(i18n("Version")) + mVersion + QStringLiteral("<br/><br/>");

    if (!mCategories.isEmpty()) {
        str += QStringLiteral("<b>%1</b><br/>").arg(i18n("Categories")) + mCategories.join(QStringLiteral(", ")) + QStringLiteral("<br/><br/>");
    }
    if (!mDocumentationUrl.isEmpty()) {
        const QString url = mDocumentationUrl.startsWith(QStringLiteral("http")) ? mDocumentationUrl : QStringLiteral("https://%1").arg(mDocumentationUrl);
        str += QStringLiteral("<b>%1</b><br/>").arg(i18n("Documentation")) + QStringLiteral("<a href=\"%2\">%1</a>").arg(mDocumentationUrl, url)
            + QStringLiteral("<br/><br/>");
    }

    return str;
}

bool AppsMarketPlaceInfo::isPaid() const
{
    return !mPricePlan.isEmpty();
}

QList<AppsMarketPlaceInfo::PricePlan> AppsMarketPlaceInfo::pricePlan() const
{
    return mPricePlan;
}

void AppsMarketPlaceInfo::setPricePlan(const QList<PricePlan> &newPricePlan)
{
    mPricePlan = newPricePlan;
}
