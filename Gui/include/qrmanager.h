#pragma once

#include "qrcodegen.hpp"

// #include <QNetworkAccessManager>
// #include <QObject>
// #include <QPixmap>

#include <QObject>
#include <QPixmap>
#include <QString>

// Ilmoitetaan luokat, mutta ei ladata niiden määrittelyjä vielä
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

using namespace qrcodegen;
using qrcodegen::QrCode;

class QrManager : public QObject {
    Q_OBJECT
  public:
    explicit QrManager(QObject* parent = nullptr);
    void start();

  signals:
    void qrCodeChanged(const QPixmap& pixmap);
    void imageReady(const QByteArray& image);

  private slots:
    void checkStatus();
    void downloadImage();
    void onStatusFinished();
    void onImageDownloaded();
    void onNewSessionFinished();

  private:
    QString m_currentUuid;
    QString m_baseUrl = "https://sudoscan.duckdns.org";
    QTimer* m_pollTimer;
    QTimer* m_sessionTimer;

    QNetworkAccessManager* m_network;

    QPixmap generateQrPixmap(const QString& text);
    void fetchNewSession();
};
