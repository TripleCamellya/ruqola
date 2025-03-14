/*
   SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
class QTextEdit;
class EncryptionTestGui : public QWidget
{
    Q_OBJECT
public:
    explicit EncryptionTestGui(QWidget *parent = nullptr);
    ~EncryptionTestGui() override;

private:
    QTextEdit *const mTextEdit;
    QTextEdit *const mTextEditResult;
};
