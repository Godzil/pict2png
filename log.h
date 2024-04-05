/*
 * C Fancy Logger
 * log.h:
 * Copyright (c) 2009-2021 986-Studio. All rights reserved.
 *
 * Created by Manoël Trapier on 20/01/2009.
 */

#ifndef _LOG_H
#define	_LOG_H

#ifdef	__cplusplus
extern "C" {
#endif

enum
{
    TLOG_ALWAYS = -1,
    TLOG_PANIC = 0,
    TLOG_ERROR,
    TLOG_WARNING,
    TLOG_NORMAL,
    TLOG_VERBOSE,
    TLOG_DEBUG,
};

//#define LOG_ALWAYS_FFLUSH
#define DYNA_LOG_LEVEL
#define SET_DEBUG_LOG

/* Set if DYNALOG is set the maximum compiled log level */
#ifndef MAXIMUM_DEBUG_LEVEL

#ifndef SET_DEBUG_LOG
#define MAXIMUM_DEBUG_LEVEL TLOG_NORMAL
#else
#define MAXIMUM_DEBUG_LEVEL TLOG_DEBUG
#endif

#endif /* MAXIMUM_DEBUG_LEVEL */

#ifndef DEFAULT_DEBUG_LEVEL

/* Set the default log level */
#ifndef SET_DEBUG_LOG
#define DEFAULT_DEBUG_LEVEL TLOG_PANIC
#else
#define DEFAULT_DEBUG_LEVEL TLOG_NORMAL
#endif

#endif

/******************************************************************************/
/*                        DO NOT MESS AFTER THIS LINE                         */
/******************************************************************************/

#ifdef DYNA_LOG_LEVEL
# ifdef MAX_DEBUG_LEVEL
#  undef MAX_DEBUG_LEVEL
# endif
# ifdef __LOG_C_INTERNAL_
int MAX_DEBUG_LEVEL = DEFAULT_DEBUG_LEVEL;
#else
extern int MAX_DEBUG_LEVEL;
#endif
#else
# ifndef MAX_DEBUG_LEVEL
#  define MAX_DEBUG_LEVEL DEFAULT_DEBUG_LEVEL
# endif
#endif

#define Log(_level, _user, _fmt, ...)\
 if (_level <= MAXIMUM_DEBUG_LEVEL)\
  if ((_level <= MAX_DEBUG_LEVEL) || (_level <= TLOG_PANIC))\
   do { log_real(_level, _user, _fmt, ##__VA_ARGS__); } while(0)

void log_real(int level, const char *user, const char *fmt, ...);

#define LOG(_level, _str, ...) if ((_level <= MAX_DEBUG_LEVEL) || (_level <= TLOG_PANIC)) do { fputs(_str, stderr); } while(0)
#define LOGCODE(_level, _user, _code)\
   if ((_level) <= MAXIMUM_DEBUG_LEVEL) do{\
    Log((_level), (_user), "");\
    if (((_level) <= MAX_DEBUG_LEVEL) || ((_level) <= TLOG_PANIC))\
       do { _code; fprintf(stderr, "\n"); } while(0); } while(0)

#define INFOL(_level, _fmt) LOGCODE(_level, "INFOL", { printf _fmt; })

#define FUNC_IN() Log(TLOG_VERBOSE, NULL, ">>%s", __func__)
#define FUNC_OUT() Log(TLOG_VERBOSE, NULL, "<<%s", __func__)
#define FUNC_OUTR(out) Log(TLOG_VERBOSE, NULL, "<<%s (%d)", __func__, out)

#ifdef	__cplusplus
}
#endif

#endif	/* _LOG_H */

