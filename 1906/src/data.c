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

#include "data.h"

char			ProfileName[128];

int				EngineTurn;												/**	Color que tiene el programa (BLANCO o NEGRO). */
int				EngineConfig;											/**	configuración del motor. */
int				EngineState;											/**	Estado del motor. */
int				SearchState;											/**	Estado de la búsqueda. */

BOOK			Book1;
BOOK			Book2;
int				OutOfBook;

BOOK_MOVE_LEARN	BookMoveLearn[128];
int				MoveLearnSize;

char			Command[256];											/**	Para leer la información del GUI y saber que comando llega. */
char			Line[256];												/**	Para leer la información del GUI y saber que comando llega. */

int				Turn;													/**	Lado que mueve (BLANCO o NEGRO). */
int				Casttle;												/**	Posibilidad para el enroque. */
int				EnPassant;												/**	Posibilidad para comer al paso. */
int				Rule50;													/**	Contador para la regla de los 50 movimientos. */

int				MaxDepth;												/**	Nivel de búsqueda en el árbol. */
int				DepthNull;												/**	Profundidad para el movimiento nulo. */
int				Li;														/**	Profundidad desde la cual iniciamos. */
int				Ply;													/**	Nivel de profundidad que estamos en el alpha_beta. */
int				Nodes;													/**	Contador de todas las visitas a nodos. */
int				Cknodes;												/**	Indica cada que numero de nodos se comprobara el tiempo. */

int				MaxTime;												/**	Máximo de tiempo disponible para un movimiento en milisegundos. */
int				Mps;													/**	Número de jugadas a realizar para cumplir el control de tiempo. */
int				Base;													/**	Tiempo base en minutos para cumplir el control de tiempo. */
int				Inc;													/**	Tiempo de incremento para el reloj en segundos después de jugar. */
int				NextTimeControl;										/**	Número de movimientos que quedan para el control de tiempo. */
int				TimeLeft;												/**	Momento en el que empezamos un movimiento. */
int				TimeLimit;												/**	No podremos exceder este límite de tiempo nunca durante una búsqueda. */
int				NoNewMov;												/**	No empezar a buscar un nuevo movimiento si ya hemos sobrepasado un tiempo. */
int				NoNewIteration;											/**	No empezar un nuevo nivel de profundidad si no disponemos de suficiente tiempo. */
int				TimeTotal;												/**	Tiempo total consumido. */

int				PiecesNumber[33];										/**	Numero de cada tipo de pieza. */
int				PawnMat[4];												/**	Material acumulado por los peones. */
int				PieceMat[4];											/**	Material acumulado por las piezas(excepto peones). */
int				OldPieceMat[4];											/**	Anterior suma del material acumulado por ambos bandos(incluye peones). */
int				OldPawnMat[4];											/**	Anterior suma del material acumulado por ambos bandos(solo peones). */
int				MatSum;													/**	Suma del material acumulado por ambos bandos(incluye peones). */

HIST_T			Moves[HIST_STACK];										/**	Donde se almacena las jugadas que se van produciendo. */
int				Nmove;													/**	Número de movimientos en la partida (blancos + negros). */
int				NmoveInit;												/**	Número de movimientos en la partida (blancos + negros). */

HIST_T			MoveStack[GEN_STACK];									/**	Memoria para los movimientos generados por el generador de movimientos. */
HIST_T*			FirstMove[MAX_DEPHT_1];

int				Killer1[MAX_DEPHT_1];									/**	Guarda los movimientos que producen un corte. */
int				Killer2[MAX_DEPHT_1];									/** Guarda los movimientos que producen un corte. */
int				MateKiller[MAX_DEPHT_1];								/**	Movimiento que amenaza mate. */

int				CasttleMask[256];										/**	Si se mueve cualquier pieza que no sea el rey o torre estarán intactas las posibilidades de enroque(15). */

MOVE			PV[MAX_DEPHT_1][MAX_DEPHT_1];							/**	Variante principal,ver libro "How Computers Play Chess" by Levy and Newborn. */
int				PVLenght[MAX_DEPHT_1];									/**	Cuantos movimientos tiene la variante principal. */

