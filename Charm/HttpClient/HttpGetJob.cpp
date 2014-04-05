#include "HttpGetJob.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

HttpGetJob::HttpGetJob(QObject* parent)
    : HttpJob(parent)
{
}

HttpGetJob::~HttpGetJob()
{
}

QUrl HttpGetJob::downloadUrl() const
{
    return m_downloadUrl;
}

void HttpGetJob::setDownloadUrl(const QUrl& url)
{
    m_downloadUrl = url;
}

QByteArray HttpGetJob::payload() const
{
    return m_payload;
}

bool HttpGetJob::execute(int state, QNetworkAccessManager *manager)
{
    if (state != Download)
        return HttpJob::execute(state, manager);
    QNetworkRequest request(m_downloadUrl);

    QNetworkReply *reply = manager->get(request);

    if (reply->error() != QNetworkReply::NoError)
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
    return true;
}

bool HttpGetJob::handle(QNetworkReply *reply)
{
    /* check for failure */
    if (reply->error() != QNetworkReply::NoError) {
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
        return false;
    }

    if (state() != Download)
        return HttpJob::handle(reply);

    m_payload = reply->readAll();
    delayedNext();
    return true;
}

QString HttpGetJob::dialogTitle() const
{
    return tr("Downloading");
}

#include "moc_HttpGetJob.cpp"
