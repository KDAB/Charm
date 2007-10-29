#include "ReportConfigurationPage.h"

ReportConfigurationPage::ReportConfigurationPage( ReportDialog* parent )
    : QWidget( parent )
{
    connect( this, SIGNAL( accept() ),
             parent, SLOT( accept() ) );
    connect( this, SIGNAL( back() ),
             parent, SLOT( back() ) );
}

#include "ReportConfigurationPage.moc"
