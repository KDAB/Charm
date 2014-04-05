#include "GetProjectCodesJob.h"

#include <QSettings>

GetProjectCodesJob::GetProjectCodesJob(QObject* parent)
    : HttpGetJob(parent)
{
    QSettings s;
    s.beginGroup(QLatin1String("httpconfig"));
    setDownloadUrl(s.value(QLatin1String("projectCodeDownloadUrl")).toUrl());
}

GetProjectCodesJob::~GetProjectCodesJob()
{
}

#include "moc_GetProjectCodesJob.cpp"
