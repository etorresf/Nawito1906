/*
 * < Nawito is a chess engine winboard derived of Danasah507>
 * Copyright (C) <2019> <Ernesto Torres Feliciano>
 * E-mail <ernesto2@nauta.cu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define			ENGINE_NAME			"Nawito1906"								/**	Nombre del motor. anho. mes. estado de dasarrollo*/
#define			ENGINE_CONFIG_FILE	"Nawito1906.ini"
#define			ENGINE_STS_FILE		"Nawito1906.sts"
#define			ENGINE_EPD_FILE		"STS13.epd"
#define			ENGINE_DATE			"15/06/2019"								/**	Fecha en que se creo el motor. */
#define			ENGINE_PROTOCOL		"winboard"									/**	Protocolo de comunicación que utiliza el motor. */
#define			ENGINE_ARCH			"i386"										/**	Arquitectura el motor. */

#define			AUTOR_NAME			"Ernesto Torres Feliciano"					/**	Nombre del autor del motor. */
#define			AUTOR_COUNTRY		"Cuba"										/**	País de procedencia del autor del motor. */
#define			AUTOR_LOCATION		"Holguin"									/**	Localidad de procedencia del autor del motor. */

typedef	enum	BOOL_TYPES			{FALSE = 0x0, TRUE}	bool;					/**	Tipo booleano y sus valores. */
typedef	enum	COLOR_TYPE			{NONE = 0x0, BLACK, WHITE, BLACK_WHITE, EMPTY = 0x20, BLACK_EMPTY, WHITE_EMPTY}	COLOR;		/**	Colores de las piezas y turnos. */

/**	Representación de las piezas con dígitos. */
typedef	enum	PIECE_TYPES			{PAWN = 0x4,  BLACK_PAWN, WHITE_PAWN, KNIGHT = 0x8,	 BLACK_KNIGHT, WHITE_KNIGHT, BISHOP = 0xC,  BLACK_BISHOP, WHITE_BISHOP,
									ROOK  = 0x10, BLACK_ROOK, WHITE_ROOK, QUEEN	 = 0x14, BLACK_QUEEN,  WHITE_QUEEN,  KING   = 0x18, BLACK_KING,   WHITE_KING}	PIECE_TYPE;

#define	NOT_PAWN					0x18							   	/**	Para determinar si una pieza no es un peón. */
#define GET_COLOR					0x23								/**	Obtiene el color de una pieza. */

/**	Valor de las piezas en centipeones. Estos valores no se utilizan en la evaluación. */
#define	PAWN_VALUE					100
#define	KNIGHT_VALUE				300
#define	BISHOP_VALUE				300
#define	ROOK_VALUE					500
#define	QUEEN_VALUE					900
#define	KING_VALUE					1000

#define	MATE						10300								///(9 * QUEEN_VALUE + 2 * (ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE))								/** Valor equivalente a dar mate. */
#define	MATE_SCORE					(MATE - 63) 						/** Identifica evaluación de mate. */

/**	Material utilizado para identificar las fases del juego. */
#define	MIDDLE_GAME_MATERIAL		3900								/**	(QUEEN_VALUE + 2 * (ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE) + 8 * PAWN_VALUE) */
#define	END_GAME_MATERIAL			3000								/**	(4 * ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE + 4 * PAWN_VALUE) */
#define	MIDDLE_END_GAME_MATERIAL	900									/**	(QUEEN_VALUE - 2 * ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE + 4 * PAWN_VALUE) */
#define	INITIAL_MATERIAL			3100								/**	(QUEEN_VALUE + 2 * (ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE)) */

/**	Casillas del tablero. */
typedef	enum	SQUARE_TYPE			{
	A1 = 0x44,B1,C1,D1,E1,F1,G1,H1,A2 = 0x54,B2,C2,D2,E2,F2,G2,H2,A3 = 0x64,B3,C3,D3,E3,F3,G3,H3,A4 = 0x74,B4,C4,D4,E4,F4,G4,H4,
	A5 = 0x84,B5,C5,D5,E5,F5,G5,H5,A6 = 0x94,B6,C6,D6,E6,F6,G6,H6,A7 = 0xA4,B7,C7,D7,E7,F7,G7,H7,A8 = 0xB4,B8,C8,D8,E8,F8,G8,H8,
}	SQUARE;

