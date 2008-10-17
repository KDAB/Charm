#include <QMessageBox>
#include <QtPlugin>

#include "Application.h"
#include "CharmVersion.h"

#if defined CHARM_BUILD_STATIC_QT
Q_IMPORT_PLUGIN(qjpeg)
#endif

int main ( int argc, char** argv )
{
	if (argc == 2 && qstrcmp(argv[1], "--version") == 0) {
		printf("Charm version " CHARM_VERSION "\n");
		return 0;
	}
	Application app ( argc,  argv );
	return app.exec();
}
