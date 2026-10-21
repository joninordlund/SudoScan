#include "include/qrmanager.h"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QUuid>

const int POLL_INTERVAL = 500;
const int SESSION = 5 * 60 * 1000;

QrManager::QrManager(QObject* parent)
{
    m_network = new QNetworkAccessManager(this);
    m_pollTimer = new QTimer(this);
    m_sessionTimer = new QTimer(this);
}

QPixmap QrManager::generateQrPixmap(const QString& text)
{
    QrCode qr = QrCode::encodeText(text.toUtf8().constData(), QrCode::Ecc::LOW);

    int border = 4;
    int displaySize = 120;
    int qrSize = qr.getSize();

    double scale = static_cast<double>(displaySize - border * 2) / qrSize;

    QPixmap pixmap(displaySize, displaySize);

    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), Qt::white);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);

    for (int y = 0; y < qrSize; y++)
    {
        for (int x = 0; x < qrSize; x++)
        {
            if (qr.getModule(x, y))
            {
                QRectF rect(x * scale + border, y * scale + border, scale, scale);
                painter.drawRect(rect);
            }
        }
    }
    return pixmap;
}

void QrManager::start()
{
    connect(m_pollTimer, &QTimer::timeout, this, &QrManager::checkStatus);
    m_pollTimer->start(POLL_INTERVAL);
    connect(m_sessionTimer, &QTimer::timeout, this, &QrManager::fetchNewSession);
    m_sessionTimer->start(SESSION);
    fetchNewSession();
}

void QrManager::checkStatus()
{
    QUrl url(m_baseUrl + "/api/status/" + m_currentUuid);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    // request.setRawHeader("ngrok-skip-browser-warning", "any-value");
    QNetworkReply* reply = m_network->get(request);
    connect(reply, &QNetworkReply::finished, this, &QrManager::onStatusFinished);
}

void QrManager::onStatusFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.object()["ready"].toBool())
        {
            qDebug() << "ready";
            m_pollTimer->stop();
            m_sessionTimer->stop();
            downloadImage();
        }
    }
    reply->deleteLater();
}

void QrManager::downloadImage()
{
    QUrl url(m_baseUrl + "/api/download/" + m_currentUuid);
    QNetworkRequest request(url);
    // request.setRawHeader("ngrok-skip-browser-warning", "any-value");
    QNetworkReply* reply = m_network->get(request);
    connect(reply, &QNetworkReply::finished, this, &QrManager::onImageDownloaded);
}

void QrManager::onImageDownloaded()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = reply->readAll();
        QApplication::beep();
        emit imageReady(data);
        fetchNewSession();
    }
    reply->deleteLater();
}

void QrManager::fetchNewSession()
{
    QUrl url(m_baseUrl + "/api/new-session");
    QNetworkRequest request(url);
    // request.setRawHeader("ngrok-skip-browser-warning", "any-value");
    // request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    QNetworkReply* reply = m_network->get(request);
    connect(reply, &QNetworkReply::finished, this, &QrManager::onNewSessionFinished);
}

void QrManager::onNewSessionFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        m_currentUuid = doc.object()["uuid"].toString();
        qDebug() << m_baseUrl + "/c/" + m_currentUuid;
        emit qrCodeChanged(generateQrPixmap(m_baseUrl + "/c/" + m_currentUuid));

        if (!m_pollTimer->isActive())
        {
            m_pollTimer->start(POLL_INTERVAL);
        }
        if (!m_sessionTimer->isActive())
        {
            m_sessionTimer->start(SESSION);
        }
    }
    else
    {
        qDebug() << "Session fetch failed:" << reply->errorString();
    }
    reply->deleteLater();
}
