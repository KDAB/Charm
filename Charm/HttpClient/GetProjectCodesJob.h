#ifndef GETPROJECTCODESJOB_H
#define GETPROJECTCODESJOB_H

#include "HttpGetJob.h"

class GetProjectCodesJob : public HttpGetJob
{
    Q_OBJECT
public:

    explicit GetProjectCodesJob(QObject* parent=0);
    ~GetProjectCodesJob();
};

#endif
