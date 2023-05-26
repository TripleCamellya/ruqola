/*
   SPDX-FileCopyrightText: 2020-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include <QWidget>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QMediaCaptureSession>
#endif
class QLabel;
class QCamera;
class QComboBox;
class QVideoWidget;
class QCameraDevice;
class QMediaRecorder;
class QToolButton;
class QMediaCaptureSession;
class LIBRUQOLAWIDGETS_TESTS_EXPORT CreateVideoMessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CreateVideoMessageWidget(QWidget *parent = nullptr);
    ~CreateVideoMessageWidget() override;

private:
    void stopCamera();
    void startCamera();
    void updateCameras();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void setCamera(const QCameraDevice &cameraDevice);
#endif
    void record();
    void pause();
    void stop();
    void updateRecordTime();
    void displayRecorderError();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QVideoWidget *const mVideoWidget;
    QScopedPointer<QMediaRecorder> mMediaRecorder;
    QMediaCaptureSession mCaptureSession;
#endif
    QLabel *const mErrorLabel;
    QCamera *const mCamera;
    QComboBox *const mListCamera;
    QToolButton *const mRecordButton;
    QLabel *const mDurationLabel;
};
