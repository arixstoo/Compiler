/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "syn.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quadruples.h"
#include "symbole.h"
#define MAX_LOOPS 100

extern int yylineno;
extern int column;
int yylex(void);
int yyerror(char *msg);
char currentScope[20] = "";
char* currentType = NULL;
extern int numQuad;
char temp[20];
int nTemp = 1;
int jumpStack[100];
int jumpTop = 0;
char* loopVariables[MAX_LOOPS];
int currentLoop = -1;

// Structure pour gérer les niveaux d'imbrication des boucles
typedef struct {
    int start;      // Start of the loop
    int condition;  // Position of condition evaluation
    int step;       // Position of step execution
    char* stepValue; 
    int end;        // End of the loop
} BranchInfo;


typedef struct {
    int condition;
    char* stepValue;
    char* loopVar;
} LoopInfo;

typedef struct {
    LoopInfo* loops;
    int capacity;
    int top;
    int initialized;
} LoopStack;


static LoopStack loopStack = {NULL, 0, -1, 0};


#define MAX_BRANCH_DEPTH 50
BranchInfo branchStack[MAX_BRANCH_DEPTH];
int branchTop = 0;

// fonctions pour la gestion des branchements
void pushBranch(int start, int condition, int end) {
    if (branchTop < MAX_BRANCH_DEPTH) {
        branchStack[branchTop].start = start;
        branchStack[branchTop].condition = condition;
        branchStack[branchTop].end = end;
        branchTop++;
    }
}

BranchInfo* topBranch() {
    return branchTop > 0 ? &branchStack[branchTop - 1] : NULL;
}
void print_lexical_stats();

void popBranch() {
    if (branchTop > 0) branchTop--;
}

// générer des variables temporaires
char* newTemp() {
    sprintf(temp, "T%d", nTemp++);
    insererSymbole(temp, "FLOAT", "VARIABLE", currentScope, yylineno);
    return strdup(temp);
}

// Fonction pour mettre à jour les adresses de saut
void updateJumpAddress(int quadIndex, int targetAddress) {
    char temp[20];
    sprintf(temp, "%d", targetAddress);
    ajouterQuad(quadIndex, 4, temp);
}


static void ensureLoopStackInitialized() {
    if (!loopStack.initialized) {
        loopStack.loops = (LoopInfo*)malloc(sizeof(LoopInfo) * 10); // Start with 10 slots
        if (loopStack.loops == NULL) {
            yyerror("Failed to initialize loop stack");
            exit(1);
        }
        loopStack.capacity = 10;
        loopStack.top = -1;
        loopStack.initialized = 1;
    }
}

// Resize the stack 
static void resizeLoopStack() {
    int newCapacity = loopStack.capacity * 2;
    LoopInfo* newLoops = (LoopInfo*)realloc(loopStack.loops, sizeof(LoopInfo) * newCapacity);
    if (newLoops == NULL) {
        yyerror("Memory allocation failed for loop stack");
        exit(1);
    }
    loopStack.loops = newLoops;
    loopStack.capacity = newCapacity;
}

// Push loop info to stack
static void pushLoop(char* loopVar) {
    ensureLoopStackInitialized();
    if (loopStack.top + 1 >= loopStack.capacity) {
        resizeLoopStack();
    }
    loopStack.top++;
    loopStack.loops[loopStack.top].loopVar = strdup(loopVar);
    loopStack.loops[loopStack.top].stepValue = NULL;
    loopStack.loops[loopStack.top].condition = -1;
}

// Get current loop info
static LoopInfo* getCurrentLoop() {
    ensureLoopStackInitialized();
    if (loopStack.top < 0) {
        yyerror("No active loops");
        exit(1);
    }
    return &loopStack.loops[loopStack.top];
}

// Pop loop info from stack
static void popLoop() {
    if (!loopStack.initialized || loopStack.top < 0) {
        yyerror("Stack underflow in loop stack");
        exit(1);
    }
    free(loopStack.loops[loopStack.top].loopVar);
    free(loopStack.loops[loopStack.top].stepValue);
    loopStack.top--;
}



