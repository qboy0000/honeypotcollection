 /*
  * Compile with:
  * flex -o config_syntax.c config_syntax.l
  */

%{
#include <stdio.h>
#include <pcap.h>
#include <dumbnet.h>
#include <glib.h>
#include "config_rules.h"

#define YY_NO_INPUT
%}

%option nounput

%%

	/* Honeybrid configuration keywords */
config		{ return CONFIGURATION; }
target		{ return TARGET; }
module		{ return MODULE; }
filter		{ return FILTER; }
frontend 	{ return FRONTEND; }
backend  	{ return BACKEND; }
limit 	 	{ return LIMIT; }
backpick	{ return BACKPICK; }
uplink	    { return UPLINK; }

	/* Delimiters */
"{"		{ return OPEN; }
"}"		{ return END; }
";"		{ return SEMICOLON; }
"\""		{ return QUOTE; }
":"|"="         { yylval.string = strdup(yytext); return EQ; }

	/* Content Variables */
[0-9]+				{ yylval.number = atoi(yytext); return NUMBER; }
[a-zA-Z0-9_-]+			{ yylval.string = strdup(yytext); return WORD; }
[a-zA-Z0-9()<>|&^!\/._-]+	{ yylval.string = strdup(yytext); return EXPR; }

	/* Ignored */
"//"[^\n]* 	/* ignored */	
"#"[^\n]*  	/* ignored */
\n	   	{ yylineno++; }  
[ \t]+	   	/* ignore white space */ 

%%
