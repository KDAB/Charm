#ifndef UPLOADTIMESHEETJOB_H
#define UPLOADTIMESHEETJOB_H

#include "HttpJob.h"

#include <QUrl>

class UploadTimesheetJob : public HttpJob
{
    Q_OBJECT
public:

    explicit UploadTimesheetJob(QObject* parent=0);
    ~UploadTimesheetJob();

    QByteArray payload() const;
    void setPayload(const QByteArray &payload);
    QString fileName() const;
    void setFileName(const QString &fileName);
    QUrl uploadUrl() const;
    void setUploadUrl(const QUrl& url);

public slots:

    virtual bool execute(int state, QNetworkAccessManager *manager);
    virtual bool handle(QNetworkReply *reply);

protected:

    enum State {
        UploadTimesheet = HttpJob::Base
    };

    QString dialogTitle() const;

private:
    QByteArray m_payload;
    QString m_fileName;
    QUrl m_uploadUrl;
};

#endif
