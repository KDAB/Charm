#ifndef WEEKLYSUMMARY_H
#define WEEKLYSUMMARY_H

#include <QString>
#include <QVector>
#include "Core/Task.h"

class WeeklySummary
{
public:
    WeeklySummary();

    TaskId task;
    QString taskname;
    QVector<int> durations;
};

#endif // WEEKLYSUMMARY_H
