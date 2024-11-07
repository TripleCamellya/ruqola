/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "connection.h"
#include "authenticationmanager/restauthenticationmanager.h"
#include "restapimethod.h"
#include "ruqola.h"
#include "ruqola_debug.h"

#include "downloadfilejob.h"
#include "serverinfojob.h"
#include "settings/privateinfojob.h"

#include "rooms/roomsmuteuserjob.h"
#include "rooms/roomsunmuteuserjob.h"
#include "users/forgotpasswordjob.h"
#include "users/getavatarjob.h"
#include "users/getpresencejob.h"
#include "users/userinfojob.h"
#include "users/userspresencejob.h"

#include "misc/owninfojob.h"

#include "authentication/logoutjob.h"

#include "chat/getmentionedmessagesjob.h"
#include "chat/getmessagejob.h"
#include "chat/getpinnedmessagesjob.h"
#include "chat/getsnippetedmessagesjob.h"
#include "chat/getstarredmessagesjob.h"
#include "chat/getthreadmessagesjob.h"
#include "chat/getthreadsjob.h"
#include "chat/ignoreuserjob.h"
#include "chat/postmessagejob.h"
#include "chat/reactonmessagejob.h"
#include "chat/sendmessagejob.h"
#include "chat/syncthreadmessagesjob.h"
#include "chat/updatemessagejob.h"

#include "channels/channeladdleaderjob.h"
#include "channels/channeladdmoderatorjob.h"
#include "channels/channeladdownerjob.h"
#include "channels/channeldeletejob.h"
#include "channels/channelfilesjob.h"
#include "channels/channelgetallusermentionsjob.h"
#include "channels/channelgetcountersjob.h"
#include "channels/channelinvitejob.h"
#include "channels/channelkickjob.h"
#include "channels/channelmembersjob.h"
#include "channels/channelremoveleaderjob.h"
#include "channels/channelremovemoderatorjob.h"
#include "channels/channelremoveownerjob.h"
#include "channels/createchanneljob.h"
#include "channels/getchannelrolesjob.h"
#include "channels/leavechanneljob.h"
#include "channels/setchanneltypejob.h"
#include "channels/setjoincodechanneljob.h"

#include "groups/changegroupsnamejob.h"
#include "groups/creategroupsjob.h"
#include "groups/getgrouprolesjob.h"
#include "groups/groupaddleaderjob.h"
#include "groups/groupaddmoderatorjob.h"
#include "groups/groupaddownerjob.h"
#include "groups/groupremoveleaderjob.h"
#include "groups/groupremovemoderatorjob.h"
#include "groups/groupremoveownerjob.h"
#include "groups/groupsdeletejob.h"
#include "groups/groupsinvitejob.h"
#include "groups/groupskickjob.h"
#include "groups/leavegroupsjob.h"

#include "rooms/getdiscussionsjob.h"
#include "rooms/roomfavoritejob.h"
#include "rooms/roomstartdiscussionjob.h"
#include "rooms/savenotificationjob.h"

#include "subscriptions/markroomasreadjob.h"

#include "permissions/permissionslistalljob.h"

#include "autotranslate/translatesavesettingsjob.h"

#include "custom/customuserstatuslistjob.h"

#include "2fa/user2fasendemailcodejob.h"

#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QTimer>

using namespace std::chrono_literals;
using namespace RocketChatRestApi;

Connection::Connection(QObject *parent)
    : QObject(parent)
    , mNetworkAccessManager(new QNetworkAccessManager(this))
    , mCookieJar(new QNetworkCookieJar(this))
    , mRestApiMethod(new RestApiMethod)
    , mRESTAuthenticationManager(new RESTAuthenticationManager(this, this))
{
    mNetworkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    mNetworkAccessManager->setCookieJar(mCookieJar);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &Connection::slotResult);
    connect(mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &Connection::slotSslErrors);
    if (Ruqola::self()->useRestApiLogin()) {
        connect(mRESTAuthenticationManager, &RESTAuthenticationManager::loginStatusChanged, this, &Connection::loginStatusChanged);
    }
}

Connection::~Connection()
{
    delete mRestApiMethod;
}

RESTAuthenticationManager *Connection::authenticationManager() const
{
    return mRESTAuthenticationManager;
}

void Connection::setRestApiLogger(RocketChatRestApi::AbstractLogger *logger)
{
    mRuqolaLogger = logger;
}

