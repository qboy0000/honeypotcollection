/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "config_rules.y"

#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <pcap.h>
#include <dumbnet.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "globals.h"
#include "structs.h"
#include "convenience.h"
#include "decision_engine.h"
#include "modules.h"
#include <sys/stat.h>
#include <fcntl.h>

/*! Type of capture link */
#define LINKTYPE 1 	//LINKTYPE_ETHERNET=1 \todo dynamically assign link type from nfqueue

enum { SOURCE = 1, DESTINATION, SOURCE_AND_DESTINATION, SOURCE_OR_DESTINATION };
extern int  yylineno;
extern char *yytext;
static void yyerror(const char *msg);

int yylex(void);

int yywrap() {
	/*! should return 0 if additional input has to be parsed, 1 if the end has been reached */
	return 1;
}

char* str_append(char * root, char * str);
char* int_append(char * root, int num);



/* Line 268 of yacc.c  */
#line 112 "config_rules.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OPEN = 258,
     END = 259,
     SEMICOLON = 260,
     QUOTE = 261,
     DOT = 262,
     MODULE = 263,
     FILTER = 264,
     FRONTEND = 265,
     BACKEND = 266,
     BACKPICK = 267,
     LIMIT = 268,
     CONFIGURATION = 269,
     TARGET = 270,
     UPLINK = 271,
     NUMBER = 272,
     WORD = 273,
     EQ = 274,
     EXPR = 275
   };
#endif
/* Tokens.  */
#define OPEN 258
#define END 259
#define SEMICOLON 260
#define QUOTE 261
#define DOT 262
#define MODULE 263
#define FILTER 264
#define FRONTEND 265
#define BACKEND 266
#define BACKPICK 267
#define LIMIT 268
#define CONFIGURATION 269
#define TARGET 270
#define UPLINK 271
#define NUMBER 272
#define WORD 273
#define EQ 274
#define EXPR 275




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 59 "config_rules.y"

	int    number;
	char * string;
	GString * gstring;
	struct GHashTable * hash;
	struct interface * uplink;
	struct target * target;
	struct addr * addr;



