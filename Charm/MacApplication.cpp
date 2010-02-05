#include "MacApplication.h"

MacApplication::MacApplication( int& argc, char* argv[] )
    : QApplication( argc, argv )
{
}

QList< QShortcut* > MacApplication::shortcuts( QWidget* parent )
{
    QList< QShortcut* > result;
    if( parent == 0 )
    {
        const QWidgetList widgets = QApplication::topLevelWidgets();
        for( QWidgetList::const_iterator it = widgets.begin(); it != widgets.end(); ++it )
            result += shortcuts( *it );
    }
    else
    {
        const QList< QShortcut* > cuts = parent->findChildren< QShortcut* >();
        for( QList< QShortcut* >::const_iterator it = cuts.begin(); it != cuts.end(); ++it )
            if( (*it)->context() == Qt::ApplicationShortcut )
                result.push_back( *it );

        const QList< QWidget* > children = parent->findChildren< QWidget* >();
        for( QList< QWidget* >::const_iterator it = children.begin(); it != children.end(); ++it )
            result += shortcuts( *it );
    }
    return result;
}

QList< QShortcut* > MacApplication::activeShortcuts( const QKeySequence& seq, bool autorep, QWidget* parent )
{
    const QList< QShortcut* > cuts = shortcuts( parent );
    QList< QShortcut* > result;
    for( QList< QShortcut* >::const_iterator it = cuts.begin(); it != cuts.end(); ++it )
        if( (*it)->context() == Qt::ApplicationShortcut && ((*it)->autoRepeat() == autorep || !autorep ) && (*it)->isEnabled() && (*it)->key().matches( seq ) )
            result.push_back( *it );
    return result;
}

#include "MacApplication.moc"
