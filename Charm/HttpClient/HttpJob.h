#ifndef HTTPJOB_H
#define HTTPJOB_H

#include <QObject>
#include <QPointer>
#include <QUrl>
#include <QWidget>

namespace QKeychain {
    class Job;
}

class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;
class QSslError;

class HttpJob : public QObject
{
    Q_OBJECT
public:

    static bool credentialsAvailable();

    enum Error {
        NoError=0,
        Canceled,
        NotConfigured,
        AuthenticationFailed,
        SomethingWentWrong
    };

    explicit HttpJob(QObject* parent=0);
    ~HttpJob();

    QString username() const;
    void setUsername(const QString &value);

    QString password() const;
    void setPassword(const QString &value);

    QUrl portalUrl() const;
    void setPortalUrl(const QUrl &value);

    QUrl loginUrl() const;
    void setLoginUrl(const QUrl &value);

    QString errorString() const;
    int error() const;

    void start();
    void cancel();

    QWidget* parentWidget() const;
    void setParentWidget(QWidget* pw);

signals:
    void finished(HttpJob*);

protected:

    enum State {
        Ready = 0,
        Init,
        Login,
        Portal,
        Base
    };

    int state() const;

    virtual bool execute(int state, QNetworkAccessManager *manager);

    virtual QString dialogTitle() const = 0;

    void emitFinished();
    void setErrorAndEmitFinished(int code, const QString& errorString);
    void delayedNext();

protected Q_SLOTS:
    virtual bool handle(QNetworkReply *reply);

private Q_SLOTS:
    void doStart();
    void doCancel();
    void next();
    void passwordRead(QKeychain::Job*);
    void passwordWritten();

private:
    QNetworkAccessManager *m_networkManager;
    QString m_username;
    QString m_password;
    int m_currentState;
    int m_errorCode;
    QString m_errorString;
    QPointer<QWidget> m_parentWidget;
    QPointer<QProgressDialog> m_dialog;
    QUrl m_loginUrl;
    QUrl m_portalUrl;
    bool m_lastAuthenticationFailed;
};

#endif
