/*
   Copyright (c) 2018-2021 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "textconvertertest.h"
#include "emoticons/emojimanager.h"
#include "messages/message.h"
#include "ruqola_autotest_helper.h"
#include "textconverter.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTest>

#include <KColorScheme>

QTEST_GUILESS_MAIN(TextConverterTest)

namespace
{
QString prepareExpectedOutput(QString output)
{
    KColorScheme scheme;
    const auto codeBackgroundColor = scheme.background(KColorScheme::AlternateBackground).color().name();
    const auto codeBorderColor = scheme.foreground(KColorScheme::InactiveText).color().name();

    const auto userHighlightForegroundColor = scheme.foreground(KColorScheme::PositiveText).color().name();
    const auto userHighlightBackgroundColor = scheme.background(KColorScheme::PositiveBackground).color().name();

    output.replace(QLatin1String("$BGCOLOR$"), codeBackgroundColor);
    output.replace(QLatin1String("$BORDERCOLOR$"), codeBorderColor);
    output.replace(QLatin1String("$USERCOLOR$"), userHighlightForegroundColor);
    output.replace(QLatin1String("$USERBGCOLOR$"), userHighlightBackgroundColor);

    output.prepend(QLatin1String("<qt>"));
    output.append(QLatin1String("</qt>"));

    return output;
}
}

TextConverterTest::TextConverterTest(QObject *parent)
    : QObject(parent)
{
}

void TextConverterTest::shouldConvertText_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("simpletext") << QStringLiteral("foo") << QStringLiteral("<div>foo</div>");
    QTest::newRow("customemojiwithoutmanager") << QStringLiteral(":foo:") << QStringLiteral("<div>:foo:</div>");

    QTest::newRow("testurl") << QStringLiteral("http://www.kde.org http://www.kde.org")
                             << QStringLiteral(
                                    "<div><a href=\"http://www.kde.org\">http://www.kde.org</a> <a href=\"http://www.kde.org\">http://www.kde.org</a></div>");
    QTest::newRow("named-url") << QStringLiteral("[example.com](http://example.com)")
                               << QStringLiteral("<div><a href=\"http://example.com\">example.com</a></div>");
    QTest::newRow("bold") << QStringLiteral("*bla*") << QStringLiteral("<div><b>bla</b></div>");
    QTest::newRow("multi star") << QStringLiteral("**bla**") << QStringLiteral("<div><b>bla</b></div>");
    QTest::newRow("multi star2") << QStringLiteral("***bla***") << QStringLiteral("<div>***bla***</div>");
    QTest::newRow("multi star3") << QStringLiteral("***bla ******") << QStringLiteral("<div>***bla ******</div>");
    QTest::newRow("Remove <br/>") << QStringLiteral("foo<br />") << QStringLiteral("<div>foo</div>");

    QTest::newRow("0.6.3") << QStringLiteral("0.6.3") << QStringLiteral("<div>0.6.3</div>");
    // Bug 391520
    QTest::newRow("multi-line") << QStringLiteral("These are the options:\n- a\n- b")
                                << QStringLiteral("<div>These are the options:<br />\n- a<br />\n- b</div>");

    QTest::newRow("word@") << QStringLiteral("@foo") << QStringLiteral("<div><a href='ruqola:/user/foo'>@foo</a></div>");
    QTest::newRow("word@-2") << QStringLiteral("@foo.bla") << QStringLiteral("<div><a href='ruqola:/user/foo.bla'>@foo.bla</a></div>");
    QTest::newRow("word@-3") << QStringLiteral("@foo.bla.bli") << QStringLiteral("<div><a href='ruqola:/user/foo.bla.bli'>@foo.bla.bli</a></div>");
    QTest::newRow("word@-4") << QStringLiteral("@foo.bla.bli dd") << QStringLiteral("<div><a href='ruqola:/user/foo.bla.bli'>@foo.bla.bli</a> dd</div>");
    QTest::newRow("word@-5") << QStringLiteral("bla bla 21 @foo.bla.bli dd")
                             << QStringLiteral("<div>bla bla 21 <a href='ruqola:/user/foo.bla.bli'>@foo.bla.bli</a> dd</div>");
    QTest::newRow("word@-6") << QStringLiteral("@foo-bla") << QStringLiteral("<div><a href='ruqola:/user/foo-bla'>@foo-bla</a></div>");
    QTest::newRow("word@-7") << QStringLiteral("@foo_bla") << QStringLiteral("<div><a href='ruqola:/user/foo_bla'>@foo_bla</a></div>");
    QTest::newRow("word@-8") << QStringLiteral("bli@foo_bla") << QStringLiteral("<div>bli@foo_bla</div>");

    QTest::newRow("word#") << QStringLiteral("#foo") << QStringLiteral("<div><a href='ruqola:/room/foo'>#foo</a></div>");
    QTest::newRow("word#-2") << QStringLiteral("#foo.bla") << QStringLiteral("<div><a href='ruqola:/room/foo.bla'>#foo.bla</a></div>");
    QTest::newRow("word#-3") << QStringLiteral("#foo.bla.bli") << QStringLiteral("<div><a href='ruqola:/room/foo.bla.bli'>#foo.bla.bli</a></div>");
    QTest::newRow("word#-4") << QStringLiteral("#foo.bla.bli dd") << QStringLiteral("<div><a href='ruqola:/room/foo.bla.bli'>#foo.bla.bli</a> dd</div>");
    QTest::newRow("word#-5") << QStringLiteral("bla bla 21 #foo.bla.bli dd")
                             << QStringLiteral("<div>bla bla 21 <a href='ruqola:/room/foo.bla.bli'>#foo.bla.bli</a> dd</div>");
    QTest::newRow("word#-6") << QStringLiteral("#foo-bla") << QStringLiteral("<div><a href='ruqola:/room/foo-bla'>#foo-bla</a></div>");
    QTest::newRow("word#-7") << QStringLiteral("#foo_bla") << QStringLiteral("<div><a href='ruqola:/room/foo_bla'>#foo_bla</a></div>");
    // Test parsing when it's in an url... don't replace it.

    QTest::newRow("url") << QStringLiteral("http://www.kde.org#foo_bla")
                         << QStringLiteral("<div><a href=\"http://www.kde.org#foo_bla\">http://www.kde.org#foo_bla</a></div>");

    QTest::newRow("0.6.3") << QStringLiteral("0.6.3") << QStringLiteral("<div>0.6.3</div>");
    QTest::newRow("`code`")
        << QStringLiteral("foo `bla` bar `asdf` test")
        << QStringLiteral("<div>foo <code style='background-color:$BGCOLOR$'>bla</code> bar <code style='background-color:$BGCOLOR$'>asdf</code> test</div>");
    QTest::newRow("`code \\` escaped`") << QStringLiteral("foo `bla\\` bar \\`asdf` test")
                                        << QStringLiteral("<div>foo <code style='background-color:$BGCOLOR$'>bla\\` bar \\`asdf</code> test</div>");
    QTest::newRow("quote<")
        << QStringLiteral("`inside quotes: <<....` this is outside the quotes <<.")
        << QStringLiteral("<div><code style='background-color:$BGCOLOR$'>inside quotes: &lt;&lt;....</code> this is outside the quotes &lt;&lt;.</div>");
}

void TextConverterTest::shouldConvertText()
{
    QFETCH(QString, input);
    QFETCH(QString, output);

    output = prepareExpectedOutput(output);

    QEXPECT_FAIL("multi star", "Bug in kf5", Continue);
    QEXPECT_FAIL("Remove <br/>", "Bug in kf5", Continue);

    QString needUpdateMessageId;
    QCOMPARE(TextConverter::convertMessageText(input, QString(), {}, {}, nullptr, nullptr, needUpdateMessageId),
             output); // TODO add autotests for highlightwords
}

void TextConverterTest::shouldHighlightWords_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("username");
    QTest::addColumn<QStringList>("highlightWords");
    QTest::addColumn<QString>("output");
    QTest::newRow("empty") << QString() << QString() << QStringList{} << QString();
    const QStringList highlightWords{QStringLiteral("ruqola"), QStringLiteral("kde")};
    QTest::newRow("lowercase") << QStringLiteral("Ruqola") << QStringLiteral("foo") << highlightWords
                               << QStringLiteral("<div><a style=\"color:$USERCOLOR$;background-color:$USERBGCOLOR$;\">Ruqola</a></div>");
    QTest::newRow("two-word") << QStringLiteral("Ruqola kde") << QStringLiteral("foo") << highlightWords
                              << QStringLiteral(
                                     "<div><a style=\"color:$USERCOLOR$;background-color:$USERBGCOLOR$;\">Ruqola</a> <a "
                                     "style=\"color:$USERCOLOR$;background-color:$USERBGCOLOR$;\">kde</a></div>");

    QTest::newRow("words") << QStringLiteral("Ruqola bla kde KDE.") << QStringLiteral("foo") << highlightWords
                           << QStringLiteral(
                                  "<div><a style=\"color:$USERCOLOR$;background-color:$USERBGCOLOR$;\">Ruqola</a> bla <a "
                                  "style=\"color:$USERCOLOR$;background-color:$USERBGCOLOR$;\">kde</a> <a "
                                  "style=\"color:$USERCOLOR$;background-color:$USERBGCOLOR$;\">KDE</a>.</div>");

    QTest::newRow("wordinurl") << QStringLiteral("https://www.kde.org/~/bla/bli.txt") << QStringLiteral("bla") << highlightWords
                               << QStringLiteral(
                                      "<div><a href=\"https://www.kde.org/~/bla/bli.txt\">https://www.<a "
                                      "style=\"color:$USERCOLOR$;background-color:$USERBGCOLOR$;\">kde</a>.org/~/bla/bli.txt</a></div>");
}

void TextConverterTest::shouldHighlightWords()
{
    QFETCH(QString, input);
    QFETCH(QString, username);
    QFETCH(QStringList, highlightWords);
    QFETCH(QString, output);

    output = prepareExpectedOutput(output);

    QString needUpdateMessageId;
    QCOMPARE(TextConverter::convertMessageText(input, username, {}, highlightWords, nullptr, nullptr, needUpdateMessageId), output);
}

void TextConverterTest::shouldHighlightText_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("username");
    QTest::addColumn<QString>("output");
    QTest::newRow("empty") << QString() << QString() << QString();
    QTest::newRow("word@1") << QStringLiteral("@foo") << QString() << QStringLiteral("<div><a href='ruqola:/user/foo'>@foo</a></div>");
    KColorScheme colorScheme;
    const auto userMentionForegroundColor = colorScheme.foreground(KColorScheme::ActiveText).color().name();
    const auto userMentionBackgroundColor = colorScheme.background(KColorScheme::ActiveBackground).color().name();
    QTest::newRow("word@1-username") << QStringLiteral("@foo") << QStringLiteral("foo")
                                     << QStringLiteral("<div><a href='ruqola:/user/foo' style=\"color:%1;background-color:%2;\">@foo</a></div>")
                                            .arg(userMentionForegroundColor, userMentionBackgroundColor);
    QTest::newRow("word@2-username") << QStringLiteral("bla bla @foo") << QStringLiteral("foo")
                                     << QStringLiteral("<div>bla bla <a href='ruqola:/user/foo' style=\"color:%1;background-color:%2;\">@foo</a></div>")
                                            .arg(userMentionForegroundColor, userMentionBackgroundColor);
}

void TextConverterTest::shouldHighlightText()
{
    QFETCH(QString, input);
    QFETCH(QString, username);
    QFETCH(QString, output);

    output = prepareExpectedOutput(output);
    QString needUpdateMessageId;
    QCOMPARE(TextConverter::convertMessageText(input, username, {}, {}, nullptr, nullptr, needUpdateMessageId), output);
}

void TextConverterTest::shouldConvertTextWithEmoji_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
    QTest::addColumn<QString>("serverUrl");
    QTest::newRow("empty") << QString() << QString() << QStringLiteral("www.kde.org");
    QTest::newRow("bold") << QStringLiteral("*foo*") << QStringLiteral("<div><b>foo</b></div>") << QStringLiteral("www.kde.org");
    QTest::newRow("italic") << QStringLiteral("_foo_") << QStringLiteral("<div><i>foo</i></div>") << QStringLiteral("www.kde.org");
    // TODO error
    QTest::newRow("italic2") << QStringLiteral("_personal: theming related tasks_") << QStringLiteral("<div><i>personal: theming related tasks</i></div>")
                             << QStringLiteral("www.kde.org");
    QTest::newRow("simpletext") << QStringLiteral("foo") << QStringLiteral("<div>foo</div>") << QStringLiteral("www.kde.org");
    QTest::newRow("customemojiwithmanager") << QStringLiteral(":foo:") << QStringLiteral("<div>:foo:</div>") << QStringLiteral("www.kde.org");

    QString smiley;
    smiley += QChar(0xD83D);
    smiley += QChar(0xDE42);
    const auto smileyText = QStringLiteral("<span style=\"font: x-large NotoColorEmoji\" title=\":slight_smile:\">%1</span>").arg(smiley);
    QTest::newRow("ascii-smiley") << QStringLiteral(":)") << QStringLiteral("<div>%1</div>").arg(smileyText) << QStringLiteral("www.kde.org");
    QTest::newRow("multi-smiley") << QStringLiteral(":):slight_smile::):)") << QStringLiteral("<div>%1</div>").arg(smileyText.repeated(4))
                                  << QStringLiteral("www.kde.org");

    QTest::newRow("url") << QStringLiteral("https://www.kde.org") << QStringLiteral("<div><a href=\"https://www.kde.org\">https://www.kde.org</a></div>")
                         << QStringLiteral("www.kde.org");

    QTest::newRow("\")") << QStringLiteral("\")") << QStringLiteral("<div>&quot;)</div>") << QStringLiteral("www.kde.org");

    //    QTest::newRow("customemojiwithmanager1") << QStringLiteral(":vader:")
    //                                             << QStringLiteral("<div><img height='22' width='22' src='http://www.kde.org/emoji-custom/vader.png'
    //                                             title=':vader:'/></div>")
    //                                             << QStringLiteral("www.kde.org");
    //    QTest::newRow("customemojiwithmanager2") << QStringLiteral(":vader::vader:")
    //                                             << QStringLiteral(
    //        "<div><img height='22' width='22' src='http://www.kde.org/emoji-custom/vader.png' title=':vader:'/><img height='22' width='22'
    //        src='http://www.kde.org/emoji-custom/vader.png' title=':vader:'/></div>")
    //                                             << QStringLiteral("www.kde.org");

    //    //Use server with http://
    //    QTest::newRow("customemojiwithmanager3") << QStringLiteral("the icon for vader is :vader::vader:")
    //                                             << QStringLiteral(
    //        "<div>the icon for vader is <img height='22' width='22' src='http://www.kde.org/emoji-custom/vader.png' title=':vader:'/><img height='22'
    //        width='22' src='http://www.kde.org/emoji-custom/vader.png' title=':vader:'/></div>")
    //                                             << QStringLiteral("http://www.kde.org");

    //    //alias support
    //    QTest::newRow("customemojiwithmanager4-with-alias") << QStringLiteral(":darth::vader:")
    //                                                        << QStringLiteral(
    //        "<div><img height='22' width='22' src='http://www.kde.org/emoji-custom/vader.png' title=':vader:'/><img height='22' width='22'
    //        src='http://www.kde.org/emoji-custom/vader.png' title=':vader:'/></div>")
    //                                                        << QStringLiteral("http://www.kde.org");

    QTest::newRow("quotedcode1") << QStringLiteral("bla```foo```blub")
                                 << QStringLiteral(
                                        "<div>bla</div><table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid "
                                        "$BORDERCOLOR$'><code>foo</code></td></tr></table><div>blub</div>")
                                 << QStringLiteral("www.kde.org");
    QTest::newRow("quotedcode2") << QStringLiteral("bla\n```foo```bli")
                                 << QStringLiteral(
                                        "<div>bla</div><table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid "
                                        "$BORDERCOLOR$'><code>foo</code></td></tr></table><div>bli</div>")
                                 << QStringLiteral("www.kde.org");
    QTest::newRow("quotedcode3") << QStringLiteral("bla\n```foo```")
                                 << QStringLiteral(
                                        "<div>bla</div><table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid "
                                        "$BORDERCOLOR$'><code>foo</code></td></tr></table>")
                                 << QStringLiteral("www.kde.org");
    QTest::newRow("quotedcode4") << QStringLiteral("```foo```\nff")
                                 << QStringLiteral(
                                        "<table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid "
                                        "$BORDERCOLOR$'><code>foo</code></td></tr></table><div>ff</div>")
                                 << QStringLiteral("www.kde.org");
    QTest::newRow("quotedcode5") << QStringLiteral("bla\n```\nfoo\n```\nff")
                                 << QStringLiteral(
                                        "<div>bla</div><table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid "
                                        "$BORDERCOLOR$'><code>foo</code></td></tr></table><div>ff</div>")
                                 << QStringLiteral("www.kde.org");
    QTest::newRow("quotedcode6")
        << QStringLiteral("*foo*\n```\nfoo\n```\n*bar*\n```blub```\n*asdf*")
        << QStringLiteral(
               "<div><b>foo</b></div><table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid "
               "$BORDERCOLOR$'><code>foo</code></td></tr></table><div><b>bar</b></div><table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: "
               "1px solid $BORDERCOLOR$'><code>blub</code></td></tr></table><div><b>asdf</b></div>")
        << QStringLiteral("www.kde.org");
    QTest::newRow("quotedcode7") << QStringLiteral(":)\n```\n:)\n```\n:)")
                                 << QStringLiteral(
                                        "<div>%1</div><table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid "
                                        "$BORDERCOLOR$'><code>:)</code></td></tr></table><div>%1</div>")
                                        .arg(smileyText)
                                 << QStringLiteral("www.kde.org");
    QTest::newRow("quotedcode8")
        << QStringLiteral("```javascript\ncode\n```")
        << QStringLiteral(
               "<table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid $BORDERCOLOR$'><code>code</code></td></tr></table>")
        << QStringLiteral("www.kde.org");
    QTest::newRow("quotedcode9")
        << QStringLiteral("```blub\ncode\n```")
        << QStringLiteral(
               "<table><tr><td style='background-color:$BGCOLOR$; padding: 5px; border: 1px solid $BORDERCOLOR$'><code>blub<br>code</code></td></tr></table>")
        << QStringLiteral("www.kde.org");

    QTest::newRow("inline-code-with-brackets")
        << QStringLiteral("`[[test]]` and `a[b` something")
        << QStringLiteral(
               "<div><code style='background-color:$BGCOLOR$'>[[test]]</code> and <code style='background-color:$BGCOLOR$'>a[b</code> "
               "something</div>")
        << QStringLiteral("www.kde.org");

    QTest::newRow("inline-code-with-tilde") << QStringLiteral("`auto [a, b] = std::minmax_element(~~~);`")
                                            << QStringLiteral(
                                                   "<div><code style='background-color:$BGCOLOR$'>auto [a, b] = "
                                                   "std::minmax_element(~~~);</code></div>")
                                            << QStringLiteral("www.kde.org");

    QTest::newRow("inline-code-with-smiley") << QStringLiteral(":) `:)` :)")
                                             << QStringLiteral("<div>%1 <code style='background-color:$BGCOLOR$'>:)</code> %1</div>").arg(smileyText)
                                             << QStringLiteral("www.kde.org");
}

void TextConverterTest::shouldConvertTextWithEmoji()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    QFETCH(QString, serverUrl);

    output = prepareExpectedOutput(output);

    // Load emoji
    const QString originalJsonFile = QLatin1String(RUQOLA_DATA_DIR) + QLatin1String("/json/restapi/emojiparent.json");
    const QJsonObject obj = AutoTestHelper::loadJsonObject(originalJsonFile);
    EmojiManager manager;
    manager.loadCustomEmoji(obj);
    manager.setServerUrl(serverUrl);

    QString needUpdateMessageId;
    auto actualOutput = TextConverter::convertMessageText(input, QString(), {}, {}, &manager, nullptr, needUpdateMessageId);
    if (QLatin1String(QTest::currentDataTag()) == QLatin1String("quotedcode7")) {
        // remove additional highlighting of the ':)' symbols within the <code> block
        // the text color is syntax highlighting theme dependent, so hard for us to check
        actualOutput.replace(QRegularExpression(QStringLiteral("<code><span style=\".+\">:\\)</span></code>")), QLatin1String("<code>:)</code>"));
    }
    QCOMPARE(actualOutput, output); // TODO add autotest for highlightwords
}