/* Line 293 of yacc.c  */
#line 200 "config_rules.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 212 "config_rules.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   122

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  21
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  13
/* YYNRULES -- Number of rules.  */
#define YYNRULES  40
/* YYNRULES -- Number of states.  */
#define YYNSTATES  104

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   275

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    21,    22,
      26,    30,    34,    38,    44,    52,    53,    61,    69,    75,
      83,    84,    90,    96,   102,   107,   108,   115,   120,   128,
     135,   140,   148,   159,   173,   180,   182,   183,   186,   189,
     192
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      22,     0,    -1,    -1,    22,    23,    -1,    22,    26,    -1,
      22,    28,    -1,    22,    30,    -1,    14,     3,    24,     4,
      -1,    -1,    24,    25,     5,    -1,    18,    19,    18,    -1,
      18,    19,    20,    -1,    18,    19,    17,    -1,    18,    19,
       6,    32,     6,    -1,    16,     6,    18,     6,     3,    27,
       4,    -1,    -1,    27,    18,    19,     6,    32,     6,     5,
      -1,    27,    18,    19,     6,    18,     6,     5,    -1,    27,
      18,    19,    17,     5,    -1,     8,     6,    18,     6,     3,
      29,     4,    -1,    -1,    29,    18,    19,    18,     5,    -1,
      29,    18,    19,    20,     5,    -1,    29,    18,    19,    17,
       5,    -1,    15,     3,    31,     4,    -1,    -1,    31,     9,
       6,    33,     6,     5,    -1,    31,    10,    32,     5,    -1,
      31,    10,    32,     6,    33,     6,     5,    -1,    31,    12,
       6,    33,     6,     5,    -1,    31,    11,    32,     5,    -1,
      31,    11,    32,     6,    33,     6,     5,    -1,    31,    11,
       6,    33,     6,    32,     6,    33,     6,     5,    -1,    31,
      11,     6,    33,     6,    32,     6,    33,     6,     6,    33,
       6,     5,    -1,    31,    13,     6,    33,     6,     5,    -1,
      20,    -1,    -1,    33,    18,    -1,    33,    17,    -1,    33,
      20,    -1,    33,    19,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    87,    87,    88,    89,    90,    91,    97,   100,   101,
     104,   109,   114,   121,   133,   146,   150,   162,   172,   188,
     229,   233,   238,   243,   256,   276,   287,   299,   305,   312,
     318,   341,   365,   391,   418,   423,   432,   435,   440,   444,
     449
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OPEN", "END", "SEMICOLON", "QUOTE",
  "DOT", "MODULE", "FILTER", "FRONTEND", "BACKEND", "BACKPICK", "LIMIT",
  "CONFIGURATION", "TARGET", "UPLINK", "NUMBER", "WORD", "EQ", "EXPR",
  "$accept", "configuration", "config", "parameters", "parameter",
  "uplink", "uplink_settings", "module", "module_settings", "target",
  "rule", "honeynet", "equation", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    21,    22,    22,    22,    22,    22,    23,    24,    24,
      25,    25,    25,    25,    26,    27,    27,    27,    27,    28,
      29,    29,    29,    29,    30,    31,    31,    31,    31,    31,
      31,    31,    31,    31,    31,    32,    33,    33,    33,    33,
      33
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     4,     0,     3,
       3,     3,     3,     5,     7,     0,     7,     7,     5,     7,
       0,     5,     5,     5,     4,     0,     6,     4,     7,     6,
       4,     7,    10,    13,     6,     1,     0,     2,     2,     2,
       2
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     3,     4,     5,
       6,     0,     8,    25,     0,     0,     0,     0,     0,     0,
       7,     0,     0,    24,     0,     0,     0,     0,     0,     0,
      20,     0,     9,    36,    35,     0,    36,     0,    36,    36,
      15,     0,     0,    12,    10,    11,     0,    27,    36,     0,
      30,    36,     0,     0,     0,    19,     0,     0,     0,    38,
      37,    40,    39,     0,     0,     0,     0,     0,    14,     0,
       0,    13,    26,     0,     0,     0,    29,    34,     0,     0,
       0,     0,    28,    36,    31,     0,     0,    23,    21,    22,
       0,     0,     0,    18,     0,     0,     0,    32,    36,    17,
      16,     0,     0,    33
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     7,    16,    22,     8,    54,     9,    41,    10,
      17,    35,    46
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -37
static const yytype_int8 yypact[] =
{
     -37,     5,   -37,    10,    37,    68,    32,   -37,   -37,   -37,
     -37,    -7,   -37,   -37,    24,    81,     6,    82,    83,    85,
     -37,    71,    92,   -37,    93,    78,     3,    94,    95,    99,
     -37,    63,   -37,   -37,   -37,     2,   -37,    58,   -37,   -37,
     -37,    64,    78,   -37,   -37,   -37,     8,   -37,   -37,    12,
     -37,   -37,    16,    31,    66,   -37,    84,    98,   100,   -37,
     -37,   -37,   -37,    35,    78,    39,   101,   102,   -37,    89,
      26,   -37,   -37,   104,   105,   107,   -37,   -37,    79,   108,
     109,   110,   -37,   -37,   -37,   -14,   111,   -37,   -37,   -37,
      55,   112,   113,   -37,    61,   115,   116,   -37,   -37,   -37,
     -37,    59,   117,   -37
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -25,   -36
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      49,    37,    52,    53,    91,     2,    34,    47,    48,    36,
      20,    15,    63,     3,    58,    65,    11,    57,    64,     4,
       5,     6,    66,    34,    21,    59,    60,    61,    62,    59,
      60,    61,    62,    59,    60,    61,    62,    67,    14,    74,
      12,    73,    18,    79,    80,    75,    81,    90,    59,    60,
      61,    62,    59,    60,    61,    62,    59,    60,    61,    62,
      92,    94,   101,    50,    51,   102,    97,    98,    55,    42,
      68,    13,    59,    60,    61,    62,    59,    60,    61,    62,
      43,    44,    56,    45,    69,    85,    23,    19,    30,    29,
      31,    24,    25,    26,    27,    28,    86,    32,    34,    33,
      38,    39,    40,    70,    71,    72,    76,    77,    78,    82,
       0,    83,    84,    87,    88,    89,    93,     0,    95,    96,
      99,   100,   103
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-37))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
      36,    26,    38,    39,    18,     0,    20,     5,     6,     6,
       4,    18,    48,     8,     6,    51,     6,    42,     6,    14,
      15,    16,     6,    20,    18,    17,    18,    19,    20,    17,
      18,    19,    20,    17,    18,    19,    20,     6,     6,    64,
       3,     6,    18,    17,    18,     6,    20,    83,    17,    18,
      19,    20,    17,    18,    19,    20,    17,    18,    19,    20,
      85,     6,    98,     5,     6,     6,     5,     6,     4,     6,
       4,     3,    17,    18,    19,    20,    17,    18,    19,    20,
      17,    18,    18,    20,    18,     6,     4,     6,     3,     6,
      19,     9,    10,    11,    12,    13,    17,     5,    20,     6,
       6,     6,     3,    19,     6,     5,     5,     5,    19,     5,
      -1,     6,     5,     5,     5,     5,     5,    -1,     6,     6,
       5,     5,     5
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    22,     0,     8,    14,    15,    16,    23,    26,    28,
      30,     6,     3,     3,     6,    18,    24,    31,    18,     6,
       4,    18,    25,     4,     9,    10,    11,    12,    13,     6,
       3,    19,     5,     6,    20,    32,     6,    32,     6,     6,
       3,    29,     6,    17,    18,    20,    33,     5,     6,    33,
       5,     6,    33,    33,    27,     4,    18,    32,     6,    17,
      18,    19,    20,    33,     6,    33,     6,     6,     4,    18,
      19,     6,     5,     6,    32,     6,     5,     5,    19,    17,
      18,    20,     5,     6,     5,     6,    17,     5,     5,     5,
      33,    18,    32,     5,     6,     6,     6,     5,     6,     5,
       5,    33,     6,     5
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:

