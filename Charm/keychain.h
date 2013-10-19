/******************************************************************************
 *   Copyright (C) 2011-2013 Frank Osterfeld <frank.osterfeld@gmail.com>      *
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE. For licensing and distribution        *
 * details, check the accompanying file 'COPYING'.                            *
 *****************************************************************************/
#ifndef KEYCHAIN_H
#define KEYCHAIN_H

#include <QtCore/QObject>
#include <QtCore/QString>

class QSettings;

#define QTKEYCHAIN_VERSION 0x000100

namespace QKeychain {

/**
 * Error codes
 */
enum Error {
    NoError=0, /**< No error occurred, operation was successful */
    EntryNotFound, /**< For the given key no data was found */
    CouldNotDeleteEntry, /**< Could not delete existing secret data */
    AccessDeniedByUser, /**< User denied access to keychain */
    AccessDenied, /**< Access denied for other reasons */
    NoBackendAvailable, /**< No platform-specific keychain service available */
    NotImplemented, /**< Not implemented on platform */
    OtherError /**< Something else went wrong (errorString() might provide details) */
};

class JobExecutor;
class JobPrivate;

class Job : public QObject {
    Q_OBJECT
public:
    explicit Job( const QString& service, QObject* parent=0 );
    ~Job();

    QSettings* settings() const;
    void setSettings( QSettings* settings );

    void start();

    QString service() const;

    Error error() const;
    QString errorString() const;

    bool autoDelete() const;
    void setAutoDelete( bool autoDelete );

    bool insecureFallback() const;
    void setInsecureFallback( bool insecureFallback );

Q_SIGNALS:
    void finished( QKeychain::Job* );

protected:
    Q_INVOKABLE virtual void doStart() = 0;

    void setError( Error error );
    void setErrorString( const QString& errorString );
    void emitFinished();
    void emitFinishedWithError(Error, const QString& errorString);

private:
    JobPrivate* const d;
};

class ReadPasswordJobPrivate;

class ReadPasswordJob : public Job {
    Q_OBJECT
public:
    explicit ReadPasswordJob( const QString& service, QObject* parent=0 );
    ~ReadPasswordJob();

    QString key() const;
    void setKey( const QString& key );

    QByteArray binaryData() const;
    QString textData() const;

protected:
    void doStart();

private:
    friend class QKeychain::ReadPasswordJobPrivate;
    friend class QKeychain::JobExecutor;
    ReadPasswordJobPrivate* const d;
};

class WritePasswordJobPrivate;

class WritePasswordJob : public Job {
    Q_OBJECT
public:
    explicit WritePasswordJob( const QString& service, QObject* parent=0 );
    ~WritePasswordJob();

    QString key() const;
    void setKey( const QString& key );

    void setBinaryData( const QByteArray& data );
    void setTextData( const QString& data );

protected:
    void doStart();

private:
    friend class QKeychain::JobExecutor;
    friend class QKeychain::WritePasswordJobPrivate;
    WritePasswordJobPrivate* const d;
};

class DeletePasswordJobPrivate;

class DeletePasswordJob : public Job {
    Q_OBJECT
public:
    explicit DeletePasswordJob( const QString& service, QObject* parent=0 );
    ~DeletePasswordJob();

    QString key() const;
    void setKey( const QString& key );

protected:
    void doStart();

private:
    friend class QKeychain::DeletePasswordJobPrivate;
    DeletePasswordJobPrivate* const d;
};

} // namespace QtKeychain

#endif
