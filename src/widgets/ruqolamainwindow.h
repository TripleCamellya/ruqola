/*
   SPDX-FileCopyrightText: 2020-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "channelgroupbasejob.h"
#include "libruqolawidgets_export.h"
#include <KXmlGuiWindow>
#include <QPointer>
#include <config-ruqola.h>
class StatusCombobox;
class RuqolaCentralWidget;
class QAction;
class QWidgetAction;
class ServerMenu;
class RocketChatAccount;
class QLabel;
class AccountsOverviewWidget;
class Notification;
class StatusModelFilterProxyModel;
class QCommandLineParser;
class SwitchChannelTreeViewManager;
class KToggleAction;
class KHamburgerMenu;
class KToggleFullScreenAction;
class QToolButton;
class KActionMenu;
class AccountManager;
class LIBRUQOLAWIDGETS_EXPORT RuqolaMainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit RuqolaMainWindow(QWidget *parent = nullptr);
    ~RuqolaMainWindow() override;

#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
    void slotActivateRequested(const QStringList &arguments, const QString &workingDirectory);
#endif

    void parseCommandLine(QCommandLineParser *parser);

protected:
    Q_REQUIRED_RESULT bool queryClose() override;

private:
    Q_REQUIRED_RESULT bool canCreateChannels() const;
    Q_REQUIRED_RESULT bool canCreateDirectMessages() const;
    Q_REQUIRED_RESULT bool canCreateTeams() const;
    void slotRegisterUserSuccessed();
    void setupActions();
    void readConfig();
    void setupStatusBar();
    void slotAddServer();
    void slotServerInfo();
    void slotConfigure();
    void slotLogout();
    void slotSelectNextUnreadChannel();
    void slotCreateNewChannel();
    void slotTypingNotificationChanged(const QString &roomId, const QString &notificationStr);
    void slotClearNotification();
    void slotAccountChanged();
    void slotUnreadOnTop(bool checked);
    void slotRoomListSortByLastMessage();
    void slotRoomListSortAlphabetically();
    void slotShowLog();
    void updateActions();
    void slotMissingChannelPassword(const RocketChatRestApi::ChannelGroupBaseJob::ChannelGroupInfo &channelInfo);
    void slotClearAccountAlerts();
    void slotLoginPageActivated(bool loginPageActivated);
    void slotConfigureNotifications();
    void slotRaiseWindow();
    void slotRegisterNewUser();
    void slotAdministrator();
    void slotConfigureMyAccount();
    void updateNotification(bool hasAlert, int nbUnread, const QString &accountName);
    void logout(const QString &accountName);
    void slotRoomNeedAttention();
    void slotCreateDirectMessages();
    void slotClose();
    void slotStatusChanged();
    void createSystemTray();
    void slotUpdateCustomUserStatus();
    void slotCreateDiscussion();
    void slotCreateTeam();
    void slotPermissionChanged();
    void slotDirectory();
    void showNextView();
    void showPreviousView();
    void slotMessageUrlNotFound(const QString &str);
    void slotHistorySwitchChannel(const QString &identifier);
    void undoSwitchChannel();
    void redoSwitchChannel();
    void slotToggleMenubar(bool dontShowWarning);
    void updateHamburgerMenu();
    void slotFullScreen(bool t);
    void slotOpenNotificationHistory();
    void slotNewNotification();
    void slotShowNotifyMessage(const QString &accountName, const QString &messageId, const QString &roomId);
    void slotAdministratorServerSettings();
    void slotRocketChatInformation();
    Q_REQUIRED_RESULT bool hasBannerInfo() const;
    void slotShowRoomAvatar(bool checked);
    void slotPrivateSettingsChanged();
    void slotShowFavoriteRoom(bool checked);
    void slotShowServerInfo();
    void slotWhatsNew();
#if HAVE_DATABASE_SUPPORT
    void slotShowDatabaseMessages();
#endif
    bool mReallyClose{false};

    RuqolaCentralWidget *const mMainWidget;
    QAction *mServerInfo = nullptr;
    QAction *mLogout = nullptr;
    QAction *mNextUnreadChannel = nullptr;
    QAction *mCreateNewChannel = nullptr;
    QAction *mCreateDirectMessages = nullptr;
    QAction *mUnreadOnTop = nullptr;
    QAction *mRoomListSortByLastMessage = nullptr;
    QAction *mRoomListSortAlphabetically = nullptr;
    QAction *mShowLog = nullptr;
    QAction *mShowServerInfo = nullptr;
    QAction *mClearAlerts = nullptr;
    QAction *mRegisterNewUser = nullptr;
    QAction *mMyAccount = nullptr;
    QAction *mAdministrator = nullptr;
    QAction *mAdministratorServerSettings = nullptr;
    QAction *mCreateDiscussion = nullptr;
    QAction *mCreateTeam = nullptr;
    QAction *mDirectory = nullptr;
    QAction *mNextTab = nullptr;
    QAction *mPreviewTab = nullptr;
    QWidgetAction *mStatus = nullptr;
    ServerMenu *mServerMenu = nullptr;
    QPointer<RocketChatAccount> mCurrentRocketChatAccount;
    QLabel *mStatusBarTypingMessage = nullptr;
    AccountsOverviewWidget *mAccountOverviewWidget = nullptr;
    Notification *mNotification = nullptr;
    StatusCombobox *mStatusComboBox = nullptr;
    StatusModelFilterProxyModel *mStatusProxyModel = nullptr;
    SwitchChannelTreeViewManager *const mSwitchChannelTreeManager;
    KToggleAction *mShowMenuBarAction = nullptr;
    KHamburgerMenu *mHamburgerMenu = nullptr;
    KToggleFullScreenAction *mShowFullScreenAction = nullptr;
    QToolButton *mNotificationToolButton = nullptr;
    QAction *mShowNotifyHistory = nullptr;
    KActionMenu *mAdministratorMenu = nullptr;
    AccountManager *mAccountManager = nullptr;
    QAction *mShowRocketChatServerInfo = nullptr;
    QAction *mRoomAvatar = nullptr;
    QAction *mRoomFavorite = nullptr;
#if HAVE_DATABASE_SUPPORT
    QAction *mShowDatabaseMessages = nullptr;
#endif
    QAction *mMenuDebug = nullptr;
};
