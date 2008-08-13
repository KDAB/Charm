#include <QMessageBox>
#include <QtPlugin>

#include "Application.h"

#if defined CHARM_BUILD_STATIC_QT
Q_IMPORT_PLUGIN(qjpeg)
#endif

// FIXME temp
#include "TimeTrackingView.h"

int main ( int argc, char** argv )
{
	Application app ( argc,  argv );
        TimeTrackingView widget;
        widget.show();
	return app.exec();
}
