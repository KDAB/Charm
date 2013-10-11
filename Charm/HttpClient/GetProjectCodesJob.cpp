#include "GetProjectCodesJob.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QTimer>

GetProjectCodesJob::GetProjectCodesJob(QObject* parent)
    : HttpJob(parent)
{
    QSettings s;
    s.beginGroup(QLatin1String("httpconfig"));
    setDownloadUrl(s.value(QLatin1String("projectCodeDownloadUrl")).toUrl());
}

GetProjectCodesJob::~GetProjectCodesJob()
{
}

QByteArray GetProjectCodesJob::payload() const
{
    return m_payload;
}

bool GetProjectCodesJob::execute(int state, QNetworkAccessManager *manager)
{
    if (state != GetProjectCodes)
        return HttpJob::execute(state, manager);
    QNetworkRequest request(m_downloadUrl);

    QNetworkReply *reply = manager->get(request);

    if (reply->error() != QNetworkReply::NoError)
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
    return true;
}

bool GetProjectCodesJob::handle(QNetworkReply *reply)
{
    /* check for failure */
    if (reply->error() != QNetworkReply::NoError) {
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
        return false;
    }

    if (state() != GetProjectCodes)
        return HttpJob::handle(reply);

    m_payload = reply->readAll();
    delayedNext();
    return true;
}

QUrl GetProjectCodesJob::downloadUrl() const
{
    return m_downloadUrl;
}

void GetProjectCodesJob::setDownloadUrl(const QUrl& url)
{
    m_downloadUrl = url;
}

QString GetProjectCodesJob::dialogTitle() const
{
    return tr("Downloading");
}

#include "GetProjectCodesJob.moc"
