#ifndef HTTPGETJOB_H
#define HTTPGETJOB_H

#include "HttpJob.h"

#include <QUrl>

class HttpGetJob : public HttpJob
{
    Q_OBJECT
    Q_DISABLE_COPY(HttpGetJob)
public:
    explicit HttpGetJob(QObject *parent = 0);

    QUrl downloadUrl() const;
    void setDownloadUrl(const QUrl &url);

    QByteArray payload() const;

public slots:
    bool execute(int state, QNetworkAccessManager *manager); //Q_DECL_OVERRIDE
    bool handle(QNetworkReply *reply); //Q_DECL_OVERRIDE

protected:

    enum State {
        Download = HttpJob::Base
    };

    ~HttpGetJob();

    QString dialogTitle() const; // Q_DECL_OVERRIDE

private:
    QByteArray m_payload;
    QUrl m_downloadUrl;
};

#endif
