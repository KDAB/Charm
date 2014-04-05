#ifndef GETTIMESHEETSTATUSJOB_H
#define GETTIMESHEETSTATUSJOB_H

#include "HttpGetJob.h"

class GetTimesheetStatusJob : public HttpGetJob
{
    Q_OBJECT
public:

    explicit GetTimesheetStatusJob(QObject *parent = 0);
    ~GetTimesheetStatusJob();

    int requestOffset() const;
    void setRequestOffset(int offset);

    int requestCount() const;
    void setRequestCount(int count);

private:

    void updateDownloadUrl();

private:

    QUrl m_downloadUrl;
    int m_requestOffset;
    int m_requestCount;
};

#endif