#line 219 "syn.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "syn.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_VAR_GLOBAL = 3,                 /* VAR_GLOBAL  */
  YYSYMBOL_DECLARATION = 4,                /* DECLARATION  */
  YYSYMBOL_INSTRUCTION = 5,                /* INSTRUCTION  */
  YYSYMBOL_INTEGER = 6,                    /* INTEGER  */
  YYSYMBOL_FLOAT = 7,                      /* FLOAT  */
  YYSYMBOL_CHAR = 8,                       /* CHAR  */
  YYSYMBOL_IDF = 9,                        /* IDF  */
  YYSYMBOL_CONST_INT = 10,                 /* CONST_INT  */
  YYSYMBOL_CONST_FLOAT = 11,               /* CONST_FLOAT  */
  YYSYMBOL_CONST_CHAR = 12,                /* CONST_CHAR  */
  YYSYMBOL_CONST_STRING = 13,              /* CONST_STRING  */
  YYSYMBOL_CONST = 14,                     /* CONST  */
  YYSYMBOL_IF = 15,                        /* IF  */
  YYSYMBOL_ELSE = 16,                      /* ELSE  */
  YYSYMBOL_FOR = 17,                       /* FOR  */
  YYSYMBOL_READ = 18,                      /* READ  */
  YYSYMBOL_WRITE = 19,                     /* WRITE  */
  YYSYMBOL_EQ = 20,                        /* EQ  */
  YYSYMBOL_NEQ = 21,                       /* NEQ  */
  YYSYMBOL_LT = 22,                        /* LT  */
  YYSYMBOL_GT = 23,                        /* GT  */
  YYSYMBOL_LTE = 24,                       /* LTE  */
  YYSYMBOL_GTE = 25,                       /* GTE  */
  YYSYMBOL_AND = 26,                       /* AND  */
  YYSYMBOL_OR = 27,                        /* OR  */
  YYSYMBOL_NOT = 28,                       /* NOT  */
  YYSYMBOL_29_ = 29,                       /* '+'  */
  YYSYMBOL_30_ = 30,                       /* '-'  */
  YYSYMBOL_31_ = 31,                       /* '*'  */
  YYSYMBOL_32_ = 32,                       /* '/'  */
  YYSYMBOL_UNARY_MINUS = 33,               /* UNARY_MINUS  */
  YYSYMBOL_34_ = 34,                       /* '{'  */
  YYSYMBOL_35_ = 35,                       /* '}'  */
  YYSYMBOL_36_ = 36,                       /* ';'  */
  YYSYMBOL_37_ = 37,                       /* ','  */
  YYSYMBOL_38_ = 38,                       /* '['  */
  YYSYMBOL_39_ = 39,                       /* ']'  */
  YYSYMBOL_40_ = 40,                       /* '='  */
  YYSYMBOL_41_ = 41,                       /* '('  */
  YYSYMBOL_42_ = 42,                       /* ')'  */
  YYSYMBOL_43_ = 43,                       /* ':'  */
  YYSYMBOL_YYACCEPT = 44,                  /* $accept  */
  YYSYMBOL_program = 45,                   /* program  */
  YYSYMBOL_46_1 = 46,                      /* $@1  */
  YYSYMBOL_47_2 = 47,                      /* $@2  */
  YYSYMBOL_declarations = 48,              /* declarations  */
  YYSYMBOL_declaration = 49,               /* declaration  */
  YYSYMBOL_type = 50,                      /* type  */
  YYSYMBOL_var_list = 51,                  /* var_list  */
  YYSYMBOL_var_decl = 52,                  /* var_decl  */
  YYSYMBOL_const_declaration = 53,         /* const_declaration  */
  YYSYMBOL_constant = 54,                  /* constant  */
  YYSYMBOL_instructions = 55,              /* instructions  */
  YYSYMBOL_instruction = 56,               /* instruction  */
  YYSYMBOL_assignment = 57,                /* assignment  */
  YYSYMBOL_array_elem = 58,                /* array_elem  */
  YYSYMBOL_if_statement = 59,              /* if_statement  */
  YYSYMBOL_60_3 = 60,                      /* $@3  */
  YYSYMBOL_61_4 = 61,                      /* $@4  */
  YYSYMBOL_else_part = 62,                 /* else_part  */
  YYSYMBOL_for_statement = 63,             /* for_statement  */
  YYSYMBOL_64_5 = 64,                      /* $@5  */
  YYSYMBOL_65_6 = 65,                      /* $@6  */
  YYSYMBOL_for_init = 66,                  /* for_init  */
  YYSYMBOL_for_step = 67,                  /* for_step  */
  YYSYMBOL_io_statement = 68,              /* io_statement  */
  YYSYMBOL_write_args = 69,                /* write_args  */
  YYSYMBOL_expression = 70,                /* expression  */
  YYSYMBOL_condition = 71,                 /* condition  */
  YYSYMBOL_comparison_op = 72              /* comparison_op  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   196

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  29
/* YYNRULES -- Number of rules.  */
#define YYNRULES  67
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  147

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   284


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      41,    42,    31,    29,    37,    30,     2,    32,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    43,    36,
       2,    40,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    38,     2,    39,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,    35,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    33
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   196,   196,   199,   196,   211,   212,   216,   217,   221,
     222,   223,   227,   229,   234,   240,   262,   280,   285,   290,
     298,   299,   303,   304,   305,   306,   310,   349,   391,   430,
     437,   430,   454,   460,   468,   471,   468,   510,   530,   541,
     550,   574,   578,   581,   584,   587,   593,   602,   611,   620,
     649,   674,   699,   722,   745,   781,   784,   803,   821,   826,
     830,   833,   839,   840,   841,   842,   843,   844
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "VAR_GLOBAL",
  "DECLARATION", "INSTRUCTION", "INTEGER", "FLOAT", "CHAR", "IDF",
  "CONST_INT", "CONST_FLOAT", "CONST_CHAR", "CONST_STRING", "CONST", "IF",
  "ELSE", "FOR", "READ", "WRITE", "EQ", "NEQ", "LT", "GT", "LTE", "GTE",
  "AND", "OR", "NOT", "'+'", "'-'", "'*'", "'/'", "UNARY_MINUS", "'{'",
  "'}'", "';'", "','", "'['", "']'", "'='", "'('", "')'", "':'", "$accept",
  "program", "$@1", "$@2", "declarations", "declaration", "type",
  "var_list", "var_decl", "const_declaration", "constant", "instructions",
  "instruction", "assignment", "array_elem", "if_statement", "$@3", "$@4",
  "else_part", "for_statement", "$@5", "$@6", "for_init", "for_step",
  "io_statement", "write_args", "expression", "condition", "comparison_op", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-74)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      22,   -74,    50,    28,   -74,   -74,    49,   -74,   -74,   -74,
      -4,    60,   -74,    61,   -74,    65,   -74,    45,   -13,   -74,
      56,    86,   100,   -74,    61,     3,   -74,   102,   -74,   -74,
     -74,   -74,   104,    52,   -74,   -74,   145,   117,   -74,    71,
      33,   111,   125,   133,   134,   -74,   -74,   -74,   136,   -74,
     -74,   -74,    31,    31,    -2,   156,   164,     8,    31,   139,
     -74,   -74,   -74,    31,    31,   128,   106,    -2,    -2,   124,
      55,   138,   137,     9,   -74,   -15,   140,   132,   169,   -74,
      97,    31,    31,    31,    31,   -74,   -74,   -74,    92,    77,
     -74,   -74,   -74,   -74,   -74,   -74,    31,    -2,    -2,   -74,
      31,   -74,   171,   146,    24,   147,   -74,   148,   -74,    46,
      46,   -74,   -74,   -74,   140,   -74,   158,   151,   140,    31,
     149,   -74,   -74,   140,   -74,   -74,   -74,   143,   140,   150,
      76,    31,   153,   -74,   101,   -74,   174,   -74,   157,   -74,
     159,   -74,   -74,    83,    90,   -74,   -74
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     2,     0,     0,     1,     5,     0,     9,    10,    11,
       0,     0,     6,     0,     8,     0,     3,    14,     0,    12,
       0,     0,     0,     7,     0,     0,     5,     0,    13,    17,
      18,    19,     0,     0,    15,    16,     0,     0,    20,     0,
       0,     0,     0,     0,     0,     4,    21,    22,     0,    23,
      24,    25,     0,     0,     0,     0,     0,     0,     0,    49,
      46,    47,    48,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,     0,    42,     0,     0,    56,
       0,     0,     0,     0,     0,    28,    26,    60,     0,     0,
      62,    63,    64,    65,    66,    67,     0,     0,     0,    29,
       0,    34,     0,     0,     0,     0,    27,     0,    55,    51,
      52,    53,    54,    61,    57,    58,    59,     0,    37,     0,
       0,    39,    45,    44,    41,    50,    20,     0,    38,     0,
       0,     0,     0,    30,     0,    40,    32,    35,     0,    31,
       0,    20,    20,     0,     0,    33,    36
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -74,   -74,   -74,   -74,   168,   -74,   185,   -74,   172,   -74,
     -74,   -73,   -74,   -74,   -74,   -74,   -74,   -74,   -74,   -74,
     -74,   -74,   -74,   -74,   -74,   -74,   -52,   -22,   -74
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,    21,     6,    12,    13,    18,    19,    14,
      32,    39,    46,    47,    48,    49,   117,   136,   139,    50,
     119,   140,    72,   127,    51,    75,    69,    70,    96
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      65,    66,     7,     8,     9,    76,    77,    59,    60,    61,
      62,    79,    80,    29,    30,    31,    88,    59,    60,    61,
      62,    74,   104,    23,    24,     1,    67,   105,    63,   109,
     110,   111,   112,    59,    60,    61,    62,   122,    63,    68,
      59,    60,    61,    62,   114,    87,    89,   102,   118,    64,
       4,   103,   123,   130,    63,     7,     8,     9,     7,     8,
       9,    63,     5,    10,    16,    64,    10,   128,   143,   144,
      17,    52,    64,    53,    20,   115,   116,    83,    84,   134,
      40,    97,    98,    22,    11,    40,    41,    36,    42,    43,
      44,    41,    40,    42,    43,    44,    25,    99,    41,    40,
      42,    43,    44,    97,    98,    41,    45,    42,    43,    44,
      27,   133,    90,    91,    92,    93,    94,    95,   145,   113,
      26,    81,    82,    83,    84,   146,    81,    82,    83,    84,
      81,    82,    83,    84,   108,    81,    82,    83,    84,   108,
      35,    34,    86,   137,    90,    91,    92,    93,    94,    95,
      37,    38,    54,    81,    82,    83,    84,    81,    82,    83,
      84,    81,    82,    83,    84,    71,    55,    85,   106,    81,
      82,    83,    84,    73,    56,    57,    58,    78,   100,   107,
     101,   120,   121,   124,    97,   126,   131,   125,   129,   135,
     138,   141,   132,   142,    33,    15,    28
};

