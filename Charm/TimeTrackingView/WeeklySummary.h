#ifndef WEEKLYSUMMARY_H
#define WEEKLYSUMMARY_H

#include <QString>
#include <QVector>

#include "Core/Task.h"
#include "Core/TimeSpans.h"

class CharmDataModel;

class WeeklySummary
{
public:
    static QVector<WeeklySummary> summariesForTimespan( CharmDataModel* dataModel, const TimeSpan& timespan );

    WeeklySummary();

    TaskId task;
    QString taskname;
    QVector<int> durations;
};

#endif // WEEKLYSUMMARY_H
