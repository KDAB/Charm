#include <QMessageBox>
#include <QtPlugin>

#include "Application.h"

#if defined CHARM_BUILD_STATIC_QT
Q_IMPORT_PLUGIN(qjpeg)
#endif

int main ( int argc, char** argv )
{
	Application app ( argc,  argv );
	return app.exec();
}
