/* $Id: lexer.l,v 1.7 2016-10-27 19:45:59-07 - - $ */

%{

#include "lyutils.h"

#define YY_USER_ACTION  { lexer::advance(); }

int yylval_token (int symbol) {
   yylval = new astree (symbol, lexer::lloc, yytext);
   return symbol;
}

%}

%option 8bit
%option debug
%option nodefault
%option nounput
%option noyywrap
%option verbose
%option warn

TOK_LETTER          [A-Za-z_]
TOK_DIGIT           [0-9]
TOK_MANTISSA        ({TOK_DIGIT}+\.?{TOK_DIGIT}*|\.{TOK_DIGIT}+)
TOK_NUMBER          ([1-9]+[^.]{TOK_DIGIT}*)
TOK_NOTNUMBER       ({TOK_MANTISSA}[Ee][+-]?)
TOK_IDENT           ({TOK_LETTER}({TOK_LETTER}|{TOK_DIGIT})*)

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

{TOK_NUMBER}    { return yylval_token (TOK_NUMBER); }
{TOK_IDENT}     { return yylval_token (TOK_IDENT); }
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"^"             { return yylval_token ('^'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }

{TOK_NOTNUMBER}     { lexer::badtoken (yytext);
                  		return yylval_token (TOK_NUMBER); }
.               	{ lexer::badchar (*yytext); }

%%

