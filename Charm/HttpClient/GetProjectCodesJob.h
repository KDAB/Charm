#ifndef GETPROJECTCODESJOB_H
#define GETPROJECTCODESJOB_H

#include "HttpJob.h"

#include <QUrl>

class GetProjectCodesJob : public HttpJob
{
    Q_OBJECT
public:

    explicit GetProjectCodesJob(QObject* parent=0);
    ~GetProjectCodesJob();

    QByteArray payload() const;

    QUrl downloadUrl() const;
    void setDownloadUrl(const QUrl& url);

public slots:

    virtual bool execute(int state, QNetworkAccessManager *manager);
    virtual bool handle(QNetworkReply *reply);

protected:

    enum State {
        GetProjectCodes = HttpJob::Base
    };

    QString dialogTitle() const;

private:
    QByteArray m_payload;
    QUrl m_downloadUrl;
};

#endif
