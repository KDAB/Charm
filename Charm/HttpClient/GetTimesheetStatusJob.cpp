#include "GetTimesheetStatusJob.h"

#include <QSettings>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#   include <QUrlQuery>
#endif

GetTimesheetStatusJob::GetTimesheetStatusJob(QObject* parent)
    : HttpGetJob(parent)
    , m_requestOffset(0)
    , m_requestCount(1)
{
    QSettings s;
    s.beginGroup(QLatin1String("httpconfig"));
    m_downloadUrl = s.value(QLatin1String("timesheetsDownloadUrl")).toUrl();

    updateDownloadUrl();
}

GetTimesheetStatusJob::~GetTimesheetStatusJob()
{
}

int GetTimesheetStatusJob::requestOffset() const
{
    return m_requestOffset;
}

void GetTimesheetStatusJob::setRequestOffset(int offset)
{
    m_requestOffset = offset;
    updateDownloadUrl();
}

int GetTimesheetStatusJob::requestCount() const
{
    return m_requestCount;
}

void GetTimesheetStatusJob::setRequestCount(int count)
{
    m_requestCount = count;
    updateDownloadUrl();
}

void GetTimesheetStatusJob::updateDownloadUrl()
{
    static const QLatin1String sQueryStart("start");
    static const QLatin1String sQueryEnd("end");

    QUrl url = m_downloadUrl;

    if (m_requestOffset >= 0 && m_requestCount > 0) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        QUrlQuery query;
        query.addQueryItem(sQueryStart, QString::number(m_requestOffset));
        query.addQueryItem(sQueryEnd, QString::number(m_requestOffset + m_requestCount));
        url.setQuery(query);
#else
        url.addQueryItem(sQueryStart, QString::number(m_requestOffset));
        url.addQueryItem(sQueryEnd, QString::number(m_requestOffset + m_requestCount));
#endif
    }

    setDownloadUrl(url);
}

#include "moc_GetTimesheetStatusJob.cpp"
