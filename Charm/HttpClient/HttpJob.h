#ifndef HTTPJOB_H
#define HTTPJOB_H

#include <QObject>
#include <QUrl>

namespace QKeychain {
    class Job;
}

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QAuthenticator;

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
    void provideRequestedPassword(const QString &password);
    void passwordRequestCanceled();

Q_SIGNALS:
    void finished(HttpJob*);
    /**
     * the actual communication was started
     */
    void transferStarted();

    /**
     * The job requests the password from the user
     *
     * Must be replied to via provideRequestedPassword() or passwordRequestCanceled() to continue.
     */
    void passwordRequested();

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
    void authenticationRequired(QNetworkReply *reply , QAuthenticator *authenticator);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_username;
    QString m_password;
    int m_currentState;
    int m_errorCode;
    QString m_errorString;
    QUrl m_loginUrl;
    QUrl m_portalUrl;
    bool m_lastAuthenticationFailed;
    bool m_authenticationDoneAlready;
    bool m_passwordReadError;
};

#endif