void Connection::initializeCookies()
{
    const QString url = serverUrl();
    if (!url.isEmpty()) {
        QString host;
        const QList<QStringView> lsthost = QStringView(url).split(QStringLiteral("//"));
        if (lsthost.count() < 2) {
            host = url;
        } else {
            host = lsthost.at(1).toString();
        }

        if (!mUserId.isEmpty()) {
            QNetworkCookie userIdCookie;
            userIdCookie.setDomain(host);
            userIdCookie.setName(QByteArrayLiteral("rc_uid"));
            userIdCookie.setValue(mUserId.toLocal8Bit());
            mCookieJar->insertCookie(userIdCookie);
        }

        if (!mAuthToken.isEmpty()) {
            QNetworkCookie tokenCookie;
            tokenCookie.setDomain(host);
            tokenCookie.setName(QByteArrayLiteral("rc_token"));
            tokenCookie.setValue(mAuthToken.toLocal8Bit());
            mCookieJar->insertCookie(tokenCookie);
        }
    } else {
        qCWarning(RUQOLA_LOG) << "We can not initialize cookies as server url is empty.";
    }
}

void Connection::setAuthToken(const QString &authToken)
{
    const bool isChanged = (mAuthToken != authToken);
    mAuthToken = authToken;
    if (isChanged) {
        if (!mAuthToken.isEmpty()) {
            initializeCookies();
        }
    }
}

void Connection::setUserId(const QString &userId)
{
    const bool isChanged = (mUserId != userId);
    mUserId = userId;
    if (isChanged) {
        if (!mUserId.isEmpty()) {
            initializeCookies();
        }
    }
}

void Connection::slotResult(QNetworkReply *reply)
{
    const auto error = reply->error();
    if (error != QNetworkReply::NoError) {
        const auto jobClassName = reply->property("jobClassName").value<QByteArray>();
        qCWarning(RUQOLA_LOG) << jobClassName << "error reply: " << reply->errorString() << " ERROR type " << error;

        if (RocketChatRestApi::networkErrorsNeedingReconnect().contains(error) && !mNetworkErrorEmitted) {
            mNetworkErrorEmitted = true;
            QTimer::singleShot(1ms, this, [this] {
                // This will delete "this" Connection, so no need to reset mNetworkErrorEmitted
                Q_EMIT networkError();
            });
        }
    }
}

void Connection::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    qCDebug(RUQOLA_LOG) << " void RestApiRequest::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)" << error.count();
    reply->ignoreSslErrors(error);
}

void Connection::setPassword(const QString &password)
{
    mPassword = password;
}

void Connection::setUserName(const QString &userName)
{
    mUserName = userName;
}

QString Connection::serverUrl() const
{
    return mRestApiMethod->serverUrl();
}

void Connection::setServerUrl(const QString &serverUrl)
{
    mRestApiMethod->setServerUrl(serverUrl);
}

QString Connection::authToken() const
{
    return mAuthToken;
}

QString Connection::userId() const
{
    return mUserId;
}

void Connection::slotLogin(const QString &authToken, const QString &userId)
{
    mAuthToken = authToken;
    mUserId = userId;
}

void Connection::slotLogout()
{
    mAuthToken.clear();
    mUserId.clear();
}

void Connection::initializeRestApiJob(RocketChatRestApi::RestApiAbstractJob *job)
{
    connect(job, &RocketChatRestApi::RestApiAbstractJob::failed, this, &Connection::failed);

    job->setNetworkAccessManager(mNetworkAccessManager);
    job->setRestApiLogger(mRuqolaLogger);
    job->setRestApiMethod(mRestApiMethod);
    if (job->requireHttpAuthentication()) {
        job->setAuthToken(mAuthToken);
        job->setUserId(mUserId);
    }
}

void Connection::logout()
{
    auto job = new LogoutJob(this);
    connect(job, &LogoutJob::logoutDone, this, &Connection::slotLogout);
    initializeRestApiJob(job);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start LogoutJob job";
    }
}

void Connection::getAvatar(const UserBaseJob::UserInfo &info)
{
    auto job = new GetAvatarJob(this);
    connect(job, &GetAvatarJob::avatar, this, &Connection::avatar);
    initializeRestApiJob(job);
    job->setUserInfo(info);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start getAvatar job";
    }
}

void Connection::getPrivateSettings()
{
    auto job = new PrivateInfoJob(this);
    connect(job, &PrivateInfoJob::privateInfoDone, this, &Connection::privateInfoDone);
    initializeRestApiJob(job);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start getPrivateSettings job";
    }
}

void Connection::getOwnInfo()
{
    auto job = new OwnInfoJob(this);
    connect(job, &OwnInfoJob::ownInfoDone, this, &Connection::getOwnInfoDone);
    initializeRestApiJob(job);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start getOwnInfo job";
    }
}

DownloadFileJob *Connection::downloadFile(const QUrl &url, const QUrl &localFileUrl, const QByteArray &mimeType, bool requiredAuthentication)
{
    auto job = new DownloadFileJob(this);
    connect(job, &DownloadFileJob::downloadFileDone, this, &Connection::downloadFileDone);
    job->setUrl(url);
    job->setMimeType(mimeType);
    job->setLocalFileUrl(localFileUrl);
    job->setRequiredAuthentication(requiredAuthentication);
    initializeRestApiJob(job);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start DownloadFileJob job";
    }
    return job;
}

