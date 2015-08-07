/* Define to the version from CMake */
#define CHARM_VERSION "@Charm_VERSION@"
/* Define if you have enabled the idle detection */
#cmakedefine CHARM_IDLE_DETECTION
/* Defined if idle detection is available on X11 */
#cmakedefine CHARM_IDLE_DETECTION_AVAILABLE_X11 1
/* Delay for idle detection, default is 360 */
#define CHARM_IDLE_TIME @CHARM_IDLE_TIME@
/* Define the url where to check for updates */
#define UPDATE_CHECK_URL "@UPDATE_CHECK_URL@"
