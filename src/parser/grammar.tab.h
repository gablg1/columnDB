/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CREATE = 258,
     DB = 259,
     TBL = 260,
     COL = 261,
     MIN_T = 262,
     AVG = 263,
     ADD = 264,
     SUB = 265,
     MAX_T = 266,
     BEGIN_COMMENT = 267,
     SORTED_T = 268,
     BTREE_T = 269,
     UNSORTED_T = 270,
     PRIMARY_T = 271,
     REL_INSERT = 272,
     SELECT = 273,
     FETCH = 274,
     TUPLE = 275,
     LOAD = 276,
     SHUTDOWN = 277,
     NULL_T = 278,
     WORD = 279,
     DOTTED_WORD = 280,
     INT = 281
   };
#endif
/* Tokens.  */
#define CREATE 258
#define DB 259
#define TBL 260
#define COL 261
#define MIN_T 262
#define AVG 263
#define ADD 264
#define SUB 265
#define MAX_T 266
#define BEGIN_COMMENT 267
#define SORTED_T 268
#define BTREE_T 269
#define UNSORTED_T 270
#define PRIMARY_T 271
#define REL_INSERT 272
#define SELECT 273
#define FETCH 274
#define TUPLE 275
#define LOAD 276
#define SHUTDOWN 277
#define NULL_T 278
#define WORD 279
#define DOTTED_WORD 280
#define INT 281




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 20 "grammar.y"
{
    char *str;
    int val;
    struct list *list;
    struct MaybeInt maybe_val;
    void *ptr;
}
/* Line 1529 of yacc.c.  */
#line 109 "grammar.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

