// $Id: auxlib.cpp,v 1.3 2017-10-11 14:28:14-07 - - $

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "auxlib.h"

string exec::execname;
int exec::exit_status = EXIT_SUCCESS;

const char* debugflags = "";
bool alldebugflags = false;


void auxlib::set_debugflags (const char* flags) {
   debugflags = flags;
   assert (debugflags != nullptr);
   if (strchr (debugflags, '@') != nullptr) alldebugflags = true;
   DEBUGF ('x', "Debugflags = \"%s\", all = %d\n",
           debugflags, alldebugflags);
}