void Connection::serverInfo()
{
    auto job = new ServerInfoJob(this);
    initializeRestApiJob(job);
    connect(job, &ServerInfoJob::serverInfoDone, this, &Connection::serverInfoDone);
    connect(job, &ServerInfoJob::serverInfoFailed, this, &Connection::serverInfoFailed);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start ServerInfoJob job";
    }
}

void Connection::postMessage(const QByteArray &roomId, const QString &text)
{
    auto job = new PostMessageJob(this);
    connect(job, &PostMessageJob::postMessageDone, this, &Connection::postMessageDone);
    initializeRestApiJob(job);
    job->setRoomIds({roomId});
    job->setText(text);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start PostMessageJob job";
    }
}

void Connection::slotAddJoinCodeToChannel(const QString &channelId, const QString &password)
{
    setJoinCodeChannel(channelId, password);
}

void Connection::createChannels(const RocketChatRestApi::CreateChannelTeamInfo &info)
{
    auto job = new CreateChannelJob(this);
    connect(job, &CreateChannelJob::addJoinCodeToChannel, this, &Connection::slotAddJoinCodeToChannel);
    initializeRestApiJob(job);
    job->setCreateChannelInfo(info);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start CreateChannelJob job";
    }
}

void Connection::createGroups(const RocketChatRestApi::CreateChannelTeamInfo &info)
{
    auto job = new CreateGroupsJob(this);
    initializeRestApiJob(job);
    job->setCreateGroupsInfo(info);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start CreateGroupsJob job";
    }
}

void Connection::leaveChannel(const QByteArray &roomId)
{
    auto job = new LeaveChannelJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start leaveChannel job";
    }
}

void Connection::leaveGroups(const QByteArray &roomId)
{
    auto job = new LeaveGroupsJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start leaveGroups job";
    }
}

void Connection::updateMessage(const QByteArray &roomId, const QByteArray &messageId, const QString &text)
{
    auto job = new UpdateMessageJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setMessageId(messageId);
    job->setUpdatedText(text);
    connect(job, &UpdateMessageJob::updateMessageFailed, this, &Connection::updateMessageFailed);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start updateMessage job";
    }
}

void Connection::reactOnMessage(const QByteArray &messageId, const QString &emoji, bool shouldReact)
{
    auto job = new ReactOnMessageJob(this);
    initializeRestApiJob(job);
    job->setMessageId(messageId);
    QString fixedEmoji = emoji;
    if (fixedEmoji.startsWith(QLatin1Char(':'))) {
        fixedEmoji.remove(0, 1);
    }
    if (fixedEmoji.endsWith(QLatin1Char(':'))) {
        fixedEmoji.chop(1);
    }

    job->setEmoji(fixedEmoji);
    job->setShouldReact(shouldReact);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start reactOnMessage job";
    }
}

void Connection::historyChannel(const RocketChatRestApi::ChannelHistoryJob::ChannelHistoryInfo &info)
{
    // TODO
    auto job = new ChannelHistoryJob(this);
    initializeRestApiJob(job);
    job->setChannelHistoryInfo(info);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start historyChannel job";
    }
}

void Connection::historyChannel(const QString &roomId, const QString &type)
{
    auto job = new ChannelHistoryJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = roomId;
    job->setChannelGroupInfo(info);
    if (type == QLatin1Char('d')) {
        ChannelHistoryJob::ChannelHistoryInfo historyInfo;
        historyInfo.channelType = ChannelHistoryJob::Direct;
        job->setChannelHistoryInfo(historyInfo);
    } else if (type == QLatin1Char('p')) {
        ChannelHistoryJob::ChannelHistoryInfo historyInfo;
        historyInfo.channelType = ChannelHistoryJob::Groups;
        job->setChannelHistoryInfo(historyInfo);
    } else if (type == QLatin1Char('c')) {
        ChannelHistoryJob::ChannelHistoryInfo historyInfo;
        historyInfo.channelType = ChannelHistoryJob::Channel;
        job->setChannelHistoryInfo(historyInfo);
    }
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start historyChannel job";
    }
}

void Connection::filesInRoom(const QByteArray &roomId, const QString &type, int offset, int count)
{
    auto job = new ChannelFilesJob(this);
    connect(job, &ChannelFilesJob::channelFilesDone, this, &Connection::channelFilesDone);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    QueryParameters parameters;
    parameters.setCount(count);
    parameters.setOffset(offset);
    QMap<QString, QueryParameters::SortOrder> map;
    map.insert(QStringLiteral("uploadedAt"), QueryParameters::SortOrder::Descendant);
    parameters.setSorting(map);
    job->setQueryParameters(parameters);

    if (type == QLatin1Char('d')) {
        job->setChannelType(ChannelFilesJob::Direct);
    } else if (type == QLatin1Char('p')) {
        job->setChannelType(ChannelFilesJob::Groups);
    } else if (type == QLatin1Char('c')) {
        job->setChannelType(ChannelFilesJob::Channel);
    }
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start filesInRoom job";
    }
}

