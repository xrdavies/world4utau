#ifndef _H_LOG_
#define _H_LOG_

enum log_levels {
	ERROR,
    WARNING,
	INFO,
	DEBUG
};

extern int log(int level, const char *format, ...);

#define log_err(...)   log(ERROR   , __VA_ARGS__)
#define log_warn(...)  log(WARNING , __VA_ARGS__)
#define log_info(...)  log(INFO    , __VA_ARGS__)
#define log_debug(...) log(DEBUG   , __VA_ARGS__)

#endif