/**	Filas y Columnas del tablero. */
typedef	enum	RANK_TYPES			{RANK_1 = 0x4, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8} RANK_TYPE;
typedef	enum	FILE_TYPES			{FILE_A = 0x4, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H} FILE_TYPE;

#define	FILE(sq)					((sq) & 0xF)						/**	Obtiene la columna de una casilla. */
#define	RANK(sq)					((sq) >> 4)							/**	Obtiene la fila de una casilla. */

/**	Tipos de movimientos. Los movimientos de promoción son los mismos tipos de piezas. */
#define	PROMOTION					0x18
#define	PROMOTION_TYPE				0x1F
#define	PAWN_MOVE					0x3
#define	NORMAL						0x20
#define	BLACK_NORMAL				0x21
#define	WHITE_NORMAL				0x22
#define	CAPTURE						0x40
#define	BLACK_CAPTURE				0x41
#define	WHITE_CAPTURE				0x42
#define	PAWN_MOVE_CAPTURE			0x43
#define	NO_USUAL					0x80
#define	EN_PASSANT_CAPTURE			0xC3
#define	PAWN_PUSH_2_SQUARE			0xA3
#define	CASTTLE						0x80
#define	CAPTURE_PROMOTION			0x58

typedef	enum	ENGINE_CONFIG_TYPE	{MAKE_PONDER = 0x4, POST = 0x8, SPARRING = 0x20, SHOW_BOARD = 0x40, SHOW_MOVES = 0x80} ENGINE_CONFIG;/** Configuración del motor. */
typedef	enum	ENGINE_STATE_TYPE	{ENGINE_SEARCHING = 0x1, ENGINE_PONDERING = 0x2, ENGINE_ANALYZING = 0x4, ENGINE_TESTING = 0x10} ENGINE_STATE;			/** Estados del motor. */
typedef	enum	SEARCH_STATE_TYPE	{ABORT_SEARCH = 0x1, STOP_SEARCH = 0x2, STOP_PONDER = 0x4, STOP_ANALYSIS = 0x8, FOLLOW_PV = 0x10} SEARCH_STATE;		/** Estados de la búsqueda. */

typedef	enum	CASTTLE_RIGHT_TYPE	{WK = 0x1, WQ = 0x2, BK = 0x4, BQ = 0x8}	CASTTLE_RIGHT;									/** Enroques largos y cortos para el blanco y negro. */

typedef	enum	TTABLES_TYPE		{HASF_NULL = 0x0, HASF_EXACT, HASF_ALPHA, HASF_BETA = 0x4, VAL_UNKNOWN = 32767}	TTABLES;	/** TTables. */

typedef	enum	RESULT_TYPE			{BLACK_WIN = 0x1, WHITE_WIN, DRAW}	RESULT;	/**	Resultados que puede tener una partida. */

#define	MAX(x, y)					((x) > (y) ? (x) : (y))
#define	MIN(x, y)					((x) < (y) ? (x) : (y))

#define	MAX_DEPHT					63									/** Máxima profundidad con la que pensar. */
#define	MAX_DEPHT_1					64									/** Máxima profundidad con la que pensar + 1. */
#define GEN_STACK					2048
#define HIST_STACK					1024

#define GET_ID(from, to)			((from) << 8 | (to))				/** Convierte un movimiento(de, a) a un entero de 16 bits. */

#define	START_POS					"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - ;"/** Posición inicial del juego. */

#define	HIGH_HISTORY_SCORE			200000000
#define	FUTILITY_PRUNNING_SCORE		180000000

#define	HASH_SCORE					2000000000
#define	FOLLOW_PV_SCORE				1000000000
#define	MATE_KILLER_SCORE			 500000000
#define	GOOD_CAPTURE_SCORE			 300000000
#define	KILLER_SCORE				 280000000
#define	KILLER1_SCORE				 270000000
#define	KILLER2_SCORE				 260000000
#define	KILLER3_SCORE				 250000000
#define	BAD_CAPTURE_SCORE			 190000000
#define	BAD_CAPTURE2_SCORE			 0xFFFFFFFF

