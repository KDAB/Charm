#ifndef CHARMREPORT_H
#define CHARMREPORT_H

#include <QMap>
#include <QObject>

#include "Task.h"
#include "Event.h"

class QWidget;

class ReportDialog;
class StorageInterface;
class QTextDocument;
class ParagraphFormatCollection;

class CharmReport : public QObject
{
    Q_OBJECT

public:
    explicit CharmReport( QObject* parent = 0 );
    virtual ~CharmReport();

    // fetch all necessary data to create the report
    // after calling that, it should not be necessary to query the
    // storage again. create() will make the actual report later.
    virtual bool prepare();

    // create the report.
    // return true if successfull, false otherwise
    virtual bool create() = 0;

    // return the report as a QTextDocument
    // this method may change or be removed
    virtual QTextDocument* report() = 0;

    // a literal description of the report
    virtual QString description() = 0;

    // the literal name of the report
    virtual QString name() = 0;

    // the configuration widget for this dialog
    // use the report dialog pointer to connect to it's accept
    // or reject signals
    virtual QWidget* configurationPage( ReportDialog* ) = 0;

signals:
    // emit this to signal that the user has accepted the
    // configuration page:
    void accept();

protected slots:
        /** Create a report window with the content that report() returns,
        and show it. The window will be delete-on-close, so the
        creator does not have tp care for it. It will provide the
        printing functionality to the user. */
    void makeReportPreviewWindow();

protected:
    // access the default paragraph format collection
    ParagraphFormatCollection& paragraphFormats();
};

// the following are helper functions used to create reports
/** Return true if first starts before second. */
bool StartsEarlier( EventId firstId, EventId secondId );
/** Make a task name that includes the parent task's name.*/
QString tasknameWithParents( const Task& );
/** Return all events that start at or after start, and start before end. */
EventIdList eventsThatStartInTimeFrame( const QDateTime& start,
                                        const QDateTime& end );
/** Returns true if the task p is a possibly indirect parent of the c. */
bool isIndirectParent( const Task& p, const Task& c );
/** A string containing hh:mm for the given duration of seconds. */
QString hoursAndMinutes( int seconds );

#endif
