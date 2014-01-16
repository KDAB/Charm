#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <QObject>

class Controller;
class CharmDataModel;
class Configuration;

class TestApplication : public QObject
{
    Q_OBJECT
public:
    explicit TestApplication(const QString &databasePath, QObject *parent = 0);

    void initialize();
    void destroy();
    int testUserId();
    int testInstallationId();

protected:
    Controller* controller();
    CharmDataModel* model();
    Configuration* configuration();
    QString databasePath();

private:
    Controller* m_controller;
    CharmDataModel* m_model;
    Configuration* m_configuration;
    QString m_localPath;
};

#endif // TESTAPPLICATION_H
