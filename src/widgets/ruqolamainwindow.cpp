/*
   Copyright (c) 2020 Laurent Montel <montel@kde.org>

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

#include "ruqolamainwindow.h"
#include "ruqolaglobalconfig.h"

#include "config-ruqola.h"
#include "ruqola.h"
#include "rocketchataccount.h"
#include "accountmanager.h"
#include "roomwrapper.h"
#include "ruqolaserverconfig.h"
#include "receivetypingnotificationmanager.h"
#include "ruqolacentralwidget.h"
#include "misc/accountmenu.h"
#include "misc/accountsoverviewwidget.h"
#include "dialogs/serverinfodialog.h"
#include "dialogs/searchchanneldialog.h"
#include "dialogs/createnewchanneldialog.h"
#include "dialogs/createnewaccountdialog.h"
#include "dialogs/channelpassworddialog.h"
#include "registeruser/registeruserdialog.h"
#include "configuredialog/configuresettingsdialog.h"
#include <KActionCollection>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KLocalizedString>
#include <KStandardAction>
#include <KNotifyConfigWidget>
#include <QIcon>
#include <QStatusBar>
#include <QLabel>
#include <QGuiApplication>
#include <QFontDatabase>
#include <QApplication>
#include <KMessageBox>

#if HAVE_KUSERFEEDBACK
#include "userfeedback/userfeedbackmanager.h"
#include <KUserFeedback/NotificationPopup>
#include <KUserFeedback/Provider>
#endif

namespace {
static const char myConfigGroupName[] = "RuqolaMainWindow";
}

RuqolaMainWindow::RuqolaMainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    mMainWidget = new RuqolaCentralWidget(this);
    mMainWidget->setObjectName(QStringLiteral("mMainWidget"));
    connect(mMainWidget, &RuqolaCentralWidget::channelSelected, this, [this]() {
        changeActionStatus(true);
    });
    connect(mMainWidget, &RuqolaCentralWidget::loginPageActivated, this, &RuqolaMainWindow::slotLoginPageActivated);
    setCentralWidget(mMainWidget);
    setupActions();
    setupStatusBar();
    setupGUI(/*QStringLiteral(":/kxmlgui5/ruqola/ruqolaui.rc")*/);
    readConfig();
    connect(Ruqola::self()->accountManager(), &AccountManager::currentAccountChanged, this, &RuqolaMainWindow::slotAccountChanged);
    slotAccountChanged();
#if HAVE_KUSERFEEDBACK
    KUserFeedback::NotificationPopup *userFeedBackNotificationPopup = new KUserFeedback::NotificationPopup(this);
    userFeedBackNotificationPopup->setFeedbackProvider(UserFeedBackManager::self()->userFeedbackProvider());
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    // Create systray to show notifications on Desktop
    connect(Ruqola::self()->notification(), &Notification::alert, this, [this]() {
        QApplication::alert(this, 0);
    });
#endif
}

RuqolaMainWindow::~RuqolaMainWindow()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(myConfigGroupName);
    group.writeEntry("Size", size());

    delete mMainWidget; // before Ruqola::destroy()

    Ruqola::destroy();
}

void RuqolaMainWindow::setupStatusBar()
{
    mStatusBarTypingMessage = new QLabel(this);
    mStatusBarTypingMessage->setTextFormat(Qt::RichText);
    mStatusBarTypingMessage->setObjectName(QStringLiteral("mStatusBarTypingMessage"));
    statusBar()->addPermanentWidget(mStatusBarTypingMessage);
    mAccountOverviewWidget = new AccountsOverviewWidget(this);
    statusBar()->addPermanentWidget(mAccountOverviewWidget);
}

void RuqolaMainWindow::slotAccountChanged()
{
    if (mCurrentRocketChatAccount) {
        disconnect(mCurrentRocketChatAccount, nullptr, this, nullptr);
    }
    mCurrentRocketChatAccount = Ruqola::self()->rocketChatAccount();
    connect(mCurrentRocketChatAccount->receiveTypingNotificationManager(), &ReceiveTypingNotificationManager::notificationChanged, this, &RuqolaMainWindow::slotTypingNotificationChanged);
    connect(mCurrentRocketChatAccount->receiveTypingNotificationManager(), &ReceiveTypingNotificationManager::clearNotification, this, &RuqolaMainWindow::slotClearNotification);
    connect(mCurrentRocketChatAccount, &RocketChatAccount::missingChannelPassword, this, &RuqolaMainWindow::slotMissingChannelPassword);
    connect(mCurrentRocketChatAccount, &RocketChatAccount::publicSettingChanged, this, &RuqolaMainWindow::updateActions);
    connect(mCurrentRocketChatAccount, &RocketChatAccount::serverVersionChanged, this, &RuqolaMainWindow::updateActions);
    connect(mCurrentRocketChatAccount, &RocketChatAccount::raiseWindow, this, &RuqolaMainWindow::slotRaiseWindow);
    connect(mCurrentRocketChatAccount, &RocketChatAccount::registerUserSuccess, this, &RuqolaMainWindow::slotRegisterUserSuccessed);

    updateActions();
    changeActionStatus(false); //Disable actions when switching.
    slotClearNotification(); //Clear notification when we switch too.
    mMainWidget->setCurrentRocketChatAccount(mCurrentRocketChatAccount);
}

