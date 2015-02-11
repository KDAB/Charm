#include "GetUserInfoJob.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

GetUserInfoJob::GetUserInfoJob(QObject* parent, const QString &schema)
    : HttpJob(parent),
      m_schema(schema)
{
    QSettings s;
    s.beginGroup(m_schema);
    setDownloadUrl(s.value(QLatin1String("userInfoDownloadUrl")).toUrl());
}

GetUserInfoJob::~GetUserInfoJob()
{
}

QByteArray GetUserInfoJob::userInfo() const
{
    return m_userInfo;
}

QString GetUserInfoJob::schema() const {
    return m_schema;
}

void GetUserInfoJob::setSchema(const QString &schema) {
    m_schema = schema;
}



bool GetUserInfoJob::execute(int state, QNetworkAccessManager *manager)
{
    if (state != GetProjectCodes)
        return HttpJob::execute(state, manager);
    QNetworkRequest request(m_downloadUrl);

    QNetworkReply *reply = manager->get(request);

    if (reply->error() != QNetworkReply::NoError)
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
    return true;
}

bool GetUserInfoJob::handle(QNetworkReply *reply)
{

    /* check for failure */
    if (reply->error() != QNetworkReply::NoError) {
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
        return false;
    }

    m_userInfo = reply->readAll();

    delayedNext();
    return true;
}

QUrl GetUserInfoJob::downloadUrl() const
{
    return m_downloadUrl;
}

void GetUserInfoJob::setDownloadUrl(const QUrl& url)
{
    m_downloadUrl = url;
}