/********************************************************************************************************
 *	Utilizada en las listas de piezas.																	*
 ********************************************************************************************************/
typedef struct	PIECE_STRUCT	{
	int			Type,											/** Tipo de pieza. */
				Square;											/** Casilla del tablero que ocupa la pieza. */
}	PIECE;

/********************************************************************************************************
 *	Utilizada en las listas de movimientos y en otras operaciones como hacer o deshacer un movimiento.	*
 ********************************************************************************************************/
typedef struct MOVE_STRUCT		{
	int			From,											/** Casilla del tablero que ocupa la pieza. */
				To;												/** Casilla del tablero a donde se moverá la pieza. */
	unsigned	Score;											/** Valor del movimiento realizado. */
	int			Type,											/** Tipo de movimiento realizado. */
				ID;												/** Identificador para realizar rápidas comparaciones entre los movimientos. */
}	MOVE;

/********************************************************************************************************
 *	Utilizada para guardar las jugadas con sus datos de la partida.										*
 ********************************************************************************************************/
typedef struct	HIST_T_STRUCT	{
	MOVE		Move;											/** Movimiento realizado. */
	int 		Casttle,										/** Posibilidad de enroque. */
				Rule50,											/** Número de movimientos sin realizar captura o movimiento de peón. */
				EnPassant;										/** Casilla de captura al paso. */
	uint64_t	Hash;											/** Número asociado a la posición. */
}	HIST_T;

/********************************************************************************************************
 *	Utilizada para no tener que analizar posiciones previamente analizadas. 40 byte.					*
 ********************************************************************************************************/
typedef struct	BUCKET_STRUCT	{
	uint64_t	Hash;											/** Número asociado a la posición. */
	int			Depth1,
				Type,
				MoveID,
				Nmove,
				Value;
}	BUCKET;

#define	NUM_BUCKETS 4
typedef struct	TTYPE_STRUCT	{
	BUCKET		Bucket[NUM_BUCKETS];
}	TTYPE;

/********************************************************************************************************
 *	Evaluación de cache: para no tener que analizar posiciones previamente analizadas.					*
 ********************************************************************************************************/
typedef struct	ETYPE_STRUCT	{
	uint64_t	Hash;											/** Número asociado a la posición. */
	int			Value;											/** Valor de la posición. */
}	ETYPE;

/** see */
typedef struct	TYPE_SEE_STRUCT	{
	int			Slide,
				Direction,
				Square;
}	TYPE_SEE;

#define			MAX_NUM_POS			0xFFFFF
#define			MAX_NUM_MOVE		20

typedef	enum	BOOK_STATES	{USE_BOOK = 8, BOOK_LEARN = 16} BOOK_STATE;/** Configuración del motor. */

/********************************************************************************************************
 *	Utilizada para guardar las jugadas con sus datos de la partida.										*
 ********************************************************************************************************/
typedef struct BOOK_MOVE_STRUCT	{
	uint16_t	Move,
				T,
				N,
				t;
}	BOOK_MOVE;

typedef struct	BOOK_MOVE_LIST_STRUCT			{
	uint64_t	Key;						/** Número asociado a la posición. Basado en Polyglot hash. */
	uint8_t		Size;

	BOOK_MOVE*	Item;
}	BOOK_MOVE_LIST;

/********************************************************************************************************
 *	Utilizada para guardar las jugadas con sus datos de la partida.										*
 ********************************************************************************************************/
typedef struct	BOOK_STRUCT		{
	uint32_t	NumKey,
				Size,
				LeftGames,
				Iteration,
				State;
	char		Name[64];
	BOOK_MOVE_LIST*	Moves;
}	BOOK;

typedef struct BOOK_MOVE_LEARN_STRUCT	{
	MOVE		Move;
	uint64_t	Key;
	uint8_t		Side;
}	BOOK_MOVE_LEARN;

#endif // DEFS_H_INCLUDED


///	170
