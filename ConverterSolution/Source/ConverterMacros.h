
#pragma once

#ifdef _DEBUG
	#define DEBUG_COMMAND(command) command
#else
	#define DEBUG_COMMAND(command)
#endif

#define PARENT_DIRECTORY_NAME "VIDEO_TS"
#define VOB_EXTENSION ".VOB"
#define MKV_EXTENSION ".MKV"
#define PACKAGE_NAME "VTS_PACKAGE.VOB"

#define ONE_GB 1048576000