void RuqolaMainWindow::slotRaiseWindow()
{
    show();
    raise();
    activateWindow();
}

void RuqolaMainWindow::changeActionStatus(bool enabled)
{
    mSaveAs->setEnabled(enabled);
}

void RuqolaMainWindow::updateActions()
{
    mUnreadOnTop->setChecked(mCurrentRocketChatAccount->sortUnreadOnTop());
    mRegisterNewUser->setEnabled(mCurrentRocketChatAccount->registrationFromEnabled());
}

void RuqolaMainWindow::readConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group = KConfigGroup(config, myConfigGroupName);
    const QSize sizeDialog = group.readEntry("Size", QSize(800, 600));
    if (sizeDialog.isValid()) {
        resize(sizeDialog);
    }
}

void RuqolaMainWindow::slotClearNotification()
{
    mStatusBarTypingMessage->clear();
}

void RuqolaMainWindow::slotTypingNotificationChanged(const QString &roomId, const QString &notificationStr)
{
    if (mMainWidget->roomId() == roomId) {
        mStatusBarTypingMessage->setText(notificationStr);
    }
}

void RuqolaMainWindow::setupActions()
{
    KActionCollection *ac = actionCollection();

    KStandardAction::quit(this, &RuqolaMainWindow::close, ac);
    KStandardAction::preferences(this, &RuqolaMainWindow::slotConfigure, ac);
    KStandardAction::configureNotifications(this, &RuqolaMainWindow::slotConfigureNotifications, ac);

    mSaveAs = KStandardAction::saveAs(this, &RuqolaMainWindow::slotSaveAs, ac);
    mSaveAs->setText(i18n("Save As Text..."));

    QAction *act = new QAction(i18n("Add Account..."), this);
    connect(act, &QAction::triggered, this, &RuqolaMainWindow::slotAddAccount);
    ac->addAction(QStringLiteral("add_account"), act);

    //Move in specific server widget
    mServerInfo = new QAction(i18n("Server Info..."), this);
    connect(mServerInfo, &QAction::triggered, this, &RuqolaMainWindow::slotServerInfo);
    ac->addAction(QStringLiteral("server_info"), mServerInfo);

    mLogout = new QAction(i18n("Logout Current Account"), this);
    connect(mLogout, &QAction::triggered, this, &RuqolaMainWindow::slotLogout);
    ac->addAction(QStringLiteral("logout"), mLogout);

    mSearchChannel = new QAction(i18n("Search Channel..."), this);
    connect(mSearchChannel, &QAction::triggered, this, &RuqolaMainWindow::slotSearchChannel);
    ac->addAction(QStringLiteral("search_channel"), mSearchChannel);

    mCreateNewChannel = new QAction(i18n("Create New Channel..."), this);
    mCreateNewChannel->setIcon(QIcon::fromTheme(QStringLiteral("irc-join-channel")));
    connect(mCreateNewChannel, &QAction::triggered, this, &RuqolaMainWindow::slotCreateNewChannel);
    ac->addAction(QStringLiteral("create_new_channel"), mCreateNewChannel);

    mAccountMenu = new AccountMenu(this);
    mAccountMenu->setActionCollection(ac);
    ac->addAction(QStringLiteral("account_menu"), mAccountMenu);

    mUnreadOnTop = new QAction(i18n("Unread on Top"), this);
    mUnreadOnTop->setCheckable(true);
    connect(mUnreadOnTop, &QAction::triggered, this, &RuqolaMainWindow::slotUnreadOnTop);
    ac->addAction(QStringLiteral("unread_on_top"), mUnreadOnTop);

    mClearAlerts = new QAction(i18n("Mark all channels read"), this);
    ac->setDefaultShortcut(mClearAlerts, Qt::SHIFT + Qt::Key_Escape);
    connect(mClearAlerts, &QAction::triggered, this, &RuqolaMainWindow::slotClearAccountAlerts);
    ac->addAction(QStringLiteral("mark_all_channels_read"), mClearAlerts);

    mRegisterNewUser = new QAction(i18n("Register a New User..."), this);
    connect(mRegisterNewUser, &QAction::triggered, this, &RuqolaMainWindow::slotRegisterNewUser);
    ac->addAction(QStringLiteral("register_new_user"), mRegisterNewUser);
}

