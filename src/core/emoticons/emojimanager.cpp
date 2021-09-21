/*
   Copyright (c) 2018-2021 Laurent Montel <montel@kde.org>

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

#include "emoticons/emojimanager.h"
#include "emoticons/unicodeemoticonmanager.h"
#include "rocketchataccount.h"
#include "ruqola.h"
#include "ruqola_debug.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpressionMatch>
#include <QTextStream>

EmojiManager::EmojiManager(QObject *parent)
    : QObject(parent)
{
}

EmojiManager::~EmojiManager()
{
}

QVector<UnicodeEmoticon> EmojiManager::unicodeEmojiList() const
{
    return UnicodeEmoticonManager::self()->unicodeEmojiList();
}

QVector<EmoticonCategory> EmojiManager::categories() const
{
    return UnicodeEmoticonManager::self()->categories();
}

QVector<UnicodeEmoticon> EmojiManager::emojisForCategory(const QString &category) const
{
    return UnicodeEmoticonManager::self()->emojisForCategory(category);
}

void EmojiManager::addUpdateEmojiCustomList(const QJsonArray &arrayEmojiCustomArray)
{
    for (int i = 0; i < arrayEmojiCustomArray.count(); ++i) {
        const QJsonObject obj = arrayEmojiCustomArray.at(i).toObject();
        const QJsonObject customEmojiObj = obj.value(QLatin1String("emojiData")).toObject();
        if (!customEmojiObj.isEmpty()) {
            if (customEmojiObj.contains(QLatin1String("_id"))) {
                bool found = false;
                const QString identifier = customEmojiObj.value(QLatin1String("_id")).toString();
                for (auto emoji : std::as_const(mCustomEmojiList)) {
                    if (emoji.identifier() == identifier) {
                        emoji.parseEmoji(customEmojiObj, true);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    // Parse
                    CustomEmoji newStatus;
                    newStatus.parseEmoji(customEmojiObj, true);
                    if (newStatus.isValid()) {
                        mCustomEmojiList.append(newStatus);
                    }
                }
            } else {
                qCWarning(RUQOLA_LOG) << "addUpdateEmojiCustomList invalid QJsonObject" << customEmojiObj;
            }
        }
    }

    // New QJsonArray([{"emojiData":{"_id":"HdN28k4PQ6J9xLkZ8","_updatedAt":{"$date":1631885946222},"aliases":["roo"],"extension":"png","name":"ruqola"}}])
    // Update
    // QJsonArray([{"emojiData":{"_id":"vxE6eG5FrZCvbgM3t","aliases":["rooss"],"extension":"png","name":"xxx","newFile":true,"previousExtension":"png","previousName":"ruqolas"}}
    Q_EMIT customEmojiChanged();
}

void EmojiManager::deleteEmojiCustom(const QJsonArray &arrayEmojiCustomArray)
{
    // ([{"emojiData":{"_id":"PpawhZMaseBcEuGCG","_updatedAt":{"$date":1631858916014},"aliases":[],"extension":"png","name":"ruqolaff"}}])
    const int count{arrayEmojiCustomArray.count()};
    for (int i = 0; i < count; ++i) {
        const QJsonObject obj = arrayEmojiCustomArray.at(i).toObject();
        const QJsonObject emojiData = obj.value(QStringLiteral("emojiData")).toObject();
        const QString identifier = emojiData.value(QStringLiteral("_id")).toString();
        if (!identifier.isEmpty()) {
            for (const auto &emoji : std::as_const(mCustomEmojiList)) {
                if (emoji.identifier() == identifier) {
                    mCustomEmojiList.removeAll(emoji);
                    break;
                }
            }
        }
    }
    Q_EMIT customEmojiChanged();
}

void EmojiManager::loadCustomEmoji(const QJsonObject &obj)
{
    mCustomEmojiList.clear();
    const QJsonObject result = obj.value(QLatin1String("emojis")).toObject();
    const QJsonArray array = result.value(QLatin1String("update")).toArray();
    // TODO add support for remove when we store it in local
    for (int i = 0, total = array.size(); i < total; ++i) {
        const QJsonObject emojiJson = array.at(i).toObject();
        CustomEmoji emoji;
        emoji.parseEmoji(emojiJson);
        if (emoji.isValid()) {
            mCustomEmojiList.append(emoji);
        }
    }

    // clear cache
    mReplacePatternDirty = true;
}

int EmojiManager::count() const
{
    return mCustomEmojiList.count() + UnicodeEmoticonManager::self()->count();
}

bool EmojiManager::isAnimatedImage(const QString &emojiIdentifier) const
{
    if (emojiIdentifier.startsWith(QLatin1Char(':')) && emojiIdentifier.endsWith(QLatin1Char(':'))) {
        for (int i = 0, total = mCustomEmojiList.size(); i < total; ++i) {
            const CustomEmoji emoji = mCustomEmojiList.at(i);
            if (emoji.hasEmoji(emojiIdentifier)) {
                return emoji.isAnimatedImage();
            }
        }
    }
    return false;
}

UnicodeEmoticon EmojiManager::unicodeEmoticonForEmoji(const QString &emojiIdentifier) const
{
    return UnicodeEmoticonManager::self()->unicodeEmoticonForEmoji(emojiIdentifier);
}

QString EmojiManager::customEmojiFileName(const QString &emojiIdentifier) const
{
    for (const CustomEmoji &customEmoji : mCustomEmojiList) {
        if (customEmoji.hasEmoji(emojiIdentifier)) {
            return customEmoji.emojiFileName();
        }
    }
    return {};
}

QString EmojiManager::normalizedReactionEmoji(const QString &emojiIdentifier) const
{
    for (const auto &customEmoji : mCustomEmojiList) {
        if (customEmoji.hasEmoji(emojiIdentifier)) {
            return customEmoji.emojiIdentifier();
        }
    }
    const auto unicodeEmojis = unicodeEmojiList();
    for (const auto &unicodeEmoji : unicodeEmojis) {
        if (unicodeEmoji.hasEmoji(emojiIdentifier)) {
            return unicodeEmoji.identifier();
        }
    }
    return emojiIdentifier;
}

QString EmojiManager::replaceEmojiIdentifier(const QString &emojiIdentifier, bool isReaction)
{
    if (mServerUrl.isEmpty()) {
        qCWarning(RUQOLA_LOG) << "Server Url not defined";
        return emojiIdentifier;
    }
    auto rocketChatAccount = Ruqola::self()->rocketChatAccount();
    if (!rocketChatAccount->ownUserPreferences().convertAsciiEmoji()) {
        return emojiIdentifier;
    }
    if (emojiIdentifier.startsWith(QLatin1Char(':')) && emojiIdentifier.endsWith(QLatin1Char(':'))) {
        for (const CustomEmoji &emoji : std::as_const(mCustomEmojiList)) {
            if (emoji.hasEmoji(emojiIdentifier)) {
                QString cachedHtml = emoji.cachedHtml();
                if (cachedHtml.isEmpty()) {
                    // For the moment we can't support animated image as emoticon in text. Only as Reaction.
                    if (emoji.isAnimatedImage() && isReaction) {
                        cachedHtml = emoji.generateAnimatedUrlFromCustomEmoji(mServerUrl);
                    } else {
                        const QString fileName = customEmojiFileName(emojiIdentifier);
                        if (!fileName.isEmpty()) {
                            const QUrl emojiUrl = rocketChatAccount->attachmentUrl(fileName);
                            if (emojiUrl.isEmpty()) {
                                // The download is happening, this will all be updated again later
                            } else {
                                cachedHtml = emoji.generateHtmlFromCustomEmojiLocalPath(emojiUrl.path());
                            }
                        } else {
                            qCDebug(RUQOLA_LOG) << " Impossible to find custom emoji " << emojiIdentifier;
                        }
                    }
                }
                return cachedHtml;
            }
        }
    }

    const UnicodeEmoticon unicodeEmoticon = unicodeEmoticonForEmoji(emojiIdentifier);
    if (unicodeEmoticon.isValid()) {
        return unicodeEmoticon.unicodeDisplay();
    }

    return emojiIdentifier;
}

void EmojiManager::replaceEmojis(QString *str)
{
    Q_ASSERT(str);
    if (mReplacePatternDirty) {
        // build a regexp pattern for all the possible emoticons we want to replace
        // i.e. this is going to build a pattern like this:
        // \:smiley\:|\:\-\)|...
        // to optimize it a bit, we use a common pattern that matches most
        // emojis and then we only need to add the other special (ascii) ones
        // otherwise the pattern could become extremely long
        //
        // furthermore, we don't want to replace emojis (esp. non-colon escaped ones) in the
        // middle of another string, such as within a URL or such. at the same time, multiple
        // smileys may come after another...
        const auto commonPattern = QLatin1String(":[\\w\\-]+:");
        // TODO: use QRegularExpression::anchoredPattern once ruqola depends on Qt 5.15
        static const QRegularExpression common(QLatin1Char('^') + commonPattern + QLatin1Char('$'));

        QString pattern;
        QTextStream stream(&pattern);
        // prevent replacements within other strings, use a negative-lookbehind to rule out
        // that we are within some word or link or such
        stream << "(?<![\\w\\-])";
        // put all other patterns in a non-capturing group
        stream << "(?:";
        stream << commonPattern;

        auto addEmoji = [&](const QString &string) {
            if (common.match(string).hasMatch()) {
                return;
            }
            stream << '|';
            stream << QRegularExpression::escape(string);
        };
        auto addEmojis = [&](const auto &emojis) {
            for (const auto &emoji : emojis) {
                addEmoji(emoji.identifier());
                const auto aliases = emoji.aliases();
                for (const auto &alias : aliases) {
                    addEmoji(alias);
                }
            }
        };

        addEmojis(mCustomEmojiList);
        addEmojis(unicodeEmojiList());
        // close non-capturing group
        stream << ")";
        stream.flush();

        mReplacePattern.setPattern(pattern);
        mReplacePattern.optimize();
        mReplacePatternDirty = false;
    }

    if (mReplacePattern.pattern().isEmpty() || !mReplacePattern.isValid()) {
        qCWarning(RUQOLA_LOG) << "invalid emoji replace pattern" << mReplacePattern.pattern() << mReplacePattern.errorString();
        return;
    }

    int offset = 0;
    while (offset < str->size()) {
        const auto match = mReplacePattern.match(str, offset);
        if (!match.hasMatch()) {
            break;
        }
        const auto word = match.captured();
        const auto replaceWord = replaceEmojiIdentifier(word);
        str->replace(match.capturedStart(), word.size(), replaceWord);
        offset = match.capturedStart() + replaceWord.size();
    }
}

QString EmojiManager::serverUrl() const
{
    return mServerUrl;
}

void EmojiManager::setServerUrl(const QString &serverUrl)
{
    if (mServerUrl != serverUrl) {
        mServerUrl = serverUrl;
        clearCustomEmojiCachedHtml();
    }
}

void EmojiManager::clearCustomEmojiCachedHtml()
{
    for (int i = 0, total = mCustomEmojiList.size(); i < total; ++i) {
        mCustomEmojiList[i].clearCachedHtml();
    }
}

const QVector<CustomEmoji> &EmojiManager::customEmojiList() const
{
    return mCustomEmojiList;
}
