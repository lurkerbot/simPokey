#ifndef __MAIN_H
#define __MAIN_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <zlog.h>
#include <uv.h>
#include <signal.h>
#include "PoKeysLib.h"
#include "ProSimDataSource.h"
#include "device/pokey/pokey.h"
#include "config/config.h"
#include "pin/pin.h"
#include "cli/cli.h"

const char *configFile = "../config/config.cfg";
const char *logConfigFile = "../config/zlog.conf";
zlog_category_t *logHandler;

#endif