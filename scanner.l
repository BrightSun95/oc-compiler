%{

#include "lyutils.h"
#include "string_set.h"

#define YY_USER_ACTION  { lexer::advance(); }

int yylval_token (int symbol) {
   yylval = new astree (symbol, lexer::lloc, yytext);
   string_set::intern(yytext);
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
TOK_CHAR_CON        ('([^\\'\n]|\\[\\'"0nt])') 
TOK_STRCON          (\"([^\\"\n]|\\[\\'"0nt])*\")
TOK_NUMBER          ({TOK_DIGIT}+)
TOK_NOTNUMBER       ({TOK_MANTISSA}[Ee][+-]?)
TOK_INT             ({TOK_DIGIT}+)
TOK_IDENT           ({TOK_LETTER}({TOK_LETTER}|{TOK_DIGIT})*)


%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

"if"            { return yylval_token (TOK_IF);     }
"else"          { return yylval_token (TOK_ELSE);   }
"while"         { return yylval_token (TOK_WHILE);  }
"return"        { return yylval_token (TOK_RETURN); }
"string"        { return yylval_token (TOK_STRING); }
"struct"        { return yylval_token (TOK_STRUCT); }
"void"          { return yylval_token (TOK_VOID);   }
"new"           { return yylval_token (TOK_NEW);    }
"null"          { return yylval_token (TOK_NULL);   }

"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"%"             { return yylval_token ('%'); }

"!="            { return yylval_token (TOK_NE);     }
"=="            { return yylval_token (TOK_EQ);     }
"!"             { return yylval_token ('!');        }
"<="            { return yylval_token (TOK_LE);     }
">="            { return yylval_token (TOK_GE);     }
"<"             { return yylval_token (TOK_LT);     }
">"             { return yylval_token (TOK_GT);     }
"="             { return yylval_token ('=');        }

"[]"            { return yylval_token (TOK_ARRAY);}
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
","             { return yylval_token (','); }
"."             { return yylval_token ('.'); }
";"             { return yylval_token (';'); }


{TOK_NUMBER}    { return yylval_token (TOK_INTCON); }

{TOK_IDENT}     { return yylval_token (TOK_IDENT);  }


{TOK_NOTNUMBER}     { lexer::badtoken (yytext);
                        return yylval_token (TOK_INTCON); }
{TOK_CHAR_CON}      { return yylval_token (TOK_CHARCON); }
{TOK_STRCON}        { return yylval_token (TOK_STRINGCON);}

.                   { lexer::badchar (*yytext); }

%%

