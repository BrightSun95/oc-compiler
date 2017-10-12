// $Id: auxlib.h,v 1.5 2017-10-11 14:33:45-07 - - $

#ifndef __AUXLIB_H__
#define __AUXLIB_H__

#include <string>
using namespace std;

#include <stdarg.h>

//
// DESCRIPTION
//    Auxiliary library containing miscellaneous useful things.
//

//
// Error message and exit status utility.
//

struct exec {
   static string execname;
   static int exit_status;
};


//
// Debugging utility.
//

void set_debugflags (const char* flags);
// Sets a string of debug flags to be used by DEBUGF statements.
// Uses the address of the string, and does not copy it, so
// it must not be dangling.  If a particular debug flag has
// been set, messages are printed.  The format is identical to
// printf format.  The flag "@" turns on all flags.

bool is_debugflag (char flag);
// Checks to see if a debugflag is set.

#ifdef NDEBUG
// Do not generate any code.
#define DEBUGF(FLAG,...)   /**/
#define DEBUGSTMT(FLAG,STMTS) /**/
#else
// Generate debugging code.
void __debugprintf (char flag, const char* file, int line,
                    const char* func, const char* format, ...);
#define DEBUGF(FLAG,...) \
        __debugprintf (FLAG, __FILE__, __LINE__, __PRETTY_FUNCTION__, \
                       __VA_ARGS__)
#define DEBUGSTMT(FLAG,STMTS) \
        if (is_debugflag (FLAG)) { DEBUGF (FLAG, "\n"); STMTS }
#endif

#endif