PIECE			E					= {EMPTY,0};						/**	Pieza a la que se hace referencia cuando la casilla del tablero esta vacía. */
PIECE			N					= {0,0};							/**	Pieza a la que se hace referencia cuando la casilla esta fuera del tablero. */

PIECE*			Board[256]			= {									/**	Posición actual. */
	&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,
	&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,
	&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,
	&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,
	&N,&N,&N,&N,&E,&E,&E,&E,&E,&E,&E,&E,&N,&N,&N,&N,
	&N,&N,&N,&N,&E,&E,&E,&E,&E,&E,&E,&E,&N,&N,&N,&N,
	&N,&N,&N,&N,&E,&E,&E,&E,&E,&E,&E,&E,&N,&N,&N,&N,
	&N,&N,&N,&N,&E,&E,&E,&E,&E,&E,&E,&E,&N,&N,&N,&N,
	&N,&N,&N,&N,&E,&E,&E,&E,&E,&E,&E,&E,&N,&N,&N,&N,
	&N,&N,&N,&N,&E,&E,&E,&E,&E,&E,&E,&E,&N,&N,&N,&N,
	&N,&N,&N,&N,&E,&E,&E,&E,&E,&E,&E,&E,&N,&N,&N,&N,
	&N,&N,&N,&N,&E,&E,&E,&E,&E,&E,&E,&E,&N,&N,&N,&N,
	&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,
	&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,
	&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,
	&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N,&N
};

