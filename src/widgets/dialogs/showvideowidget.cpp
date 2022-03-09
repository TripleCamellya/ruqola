/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "showvideowidget.h"
#include <KLocalizedString>

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QStyle>
#include <QTime>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVideoWidget>

ShowVideoWidget::ShowVideoWidget(QWidget *parent)
    : QWidget(parent)
    , mMediaPlayer(new QMediaPlayer(this
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                                    ,
                                    QMediaPlayer::VideoSurface
#endif
                                    ))
    , mPlayButton(new QPushButton(this))
    , mPositionSlider(new QSlider(Qt::Horizontal, this))
    , mErrorLabel(new QLabel(this))
    , mSoundButton(new QToolButton(this))
    , mSoundSlider(new QSlider(Qt::Horizontal, this))
    , mLabelDuration(new QLabel(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mLabelDuration->setObjectName(QStringLiteral("mLabelDuration"));

    mMediaPlayer->setObjectName(QStringLiteral("mMediaPlayer"));
    auto videoWidget = new QVideoWidget(this);
    videoWidget->setObjectName(QStringLiteral("videoWidget"));

    mainLayout->addWidget(videoWidget);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setObjectName(QStringLiteral("controlLayout"));
    controlLayout->setContentsMargins({});
    mainLayout->addLayout(controlLayout);

    mPlayButton->setObjectName(QStringLiteral("mPlayButton"));
    mPlayButton->setEnabled(false);
    mPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    controlLayout->addWidget(mPlayButton);
    connect(mPlayButton, &QAbstractButton::clicked, this, &ShowVideoWidget::play);

    mPositionSlider->setObjectName(QStringLiteral("mPositionSlider"));
    mPositionSlider->setRange(0, 0);
    controlLayout->addWidget(mPositionSlider);
    controlLayout->addWidget(mLabelDuration);

    connect(mPositionSlider, &QAbstractSlider::sliderMoved, this, &ShowVideoWidget::setPosition);

    mErrorLabel->setObjectName(QStringLiteral("mErrorLabel"));
    mErrorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    mainLayout->addWidget(mErrorLabel);
    mMediaPlayer->setVideoOutput(videoWidget);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(mMediaPlayer, &QMediaPlayer::stateChanged, this, &ShowVideoWidget::mediaStateChanged);
#endif
    connect(mMediaPlayer, &QMediaPlayer::positionChanged, this, &ShowVideoWidget::positionChanged);
    connect(mMediaPlayer, &QMediaPlayer::durationChanged, this, &ShowVideoWidget::durationChanged);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(mMediaPlayer, qOverload<QMediaPlayer::Error>(&QMediaPlayer::error), this, &ShowVideoWidget::handleError);
#endif
    mSoundButton->setCheckable(true);
    mSoundButton->setObjectName(QStringLiteral("mSoundButton"));
    mSoundButton->setIcon(QIcon::fromTheme(QStringLiteral("player-volume")));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(mSoundButton, &QToolButton::clicked, mMediaPlayer, &QMediaPlayer::setMuted);
    connect(mMediaPlayer, &QMediaPlayer::mutedChanged, this, &ShowVideoWidget::muteChanged);
#endif
    controlLayout->addWidget(mSoundButton);
    mSoundSlider->setObjectName(QStringLiteral("mSoundSlider"));
    mSoundSlider->setRange(0, 100);
    mSoundSlider->setValue(100);
    mSoundSlider->setTickPosition(QSlider::TicksAbove);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(mSoundSlider, &QAbstractSlider::valueChanged, mMediaPlayer, &QMediaPlayer::setVolume);
#endif
    controlLayout->addWidget(mSoundSlider);
}

ShowVideoWidget::~ShowVideoWidget() = default;

void ShowVideoWidget::positionChanged(qint64 progress)
{
    if (!mPositionSlider->isSliderDown())
        mPositionSlider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}

void ShowVideoWidget::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || mDuration) {
        const QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60, currentInfo % 60, (currentInfo * 1000) % 1000);
        const QTime totalTime((mDuration / 3600) % 60, (mDuration / 60) % 60, mDuration % 60, (mDuration * 1000) % 1000);
        QString format = QStringLiteral("mm:ss");
        if (mDuration > 3600)
            format = QStringLiteral("hh:mm:ss");
        tStr = currentTime.toString(format) + QStringLiteral(" / ") + totalTime.toString(format);
    }
    mLabelDuration->setText(tStr);
}

void ShowVideoWidget::muteChanged(bool state)
{
    mSoundButton->setIcon(state ? QIcon::fromTheme(QStringLiteral("player-volume-muted")) : QIcon::fromTheme(QStringLiteral("player-volume")));
}

void ShowVideoWidget::setVideoUrl(const QUrl &url)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    mErrorLabel->setText(QString());
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    mMediaPlayer->setMedia(url);
    mPlayButton->setEnabled(true);
#endif
}

void ShowVideoWidget::play()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    switch (mMediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        mMediaPlayer->pause();
        break;
    default:
        mMediaPlayer->play();
        break;
    }
#endif
}
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void ShowVideoWidget::mediaStateChanged(QMediaPlayer::State state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        mPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        mPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}
#endif

void ShowVideoWidget::durationChanged(qint64 duration)
{
    mDuration = duration / 1000;
    mPositionSlider->setMaximum(mDuration);
}

void ShowVideoWidget::setPosition(int position)
{
    mMediaPlayer->setPosition(position * 1000);
}

void ShowVideoWidget::handleError()
{
    mPlayButton->setEnabled(false);
    const QString errorString = mMediaPlayer->errorString();
    QString message = i18n("Error: "); // i18n ?
    if (errorString.isEmpty()) {
        message += QStringLiteral(" #") + QString::number(int(mMediaPlayer->error()));
    } else {
        message += errorString;
    }
    mErrorLabel->setText(message);
}
