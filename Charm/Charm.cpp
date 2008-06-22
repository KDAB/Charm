#include <QMessageBox>
#include <QtPlugin>

#include "Core/CharmExceptions.h"
#include "Application.h"

Q_IMPORT_PLUGIN(qjpeg)

int main ( int argc, char** argv )
{
	Application app ( argc,  argv );
	try {
		return app.exec();
	} catch ( UnsupportedDatabaseVersionException& e ) {
		QMessageBox::critical( 0,
				QObject::tr( "Charm Database Error"),
				QObject::tr( e.what() ) );
	}
}