// FIXME
void Connection::membersInRoom(const QByteArray &roomId, const QString &type, int offset, int count)
{
    auto job = new ChannelMembersJob(this);
    QueryParameters parameters;
    parameters.setCount(count);
    parameters.setOffset(offset);
    job->setQueryParameters(parameters);
    connect(job, &ChannelMembersJob::channelMembersDone, this, &Connection::channelMembersDone);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (type == QLatin1Char('d')) {
        job->setChannelType(ChannelMembersJob::Direct);
    } else if (type == QLatin1Char('p')) {
        job->setChannelType(ChannelMembersJob::Groups);
    } else if (type == QLatin1Char('c')) {
        job->setChannelType(ChannelMembersJob::Channel);
    }
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start membersInRoom job";
    }
}

void Connection::addUserInChannel(const QByteArray &roomId, const QByteArray &userId)
{
    auto job = new ChannelInviteJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    job->setInviteUserId(QString::fromLatin1(userId));
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start addUserInChannel job";
    }
}

void Connection::addUserInGroup(const QByteArray &roomId, const QByteArray &userId)
{
    auto job = new GroupsInviteJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);

    job->setInviteUserId(QString::fromLatin1(userId));
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start addUserInGroup job";
    }
}

void Connection::markRoomAsRead(const QByteArray &roomId)
{
    auto job = new MarkRoomAsReadJob(this);
    job->setRoomId(roomId);
    initializeRestApiJob(job);
    connect(job, &MarkRoomAsReadJob::markAsReadDone, this, &Connection::markAsReadDone);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start markAsRead job";
    }
}

void Connection::markAsFavorite(const QByteArray &roomId, bool favorite)
{
    auto job = new RoomFavoriteJob(this);
    initializeRestApiJob(job);
    job->setFavorite(favorite);
    job->setRoomId(QString::fromLatin1(roomId));
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start markAsFavorite job";
    }
}

void Connection::disableNotifications(const QByteArray &roomId, bool value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setDisableNotifications(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start disableNotifications job";
    }
}

void Connection::muteGroupMentions(const QByteArray &roomId, bool value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setMuteGroupMentions(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start muteGroupMentions job";
    }
}

void Connection::desktopDurationNotifications(const QByteArray &roomId, int value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setDesktopNotificationDuration(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start desktopDurationNotifications job";
    }
}

void Connection::hideMentionStatus(const QByteArray &roomId, bool value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setHideMentionStatus(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start desktopSoundNotifications job";
    }
}

void Connection::desktopSoundNotifications(const QByteArray &roomId, const QString &value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setAudioNotificationValue(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start desktopSoundNotifications job";
    }
}

void Connection::changeGroupName(const QString &roomId, const QString &newName)
{
    auto job = new ChangeGroupsNameJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = roomId;
    job->setChannelGroupInfo(info);
    job->setName(newName);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start changeGroupName job";
    }
}

void Connection::hideUnreadStatus(const QByteArray &roomId, bool value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setHideUnreadStatus(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start hideUnreadStatus job";
    }
}

void Connection::desktopNotifications(const QByteArray &roomId, const QString &value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setDesktopNotifications(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start desktopNotifications job";
    }
}

void Connection::emailNotifications(const QByteArray &roomId, const QString &value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setEmailNotifications(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start emailNotifications job";
    }
}

void Connection::mobilePushNotifications(const QByteArray &roomId, const QString &value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setMobilePushNotifications(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start mobilePushNotifications job";
    }
}

void Connection::unreadAlert(const QByteArray &roomId, const QString &value)
{
    auto job = new SaveNotificationJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    job->setUnreadAlert(value);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start unreadAlert job";
    }
}

void Connection::setAvatar(const UserInfoJob::UserInfo &info, const SetAvatarJob::SetAvatarInfo &avatarInfo)
{
    auto job = new SetAvatarJob(this);
    initializeRestApiJob(job);

    job->setAvatarInfo(avatarInfo);
    job->setUserInfo(info);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start setAvatar job";
    }
}

void Connection::forgotPassword(const QString &email)
{
    auto job = new ForgotPasswordJob(this);
    initializeRestApiJob(job);
    job->setEmail(email);
    connect(job, &ForgotPasswordJob::forgotPasswordDone, this, &Connection::forgotPasswordDone);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start forgotPassword job";
    }
}

void Connection::userInfo(const QString &identifier, bool userName)
{
    auto job = new UserInfoJob(this);
    initializeRestApiJob(job);
    UserInfoJob::UserInfo info;
    info.userIdentifier = identifier;
    info.userInfoType = userName ? UserInfoJob::UserInfoType::UserName : UserInfoJob::UserInfoType::UserId;
    job->setUserInfo(info);
    connect(job, &UserInfoJob::userInfoDone, this, &Connection::userInfoDone);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start userInfo job";
    }
}

