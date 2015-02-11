#ifndef GETUSERINFOJOB_H
#define GETUSERINFOJOB_H

#include "HttpJob.h"
#include <QUrl>
#include <QDebug>
#include <QVariant>
#include <QVariantMap>

class GetUserInfoJob : public HttpJob
{
    Q_OBJECT
public:

    explicit GetUserInfoJob(QObject* parent=0, const QString &schema = " ");
    ~GetUserInfoJob();

    QByteArray userInfo() const;

    QUrl downloadUrl() const;
    void setDownloadUrl(const QUrl& url);
    QString schema() const;
    void setSchema(const QString &schema);

public slots:

    virtual bool execute(int state, QNetworkAccessManager *manager);
    virtual bool handle(QNetworkReply *reply);

protected:

    enum State {
        GetProjectCodes = HttpJob::Base
    };

private:
    QByteArray m_userInfo;
    QUrl m_downloadUrl;
    QString m_schema;
};

#endif // GETUSERINFOJOB_H
