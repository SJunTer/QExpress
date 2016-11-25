#ifndef OPERATEDEFINE_H
#define OPERATEDEFINE_H

#include "operatesql.h"

#define VALUE_NULL ("NULL")

#define TABLE_CARGO ("Cargo")
#define CARGO_COL_NUM (5)
#define CARGO_ID ("id")
#define CARGO_DESCRIPTION ("description")
#define CARGO_DESTINATION ("destination")
#define CARGO_RECIVER ("reciver")
#define CARGO_STATUS ("status")

#define TABLE_TRUCK ("Truck")
#define TRUCK_COL_NUM (4)
#define TRUCK_ID ("id")
#define TRUCK_BRAND ("brand")
#define TRUCK_LPN ("lpn")
#define TRUCK_FLAG ("status")

#define TABLE_ACCOUNT ("Account")
#define ACCOUNT_COL_NUM (9) // search alter包括salt
#define ACCOUNT_ID ("id")
#define ACCOUNT_USERNAME ("username")
#define ACCOUNT_PASSWORD ("password")
#define ACCOUNT_REAL_NAME ("real_name")
#define ACCOUNT_PHONE ("phone")
#define ACCOUNT_EMAIL ("email")
#define ACCOUNT_STATUS ("status")
#define ACCOUNT_LAST_LOGIN_TIME ("last_login_time")

#define TABLE_PATH ("Path")
#define PATH_COL_NUM (8)
#define PATH_ID ("id")
#define PATH_TRUCK ("truck")
#define PATH_DRIVER ("driver")
#define PATH_CARGO ("cargo")
#define PATH_STATUS ("status")
#define PATH_POINTS ("points")
#define PATH_START_TIME ("start_time")
#define PATH_MOVE_TIME ("move_time")

#endif // OPERATEDEFINE_H