void RuqolaMainWindow::slotClearAccountAlerts()
{
    if (auto acct = Ruqola::self()->accountManager()->account()) {
        acct->clearAllUnreadMessages();
    }
}

void RuqolaMainWindow::slotCreateNewChannel()
{
    QPointer<CreateNewChannelDialog> dlg = new CreateNewChannelDialog(this);
    CreateNewChannelWidget::Features flags;
    if (mCurrentRocketChatAccount->ruqolaServerConfig()->broadCastEnabled()) {
        flags |= CreateNewChannelWidget::Feature::BroadCast;
    }
    if (mCurrentRocketChatAccount->ruqolaServerConfig()->encryptionEnabled()) {
        flags |= CreateNewChannelWidget::Feature::Encrypted;
    }
    dlg->setFeatures(flags);
    if (dlg->exec()) {
        const CreateNewChannelDialog::NewChannelInfo info = dlg->channelInfo();
        //TODO adapt createNewChannel api for using QStringList
        mCurrentRocketChatAccount->createNewChannel(info.channelName, info.readOnly, info.privateChannel, info.usersName.join(QLatin1Char(',')), info.encryptedRoom, info.password, info.broadCast);
    }
    delete dlg;
}

void RuqolaMainWindow::slotConfigure()
{
    QPointer<ConfigureSettingsDialog> dlg = new ConfigureSettingsDialog(this);
    if (dlg->exec()) {
        if (RuqolaGlobalConfig::self()->useCustomFont()) {
            qApp->setFont(RuqolaGlobalConfig::self()->generalFont());
        } else {
            qApp->setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
        }

        mAccountOverviewWidget->updateButtons();
    }
    delete dlg;
}

void RuqolaMainWindow::slotAddAccount()
{
    QPointer<CreateNewAccountDialog> dlg = new CreateNewAccountDialog(this);
    const QStringList lst = Ruqola::self()->accountManager()->accountsName();
    dlg->setExistingAccountName(lst);
    if (dlg->exec()) {
        const CreateNewAccountDialog::AccountInfo info = dlg->accountInfo();
        Ruqola::self()->accountManager()->addAccount(info.accountName, info.userName, info.serverName);
    }
    delete dlg;
}

void RuqolaMainWindow::slotServerInfo()
{
    QPointer<ServerInfoDialog> dlg = new ServerInfoDialog(this);
    dlg->setServerConfigInfo(mCurrentRocketChatAccount->serverConfigInfo());
    dlg->exec();
    delete dlg;
}

void RuqolaMainWindow::slotLogout()
{
    mCurrentRocketChatAccount->logOut();
}

void RuqolaMainWindow::slotSearchChannel()
{
    QPointer<SearchChannelDialog> dlg = new SearchChannelDialog(this);
    dlg->exec();
    delete dlg;
}

void RuqolaMainWindow::slotUnreadOnTop(bool checked)
{
    mCurrentRocketChatAccount->setSortUnreadOnTop(checked);
}

void RuqolaMainWindow::slotMissingChannelPassword(const RocketChatRestApi::ChannelBaseJob::ChannelInfo &channelInfo)
{
    QPointer<ChannelPasswordDialog> dlg = new ChannelPasswordDialog(this);
    //TODO add channel name!
    if (dlg->exec()) {
        //FIXME channelinfo
        mCurrentRocketChatAccount->joinRoom(channelInfo.channelInfoIdentifier, dlg->password());
    }
    delete dlg;
}

void RuqolaMainWindow::slotSaveAs()
{
    qWarning() << "RuqolaMainWindow::slotSaveAs not implemented yet ";
}

void RuqolaMainWindow::slotLoginPageActivated(bool loginPageActivated)
{
    mSearchChannel->setEnabled(!loginPageActivated);
    mCreateNewChannel->setEnabled(!loginPageActivated);
    mSaveAs->setEnabled(!loginPageActivated);
    mLogout->setEnabled(!loginPageActivated);
    mClearAlerts->setEnabled(!loginPageActivated);
}

void RuqolaMainWindow::slotConfigureNotifications()
{
    KNotifyConfigWidget::configure(this);
}

void RuqolaMainWindow::slotRegisterNewUser()
{
    QPointer<RegisterUserDialog> dlg = new RegisterUserDialog(this);
    if (dlg->exec()) {
        mCurrentRocketChatAccount->registerNewUser(dlg->registerUserInfo());
    }
    delete dlg;
}

void RuqolaMainWindow::slotRegisterUserSuccessed()
{
    KMessageBox::information(this, i18n("We have sent you an email to confirm your registration.\nIf you do not receive an email shortly, please come back and try again."), i18n("Register New User"));
}
