#include <QtDebug>

#include "Core/CharmConstants.h"
#include "ModelConnector.h"
#include "Application.h"
#include "ViewHelpers.h"
#include "CharmReport.h"
#include "Core/CharmDataModel.h"
#include "ReportPreviewWindow.h"
#include "ParagraphFormatCollection.h"

CharmReport::CharmReport( QObject* parent )
    : QObject( parent )
{
}

CharmReport::~CharmReport()
{
}

bool CharmReport::prepare()
{
    return true;
}

void CharmReport::makeReportPreviewWindow()
{
    if ( prepare() && create() ) {
        qDebug() << "CharmReport::makeReportPreviewWindow: making it";
        ReportPreviewWindow* window = new ReportPreviewWindow( &VIEW );
        // FIXME TEMP_REM
        // window->setDocument( report() );
        window->show();
    } else {
        qDebug() << "CharmReport::makeReportPreviewWindow: prepare or create failed" ;
    }
}

int pointSize( float relSize, float scale )
{
    return static_cast<int>( relSize * scale );
}

ParagraphFormatCollection& CharmReport::paragraphFormats()
{
    static bool Initialized = false;
    static ParagraphFormatCollection Formats;
    if ( !Initialized ) {
        // initialize defaults
        const float FontBaseSize = 10.0;
        const float FontScaleFactor = 1.0;

#define RELFONTSIZE(x) ( pointSize( (x) * FontBaseSize, FontScaleFactor) )

        {   // default
            ParagraphFormat format;
            format.setName( "default" );
            format.blockFormat().setAlignment( Qt::AlignLeft );
            format.blockFormat().setBottomMargin( 6 );
            format.charFormat().setFontPointSize( RELFONTSIZE( 1.2 ) );
            format.charFormat().setFontFamily( "Times" );
            Formats.add( format);
        }
        // headlines
        {   // headline - level 0
            ParagraphFormat format = Formats.format( "default" );
            format.setName( "Headline" );
            format.blockFormat().setBottomMargin( 12 );
            format.charFormat().setFontPointSize( RELFONTSIZE( 2.0 ) );
            Formats.add( format );
        }
        {   // headline - level 1
            ParagraphFormat format = Formats.format( "default" );
            format.setName( "Headline1" );
            format.blockFormat().setBottomMargin( 9 );
            format.charFormat().setFontPointSize( RELFONTSIZE( 1.66666 ) );
            Formats.add( format );
        }
        {   // text format - small text
            ParagraphFormat format = Formats.format( "default" );
            format.setName( "default-small" );
            format.charFormat().setFontPointSize( RELFONTSIZE( 0.9 ) );
            Formats.add( format );
        }
        {   // text format - default italic
            ParagraphFormat format = Formats.format( "default" );
            format.setName( "default-italic" );
            format.charFormat().setFontItalic( true );
            Formats.add( format );
        }
        {   // text format - default sans
            ParagraphFormat format = Formats.format( "default" );
            format.setName( "default-sans" );
            format.charFormat().setFontFamily( "Sans Serif" );
            Formats.add( format );
        }


        // more advanced formats: take the primitives, and make them semantic:
        {   // basic table format
            ParagraphFormat format = Formats.format( "default-sans" );
            format.setName( "table-data" );
            format.charFormat().setFontPointSize( RELFONTSIZE( 0.9 ) );
            Formats.add( format );
        }
        {   // table format, italic
            ParagraphFormat format = Formats.format( "default-sans" );
            format.setName( "table-data-italic" );
            format.charFormat().setFontItalic( true );
            format.charFormat().setFontPointSize( RELFONTSIZE( 0.9 ) );
            Formats.add( format );
        }
        {   // table header cells
            ParagraphFormat format = Formats.format( "table-data" );
            format.setName( "table-header" );
            format.charFormat().setFontPointSize( RELFONTSIZE( 1.2 ) );
            Formats.add( format );
        }


        {   // table header cells - odd rows
            ParagraphFormat format = Formats.format( "table-data" );
            format.setName( "table-data-odd" );
            Formats.add( format );
        }

        {   // table header cells - even rows
            ParagraphFormat format = Formats.format( "table-data" );
            format.setName( "table-data-even" );
            format.blockFormat().setBackground( QBrush( "lightgray" ) );
            Formats.add( format );
        }



        Initialized = true;
    }

    return Formats;
}

QString tasknameWithParents( const Task& task )
{
    if ( task.isValid() ) {
        QString name = task.name().simplified();

        if ( task.parent() != 0 ) {
            const Task& parent = DATAMODEL->getTask( task.parent() );
            if ( parent.isValid() ) {
                name = tasknameWithParents( parent ) + '/' + name;
            }
        }
        return name;
    } else {
        // qWarning() << "CharmReport::tasknameWithParents: WARNING: invalid task"
        //                    << task.id();
        return QString::null;
    }
}

bool StartsEarlier( EventId firstId, EventId secondId )
{
    const Event& first = DATAMODEL->eventForId( firstId );
    const Event& second = DATAMODEL->eventForId( secondId );
    return first.startDateTime() < second.startDateTime();
}

bool isIndirectParent( const Task& p, const Task& c )
{
    if ( c.parent() == p.id() ) return true;
    if ( c.parent() == 0 ) return false;
    if ( p.parent() == 0 ) return false;

    const TaskTreeItem& item = DATAMODEL->taskTreeItem( p.parent() );
    return isIndirectParent( item.task(), p );
}

#include "CharmReport.moc"
