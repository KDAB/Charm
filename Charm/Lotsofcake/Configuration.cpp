#include "Lotsofcake/Configuration.h"
#include "Core/XmlSerialization.h"

#include <QSettings>
#include <QUrl>

static const char *s_group = "httpconfig";
static const char *s_keyUsername = "username";
static const char *s_keyTimesheetUploadUrl = "timesheetUploadUrl";
static const char *s_keyProjectCodeDownloadUrl = "projectCodeDownloadUrl";
static const char *s_keyRestUrl = "restUrl";

static void setValueIfNotNull(QSettings *s, const QString &key, const QString &value)
{
    if (!value.isNull()) {
        s->setValue(key, value);
    } else {
        s->remove(key);
    }
}

bool Lotsofcake::Configuration::isConfigured() const
{
    return !username().isEmpty();
}

void Lotsofcake::Configuration::importFromTaskExport(const TaskExport &exporter)
{
    QSettings settings;
    settings.beginGroup(QLatin1String(s_group));
    setValueIfNotNull(&settings, QLatin1String(s_keyUsername),
                      exporter.metadata(QStringLiteral("username")));
    setValueIfNotNull(&settings, QLatin1String(s_keyTimesheetUploadUrl),
                      exporter.metadata(QStringLiteral("timesheet-upload-url")));
    setValueIfNotNull(&settings, QLatin1String(s_keyProjectCodeDownloadUrl),
                      exporter.metadata(QStringLiteral("project-code-download-url")));
    setValueIfNotNull(&settings, QLatin1String(s_keyRestUrl), exporter.metadata(QStringLiteral("rest-url")));
}

QString Lotsofcake::Configuration::username() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String(s_group));
    return settings.value(QLatin1String(s_keyUsername)).toString();
}

QUrl Lotsofcake::Configuration::timesheetUploadUrl() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String(s_group));
    return settings.value(QLatin1String(s_keyTimesheetUploadUrl)).toUrl();
}

QUrl Lotsofcake::Configuration::projectCodeDownloadUrl() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String(s_group));
    return settings.value(QLatin1String(s_keyProjectCodeDownloadUrl)).toUrl();
}

QUrl Lotsofcake::Configuration::restUrl() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String(s_group));
    return settings.value(QLatin1String(s_keyRestUrl)).toUrl();
}

