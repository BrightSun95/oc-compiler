//Luke Tanner  latanner
//Kevin Crum   kbcrum
// $Id: oc.cpp,v 1.8 2017-09-21 15:51:23-07 - - $

// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#include <string>
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

const string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;
int yy_flex_debug = 0; // temporary var until implement yylex()
int yydebug = 0; // temporary var until implement yyparse()


// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

/*
// Print the meaning of a signal.
static void eprint_signal (const char* kind, int signal) {
   fprintf (stderr, ", %s %d", kind, signal);
   const char* sigstr = strsignal (signal);
   if (sigstr != nullptr) fprintf (stderr, " %s", sigstr);
}
*/

/*
// Print the status returned from a subprocess.
void eprint_status (const char* command, int status) {
   if (status == 0) return;
   fprintf (stderr, "%s: status 0x%04X", command, status);
   if (WIFEXITED (status)) {
      fprintf (stderr, ", exit %d", WEXITSTATUS (status));
   }
   if (WIFSIGNALED (status)) {
      eprint_signal ("Terminated", WTERMSIG (status));
      #ifdef WCOREDUMP
      if (WCOREDUMP (status)) fprintf (stderr, ", core dumped");
      #endif
   }
   if (WIFSTOPPED (status)) {
      eprint_signal ("Stopped", WSTOPSIG (status));
   }
   if (WIFCONTINUED (status)) {
      fprintf (stderr, ", Continued");
   }
   fprintf (stderr, "\n");
}
*/

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);

   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == nullptr) break;
      chomp (buffer, '\n');
      //printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         // fprintf line
         continue;
      }
      char* savepos = nullptr;
      char* bufptr = buffer;
      
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = nullptr;
         if (token == nullptr) break;
         //printf ("token %d.%d: [%s]\n",
         //        linenr, tokenct, token);
         string_set::intern(token);
      }
      ++linenr;
   }
}

int main (int argc, char** argv) {
   
   // Loop through argv, get opt recognizes options after -str
   // Loop will continue, grabbing options until end of file
   // An incompatible option following a '-' 
   // will throw an error to sderr
   // if a single ':' follows option, then an argument is expected 
   // to follow said option
   string D_opt = "";
   for(;;) {
      int opt = getopt (argc, argv, "@:D:ly");
      if (opt == EOF) break;
      switch (opt) {
         case '@': set_debugflags (optarg);                 break;
         case 'D': D_opt = (optarg);                        break; 
         case 'l':/*: yy_flex_debug = 1;*/                  break;  
         case 'y':/*: yydebug = 1;*/                        break;
         default:  errprintf ("bad option (%c)\n", optopt); break;
      }
   }

   exec::execname = basename (argv[0]);
   int exit_status = EXIT_SUCCESS;
   //for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[optind];
      string s_filename = filename;
      // check for .oc suffix
      if(s_filename.length()<=3){
         cerr<<"USAGE: filename must end in .oc"<<endl;
         exit_status = EXIT_FAILURE;
         return exit_status;
      }
      if(s_filename.substr(s_filename.length()-3) != ".oc"){
         cerr<<"USAGE: filename must end in .oc"<<endl;
         exit_status = EXIT_FAILURE;
         return exit_status;
      }
      string command = CPP + " " + D_opt + " " + filename;
      printf ("command=\"%s\"\n", command.c_str());
      FILE* pipe = popen (command.c_str(), "r");
      if (pipe == nullptr) {
         exit_status = EXIT_FAILURE;
         fprintf (stderr, "%s: %s: %s\n",
            exec::execname.c_str(), command.c_str(), strerror (errno));
      }else {
         cpplines (pipe, filename);
         int pclose_rc = pclose (pipe);
         // eprint_status (command.c_str(), pclose_rc);
         if (pclose_rc != 0) exit_status = EXIT_FAILURE;
      }
   //}
   FILE* out_file;
   string out_name = basename (argv[1]);
   out_name = out_name.substr(0, out_name.length()-3)+".str";
   out_file = fopen(out_name.c_str(), "w");
   string_set::dump(out_file);
   fclose(out_file);
   return exit_status;
}

