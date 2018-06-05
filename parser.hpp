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
     TIDENTIFIER = 258,
     TINTEGER = 259,
     TLPAREN = 260,
     TRPAREN = 261,
     TLBRACE = 262,
     TRBRACE = 263,
     TCOMMA = 264,
     TEQUAL = 265,
     TIF = 266,
     TELSE = 267,
     TCEQ = 268,
     TCNE = 269,
     TCLT = 270,
     TCLE = 271,
     TCGT = 272,
     TCGE = 273,
     TPLUS = 274,
     TMINUS = 275,
     TMUL = 276,
     TDIV = 277,
     TRETURN = 278,
     TAND = 279,
     TOR = 280,
     TXOR = 281,
     TMOD = 282,
     TSHIFTR = 283,
     TSHIFTL = 284
   };
#endif
/* Tokens.  */
#define TIDENTIFIER 258
#define TINTEGER 259
#define TLPAREN 260
#define TRPAREN 261
#define TLBRACE 262
#define TRBRACE 263
#define TCOMMA 264
#define TEQUAL 265
#define TIF 266
#define TELSE 267
#define TCEQ 268
#define TCNE 269
#define TCLT 270
#define TCLE 271
#define TCGT 272
#define TCGE 273
#define TPLUS 274
#define TMINUS 275
#define TMUL 276
#define TDIV 277
#define TRETURN 278
#define TAND 279
#define TOR 280
#define TXOR 281
#define TMOD 282
#define TSHIFTR 283
#define TSHIFTL 284




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 10 "parser.y"
{
  NBlock* block;
  NStatement* stmt;
  NExpression* expr;
  NIdentifier* ident;
  NVariableDeclaration* var_decl;
  std::vector<NVariableDeclaration*>* varvec;
  std::string* string;
  int token;
}
/* Line 1529 of yacc.c.  */
#line 118 "parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

