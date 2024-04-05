/*
 * C Fancy Logger
 * colour.h:
 * Copyright (c) 2009-2021 986-Studio. All rights reserved.
 *
 * Created by Manoël Trapier on 20/01/2009.
 */

#ifndef LOG_COLOUR_H
#define	LOG_COLOUR_H

#define ALLOW_COLORS

#ifdef ALLOW_COLORS
#define __C(c) "\x1B[" c "m"
#else
#define __C(c) ""
#endif

#define ANSI_COLOR __C
#define FBLACK ANSI_COLOR("30")
#define FRED ANSI_COLOR("31")
#define FGREEN ANSI_COLOR("32")
#define FYELLOW ANSI_COLOR("33")
#define FBLUE ANSI_COLOR("34")
#define FMAGENTA ANSI_COLOR("35")
#define FCYAN ANSI_COLOR("36")
#define FWHITE ANSI_COLOR("37")

#define BBLACK ANSI_COLOR("40")
#define BRED ANSI_COLOR("41")
#define BGREEN ANSI_COLOR("42")
#define BYELLOW ANSI_COLOR("43")
#define BBLUE ANSI_COLOR("44")
#define BMAGENTA ANSI_COLOR("45")
#define BCYAN ANSI_COLOR("46")
#define BWHITE ANSI_COLOR("47")

#define CNORMAL ANSI_COLOR("0")

#endif	/* LOG_COLOUR_H */