PIECE			BlackPieceList[32]	= {
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
};
PIECE			WhitePieceList[32]	= {
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
};
PIECE			BlackPawnList[16]	= {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
PIECE			WhitePawnList[16]	= {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};

PIECE*			KingPos[4]			= {&N, BlackPieceList + KING, WhitePieceList + KING, &N};
PIECE*			FirstPawn[4]		= {&N, BlackPawnList + 14, WhitePawnList + 14, &N};
PIECE*			FirstPiece[4]		= {&N, BlackPieceList + KING, WhitePieceList + KING, &N};

PIECE			CaptureStack[64];
PIECE*			LastCapture;

char			PiecesChar[33]		= "     pP  nN  bB  rR  qQ  kK      ";	/**	Iniciales de las piezas en Ingles. */
char			PromotionChar[33]	= "         nn  bb  rr  qq          ";	/**	Iniciales de las promociones en Ingles. */

unsigned		BlackHistory[65536];
unsigned		WhiteHistory[65536];
unsigned*		History[4]			= {BlackHistory, BlackHistory, WhiteHistory, WhiteHistory};						/**	Utilizado para la ordenación movimientos. */
unsigned		MaxHistory[4];											/**	Se limita la historia para que no sobrepase el valor de otros movimientos. */

int				KingAttackTable[512]	= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,16,15,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,-15,-16,-17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
int				QueenAttackTable[512]	= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,17,0,0,0,0,0,0,16,0,0,0,0,0,0,15,0,0,17,0,0,0,0,0,16,0,0,0,0,0,15,0,0,0,0,17,0,0,0,0,16,0,0,0,0,15,0,0,0,0,0,0,17,0,0,0,16,0,0,0,15,
	0,0,0,0,0,0,0,0,17,0,0,16,0,0,15,0,0,0,0,0,0,0,0,0,0,17,0,16,0,15,0,0,0,0,0,0,0,0,0,0,0,0,17,16,15,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,-1,-1,-1,-1,
	-1,-1,-1,0,0,0,0,0,0,0,-15,-16,-17,0,0,0,0,0,0,0,0,0,0,0,0,-15,0,-16,0,-17,0,0,0,0,0,0,0,0,0,0,-15,0,0,-16,0,0,-17,0,0,0,0,0,0,0,0,-15,0,0,0,-16,0,0,0,-17,
	0,0,0,0,0,0,-15,0,0,0,0,-16,0,0,0,0,-17,0,0,0,0,-15,0,0,0,0,0,-16,0,0,0,0,0,-17,0,0,-15,0,0,0,0,0,0,-16,0,0,0,0,0,0,-17,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
int				RookAttackTable[512]	= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,-1,-1,-1,-1,
	-1,-1,-1,0,0,0,0,0,0,0,0,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-16,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
int				BishopAttackTable[512]	= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,17,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,17,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,17,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,17,0,0,0,0,0,0,0,15,
	0,0,0,0,0,0,0,0,17,0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,17,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,0,17,0,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,-15,0,-17,0,0,0,0,0,0,0,0,0,0,0,0,-15,0,0,0,-17,0,0,0,0,0,0,0,0,0,0,-15,0,0,0,0,0,-17,0,0,0,0,0,0,0,0,-15,0,0,0,0,0,0,0,-17,
	0,0,0,0,0,0,-15,0,0,0,0,0,0,0,0,0,-17,0,0,0,0,-15,0,0,0,0,0,0,0,0,0,0,0,-17,0,0,-15,0,0,0,0,0,0,0,0,0,0,0,0,0,-17,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
int				KnightAttackTable[512]	= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,0,31,0,0,0,0,0,0,0,0,0,0,0,0,18,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,-14,0,0,0,-18,0,0,0,0,0,0,0,0,0,0,0,0,-31,0,-33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
int				EmptyTable[512]	= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
/**	Relación que existe entre 2 casillas del tablero según el tipo de pieza. */
int*			AttackTable[33]			= {
	EmptyTable,EmptyTable,EmptyTable,EmptyTable,EmptyTable,EmptyTable,EmptyTable,EmptyTable,EmptyTable,
	KnightAttackTable,KnightAttackTable,EmptyTable,EmptyTable,
	BishopAttackTable,BishopAttackTable,EmptyTable,EmptyTable,
	RookAttackTable,RookAttackTable,EmptyTable,EmptyTable,
	QueenAttackTable,QueenAttackTable,EmptyTable,EmptyTable,
	KingAttackTable,KingAttackTable,EmptyTable,EmptyTable,
	EmptyTable,EmptyTable,EmptyTable,EmptyTable,
};
int				ChebyshevDistance[512]	= {								/**	Utilizado para obtener la distancia entre dos casilla. */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,7,6,6,6,6,6,6,6,6,6,6,6,6,6,7,0,7,6,5,5,5,5,5,5,5,5,5,5,5,6,7,0,7,6,5,4,4,4,4,4,4,4,4,4,
	5,6,7,0,7,6,5,4,3,3,3,3,3,3,3,4,5,6,7,0,7,6,5,4,3,2,2,2,2,2,3,4,5,6,7,0,7,6,5,4,3,2,1,1,1,2,3,4,5,6,7,0,7,6,5,4,3,2,1,0,1,2,3,4,
	5,6,7,0,7,6,5,4,3,2,1,1,1,2,3,4,5,6,7,0,7,6,5,4,3,2,2,2,2,2,3,4,5,6,7,0,7,6,5,4,3,3,3,3,3,3,3,4,5,6,7,0,7,6,5,4,4,4,4,4,4,4,4,4,
	5,6,7,0,7,6,5,5,5,5,5,5,5,5,5,5,5,6,7,0,7,6,6,6,6,6,6,6,6,6,6,6,6,6,7,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

int				TTsize;
int				ETsize;
uint16_t		HashAge;
uint64_t		Hash;													/**	Llave hash para la actual posición. */
uint32_t		HashMat;
int				ETMask;
int				TTMask;

MOVE			NoMove = {0,0,0,0,0};									/**	Cuando no tenemos un movimiento,por ejemplo en las tablas hash. */

int				Noise;

/**	Variación que se realiza a una casilla según el tipo de pieza. Útil al generar los movimientos de las piezas,excepto peones. */
int				Offsets[64]				= {	0,-33,-31,-18,-14,14,18,31,33,0,33,31,18,14,-14,-18,-31,-33,0,-17,-15,15,17,0,17,15,-15,-17,0,-16,16,-1,1,0,16,-16,1,-1,0,-17,-16,-15,15,16,17,-1,1,0,17,16,15,-15,-16,-17,1,-1,0,0,0,0,0,0,0,0};
int*			Offset[33]				= {	Offsets,Offsets,Offsets,Offsets,Offsets,Offsets,Offsets,Offsets,Offsets,Offsets+1,Offsets+10,Offsets,Offsets,Offsets+19,Offsets+24,Offsets,Offsets,Offsets+29,Offsets+34,Offsets,Offsets,Offsets+39,
											Offsets+48,Offsets,Offsets,Offsets+39,Offsets+48,Offsets,Offsets,Offsets,Offsets,Offsets,Offsets};

int				PieceValue2[33]			= {	0,0,0,0,0,PAWN_VALUE,PAWN_VALUE,0,0,KNIGHT_VALUE,KNIGHT_VALUE,0,0,BISHOP_VALUE,BISHOP_VALUE,0,0,
													 ROOK_VALUE,ROOK_VALUE,0,0,QUEEN_VALUE,QUEEN_VALUE,0,0,KING_VALUE,KING_VALUE,0,0,0,0};	/**	Valor real de las piezas según su tipo. */

/**	Casillas legales del tablero. */
int				Square64To256[64]		= {
	A8,B8,C8,D8,E8,F8,G8,H8,A7,B7,C7,D7,E7,F7,G7,H7,A6,B6,C6,D6,E6,F6,G6,H6,A5,B5,C5,D5,E5,F5,G5,H5,
	A4,B4,C4,D4,E4,F4,G4,H4,A3,B3,C3,D3,E3,F3,G3,H3,A2,B2,C2,D2,E2,F2,G2,H2,A1,B1,C1,D1,E1,F1,G1,H1,
};

int				Value1[MAX_DEPHT_1];

int				SquareColor[256]		= {									/** Utilizado para conocer el color de cada casilla. */
	NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
	NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
};

/**	Identifica si una pieza puede moverse en una forma en especifico. Retorna su color en caso de hacerlo. */
int				MovSlide[33]			= {0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x1,0x1, 0x0,0x0,0x1,0x1, 0x1,0x1,0x0,0x0, 0x2,0x2,0x0,0x0, 0x3,0x3,0x0,0x0, 0x0,0x0,0x1,0x1, 0x0,0x0,0x0,0x0};
int*			NoMovSlide				= MovSlide + 2;

int				ProfileTurn;

char*			SquareChar[256]			= {
	"? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ",
	"? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ",
	"? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ",
	"? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ",
	"? ","? ","? ","? ","a1","b1","c1","d1","e1","f1","g1","h1","? ","? ","? ","? ",
	"? ","? ","? ","? ","a2","b2","c2","d2","e2","f2","g2","h2","? ","? ","? ","? ",
	"? ","? ","? ","? ","a3","b3","c3","d3","e3","f3","g3","h3","? ","? ","? ","? ",
	"? ","? ","? ","? ","a4","b4","c4","d4","e4","f4","g4","h4","? ","? ","? ","? ",
	"? ","? ","? ","? ","a5","b5","c5","d5","e5","f5","g5","h5","? ","? ","? ","? ",
	"? ","? ","? ","? ","a6","b6","c6","d6","e6","f6","g6","h6","? ","? ","? ","? ",
	"? ","? ","? ","? ","a7","b7","c7","d7","e7","f7","g7","h7","? ","? ","? ","? ",
	"? ","? ","? ","? ","a8","b8","c8","d8","e8","f8","g8","h8","? ","? ","? ","? ",
	"? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ",
	"? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ",
	"? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ",
	"? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? ","? "
};

int				NearBlackKing[512]		= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
int				NearWhiteKing[512]		= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
int* 			NearKing[4]				= {EmptyTable, NearBlackKing, NearWhiteKing, EmptyTable,};

int				LeftPawnCapture[4]		= {-17,-17,15,15};
int				RightPawnCapture[4]		= {-15,-15,17,17};
int				Front[4]				= {-16,-16, 16, 16};
int				Backward[4]				= { 16, 16,-16,-16};
int     		QueenSideCasttle[4]		= {BQ, BQ, WQ, WQ};
int     		KingSideCasttle[4]		= {BK, BK, WK, WK};



///	300