static const yytype_uint8 yycheck[] =
{
      52,    53,     6,     7,     8,    57,    58,     9,    10,    11,
      12,    63,    64,    10,    11,    12,    68,     9,    10,    11,
      12,    13,    37,    36,    37,     3,    28,    42,    30,    81,
      82,    83,    84,     9,    10,    11,    12,    13,    30,    41,
       9,    10,    11,    12,    96,    67,    68,    38,   100,    41,
       0,    42,   104,   126,    30,     6,     7,     8,     6,     7,
       8,    30,    34,    14,     4,    41,    14,   119,   141,   142,
       9,    38,    41,    40,     9,    97,    98,    31,    32,   131,
       9,    26,    27,    38,    35,     9,    15,    35,    17,    18,
      19,    15,     9,    17,    18,    19,    40,    42,    15,     9,
      17,    18,    19,    26,    27,    15,    35,    17,    18,    19,
      10,    35,    20,    21,    22,    23,    24,    25,    35,    42,
      34,    29,    30,    31,    32,    35,    29,    30,    31,    32,
      29,    30,    31,    32,    42,    29,    30,    31,    32,    42,
      36,    39,    36,    42,    20,    21,    22,    23,    24,    25,
       5,    34,    41,    29,    30,    31,    32,    29,    30,    31,
      32,    29,    30,    31,    32,     9,    41,    39,    36,    29,
      30,    31,    32,     9,    41,    41,    40,    38,    40,    10,
      43,    10,    36,    36,    26,    34,    43,    39,    39,    36,
      16,    34,    42,    34,    26,    10,    24
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    45,    46,     0,    34,    48,     6,     7,     8,
      14,    35,    49,    50,    53,    50,     4,     9,    51,    52,
       9,    47,    38,    36,    37,    40,    34,    10,    52,    10,
      11,    12,    54,    48,    39,    36,    35,     5,    34,    55,
       9,    15,    17,    18,    19,    35,    56,    57,    58,    59,
      63,    68,    38,    40,    41,    41,    41,    41,    40,     9,
      10,    11,    12,    30,    41,    70,    70,    28,    41,    70,
      71,     9,    66,     9,    13,    69,    70,    70,    38,    70,
      70,    29,    30,    31,    32,    39,    36,    71,    70,    71,
      20,    21,    22,    23,    24,    25,    72,    26,    27,    42,
      40,    43,    38,    42,    37,    42,    36,    10,    42,    70,
      70,    70,    70,    42,    70,    71,    71,    60,    70,    64,
      10,    36,    13,    70,    36,    39,    34,    67,    70,    39,
      55,    43,    42,    35,    70,    36,    61,    42,    16,    62,
      65,    34,    34,    55,    55,    35,    35
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    44,    46,    47,    45,    48,    48,    49,    49,    50,
      50,    50,    51,    51,    52,    52,    53,    54,    54,    54,
      55,    55,    56,    56,    56,    56,    57,    57,    58,    60,
      61,    59,    62,    62,    64,    65,    63,    66,    67,    68,
      68,    68,    69,    69,    69,    69,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    71,    71,    71,
      71,    71,    72,    72,    72,    72,    72,    72
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,    14,     0,     2,     3,     1,     1,
       1,     1,     1,     3,     1,     4,     6,     1,     1,     1,
       0,     2,     1,     1,     1,     1,     4,     4,     4,     0,
       0,    10,     0,     4,     0,     0,    13,     3,     1,     5,
       8,     5,     1,     1,     3,     3,     1,     1,     1,     1,
       4,     3,     3,     3,     3,     3,     2,     3,     3,     3,
       2,     3,     1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 196 "syn.y"
                 {
        strcpy(currentScope, "GLOBAL");
    }
#line 1380 "syn.tab.c"
    break;

  case 3: /* $@2: %empty  */
#line 199 "syn.y"
                  {
        strcpy(currentScope, "LOCAL");
      }
#line 1388 "syn.tab.c"
    break;

  case 4: /* program: VAR_GLOBAL $@1 '{' declarations '}' DECLARATION $@2 '{' declarations '}' INSTRUCTION '{' instructions '}'  */
#line 203 "syn.y"
      {
          printf("\nAnalyse syntaxique et sémantique terminée avec succès\n");
          print_lexical_stats();
          afficherTS();
          afficherQuad();
      }
#line 1399 "syn.tab.c"
    break;

  case 9: /* type: INTEGER  */
#line 221 "syn.y"
              { currentType = "INTEGER"; (yyval.string) = "INTEGER"; }
#line 1405 "syn.tab.c"
    break;

  case 10: /* type: FLOAT  */
#line 222 "syn.y"
            { currentType = "FLOAT"; (yyval.string) = "FLOAT"; }
#line 1411 "syn.tab.c"
    break;

  case 11: /* type: CHAR  */
#line 223 "syn.y"
           { currentType = "CHAR"; (yyval.string) = "CHAR"; }
#line 1417 "syn.tab.c"
    break;

  case 12: /* var_list: var_decl  */
#line 227 "syn.y"
               {
    }
#line 1424 "syn.tab.c"
    break;

  case 13: /* var_list: var_list ',' var_decl  */
#line 229 "syn.y"
                            {
    }
#line 1431 "syn.tab.c"
    break;

  case 14: /* var_decl: IDF  */
#line 234 "syn.y"
          {
        int idx = insererSymbole((yyvsp[0].string), currentType, "VARIABLE", currentScope, yylineno);
        if (idx == -1) {
            yyerror("Erreur de déclaration");
        }
    }
#line 1442 "syn.tab.c"
    break;

  case 15: /* var_decl: IDF '[' CONST_INT ']'  */
#line 240 "syn.y"
                            {
        if ((yyvsp[-1].integer) <= 0) {
            yyerror("La taille du tableau doit être positive");
        }
        
        // Insérer le tableau de base
        int baseIdx = insererSymbole((yyvsp[-3].string), currentType, "TABLEAU", currentScope, yylineno);
        
        if (baseIdx != -1) {
            // Créer des entrées individuelles pour chaque élément du tableau
            for (int i = 0; i < (yyvsp[-1].integer); i++) {
                int elemIdx = insererElementTableau((yyvsp[-3].string), i, currentType, currentScope, yylineno);
                if (elemIdx == -1) {
                    yyerror("Erreur lors de la création des éléments du tableau");
                    break;
                }
            }
        }
    }
#line 1466 "syn.tab.c"
    break;

  case 16: /* const_declaration: CONST type IDF '=' constant ';'  */
#line 262 "syn.y"
                                      {
        int idx = insererSymbole((yyvsp[-3].string), (yyvsp[-4].string), "CONSTANT", currentScope, yylineno);
        if (idx != -1) {
            // Check type compatibility
            if (strcmp((yyvsp[-4].string), (yyvsp[-1].exp).type) != 0) {
                char errorMsg[100];
                sprintf(errorMsg, "Erreur : Type incompatible pour la constante '%s'. Déclaré : %s, donné : %s.", 
                        (yyvsp[-3].string), (yyvsp[-4].string), (yyvsp[-1].exp).type);
                yyerror(errorMsg);
            } else {
                // Initialize the constant with its value
                setConstanteValeur(idx, (yyvsp[-4].string), (yyvsp[-1].exp).value);
            }
        }
    }
#line 1486 "syn.tab.c"
    break;

  case 17: /* constant: CONST_INT  */
#line 280 "syn.y"
                {
        (yyval.exp).type = "INTEGER";
        sprintf(temp, "%d", (yyvsp[0].integer));
        (yyval.exp).value = strdup(temp);
    }
#line 1496 "syn.tab.c"
    break;

  case 18: /* constant: CONST_FLOAT  */
#line 285 "syn.y"
                  {
        (yyval.exp).type = "FLOAT";
        sprintf(temp, "%f", (yyvsp[0].real));
        (yyval.exp).value = strdup(temp);
    }
#line 1506 "syn.tab.c"
    break;

  case 19: /* constant: CONST_CHAR  */
#line 290 "syn.y"
                 {
        (yyval.exp).type = "CHAR";
        sprintf(temp, "'%c'", (yyvsp[0].character));
        (yyval.exp).value = strdup(temp);
    }
#line 1516 "syn.tab.c"
    break;

  case 26: /* assignment: IDF '=' expression ';'  */
#line 310 "syn.y"
                             {
        int idx = rechercherSymbole((yyvsp[-3].string));
        if (idx == -1) {
            yyerror("Variable non déclarée");
        } else {
            int erreur=1;
            // Type compatibility check
            if (strcmp(ts.table[idx].type, (yyvsp[-1].exp).type) != 0) {
                char errorMsg[100];
                sprintf(errorMsg, "Type incompatible pour la variable '%s'. Déclaré : %s, donné : %s.", 
                        ts.table[idx].nom, ts.table[idx].type, (yyvsp[-1].exp).type);
                yyerror(errorMsg);
                erreur = -1;
            }

            if(erreur != -1){
                // Character type special handling
                if (strcmp(ts.table[idx].type, "CHAR") == 0) {
                    if (strlen((yyvsp[-1].exp).value) > 1) {
                        printf("Avertissement ligne %d: La variable CHAR '%s' ne peut contenir qu'un seul caractère. Seul le premier caractère sera utilisé.\n", 
                            yylineno, (yyvsp[-3].string));
                    }
                    // Take only the first character
                    ts.table[idx].valeur.caractere = (yyvsp[-1].exp).value[1];
                }


                // Assign value based on type
                if (strcmp(ts.table[idx].type, "INTEGER") == 0) {
                    ts.table[idx].valeur.entier = atoi((yyvsp[-1].exp).value);
                } else if (strcmp(ts.table[idx].type, "FLOAT") == 0) {
                    ts.table[idx].valeur.reel = atof((yyvsp[-1].exp).value);
                }
                // Generate quadruplet
                insererQuadComplet("=", (yyvsp[-1].exp).temp, "", (yyvsp[-3].string));
                ts.table[idx].initialise = 1;
            }
        }
    }
#line 1560 "syn.tab.c"
    break;

  case 27: /* assignment: array_elem '=' expression ';'  */
#line 349 "syn.y"
                                    {
        int idx = rechercherSymbole((yyvsp[-3].exp).temp);
        if (idx == -1) {
            yyerror("Tableau non déclaré");
        } else {
            int erreur=1;
            // Type compatibility check
            if (strcmp(ts.table[idx].type, (yyvsp[-1].exp).type) != 0) {
                char errorMsg[100];
                sprintf(errorMsg, "Type incompatible pour la variable '%s'. Déclaré : %s, donné : %s.", 
                        ts.table[idx].nom, ts.table[idx].type, (yyvsp[-1].exp).type);
                yyerror(errorMsg);
                erreur = -1;
            }

            if(erreur != -1){
                // Character type special handling
                if (strcmp(ts.table[idx].type, "CHAR") == 0) {
                    if (strlen((yyvsp[-1].exp).value) > 1) {
                        printf("Avertissement ligne %d: La variable CHAR '%s' ne peut contenir qu'un seul caractère. Seul le premier caractère sera utilisé.\n", 
                            yylineno, (yyvsp[-3].exp));
                    }
                    // Take only the first character
                    ts.table[idx].valeur.caractere = (yyvsp[-1].exp).value[1];
                }


                // Assign value based on type
                if (strcmp(ts.table[idx].type, "INTEGER") == 0) {
                    ts.table[idx].valeur.entier = atoi((yyvsp[-1].exp).value);
                } else if (strcmp(ts.table[idx].type, "FLOAT") == 0) {
                    ts.table[idx].valeur.reel = atof((yyvsp[-1].exp).value);
                }
                // Generate quadruplet
                insererQuadComplet("ARRAY_ASSIGN", (yyvsp[-1].exp).temp, "", (yyvsp[-3].exp).temp);
                ts.table[idx].initialise = 1;
            }
        }
    }
#line 1604 "syn.tab.c"
    break;

  case 28: /* array_elem: IDF '[' expression ']'  */
#line 391 "syn.y"
                             {
        int baseIdx = rechercherSymbole((yyvsp[-3].string));
        if (baseIdx == -1) {
            yyerror("Tableau non déclaré");
            (yyval.exp).temp = newTemp();
            (yyval.exp).type = "INTEGER";
            (yyval.exp).value = strdup("0");
        } else if (strcmp(ts.table[baseIdx].nature, "TABLEAU") != 0) {
            yyerror("Identificateur n'est pas un tableau");
            (yyval.exp).temp = newTemp();
            (yyval.exp).type = strdup(ts.table[baseIdx].type);
            (yyval.exp).value = strdup("0");
        } else {
            // Convert expression to integer index
            int indexValue = atoi((yyvsp[-1].exp).value);
            
            // Check array bounds with precise naming
            char elemName[MAX_NAME_LENGTH];
            sprintf(elemName, "%s[%d]", (yyvsp[-3].string), indexValue);
            
            int elemIdx = rechercherSymbole(elemName);
            if (elemIdx == -1) {
                yyerror("Élément de tableau non déclaré");
                elemIdx = insererElementTableau((yyvsp[-3].string), indexValue, 
                                                ts.table[baseIdx].type, 
                                                currentScope, yylineno);
            }

            (yyval.exp).temp = strdup(elemName);
            (yyval.exp).type = strdup(ts.table[baseIdx].type);
            (yyval.exp).array_size = indexValue;

            // Add quadruplet for array access
            insererQuadComplet("ARRAY_ACCESS", (yyvsp[-3].string), (yyvsp[-1].exp).value, (yyval.exp).temp);
        }
    }
#line 1645 "syn.tab.c"
    break;

  case 29: /* $@3: %empty  */
#line 430 "syn.y"
                           {
        // Sauvegarder la position actuelle pour le branchement conditionnel
        pushBranch(numQuad - 1, numQuad, -1);  // -1 pour end (à remplir plus tard)
        
        // Générer le saut conditionnel
        insererQuadComplet("BZ", (yyvsp[-1].string), "", "?");
    }
#line 1657 "syn.tab.c"
    break;

  case 30: /* $@4: %empty  */
#line 437 "syn.y"
                         {
        BranchInfo* current = topBranch();
        
        // Générer le saut inconditionnel pour sauter la partie ELSE
        int jumpToEnd = numQuad;
        insererQuadComplet("BR", "", "", "?");
        
        // Mettre à jour le saut conditionnel avec l'adresse du ELSE
        updateJumpAddress(current->condition, numQuad);
        
        // Sauvegarder la position pour la mise à jour finale
        current->end = jumpToEnd;
    }
#line 1675 "syn.tab.c"
    break;

  case 32: /* else_part: %empty  */
#line 454 "syn.y"
                 {
        BranchInfo* current = topBranch();
        // Mettre à jour le saut à la fin du IF
        updateJumpAddress(current->end, numQuad);
        popBranch();
    }
#line 1686 "syn.tab.c"
    break;

  case 33: /* else_part: ELSE '{' instructions '}'  */
#line 460 "syn.y"
                                {
        BranchInfo* current = topBranch();
        // Mettre à jour le saut à la fin du ELSE
        updateJumpAddress(current->end, numQuad);
        popBranch();
    }
#line 1697 "syn.tab.c"
    break;

  case 34: /* $@5: %empty  */
#line 468 "syn.y"
                                     {
    // Save start of loop
    pushBranch(numQuad, -1, -1);
}
#line 1706 "syn.tab.c"
    break;

  case 35: /* $@6: %empty  */
#line 471 "syn.y"
                              {
    BranchInfo* current = topBranch();
    LoopInfo* currentLoop = getCurrentLoop();
    
    // Compare loop var with end expression
    char temp[20];
    sprintf(temp, "t%d", tempCounter++);
    
    // Use GE operator for loop condition
    insererQuadComplet("GE", currentLoop->loopVar, (yyvsp[-1].exp).temp, temp);
    
    // Store condition position
    current->condition = numQuad;
    insererQuadComplet("BNZ", temp, "", "");
}
#line 1726 "syn.tab.c"
    break;

  case 36: /* for_statement: FOR '(' for_init ':' $@5 for_step ':' expression ')' $@6 '{' instructions '}'  */
#line 485 "syn.y"
                       {
    BranchInfo* current = topBranch();
    LoopInfo* currentLoop = getCurrentLoop();
    
    // Add step value to loop variable
    char stepTemp[20];
    sprintf(stepTemp, "t%d", tempCounter++);
    insererQuadComplet("+", currentLoop->loopVar, currentLoop->stepValue, stepTemp);
    insererQuadComplet("=", stepTemp, "", currentLoop->loopVar);
    
    // Jump back to condition
    char condTemp[20];
    sprintf(condTemp, "%d", current->condition);
    insererQuadComplet("BR", "", "", condTemp);
    
    // Update branch target
    char endTemp[20];
    sprintf(endTemp, "%d", numQuad);
    updateQuad(current->condition, endTemp, 3);
    
    // Clean up current loop
    popLoop();
    popBranch();
}
#line 1755 "syn.tab.c"
    break;

  case 37: /* for_init: IDF '=' expression  */
#line 510 "syn.y"
                              {
    int idx = rechercherSymbole((yyvsp[-2].string));
    if (idx == -1) {
        yyerror("Variable non déclarée");
    } else {
        // Check if the initialization expression is of type INTEGER
        if (strcmp((yyvsp[0].exp).type, "INTEGER") != 0) {
            char errorMsg[100];
            sprintf(errorMsg, "La valeur d'initialisation de la boucle FOR doit être de type INTEGER, trouvé: %s", (yyvsp[0].exp).type);
            yyerror(errorMsg);
        } else {
            // Now we can assign the integer expression to the loop variable
            insererQuadComplet("=", (yyvsp[0].exp).temp, "", (yyvsp[-2].string));
            ts.table[idx].initialise = 1;
            pushLoop((yyvsp[-2].string));
        }
    }
}
#line 1778 "syn.tab.c"
    break;

  case 38: /* for_step: expression  */
#line 530 "syn.y"
                      {
    // Check if step value is of type INTEGER
    if (strcmp((yyvsp[0].exp).type, "INTEGER") != 0) {
        yyerror("Le pas de la boucle FOR doit être de type INTEGER");
    } else {
        LoopInfo* currentLoop = getCurrentLoop();
        currentLoop->stepValue = strdup((yyvsp[0].exp).temp);
    }
}
#line 1792 "syn.tab.c"
    break;

  case 39: /* io_statement: READ '(' IDF ')' ';'  */
#line 541 "syn.y"
                           {
        int idx = rechercherSymbole((yyvsp[-2].string));
        if (idx == -1) {
            yyerror("Variable non déclarée");
        } else {
            ts.table[idx].initialise = 1;
            insererQuadComplet("READ", "", "", (yyvsp[-2].string));
        }
    }
#line 1806 "syn.tab.c"
    break;

  case 40: /* io_statement: READ '(' IDF '[' CONST_INT ']' ')' ';'  */
#line 550 "syn.y"
                                            {
        int idx = rechercherSymbole((yyvsp[-5].string));
        if (idx == -1) {
            yyerror("Tableau non déclaré");
        } else {
            idx = idx + (yyvsp[-3].integer) + 1;
            /*$$.temp = strdup($3);
            $$.type = strdup(ts.table[idx].type);
            
            // Create a string representation of the value
            $$.value = malloc(50);  // Allocate enough memory
            if (strcmp($$.type, "INTEGER") == 0) {
                sprintf($$.value, "%d", ts.table[idx].valeur.entier);
            } else if (strcmp($$.type, "FLOAT") == 0) {
                sprintf($$.value, "%f", ts.table[idx].valeur.reel);
            } else if (strcmp($$.type, "CHAR") == 0) {
                sprintf($$.value, "'%c'", ts.table[idx].valeur.caractere);
            } else {
                strcpy($$.value, "0");
            }*/
            ts.table[idx].initialise = 1;
            insererQuadComplet("READ", "", "", (yyvsp[-5].string));
        }
    }
#line 1835 "syn.tab.c"
    break;

  case 42: /* write_args: expression  */
#line 578 "syn.y"
                 {
        insererQuadComplet("WRITE", (yyvsp[0].exp).temp, "", "");
    }
#line 1843 "syn.tab.c"
    break;

  case 43: /* write_args: CONST_STRING  */
#line 581 "syn.y"
                   {
        insererQuadComplet("WRITE", (yyvsp[0].string), "", "");
    }
#line 1851 "syn.tab.c"
    break;

  case 44: /* write_args: write_args ',' expression  */
#line 584 "syn.y"
                                {
        insererQuadComplet("WRITE", (yyvsp[0].exp).temp, "", "");
    }
#line 1859 "syn.tab.c"
    break;

  case 45: /* write_args: write_args ',' CONST_STRING  */
#line 587 "syn.y"
                                  {
        insererQuadComplet("WRITE", (yyvsp[0].string), "", "");
    }
#line 1867 "syn.tab.c"
    break;

  case 46: /* expression: CONST_INT  */
#line 593 "syn.y"
                {
        (yyval.exp).temp = newTemp();
        char val[20];
        sprintf(val, "%d", (yyvsp[0].integer));
        insererQuadComplet("=", val, "", (yyval.exp).temp);
        (yyval.exp).type = "INTEGER";
        (yyval.exp).value = malloc(20);  // Allocate memory for value
        sprintf((yyval.exp).value, "%d", (yyvsp[0].integer));
    }
#line 1881 "syn.tab.c"
    break;

  case 47: /* expression: CONST_FLOAT  */
#line 602 "syn.y"
                  {
        (yyval.exp).temp = newTemp();
        char val[20];
        sprintf(val, "%f", (yyvsp[0].real));
        insererQuadComplet("=", val, "", (yyval.exp).temp);
        (yyval.exp).type = "FLOAT";
        (yyval.exp).value = malloc(20);  // Allocate memory for value
        sprintf((yyval.exp).value, "%f", (yyvsp[0].real));
    }
#line 1895 "syn.tab.c"
    break;

  case 48: /* expression: CONST_CHAR  */
#line 611 "syn.y"
                 {
        (yyval.exp).temp = newTemp();
        char val[20];
        sprintf(val, "'%c'", (yyvsp[0].character));
        insererQuadComplet("=", val, "", (yyval.exp).temp);
        (yyval.exp).type = "CHAR";
        (yyval.exp).value = malloc(20);  // Allocate memory for value
        sprintf((yyval.exp).value, "'%c'", (yyvsp[0].character));
    }
#line 1909 "syn.tab.c"
    break;

  case 49: /* expression: IDF  */
#line 620 "syn.y"
          {
        int idx = rechercherSymbole((yyvsp[0].string));
        if (idx == -1) {
            yyerror("Variable non déclarée");
            (yyval.exp).temp = newTemp();
            (yyval.exp).type = "INTEGER";  // Default type
            (yyval.exp).value = strdup("0");
        } else if (!ts.table[idx].initialise) {
            yyerror("Variable non initialisée");
            (yyval.exp).temp = newTemp();
            (yyval.exp).type = strdup(ts.table[idx].type);
            (yyval.exp).value = strdup("0");
        } else {
            (yyval.exp).temp = strdup((yyvsp[0].string));
            (yyval.exp).type = strdup(ts.table[idx].type);
            
            // Create a string representation of the value
            (yyval.exp).value = malloc(50);  // Allocate enough memory
            if (strcmp((yyval.exp).type, "INTEGER") == 0) {
                sprintf((yyval.exp).value, "%d", ts.table[idx].valeur.entier);
            } else if (strcmp((yyval.exp).type, "FLOAT") == 0) {
                sprintf((yyval.exp).value, "%f", ts.table[idx].valeur.reel);
            } else if (strcmp((yyval.exp).type, "CHAR") == 0) {
                sprintf((yyval.exp).value, "'%c'", ts.table[idx].valeur.caractere);
            } else {
                strcpy((yyval.exp).value, "0");
            }
        }
    }
#line 1943 "syn.tab.c"
    break;

  case 50: /* expression: IDF '[' CONST_INT ']'  */
#line 649 "syn.y"
                            {
        int idx = rechercherSymbole((yyvsp[-3].string));
        if (idx == -1) {
            yyerror("Tableau non déclaré");
        } else if (! (ts.table[idx].initialise + (yyvsp[-1].integer) + 1)) {
            yyerror("Case Tableau non initialisée");
            (yyval.exp).temp = newTemp();
        } else {
            idx = idx + (yyvsp[-1].integer) + 1;
            (yyval.exp).temp = strdup((yyvsp[-3].string));
            (yyval.exp).type = strdup(ts.table[idx].type);
            
            // Create a string representation of the value
            (yyval.exp).value = malloc(50);  // Allocate enough memory
            if (strcmp((yyval.exp).type, "INTEGER") == 0) {
                sprintf((yyval.exp).value, "%d", ts.table[idx].valeur.entier);
            } else if (strcmp((yyval.exp).type, "FLOAT") == 0) {
                sprintf((yyval.exp).value, "%f", ts.table[idx].valeur.reel);
            } else if (strcmp((yyval.exp).type, "CHAR") == 0) {
                sprintf((yyval.exp).value, "'%c'", ts.table[idx].valeur.caractere);
            } else {
                strcpy((yyval.exp).value, "0");
            }
        }
    }
#line 1973 "syn.tab.c"
    break;

  case 51: /* expression: expression '+' expression  */
#line 674 "syn.y"
                                {
        (yyval.exp).temp = newTemp();
        insererQuadComplet("+", (yyvsp[-2].exp).temp, (yyvsp[0].exp).temp, (yyval.exp).temp);
        
        // Type determination and value computation
        if (strcmp((yyvsp[-2].exp).type, (yyvsp[0].exp).type) == 0) {
            (yyval.exp).type = strdup((yyvsp[-2].exp).type);
            
            // Compute value if possible
            (yyval.exp).value = malloc(50);
            if (strcmp((yyvsp[-2].exp).type, "INTEGER") == 0) {
                int val = atoi((yyvsp[-2].exp).value) + atoi((yyvsp[0].exp).value);
                sprintf((yyval.exp).value, "%d", val);
            } else if (strcmp((yyvsp[-2].exp).type, "FLOAT") == 0) {
                float val = atof((yyvsp[-2].exp).value) + atof((yyvsp[0].exp).value);
                sprintf((yyval.exp).value, "%f", val);
            } else {
                strcpy((yyval.exp).value, "0");
            }
        } else {
            // Type mismatch handling
            (yyval.exp).type = "FLOAT";  // Default to float for mixed types
            (yyval.exp).value = strdup("0");
        }
    }
#line 2003 "syn.tab.c"
    break;

  case 52: /* expression: expression '-' expression  */
#line 699 "syn.y"
                                {
        (yyval.exp).temp = newTemp();
        insererQuadComplet("-", (yyvsp[-2].exp).temp, (yyvsp[0].exp).temp, (yyval.exp).temp);
        
        // Type and value computation similar to '+'
        if (strcmp((yyvsp[-2].exp).type, (yyvsp[0].exp).type) == 0) {
            (yyval.exp).type = strdup((yyvsp[-2].exp).type);
            
            (yyval.exp).value = malloc(50);
            if (strcmp((yyvsp[-2].exp).type, "INTEGER") == 0) {
                int val = atoi((yyvsp[-2].exp).value) - atoi((yyvsp[0].exp).value);
                sprintf((yyval.exp).value, "%d", val);
            } else if (strcmp((yyvsp[-2].exp).type, "FLOAT") == 0) {
                float val = atof((yyvsp[-2].exp).value) - atof((yyvsp[0].exp).value);
                sprintf((yyval.exp).value, "%f", val);
            } else {
                strcpy((yyval.exp).value, "0");
            }
        } else {
            (yyval.exp).type = "FLOAT";
            (yyval.exp).value = strdup("0");
        }
    }
#line 2031 "syn.tab.c"
    break;

  case 53: /* expression: expression '*' expression  */
#line 722 "syn.y"
                                {
        (yyval.exp).temp = newTemp();
        insererQuadComplet("*", (yyvsp[-2].exp).temp, (yyvsp[0].exp).temp, (yyval.exp).temp);
        
        // Type and value computation similar to '+'
        if (strcmp((yyvsp[-2].exp).type, (yyvsp[0].exp).type) == 0) {
            (yyval.exp).type = strdup((yyvsp[-2].exp).type);
            
            (yyval.exp).value = malloc(50);
            if (strcmp((yyvsp[-2].exp).type, "INTEGER") == 0) {
                int val = atoi((yyvsp[-2].exp).value) * atoi((yyvsp[0].exp).value);
                sprintf((yyval.exp).value, "%d", val);
            } else if (strcmp((yyvsp[-2].exp).type, "FLOAT") == 0) {
                float val = atof((yyvsp[-2].exp).value) * atof((yyvsp[0].exp).value);
                sprintf((yyval.exp).value, "%f", val);
            } else {
                strcpy((yyval.exp).value, "0");
            }
        } else {
            (yyval.exp).type = "FLOAT";
            (yyval.exp).value = strdup("0");
        }
    }
#line 2059 "syn.tab.c"
    break;

  case 54: /* expression: expression '/' expression  */
#line 745 "syn.y"
                                {
        // Implement type conversion for integer division
        char* resultType = "FLOAT";  // Default to float for division
        
        // Determine the result type based on input types
        if (strcmp((yyvsp[-2].exp).type, "INTEGER") == 0 && strcmp((yyvsp[0].exp).type, "INTEGER") == 0) {
            printf("Warning: Integer division might lose precision. Result will be a float.\n");
        }

        // Check for division by zero
        int is_zero = 0;
        if (strcmp((yyvsp[0].exp).type, "INTEGER") == 0) {
            is_zero = (atoi((yyvsp[0].exp).value) == 0);
        } else if (strcmp((yyvsp[0].exp).type, "FLOAT") == 0) {
            is_zero = (atof((yyvsp[0].exp).value) == 0.0);
        }

        if (is_zero) {
            printf("Warning: Division by zero. Using 1 as denominator.\n");
            (yyval.exp).temp = newTemp();
            (yyval.exp).type = strdup(resultType);
            (yyval.exp).value = strdup("0.0");
            insererQuadComplet("/", (yyvsp[-2].exp).temp, "1", (yyval.exp).temp);
        } else {
            (yyval.exp).temp = newTemp();
            insererQuadComplet("/", (yyvsp[-2].exp).temp, (yyvsp[0].exp).temp, (yyval.exp).temp);
            
            (yyval.exp).type = strdup(resultType);
            (yyval.exp).value = malloc(50);
            
            // Always compute float division
            float val = (strcmp((yyvsp[-2].exp).type, "INTEGER") == 0 ? atoi((yyvsp[-2].exp).value) : atof((yyvsp[-2].exp).value)) /
                        (strcmp((yyvsp[0].exp).type, "INTEGER") == 0 ? atoi((yyvsp[0].exp).value) : atof((yyvsp[0].exp).value));
            sprintf((yyval.exp).value, "%f", val);
        }
    }
#line 2100 "syn.tab.c"
    break;

  case 55: /* expression: '(' expression ')'  */
#line 781 "syn.y"
                         {
        (yyval.exp) = (yyvsp[-1].exp);
    }
#line 2108 "syn.tab.c"
    break;

  case 56: /* expression: '-' expression  */
#line 784 "syn.y"
                                       {
        (yyval.exp).temp = newTemp();
        insererQuadComplet("*", (yyvsp[0].exp).temp, "-1", (yyval.exp).temp);
        
        (yyval.exp).type = (yyvsp[0].exp).type;
        (yyval.exp).value = malloc(50);
        if (strcmp((yyval.exp).type, "INTEGER") == 0) {
            int val = -atoi((yyvsp[0].exp).value);
            sprintf((yyval.exp).value, "%d", val);
        } else if (strcmp((yyval.exp).type, "FLOAT") == 0) {
            float val = -atof((yyvsp[0].exp).value);
            sprintf((yyval.exp).value, "%f", val);
        } else {
            strcpy((yyval.exp).value, "0");
        }
    }
#line 2129 "syn.tab.c"
    break;

  case 57: /* condition: expression comparison_op expression  */
#line 803 "syn.y"
                                          {
        (yyval.string) = newTemp();
        char* opcode;
        if (strcmp((yyvsp[-2].exp).type, "INTEGER") != 0 && 
            strcmp((yyvsp[-2].exp).type, "FLOAT") != 0 && 
            strcmp((yyvsp[-2].exp).type, "CHAR") != 0) {
            yyerror("Type invalide pour la comparaison");
        }  
        if (strcmp((yyvsp[-1].string), "==") == 0) opcode = "NE";         // Branch if not equal
        else if (strcmp((yyvsp[-1].string), "!=") == 0) opcode = "EQ";    // Branch if equal
        else if (strcmp((yyvsp[-1].string), "<") == 0) opcode = "GE";     // Branch if greater/equal
        else if (strcmp((yyvsp[-1].string), ">") == 0) opcode = "LE";     // Branch if less/equal
        else if (strcmp((yyvsp[-1].string), "<=") == 0) opcode = "GT";    // Branch if greater
        else if (strcmp((yyvsp[-1].string), ">=") == 0) opcode = "LT";    // Branch if less

        // Generate comparison quadruple
        insererQuadComplet(opcode, (yyvsp[-2].exp).temp, (yyvsp[0].exp).temp, (yyval.string));
    }
#line 2152 "syn.tab.c"
    break;

  case 58: /* condition: condition AND condition  */
#line 821 "syn.y"
                              {
        (yyval.string) = newTemp();
        // For AND, both conditions must be true
        insererQuadComplet("OR", (yyvsp[-2].string), (yyvsp[0].string), (yyval.string));  // Using OR since we're testing for falseness
    }
#line 2162 "syn.tab.c"
    break;

  case 59: /* condition: condition OR condition  */
#line 826 "syn.y"
                             {
        (yyval.string) = newTemp();
        insererQuadComplet("AND", (yyvsp[-2].string), (yyvsp[0].string), (yyval.string));  // Using AND since we're testing for falseness
    }
#line 2171 "syn.tab.c"
    break;

  case 60: /* condition: NOT condition  */
#line 830 "syn.y"
                    {
        (yyval.string) = (yyvsp[0].string);  // Simply pass through, BZ will handle the negation
    }
#line 2179 "syn.tab.c"
    break;

  case 61: /* condition: '(' condition ')'  */
#line 833 "syn.y"
                        {
        (yyval.string) = (yyvsp[-1].string);
    }
#line 2187 "syn.tab.c"
    break;

  case 62: /* comparison_op: EQ  */
#line 839 "syn.y"
         { (yyval.string) = strdup("=="); }
#line 2193 "syn.tab.c"
    break;

  case 63: /* comparison_op: NEQ  */
#line 840 "syn.y"
          { (yyval.string) = strdup("!="); }
#line 2199 "syn.tab.c"
    break;

  case 64: /* comparison_op: LT  */
#line 841 "syn.y"
         { (yyval.string) = strdup("<"); }
#line 2205 "syn.tab.c"
    break;

  case 65: /* comparison_op: GT  */
#line 842 "syn.y"
         { (yyval.string) = strdup(">"); }
#line 2211 "syn.tab.c"
    break;

  case 66: /* comparison_op: LTE  */
#line 843 "syn.y"
          { (yyval.string) = strdup("<="); }
#line 2217 "syn.tab.c"
    break;

  case 67: /* comparison_op: GTE  */
#line 844 "syn.y"
          { (yyval.string) = strdup(">="); }
#line 2223 "syn.tab.c"
    break;


#line 2227 "syn.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 847 "syn.y"


int yyerror(char *msg) {
    printf("Erreur syntaxique ligne %d, colonne %d : %s\n", yylineno, column, msg);
    return 0;
}

int main() {
    initialiserTS();   // Ensure this function is defined in symbole.h or elsewhere.
    printf("Début de l'analyse...\n");
    yyparse();
    return 0;
}