void Connection::ignoreUser(const QByteArray &roomId, const QByteArray &userId, bool ignore)
{
    auto job = new IgnoreUserJob(this);
    initializeRestApiJob(job);
    job->setIgnoreUserId(userId);
    job->setRoomId(roomId);
    job->setIgnore(ignore);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start ignoreUser job";
    }
}

void Connection::muteUser(const QByteArray &roomId, const QString &userName, bool mute)
{
    if (mute) {
        auto job = new RoomsMuteUserJob(this);
        initializeRestApiJob(job);
        job->setUserName(userName);
        job->setRoomId(roomId);
        if (!job->start()) {
            qCWarning(RUQOLA_LOG) << "Impossible to start RoomsMuteUserJob job";
        }
    } else {
        auto job = new RoomsUnmuteUserJob(this);
        initializeRestApiJob(job);
        job->setUserName(userName);
        job->setRoomId(roomId);
        if (!job->start()) {
            qCWarning(RUQOLA_LOG) << "Impossible to start RoomsMuteUserJob job";
        }
    }
}

void Connection::userPresence(const QString &userId)
{
    auto job = new GetPresenceJob(this);
    initializeRestApiJob(job);
    UserInfoJob::UserInfo info;
    info.userIdentifier = userId;
    info.userInfoType = UserInfoJob::UserInfoType::UserId;

    job->setUserInfo(info);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start userPresence job";
    }
}

void Connection::setChannelType(const QString &roomId, bool isPrivate)
{
    auto job = new SetChannelTypeJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = roomId;
    job->setChannelGroupInfo(info);
    job->setType(isPrivate ? SetChannelTypeJob::Private : SetChannelTypeJob::Public);
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start setChannelType job";
    }
}

void Connection::getGroupRoles(const QByteArray &roomId)
{
    auto job = new GetGroupRolesJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    connect(job, &GetGroupRolesJob::groupRolesDone, this, &Connection::groupRolesDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getGroupRoles job";
    }
}

void Connection::getChannelRoles(const QByteArray &roomId)
{
    auto job = new GetChannelRolesJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    connect(job, &GetChannelRolesJob::channelRolesDone, this, &Connection::channelRolesDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start GetChannelRolesJob job";
    }
}

void Connection::listAllPermissions()
{
    auto job = new PermissionsListAllJob(this);
    initializeRestApiJob(job);
    connect(job, &PermissionsListAllJob::permissionListAllDone, this, &Connection::permissionListAllDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start ListPermissionsJob job";
    }
}

void Connection::runCommand(const RunCommandJob::RunCommandInfo &runCommandInfo)
{
    auto job = new RunCommandJob(this);
    initializeRestApiJob(job);
    job->setRunCommandInfo(runCommandInfo);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start RunCommandJob job";
    }
}

void Connection::setJoinCodeChannel(const QString &roomId, const QString &joinCode)
{
    auto job = new SetJoinCodeChannelJob(this);
    initializeRestApiJob(job);
    job->setJoinCode(joinCode);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = roomId;
    job->setChannelGroupInfo(info);

    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start setjoincode";
    }
}

void Connection::channelGetAllUserMentions(const QString &roomId, int offset, int count)
{
    auto job = new ChannelGetAllUserMentionsJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    QueryParameters parameters;
    QMap<QString, QueryParameters::SortOrder> map;
    map.insert(QStringLiteral("ts"), QueryParameters::SortOrder::Descendant);
    parameters.setSorting(map);
    parameters.setCount(count);
    parameters.setOffset(offset);
    job->setQueryParameters(parameters);

    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start setChannelJoin";
    }
}

void Connection::channelKick(const QByteArray &roomId, const QByteArray &userId)
{
    auto job = new ChannelKickJob(this);
    initializeRestApiJob(job);
    job->setKickUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start channelKick";
    }
}

void Connection::groupKick(const QByteArray &roomId, const QByteArray &userId)
{
    auto job = new GroupsKickJob(this);
    initializeRestApiJob(job);
    job->setKickUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);

    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start channelKick";
    }
}

void Connection::groupAddModerator(const QByteArray &roomId, const QString &userId)
{
    auto job = new GroupAddModeratorJob(this);
    initializeRestApiJob(job);
    job->setAddModeratorUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start groupAddModerator";
    }
}

void Connection::groupRemoveModerator(const QByteArray &roomId, const QString &userId)
{
    auto job = new GroupRemoveModeratorJob(this);
    initializeRestApiJob(job);
    job->setRemoveUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.identifier = QString::fromLatin1(roomId);
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start groupRemoveModerator";
    }
}

void Connection::groupAddLeader(const QByteArray &roomId, const QString &userId)
{
    auto job = new GroupAddLeaderJob(this);
    initializeRestApiJob(job);
    job->setAddLeaderUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start groupAddLeader";
    }
}

