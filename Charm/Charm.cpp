#include <QMessageBox>
#include <QtPlugin>

#include "Core/CharmExceptions.h"
#include "Application.h"

#if defined CHARM_BUILD_STATIC_QT
Q_IMPORT_PLUGIN(qjpeg)
#endif

int main ( int argc, char** argv )
{
	Application app ( argc,  argv );
	try {
		return app.exec();
	} catch ( UnsupportedDatabaseVersionException& e ) {
		QMessageBox::critical( 0,
				QObject::tr( "Charm Database Error"),
				QObject::tr( e.what() ) );
		return 1;
	}
	return 0;
}
