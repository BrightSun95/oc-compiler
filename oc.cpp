//Luke Tanner  latanner
//Kevin Crum   kbcrum
// $Id: oc.cpp,v 1.8 2017-09-21 15:51:23-07 - - $

// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#include <string>
#include <typeinfo>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <assert.h>

#include "string_set.h"
#include "getopt.h"
#include "auxlib.h"
#include "lyutils.h"

const string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;
string cpp_command;
int exit_status;

void scan(char* name) {
   FILE* out_str;
   string out_name = basename (name);
   out_name = out_name.substr(0, out_name.length()-3);

   string out_name_str = out_name+".str";
   string out_name_tok = out_name+".tok";

   out_str = fopen(out_name_str.c_str(), "w");
   out_tok = fopen(out_name_tok.c_str(),"w"); 

   if (out_tok == NULL){
      fprintf (stderr, "Error: failed to open .tok file");
   }else{
      for(;;){
         int tok = yylex();
         if (tok == YYEOF) break;
         DEBUGF('m', "token=%d", yytext);
      }
   }
   if (out_str == NULL){
      fprintf (stderr, "Error: failed to open .str file");
   }else{
      string_set::dump(out_str);
   }
   fclose (out_str);
   fclose (out_tok);
}

// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assigns opened pipe to FILE* yyin.
void cpp_popen (char* filename) {
   cpp_command = CPP + " " + filename;
   yyin = popen (cpp_command.c_str(), "r");
   if (yyin == nullptr) {
      syserrprintf (cpp_command.c_str());
      exit (exec::exit_status);
   }else {
      if (yy_flex_debug) {
         fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                  cpp_command.c_str(), fileno (yyin));
      }
      scan(filename);
   }
}

void cpp_pclose() {
   int pclose_rc = pclose (yyin);
   eprint_status (cpp_command.c_str(), pclose_rc);
   if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;
}

void scan_opts (int argc, char** argv) {
   opterr = 0;
   yy_flex_debug = 0;
   yydebug = 0;
   string D_opt = "";
   for(;;) {
      int opt = getopt (argc, argv, "@:D:ly");
      if (opt == EOF) break;
      switch (opt) {
         case '@': set_debugflags (optarg);                 break;
         case 'D': D_opt = (optarg);                        break; 
         case 'l': yy_flex_debug = 1;                       break;  
         case 'y': yydebug = 1;                             break;
         default:  errprintf ("bad option (%c)\n", optopt); break;
      }
   }
   exit_status = EXIT_SUCCESS;
   char* filename = argv[optind];
   string s_filename = filename;
   // check for .oc suffix
   if(s_filename.length()<=3 || 
      s_filename.substr(s_filename.length()-3) != ".oc"){
      cerr<<"USAGE: filename must end in .oc"<<endl;
      exit_status = EXIT_FAILURE;
      exit (exec::exit_status);
   }

   if (optind > argc) {
      errprintf ("Usage: %s [-ly] [filename]\n",
                 exec::execname.c_str());
      exit (exec::exit_status);
   }
   cpp_popen (filename);
}


int main (int argc, char** argv) {
   exec::execname = basename (argv[0]);
   scan_opts(argc, argv);
   return exit_status;
}

