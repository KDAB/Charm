#ifndef HTCONFIG_H
#define HTCONFIG_H 1

#define HT_BASE_URL "https://%1/KdabHome/apps/"
#define HT_PORTAL_URL HT_BASE_URL "portal/"
#define HT_LOGIN_URL HT_PORTAL_URL "j_security_check"
#define HT_TIMESHEETS_URL HT_BASE_URL "timesheets/"
#define HT_TIMESHEETUPLOAD_URL HT_TIMESHEETS_URL "upload/"
#define HT_PROJECTCODES_URL HT_TIMESHEETS_URL "kdab-projectcodes.xml"

#endif