void Connection::groupRemoveLeader(const QByteArray &roomId, const QString &userId)
{
    auto job = new GroupRemoveLeaderJob(this);
    initializeRestApiJob(job);
    job->setRemoveUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.identifier = QString::fromLatin1(roomId);
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start groupRemoveLeader";
    }
}

void Connection::groupAddOwner(const QByteArray &roomId, const QString &userId)
{
    auto job = new GroupAddOwnerJob(this);
    initializeRestApiJob(job);
    job->setAddownerUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.identifier = QString::fromLatin1(roomId);
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start groupAddOwner";
    }
}

void Connection::groupRemoveOwner(const QByteArray &roomId, const QString &userId)
{
    auto job = new GroupRemoveOwnerJob(this);
    initializeRestApiJob(job);
    job->setRemoveUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);

    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start groupRemoveOwner";
    }
}

void Connection::channelAddModerator(const QByteArray &roomId, const QString &userId)
{
    auto job = new ChannelAddModeratorJob(this);
    initializeRestApiJob(job);
    job->setAddModeratorUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start channelAddModerator";
    }
}

void Connection::channelRemoveModerator(const QByteArray &roomId, const QString &userId)
{
    auto job = new ChannelRemoveModeratorJob(this);
    initializeRestApiJob(job);
    job->setRemoveUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start ChannelRemoveModeratorJob";
    }
}

void Connection::channelAddLeader(const QByteArray &roomId, const QString &userId)
{
    auto job = new ChannelAddLeaderJob(this);
    initializeRestApiJob(job);
    job->setAddLeaderUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start channelAddLeader";
    }
}

void Connection::channelRemoveLeader(const QByteArray &roomId, const QString &userId)
{
    auto job = new ChannelRemoveLeaderJob(this);
    initializeRestApiJob(job);
    job->setRemoveUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start channelRemoveLeader";
    }
}

void Connection::channelAddOwner(const QByteArray &roomId, const QString &userId)
{
    auto job = new ChannelAddOwnerJob(this);
    initializeRestApiJob(job);
    job->setAddownerUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start ChannelAddOwnerJob";
    }
}

void Connection::channelRemoveOwner(const QByteArray &roomId, const QString &userId)
{
    auto job = new ChannelRemoveOwnerJob(this);
    initializeRestApiJob(job);
    job->setRemoveUserId(userId);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start ChannelRemoveOwnerJob";
    }
}

void Connection::channelDelete(const QByteArray &roomId)
{
    auto job = new ChannelDeleteJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    connect(job, &ChannelDeleteJob::deletechannelDone, this, &Connection::deletechannelDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start ChannelDeleteJob";
    }
}

void Connection::groupDelete(const QByteArray &roomId)
{
    auto job = new GroupsDeleteJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.identifier = QString::fromLatin1(roomId);
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    job->setChannelGroupInfo(info);
    connect(job, &GroupsDeleteJob::deleteGroupsDone, this, &Connection::deleteGroupsDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start GroupsDeleteJob";
    }
}

void Connection::createDiscussion(const QByteArray &parentRoomId,
                                  const QString &discussionName,
                                  const QString &replyMessage,
                                  const QByteArray &parentMessageId,
                                  const QList<QByteArray> &users)
{
    auto job = new RoomStartDiscussionJob(this);
    initializeRestApiJob(job);
    job->setParentRoomId(parentRoomId);

    job->setDiscussionName(discussionName);
    job->setParentMessageId(parentMessageId);
    job->setReplyMessage(replyMessage);
    job->setUsers(users);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start roomStartDiscussion";
    }
}

void Connection::getDiscussions(const QByteArray &roomId, int offset, int count)
{
    auto job = new GetDiscussionsJob(this);
    initializeRestApiJob(job);
    QueryParameters parameters;
    parameters.setCount(count);
    parameters.setOffset(offset);
    job->setQueryParameters(parameters);
    job->setRoomId(roomId);
    connect(job, &GetDiscussionsJob::getDiscussionsDone, this, &Connection::getDiscussionsDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getDiscussions";
    }
}

void Connection::getThreadsList(const QByteArray &roomId, bool onlyUnread, int offset, int count)
{
    auto job = new GetThreadsJob(this);
    initializeRestApiJob(job);
    job->setRoomId(QString::fromLatin1(roomId));
    QueryParameters parameters;
    QMap<QString, QueryParameters::SortOrder> map;
    map.insert(QStringLiteral("ts"), QueryParameters::SortOrder::Descendant);
    parameters.setSorting(map);
    parameters.setCount(count);
    parameters.setOffset(offset);
    if (onlyUnread) {
        job->setSearchType(GetThreadsJob::TheadSearchType::Unread);
    }
    job->setQueryParameters(parameters);
    connect(job, &GetThreadsJob::getThreadsDone, this, [this, onlyUnread](const QJsonObject &obj, const QString &roomId) {
        Q_EMIT getThreadsDone(obj, roomId, onlyUnread);
    });
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getThreadsList";
    }
}

