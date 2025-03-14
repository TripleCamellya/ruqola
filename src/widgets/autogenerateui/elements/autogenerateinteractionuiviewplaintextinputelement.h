/*
   SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "autogenerateui/autogenerateinteractionuiviewactionable.h"

class AutoGenerateInteractionUiViewText;
// see ./packages/ui-kit/src/blocks/elements/PlainTextInputElement.ts
class LIBRUQOLAWIDGETS_TESTS_EXPORT AutoGenerateInteractionUiViewPlainTextInputElement : public AutoGenerateInteractionUiViewActionable
{
    Q_OBJECT
public:
    explicit AutoGenerateInteractionUiViewPlainTextInputElement(QObject *parent = nullptr);
    ~AutoGenerateInteractionUiViewPlainTextInputElement() override;

    [[nodiscard]] QWidget *generateWidget(RocketChatAccount *account, QWidget *parent) override;

    [[nodiscard]] AutoGenerateInteractionUiViewText *placeHolder() const;
    void setPlaceHolder(AutoGenerateInteractionUiViewText *newPlaceHolder);

    [[nodiscard]] QString initialValue() const;
    void setInitialValue(const QString &newInitialValue);

    [[nodiscard]] bool multiLine() const;
    void setMultiLine(bool newMultiLine);

    [[nodiscard]] bool operator==(const AutoGenerateInteractionUiViewPlainTextInputElement &other) const;

    [[nodiscard]] int minLength() const;
    void setMinLength(int newMinLength);

    [[nodiscard]] int maxLength() const;
    void setMaxLength(int newMaxLength);

protected:
    void parseElement(const QJsonObject &json) override;
    void serializeElement(QJsonObject &o) const override;

private:
    AutoGenerateInteractionUiViewText *mPlaceHolder = nullptr;
    QString mInitialValue;
    bool mMultiLine = false;
    int mMinLength = -1;
    int mMaxLength = -1;
};
Q_DECLARE_METATYPE(AutoGenerateInteractionUiViewPlainTextInputElement)
LIBRUQOLAWIDGETS_EXPORT QDebug operator<<(QDebug d, const AutoGenerateInteractionUiViewPlainTextInputElement &t);
