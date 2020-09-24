/*
   Copyright (c) 2017-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License or
   ( at your option ) version 3 or, at the discretion of KDE e.V.
   ( which shall act as a proxy as in section 14 of the GPLv3 ), any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "emoticons/customemoji.h"
#include "utils.h"

#include <QJsonObject>
#include <QJsonArray>

CustomEmoji::CustomEmoji()
{
}

CustomEmoji::~CustomEmoji()
{
}

bool CustomEmoji::hasEmoji(const QString &identifier) const
{
    return (mEmojiIdentifier == identifier) || mAliases.contains(identifier);
}

qint64 CustomEmoji::updatedAt() const
{
    return mUpdatedAt;
}

void CustomEmoji::setUpdatedAt(qint64 updatedAt)
{
    mUpdatedAt = updatedAt;
}

bool CustomEmoji::isAnimatedImage() const
{
    return mExtension == QLatin1String("gif");
}

void CustomEmoji::parseEmoji(const QJsonObject &emoji)
{
    mIdentifier = emoji.value(QLatin1String("_id")).toString();
    mExtension = emoji.value(QLatin1String("extension")).toString();
    mName = emoji.value(QLatin1String("name")).toString();
    mEmojiIdentifier = QLatin1Char(':') + mName + QLatin1Char(':');
    mUpdatedAt = Utils::parseIsoDate(QStringLiteral("_updatedAt"), emoji);
    const QJsonArray array = emoji.value(QLatin1String("aliases")).toArray();
    const int arrayCount = array.count();
    QStringList lst;
    lst.reserve(arrayCount);
    for (int i = 0; i < arrayCount; ++i) {
        lst.append(QLatin1Char(':') + array.at(i).toString() + QLatin1Char(':'));
    }
    mAliases = lst;
}

bool CustomEmoji::isValid() const
{
    //Add more check ?
    return !mIdentifier.isEmpty() && !mName.isEmpty();
}

QString CustomEmoji::emojiUrl(const QString &serverUrl) const
{
    QString url = serverUrl + emojiFileName();
    // ???? http ? not https ???
    if (!url.startsWith(QLatin1String("http://")) && !url.startsWith(QLatin1String("https://"))) {
        url.prepend(QLatin1String("http://"));
    }
    return url;
}

QString CustomEmoji::emojiFileName() const
{
    return QStringLiteral("/emoji-custom/%1.%2").arg(mName, mExtension);
}

QString CustomEmoji::generateAnimatedUrlFromCustomEmoji(const QString &serverUrl) const
{
    if (mCachedHtml.isEmpty()) {
        const QString url = emojiUrl(serverUrl);
        //https://rocket.chat/docs/developer-guides/realtime-api/method-calls/list-custom-emoji/#list-custom-emoji
        //http://yourhost.com/emoji-custom/Emoji%20Name.png
        //TODO customize size.
        mCachedHtml = url;
    }
    return mCachedHtml;
}

QString CustomEmoji::generateHtmlFromCustomEmojiLocalPath(const QString &emojoLocalPath) const
{
    if (mCachedHtml.isEmpty()) {
        mCachedHtml = QStringLiteral("<img height='22' width='22' src='%1' title='%2'/>").arg(emojoLocalPath, mEmojiIdentifier);
    }
    return mCachedHtml;
}

void CustomEmoji::clearCachedHtml()
{
    mCachedHtml.clear();
}

QStringList CustomEmoji::aliases() const
{
    return mAliases;
}

void CustomEmoji::setAliases(const QStringList &aliases)
{
    mAliases = aliases;
}

QString CustomEmoji::emojiIdentifier() const
{
    return mEmojiIdentifier;
}

void CustomEmoji::setEmojiIdentifier(const QString &emojiIdentifier)
{
    mEmojiIdentifier = emojiIdentifier;
}

QString CustomEmoji::cachedHtml() const
{
    return mCachedHtml;
}

QString CustomEmoji::identifier() const
{
    return mIdentifier;
}

void CustomEmoji::setIdentifier(const QString &identifier)
{
    mIdentifier = identifier;
}

QString CustomEmoji::extension() const
{
    return mExtension;
}

void CustomEmoji::setExtension(const QString &extension)
{
    mExtension = extension;
}

void CustomEmoji::setName(const QString &name)
{
    mName = name;
}

QString CustomEmoji::name() const
{
    return mName;
}

bool CustomEmoji::operator==(const CustomEmoji &other) const
{
    return (mName == other.name())
           && (mExtension == other.extension())
           && (mIdentifier == other.identifier())
           && (mAliases == other.aliases())
           && (mEmojiIdentifier == other.emojiIdentifier())
           && (mUpdatedAt == other.updatedAt());
}

QDebug operator <<(QDebug d, const CustomEmoji &t)
{
    d << "Name: " << t.name() << " ";
    d << "Identifier: " << t.identifier() << " ";
    d << "extension: " << t.extension() << " ";
    d << "aliases: " << t.aliases() << " ";
    d << "UpdatedAt: " << t.updatedAt() << " ";
    d << "EmojiIdentifier: " << t.emojiIdentifier();
    return d;
}
