/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "applicationssettingslogsinfo.h"
#include "ruqola_debug.h"

#include <QJsonObject>
using namespace Qt::Literals::StringLiterals;

ApplicationsSettingsLogsInfo::ApplicationsSettingsLogsInfo() = default;

ApplicationsSettingsLogsInfo::~ApplicationsSettingsLogsInfo() = default;

void ApplicationsSettingsLogsInfo::parseLogs(const QJsonObject &obj)
{
    mMethod = obj["method"_L1].toString();
}

bool ApplicationsSettingsLogsInfo::operator==(const ApplicationsSettingsLogsInfo &other) const
{
    // TODO
    return mMethod == other.method() && mArguments == other.arguments();
}

QList<ApplicationsSettingsLogsInfo::LogsArgument> ApplicationsSettingsLogsInfo::arguments() const
{
    return mArguments;
}

void ApplicationsSettingsLogsInfo::setArguments(const QList<LogsArgument> &newArguments)
{
    mArguments = newArguments;
}

QString ApplicationsSettingsLogsInfo::method() const
{
    return mMethod;
}

void ApplicationsSettingsLogsInfo::setMethod(const QString &newMethod)
{
    mMethod = newMethod;
}

QDebug operator<<(QDebug d, const ApplicationsSettingsLogsInfo &t)
{
    d.space() << "method" << t.method();
    d.space() << "arguments" << t.arguments();
    return d;
}

bool ApplicationsSettingsLogsInfo::LogsArgument::operator==(const LogsArgument &other) const
{
    return caller == other.caller && method == other.method && severity == other.severity && args == other.args;
}

QDebug operator<<(QDebug d, const ApplicationsSettingsLogsInfo::LogsArgument &arg)
{
    d.space() << "caller" << arg.caller;
    d.space() << "method" << arg.method;
    d.space() << "severity" << arg.severity;
    d.space() << "args" << arg.args;
    return d;
}