void Connection::getMessage(const QByteArray &messageId, const QByteArray &roomId)
{
    auto job = new GetMessageJob(this);
    initializeRestApiJob(job);
    job->setMessageId(messageId);
    job->setRoomId(roomId);
    connect(job, &GetMessageJob::getMessageDone, this, &Connection::getMessageDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getMessage";
    }
}

void Connection::getPinnedMessages(const QByteArray &roomId, int offset, int count)
{
    auto job = new GetPinnedMessagesJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    QueryParameters parameters;
    parameters.setCount(count);
    parameters.setOffset(offset);
    QMap<QString, QueryParameters::SortOrder> map;
    map.insert(QStringLiteral("ts"), QueryParameters::SortOrder::Descendant);

    job->setQueryParameters(parameters);
    connect(job, &GetPinnedMessagesJob::getPinnedMessagesDone, this, &Connection::getPinnedMessagesDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getPinnedMessagesList";
    }
}

void Connection::getMentionedMessages(const QByteArray &roomId, int offset, int count)
{
    auto job = new GetMentionedMessagesJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    QueryParameters parameters;
    QMap<QString, QueryParameters::SortOrder> map;
    map.insert(QStringLiteral("ts"), QueryParameters::SortOrder::Descendant);
    parameters.setSorting(map);
    parameters.setCount(count);
    parameters.setOffset(offset);
    job->setQueryParameters(parameters);

    connect(job, &GetMentionedMessagesJob::getMentionedMessagesDone, this, &Connection::getMentionedMessagesDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getMentionedMessages";
    }
}

void Connection::getStarredMessages(const QByteArray &roomId, int offset, int count)
{
    auto job = new GetStarredMessagesJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    QueryParameters parameters;
    QMap<QString, QueryParameters::SortOrder> map;
    map.insert(QStringLiteral("ts"), QueryParameters::SortOrder::Descendant);
    parameters.setSorting(map);
    parameters.setCount(count);
    parameters.setOffset(offset);
    job->setQueryParameters(parameters);

    connect(job, &GetStarredMessagesJob::getStarredMessagesDone, this, &Connection::getStarredMessagesDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getStarredMessagesList";
    }
}

void Connection::getSnippetedMessages(const QByteArray &roomId, int offset, int count)
{
    auto job = new GetSnippetedMessagesJob(this);
    initializeRestApiJob(job);
    job->setRoomId(roomId);
    QueryParameters parameters;
    parameters.setCount(count);
    parameters.setOffset(offset);

    QMap<QString, QueryParameters::SortOrder> map;
    map.insert(QStringLiteral("ts"), QueryParameters::SortOrder::Descendant);
    job->setQueryParameters(parameters);
    connect(job, &GetSnippetedMessagesJob::getSnippetedMessagesDone, this, &Connection::getSnippetedMessagesDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getSnippetedMessagesList";
    }
}

void Connection::getThreadMessages(const QByteArray &threadMessageId)
{
    auto job = new GetThreadMessagesJob(this);
    initializeRestApiJob(job);
    job->setThreadMessageId(threadMessageId);
    connect(job, &GetThreadMessagesJob::getThreadMessagesDone, this, &Connection::getThreadMessagesDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start getThreadMessages";
    }
}

void Connection::syncThreadMessages(const QString &threadMessageId, const QString &timestamp)
{
    auto job = new SyncThreadMessagesJob(this);
    initializeRestApiJob(job);
    job->setThreadMessageId(threadMessageId);
    job->setTimeStamp(timestamp);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start syncThreadMessages";
    }
}

void Connection::sendMessage(const QByteArray &roomId, const QString &text, const QString &messageId, const QByteArray &threadMessageId)
{
    auto job = new SendMessageJob(this);
    initializeRestApiJob(job);
    SendMessageJob::SendMessageArguments args;
    args.roomId = QString::fromLatin1(roomId);
    args.message = text;
    args.messageId = messageId;
    args.threadMessageId = QString::fromLatin1(threadMessageId);
    job->setSendMessageArguments(std::move(args));
    if (!job->start()) {
        qCWarning(RUQOLA_LOG) << "Impossible to start job";
    }
}

void Connection::autoTranslateSaveLanguageSettings(const QByteArray &roomId, const QString &language)
{
    auto job = new TranslateSaveSettingsJob(this);
    initializeRestApiJob(job);
    job->setRoomId(QString::fromLatin1(roomId));
    job->setType(TranslateSaveSettingsJob::LanguageSetting);
    job->setLanguage(language);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start autoTranslateSaveLanguageSettings";
    }
}