/* Line 1806 of yacc.c  */
#line 88 "config_rules.y"
    { 	g_printerr("Main config parsed\n"); }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 89 "config_rules.y"
    { 	g_printerr("Uplink parsed\n"); }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 90 "config_rules.y"
    {	g_printerr("Module parsed\n"); }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 91 "config_rules.y"
    {	g_printerr("Target parsed\n"); }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 97 "config_rules.y"
    { /* nothing to do */ }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 100 "config_rules.y"
    { /* nothing to do */ }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 101 "config_rules.y"
    { /* nothing to do */  }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 104 "config_rules.y"
    {
		g_hash_table_insert(config, (yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
		g_printerr("\t'%s' => '%s'\n", (yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
		g_free((yyvsp[(2) - (3)].string));
	}
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 109 "config_rules.y"
    {
		g_hash_table_insert(config, (yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
        g_printerr("\t'%s' => '%s'\n", (yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
        g_free((yyvsp[(2) - (3)].string));
	}
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 114 "config_rules.y"
    {
		int *d =g_malloc(sizeof(int));
		*d = (yyvsp[(3) - (3)].number);
		g_hash_table_insert(config, (yyvsp[(1) - (3)].string), d);
		g_printerr("\t'%s' => %i\n", (yyvsp[(1) - (3)].string), *d);
		g_free((yyvsp[(2) - (3)].string));
    }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 121 "config_rules.y"
    {
		char *s = g_malloc0(snprintf(NULL, 0, "%s", addr_ntoa((yyvsp[(4) - (5)].addr))) + 1);
		sprintf(s, "%s", addr_ntoa((yyvsp[(4) - (5)].addr)));
        g_hash_table_insert(config, (yyvsp[(1) - (5)].string), s);
        g_printerr("\tDefining IP: '%s' => '%s'\n", (yyvsp[(1) - (5)].string), s);
		free((yyvsp[(4) - (5)].addr));
		g_free((yyvsp[(2) - (5)].string));
    }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 133 "config_rules.y"
    { 

        struct interface *iface=(struct interface *)(yyvsp[(6) - (7)].uplink);
        if(iface && iface->name && iface->ip_str && iface->mark) {
            iface->tag=(yyvsp[(3) - (7)].string);
            g_hash_table_insert(uplink, &iface->mark, iface);
            g_printerr("\t'tag' => '%s'\n", iface->tag);
        } else {
            errx(1, "Uplink configuration is incomplete!\n");
        }
    }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 146 "config_rules.y"
    {
        if (NULL == ((yyval.uplink) = g_malloc0(sizeof(struct interface))))
            errx(1, "%s: Fatal error while creating uplink table.\n", __func__);
    }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 150 "config_rules.y"
    {
        if(strcmp((yyvsp[(2) - (7)].string), "ip")) {
            errx(1, "Unrecognized option: %s. Did you mean: 'ip'?\n", (yyvsp[(2) - (7)].string)); 
        }
        struct interface *iface=(struct interface *)(yyval.uplink);
        iface->ip = (yyvsp[(5) - (7)].addr);
        iface->ip_str = g_malloc0(snprintf(NULL, 0, "%s", addr_ntoa((yyvsp[(5) - (7)].addr))) + 1);
        sprintf(iface->ip_str, "%s", addr_ntoa((yyvsp[(5) - (7)].addr)));
        g_printerr("\t'%s' => '%s'\n", (yyvsp[(2) - (7)].string), iface->ip_str);
        g_free((yyvsp[(3) - (7)].string));
        g_free((yyvsp[(2) - (7)].string));
    }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 162 "config_rules.y"
    {
        if(strcmp((yyvsp[(2) - (7)].string), "interface")) {
            errx(1, "Unrecognized option: %s. Did you mean: 'interface'?\n", (yyvsp[(2) - (7)].string)); 
        }
        struct interface *iface=(struct interface *)(yyval.uplink);
        iface->name = (yyvsp[(5) - (7)].string);
        g_printerr("\t'%s' => '%s'\n", (yyvsp[(2) - (7)].string), iface->name);
        g_free((yyvsp[(3) - (7)].string));
        g_free((yyvsp[(2) - (7)].string));
    }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 172 "config_rules.y"
    {
        if(strcmp((yyvsp[(2) - (5)].string), "mark")) {
            errx(1, "Unrecognized option: %s. Did you mean: 'mark'?\n", (yyvsp[(2) - (5)].string)); 
        }
        struct interface *iface=(struct interface *)(yyval.uplink);
        iface->mark=(yyvsp[(4) - (5)].number);
        g_printerr("\t'%s' => %i\n", (yyvsp[(2) - (5)].string), iface->mark);
        g_free((yyvsp[(3) - (5)].string));
        g_free((yyvsp[(2) - (5)].string));
    }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 188 "config_rules.y"
    {
		g_hash_table_insert(module, (yyvsp[(3) - (7)].string), (yyvsp[(6) - (7)].hash));
		g_printerr("\tmodule '%s' defined with %d parameters\n", (yyvsp[(3) - (7)].string), g_hash_table_size((GHashTable *)(yyvsp[(6) - (7)].hash)));
		if (NULL == g_hash_table_lookup((GHashTable *)(yyvsp[(6) - (7)].hash), "function")) {
			errx(1, "%s: Fatal error: missing parameter 'function' in module '%s'\n", __func__, (yyvsp[(3) - (7)].string));
		}
		
		gchar *backup_file = NULL;
		if (NULL != (backup_file = (gchar *)g_hash_table_lookup((GHashTable *)(yyvsp[(6) - (7)].hash), "backup"))) {
			int backup_fd;
			GError *error = NULL;
			GKeyFile *backup = NULL;
			backup = g_key_file_new();
			g_key_file_set_list_separator(backup, '\t');
			/*! We store a pointer to GKeyFile object in the module hash table */
			g_hash_table_insert((GHashTable *)(yyvsp[(6) - (7)].hash), "backup", backup);
			g_printerr("\t%s: New GKeyFile %p created\n", __func__, backup);
			/*! We then check if the file exists. Otherwise we create it */
			if (FALSE == g_file_test(backup_file, G_FILE_TEST_IS_REGULAR)) {
				if (-1 == (backup_fd = g_open(backup_file, O_WRONLY | O_CREAT | O_TRUNC, NULL))) {
					errx(1, "%s: Fatal error, can't create backup file for module", __func__);
				} else {
					//g_hash_table_insert((GHashTable *)$6, "backup_fd", &backup_fd);
					close(backup_fd);
				}
			} else {
				/*! If the file exists, we try to load it into memory */
				/*! \todo free all these structures, and close file descriptor when exiting */
				if (FALSE == g_key_file_load_from_file(
					g_hash_table_lookup((GHashTable *)(yyvsp[(6) - (7)].hash), "backup"),
					backup_file,
					G_KEY_FILE_KEEP_COMMENTS,
					&error)) {
					g_printerr("\t%s: can't load backup file for module: %s\n", __func__, error->message);
				}
			}
			//g_free(backup_file);
		}
	}
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 229 "config_rules.y"
    { 
		if (NULL == ((yyval.hash) = (struct GHashTable *)g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free)))		
	    	errx(1, "%s: Fatal error while creating module hash table.\n", __func__);
	}
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 233 "config_rules.y"
    {
	    g_hash_table_insert((GHashTable *)(yyval.hash), (yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].string));
	    g_printerr("\t'%s' => '%s'\n", (yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].string));
	    g_free((yyvsp[(3) - (5)].string));
	}
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 238 "config_rules.y"
    {
		g_hash_table_insert((GHashTable *)(yyval.hash), (yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].string));
        g_printerr("\t'%s' => '%s'\n", (yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].string));
        g_free((yyvsp[(3) - (5)].string));
	}
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 243 "config_rules.y"
    {
		int *d = g_malloc0(sizeof(int));
        *d = (yyvsp[(4) - (5)].number);
        g_hash_table_insert((GHashTable *)(yyval.hash), (yyvsp[(2) - (5)].string), d);
        g_printerr("\t'%s' => %i\n", (yyvsp[(2) - (5)].string), *d);
        g_free((yyvsp[(3) - (5)].string));
	}
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 256 "config_rules.y"
    {
		/*
		if (pcap_compile_nopcap(1500, LINKTYPE, $6->filter, $3, 1, 0) < 0) {
			g_printerr("PCAP ERROR: '%s'\n", $3);
                	yyerror("bad pcap filter");
		}
		*/
		g_printerr("\tGoing to add new element to target array...\n");
		g_ptr_array_add(targets, (yyvsp[(3) - (4)].target));
		g_printerr("\t...done\n");
		/*g_printerr("\tAdded a new target with the following values:\n\tfront_handler: %s\n\tfront_rule: %s\n\tback_handler: %s\n\tback_rule: %s\n\tcontrol: %s\n",
				//addr_ntoa($3->front_handler), "-", //$3->front_rule->module_name->str,
				//addr_ntoa($3->back_handler), "-"); //$3->back_rule->module_name->str);
				addr_ntoa($3->front_handler),($3->front_rule == NULL) ? "(null)" : $3->front_rule->module_name->str,
				addr_ntoa($3->back_handler), ($3->back_rule  == NULL) ? "(null)" : $3->back_rule->module_name->str,
				($3->control_rule  == NULL) ? "(null)" : $3->control_rule->module_name->str);
		*/
	}
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 276 "config_rules.y"
    {
		g_printerr("\tAllocating memory for new structure 'target'\n");
		(yyval.target) = (struct target *)g_malloc0(sizeof(struct target));
		
		// This tree holds the main backend structures
		(yyval.target)->back_handlers = g_tree_new_full((GCompareDataFunc)intcmp, NULL, g_free, (GDestroyNotify)free_backend);
		
		// This table just contains the unique backend IP's
		// The key (char *ip) lives in the struct allocated for back_handlers so don't free it here
		(yyval.target)->unique_backend_ips = g_hash_table_new(g_str_hash, g_str_equal);
	}
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 287 "config_rules.y"
    {
		//g_printerr("Read pcap filter: '%s'\n", $4);
		//g_printerr("Read pcap filter: '%s'\n", g_string_free($4, FALSE));
		//if (pcap_compile_nopcap(1500, LINKTYPE, $$->filter, $4->str, 1, 0) < 0) {
		(yyval.target)->filter = malloc(sizeof(struct bpf_program));
		if (pcap_compile_nopcap(1500, LINKTYPE, (yyval.target)->filter, (yyvsp[(4) - (6)].gstring)->str, 1, 0) < 0) {
			g_printerr("\tPCAP ERROR: '%s'\n", (yyvsp[(4) - (6)].gstring)->str);
                	yyerror("\tIncorrect pcap filter");
		}
		g_printerr("\tPCAP filter compiled:%s\n", (yyvsp[(4) - (6)].gstring)->str);	
		g_string_free((yyvsp[(4) - (6)].gstring), TRUE);
	}
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 299 "config_rules.y"
    {
		(yyval.target)->front_handler = (yyvsp[(3) - (4)].addr);
		g_printerr("\tIP %s (%d) copied to front handler\n", addr_ntoa((yyvsp[(3) - (4)].addr)), (yyvsp[(3) - (4)].addr)->addr_ip);
		//g_printerr("\tResult IP %s (%d)\n", addr_ntoa($$->front_handler), $$->front_handler->addr_ip);
		(yyval.target)->front_rule = NULL;
	}
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 305 "config_rules.y"
    {
		g_printerr("\tIP %s (%d) copied to handler\n", addr_ntoa((yyvsp[(3) - (7)].addr)), (yyvsp[(3) - (7)].addr)->addr_ip);
		(yyval.target)->front_handler = (yyvsp[(3) - (7)].addr);
		(yyval.target)->front_rule = DE_create_tree((yyvsp[(5) - (7)].gstring)->str);
		g_printerr("\tFront decision module is at %p\n", (yyval.target)->front_rule->module);
		g_string_free((yyvsp[(5) - (7)].gstring), TRUE);
	}
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 312 "config_rules.y"
    {
                g_printerr("\tCreating backend picking rule: %s\n", (yyvsp[(4) - (6)].gstring)->str);
		(yyval.target)->back_picker = DE_create_tree((yyvsp[(4) - (6)].gstring)->str);
		g_printerr("\tBackpick module is at %p\n", (yyval.target)->back_picker->module);	
		g_string_free((yyvsp[(4) - (6)].gstring), TRUE);
        }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 318 "config_rules.y"
    {
    		if((yyval.target)->back_picker == NULL) {
    			yyerror("Backend needs a rule if no backend picking rule is defined!\n");
    		}
    		
    		// This will be freed automatically when the tree is destroyed
    		(yyval.target)->backends++;
    		uint32_t *key=g_malloc0(sizeof(uint32_t));
    		*key=(yyval.target)->backends;
    		
    		struct backend *back_handler = g_malloc0(sizeof(struct backend));
    		back_handler->iface=g_malloc0(sizeof(struct interface));
    
            back_handler->iface->ip=(yyvsp[(3) - (4)].addr);
    		back_handler->iface->ip_str=g_malloc0(snprintf(NULL, 0, "%s", addr_ntoa((yyvsp[(3) - (4)].addr))) + 1);
            sprintf(back_handler->iface->ip_str, "%s", addr_ntoa((yyvsp[(3) - (4)].addr)));
    
    		g_tree_insert((yyval.target)->back_handlers, key, back_handler);
    		g_hash_table_insert((yyval.target)->unique_backend_ips, back_handler->iface->ip_str, NULL);
    		
    		g_printerr("\tBackend %u with IP %s copied to handler without rule\n",
    		    *key, back_handler->iface->ip_str);
        }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 341 "config_rules.y"
    {

    		// This will be freed automatically when the tree is destroyed
            (yyval.target)->backends++;
            uint32_t *key=g_malloc0(sizeof(uint32_t));
            *key=(yyval.target)->backends;
            
            struct backend *back_handler = g_malloc0(sizeof(struct backend));
            back_handler->iface=g_malloc0(sizeof(struct interface));
    
            back_handler->iface->ip=(yyvsp[(3) - (7)].addr);
            back_handler->iface->ip_str=g_malloc0(snprintf(NULL, 0, "%s", addr_ntoa((yyvsp[(3) - (7)].addr))) + 1);
            sprintf(back_handler->iface->ip_str, "%s", addr_ntoa((yyvsp[(3) - (7)].addr)));
            
            back_handler->rule=DE_create_tree((yyvsp[(5) - (7)].gstring)->str);
    
    		g_tree_insert((yyval.target)->back_handlers, key, back_handler);
    		g_hash_table_insert((yyval.target)->unique_backend_ips, back_handler->iface->ip_str, NULL);
    	
            g_printerr("\tBackend %u with IP %s copied to handler with rule: %s\n",
                *key, back_handler->iface->ip_str, (yyvsp[(5) - (7)].gstring)->str);
                
           	g_string_free((yyvsp[(5) - (7)].gstring), TRUE);
        }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 365 "config_rules.y"
    {

            // This will be freed automatically when the tree is destroyed
            (yyval.target)->backends++;
            uint32_t *key=g_malloc0(sizeof(uint32_t));
            *key=(yyval.target)->backends;
            
            struct backend *back_handler = g_malloc0(sizeof(struct backend));
            back_handler->iface=g_malloc0(sizeof(struct interface));

            back_handler->iface->tag=g_strdup((yyvsp[(4) - (10)].gstring)->str);
		    back_handler->iface->name=g_strdup((yyvsp[(8) - (10)].gstring)->str);
		    back_handler->iface->mark=*key; // Automatically assign iptables mark
		    back_handler->iface->ip=(yyvsp[(6) - (10)].addr);
		    back_handler->iface->ip_str=g_malloc0(snprintf(NULL, 0, "%s", addr_ntoa((yyvsp[(6) - (10)].addr))) + 1);
		    sprintf(back_handler->iface->ip_str, "%s", addr_ntoa((yyvsp[(6) - (10)].addr)));

            g_tree_insert((yyval.target)->back_handlers, key, back_handler);
            g_hash_table_insert((yyval.target)->unique_backend_ips, back_handler->iface->ip_str, NULL);

            g_printerr("\tBackend %u with IP %s on interface %s and tag %s copied to handler without a rule\n",
                *key, back_handler->iface->ip_str, back_handler->iface->name, back_handler->iface->tag);
                
            g_string_free((yyvsp[(4) - (10)].gstring), TRUE);
		    g_string_free((yyvsp[(8) - (10)].gstring), TRUE);
        }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 391 "config_rules.y"
    {
              
             // This will be freed automatically when the tree is destroyed
            (yyval.target)->backends++;
            uint32_t *key=g_malloc0(sizeof(uint32_t));
            *key=(yyval.target)->backends;
            
            struct backend *back_handler = g_malloc0(sizeof(struct backend));
            back_handler->iface=g_malloc0(sizeof(struct interface));
            
            back_handler->iface->tag=g_strdup((yyvsp[(4) - (13)].gstring)->str);
		    back_handler->iface->name=g_strdup((yyvsp[(8) - (13)].gstring)->str);
		    back_handler->iface->mark=*key; // Automatically assign iptables mark
		    back_handler->iface->ip=(yyvsp[(6) - (13)].addr);
		    back_handler->iface->ip_str=g_malloc0(snprintf(NULL, 0, "%s", addr_ntoa((yyvsp[(6) - (13)].addr))) + 1);
            sprintf(back_handler->iface->ip_str, "%s", addr_ntoa((yyvsp[(6) - (13)].addr)));
            back_handler->rule=DE_create_tree((yyvsp[(11) - (13)].gstring)->str);
            
            g_tree_insert((yyval.target)->back_handlers, key, back_handler);
            g_hash_table_insert((yyval.target)->unique_backend_ips, back_handler->iface->ip_str, NULL);

            g_printerr("\tBackend %u with IP %s on interface %s and tag %s copied to handler with rule: %s\n", 
                *key, back_handler->iface->ip_str, back_handler->iface->name, back_handler->iface->tag, (yyvsp[(11) - (13)].gstring)->str);
            g_string_free((yyvsp[(4) - (13)].gstring), TRUE);
		    g_string_free((yyvsp[(8) - (13)].gstring), TRUE);
		    g_string_free((yyvsp[(11) - (13)].gstring), TRUE);
        }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 418 "config_rules.y"
    {
		(yyval.target)->control_rule = DE_create_tree((yyvsp[(4) - (6)].gstring)->str);
		g_string_free((yyvsp[(4) - (6)].gstring), TRUE);
	}
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 423 "config_rules.y"
    {
        (yyval.addr) = (struct addr *)g_malloc0(sizeof(struct addr));
		if (addr_pton((yyvsp[(1) - (1)].string), (yyval.addr)) < 0) {
            yyerror("\tIllegal IP address");
        }
        g_free((yyvsp[(1) - (1)].string));
	}
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 432 "config_rules.y"
    { 
		(yyval.gstring) = g_string_new("");
	}
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 435 "config_rules.y"
    {
		if ((yyval.gstring)->len > 0) { g_string_append_printf((yyval.gstring), " "); }
		(yyval.gstring) = g_string_append((yyval.gstring), (yyvsp[(2) - (2)].string));
		g_free((yyvsp[(2) - (2)].string));
	 }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 440 "config_rules.y"
    { 
		if ((yyval.gstring)->len > 0) { g_string_append_printf((yyval.gstring), " "); }
		g_string_append_printf((yyval.gstring), "%d", (yyvsp[(2) - (2)].number));
	 }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 444 "config_rules.y"
    { 
		if ((yyval.gstring)->len > 0) { g_string_append_printf((yyval.gstring), " "); }
		(yyval.gstring) = g_string_append((yyval.gstring), (yyvsp[(2) - (2)].string));
		g_free((yyvsp[(2) - (2)].string));
	 }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 449 "config_rules.y"
    { 
		if ((yyval.gstring)->len > 0) { g_string_append_printf((yyval.gstring), " "); }
		(yyval.gstring) = g_string_append((yyval.gstring), (yyvsp[(2) - (2)].string));
		g_free((yyvsp[(2) - (2)].string));
	 }
    break;



/* Line 1806 of yacc.c  */
#line 2080 "config_rules.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 456 "config_rules.y"


static void  yyerror(const char *msg) {
        errx(1,"line %d: %s at '%s'", yylineno, msg, yytext);
}

char* str_append(char * root, char * str) {
	g_printerr("\t##[1] root: %s, str: %s\n", root, str);
		char *tmp = (char *)calloc(strlen(root) + strlen(str), sizeof(char));
  		strcpy(tmp, root);
  		strncat(tmp, str, strlen(root) + strlen(str));
		root = realloc(root, strlen(root) + strlen(str));
		strcpy(root, tmp);
		free(tmp);
	g_printerr("\t##[1] root: %s\n\t=====================\n", root);
		return root;
}
char* int_append(char * root, int number) {
	g_printerr("\t##[1] root: %s, num: %d\n", root, number);
		char *num = malloc(128);
		sprintf(num, "%d", number);
		char *tmp = (char *)calloc(strlen(root) + strlen(num), sizeof(char));
  		strcpy(tmp, root);
  		strncat(tmp, num, strlen(root) + strlen(num));
		root = tmp;
		free(tmp);
		free(num);
	g_printerr("\t##[1] root: %s\n\t=====================\n", root);
		return root;
}


