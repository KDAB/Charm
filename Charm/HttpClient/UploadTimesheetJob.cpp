#include "UploadTimesheetJob.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QTimer>

UploadTimesheetJob::UploadTimesheetJob(QObject* parent)
    : HttpJob(parent), m_fileName("payload")
{
    QSettings s;
    s.beginGroup(QLatin1String("httpconfig"));
    setUploadUrl(s.value(QLatin1String("timesheetUploadUrl")).toUrl());
}

UploadTimesheetJob::~UploadTimesheetJob()
{
}

QByteArray UploadTimesheetJob::payload() const
{
    return m_payload;
}

void UploadTimesheetJob::setPayload(const QByteArray &_payload)
{
    m_payload = _payload;
}

QString UploadTimesheetJob::fileName() const
{
    return m_fileName;
}

void UploadTimesheetJob::setFileName(const QString &_fileName)
{
    m_fileName = _fileName;
}

QUrl UploadTimesheetJob::uploadUrl() const
{
    return m_uploadUrl;
}

void UploadTimesheetJob::setUploadUrl(const QUrl& url)
{
    m_uploadUrl = url;
}

bool UploadTimesheetJob::execute(int state, QNetworkAccessManager *manager)
{
    if (state != UploadTimesheet)
        return HttpJob::execute(state, manager);

    QByteArray data;
    QByteArray uploadName;

    /* validate filename */
    if (!m_fileName.contains(QRegExp("^WeeklyTimeSheet-\\d\\d\\d\\d-\\d\\d$"))) {
        qDebug("Invalid filename encountered, using default (\"payload\").");
        uploadName = "payload";
    }
    else uploadName = m_fileName.toUtf8();

    /* username */
    data += "--KDAB\r\n"
            "Content-Disposition: form-data; name=\"user\"\r\n\r\n";
    data += username().toUtf8();
    data += "\r\n";

    /* payload */
    data += "--KDAB\r\n"
            "Content-Disposition: form-data; name=\"" + uploadName + "\"; filename=\"" +
            uploadName + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
    data += m_payload;
    data += "\r\n";

    /* eot */
    data += "--KDAB--\r\n";

    QNetworkRequest request(m_uploadUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=KDAB");
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());

    QNetworkReply *reply = manager->post(request, data);

    if (reply->error() != QNetworkReply::NoError)
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
     return true;
}

bool UploadTimesheetJob::handle(QNetworkReply *reply)
{
    /* check for failure */
    if (reply->error() != QNetworkReply::NoError) {
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
        return false;
    }

    if (state() != UploadTimesheet)
        return HttpJob::handle(reply);

    const QByteArray answer = reply->readAll();

    if (answer.contains("SuccessResultMessage")) {
        delayedNext();
    } else {
        setErrorAndEmitFinished(SomethingWentWrong, tr("No confirmation from server."));
    }
    return true;
}

QString UploadTimesheetJob::dialogTitle() const
{
    return tr("Uploading");
}

#include "UploadTimesheetJob.moc"