void Connection::autoTranslateSaveAutoTranslateSettings(const QByteArray &roomId, bool autoTranslate)
{
    auto job = new TranslateSaveSettingsJob(this);
    initializeRestApiJob(job);
    job->setRoomId(QString::fromLatin1(roomId));
    job->setType(TranslateSaveSettingsJob::AutoTranslateSetting);
    job->setAutoTranslate(autoTranslate);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start autoTranslateSaveAutoTranslateSettings";
    }
}

void Connection::setUserStatus(const QString &userId, SetStatusJob::StatusType status, const QString &message)
{
    auto job = new SetStatusJob(this);
    initializeRestApiJob(job);
    job->setStatusUserId(userId);
    job->setStatus(status);
    job->setStatusMessage(message);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start SetStatusJob";
    }
}

void Connection::usersPresence()
{
    auto job = new UsersPresenceJob(this);
    initializeRestApiJob(job);
    connect(job, &UsersPresenceJob::usersPresenceDone, this, &Connection::usersPresenceDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start usersPresenceJob";
    }
}

void Connection::customUserStatus()
{
    auto job = new CustomUserStatusListJob(this);
    initializeRestApiJob(job);
    connect(job, &CustomUserStatusListJob::customUserStatusDone, this, &Connection::customUserStatusDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start CustomUserStatusJob";
    }
}

void Connection::usersAutocomplete(const UsersAutocompleteJob::UsersAutocompleterInfo &info)
{
    auto job = new UsersAutocompleteJob(this);
    job->setUsersCompleterInfo(info);
    initializeRestApiJob(job);
    connect(job, &UsersAutocompleteJob::usersAutocompleteDone, this, &Connection::usersAutocompleteDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start UsersAutocompleteJob";
    }
}

void Connection::findOrCreateInvite(const QByteArray &roomId, int maxUses, int numberOfDays)
{
    auto job = new FindOrCreateInviteJob(this);
    job->setRoomId(roomId);
    job->setMaxUses(maxUses);
    job->setNumberOfDays(numberOfDays);
    initializeRestApiJob(job);
    connect(job, &FindOrCreateInviteJob::findOrCreateInviteDone, this, &Connection::findOrCreateInviteDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start findOrCreateInviteJob";
    }
}

void Connection::sendUserEmailCode(const QString &identifier)
{
    auto job = new User2FASendEmailCodeJob(this);
    job->setUsernameOrEmail(identifier);
    initializeRestApiJob(job);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start User2FASendEmailCodeJob";
    }
}

void Connection::registerNewUser(const RocketChatRestApi::RegisterUserJob::RegisterUserInfo &userInfo)
{
    auto job = new RegisterUserJob(this);
    job->setRegisterUserInfo(userInfo);
    initializeRestApiJob(job);
    connect(job, &RegisterUserJob::registerUserDone, this, &Connection::registerUserDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start RegisterUserJob";
    }
}

void Connection::updateOwnBasicInfo(const RocketChatRestApi::UsersUpdateOwnBasicInfoJob::UpdateOwnBasicInfo &info)
{
    auto job = new UsersUpdateOwnBasicInfoJob(this);
    job->setUpdateOwnBasicInfo(info);
    initializeRestApiJob(job);
    // Clear all other tokens when password was changed
    // TODO fix me connect(job, &UsersUpdateOwnBasicInfoJob::passwordChanged, this, &Connection::updateOwnBasicInfoDone);

    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start UsersUpdateOwnBasicInfoJob";
    }
}

void Connection::getRoomsAdmin(const RocketChatRestApi::AdminRoomsJob::AdminRoomsJobInfo &info)
{
    auto job = new AdminRoomsJob(this);
    job->setRoomsAdminInfo(info);
    initializeRestApiJob(job);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start RoomsAdminJob";
    }
}

void Connection::getChannelsCounter(const QByteArray &roomId)
{
    auto job = new ChannelGetCountersJob(this);
    initializeRestApiJob(job);
    ChannelGroupBaseJob::ChannelGroupInfo info;
    info.channelGroupInfoType = ChannelGroupBaseJob::ChannelGroupInfoType::Identifier;
    info.identifier = QString::fromLatin1(roomId);
    job->setChannelGroupInfo(info);
    connect(job, &ChannelGetCountersJob::channelGetCountersDone, this, &Connection::channelGetCountersDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start ChannelGetCountersJob";
    }
}

void Connection::setUserPreferences(const RocketChatRestApi::UsersSetPreferencesJob::UsersSetPreferencesInfo &info)
{
    auto job = new UsersSetPreferencesJob(this);
    job->setUsersSetPreferencesInfo(info);
    initializeRestApiJob(job);
    connect(job, &UsersSetPreferencesJob::usersSetPreferencesDone, this, &Connection::usersSetPreferencesDone);
    if (!job->start()) {
        qCDebug(RUQOLA_LOG) << "Impossible to start UsersSetPreferencesJob";
    }
}

#include "moc_connection.cpp"
