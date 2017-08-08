#ifndef LOTSOFCAKE_CONFIGURATION_H
#define LOTSOFCAKE_CONFIGURATION_H

class QString;
class QUrl;

class TaskExport;

namespace Lotsofcake {

class Configuration {
public:
    bool isConfigured() const;

    void importFromTaskExport(const TaskExport &exporter);

    QString username() const;
    QUrl timesheetUploadUrl() const;
    QUrl projectCodeDownloadUrl() const;
    QUrl restUrl() const;
};

}

#endif
