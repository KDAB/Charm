#ifndef CHARM_CMAKE_H
#define CHARM_CMAKE_H

#define CHARM_VERSION "@Charm_VERSION@"

#include <QString>
/* Define to the version from CMake */
static inline QString CharmVersion() {
    static const auto version = QStringLiteral("@Charm_VERSION@");
    return version;
}
/* Define if you have enabled the idle detection */
#cmakedefine CHARM_IDLE_DETECTION
/* Defined if idle detection is available on X11 or XCB*/
#cmakedefine CHARM_IDLE_DETECTION_AVAILABLE
/* Delay for idle detection, default is 360 */
Q_CONSTEXPR static int CharmIdleTime = @CHARM_IDLE_TIME@;
/* Define the url where to check for updates */
static inline QString CharmUpdateCheckUrl(){
    static const auto url = QStringLiteral("@UPDATE_CHECK_URL@");
    return url;
}
/* Defined if command interface is enabled */
#cmakedefine CHARM_CI_SUPPORT
#ifdef CHARM_CI_SUPPORT
/* Defined if TCP command interface is enabled */
#cmakedefine CHARM_CI_TCPSERVER
/* Defined if local socket command interface is enabled */
#cmakedefine CHARM_CI_LOCALSERVER
#endif

#endif // CHARM_CMAKE_H
