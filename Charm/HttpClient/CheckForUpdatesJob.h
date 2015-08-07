#ifndef CHECKFORUPDATESJOB
#define CHECKFORUPDATESJOB

#include <QObject>
#include <QUrl>

class QByteArray;
class QNetworkReply;
class QUrl;

class CheckForUpdatesJob : public QObject
{
    Q_OBJECT
public:

    struct JobData {
        QUrl link;
        QString releaseInformationLink;
        QString releaseVersion;
        QString errorString;
        int error = 0; // QNetworkReply::NetworkError or xml parsing error ( 999 )
        bool verbose = false; // display error message or not ( default == not )
    };

    explicit CheckForUpdatesJob( QObject* parent=nullptr );
    ~CheckForUpdatesJob();

    void start();
    void setUrl( const QUrl& url );
    void setVerbose( bool verbose );

signals:
    void finished( CheckForUpdatesJob::JobData data );

private slots:
    void jobFinished( QNetworkReply* reply );

private:
    void parseXmlData( const QByteArray& data );

    QUrl m_url;
    JobData m_jobData;
};

#endif // CHECKFORUPDATESJOB

