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

#include "defs.h"
#include "data.h"
#include "protos.h"

int		Oi;

/********************************************************************************************************
 *  Imprime un movimiento. Útil para solucionar errores y mostrar el pensamiento del motor.				*
 ********************************************************************************************************/
inline void		PrintMove(MOVE Move)			{
	printf("%s%s", SquareChar[Move.From], SquareChar[Move.To]);
	if (Move.Type & PROMOTION)
		printf("%c", PromotionChar[Move.Type & PROMOTION_TYPE]);
}

/********************************************************************************************************
 *  Imprime el nombre de un perfil en la cabecera del tablero.											*
 ********************************************************************************************************/
inline void		PrintLastMove(int Side, char* Info)	{
	MOVE LastMove	= Moves[Nmove - 1].Move;
	LastMove.ID		= GET_ID(LastMove.From, LastMove.To);

	printf("%s", Info);

	if (ProfileTurn & Side)
		printf("%s", ProfileName);

	else if (EngineTurn & Side)
		printf("%s", ENGINE_NAME);

	else printf("?");

	if (Turn & (Side ^ 3))		{
		if (LastMove.ID)		{
			printf("(");
			PrintMove(LastMove);
			printf(")");
		}
		else printf("(?)");
	}
}

/********************************************************************************************************
 *  Imprime el tablero. Útil para solucionar errores.                   								*
 ********************************************************************************************************/
inline void		PrintBoard(void)				{
    int			Square;

	/** Muestra los nombres de los perfiles y el ultimo movimiento. */
	PrintLastMove(WHITE, "\n\n   ");
	PrintLastMove(BLACK, "\t\t");

	/** Recorre el tablero imprimiendo las piezas. */
    for (Square = 0; Square < 64; Square++)	{
		if (FILE(Square64To256[Square]) == FILE_A)
			printf("\n   +---+---+---+---+---+---+---+---+\n %u |", RANK(Square64To256[Square]) - 3);

		printf(" %c |", PiecesChar[Board[Square64To256[Square]]->Type]);
	}

	printf("\n   +---+---+---+---+---+---+---+---+\n     a   b   c   d   e   f   g   h\n\n");

	if (EngineConfig & SHOW_MOVES)
		PrintMoves();
}

/********************************************************************************************************
 * Muestra una listas de piezas.																		*
 ********************************************************************************************************/
inline void		PrintPieceList(PIECE* Plist)	{
	PIECE*		Piece;

	for (Piece = Plist; Piece->Type; Piece++)
		if (Piece->Type ^ EMPTY)
			printf("%c%s ", PiecesChar[Piece->Type], SquareChar[Piece->Square]);

		else printf("%s", "    ");
}

/********************************************************************************************************
 * Muestra todas las listas de piezas.																	*
 ********************************************************************************************************/
inline void		PrintAllPieceList(void)			{
	PrintPieceList(FirstPiece[BLACK]);	PrintPieceList(FirstPawn[BLACK]);	printf("\n");
	PrintPieceList(FirstPiece[WHITE]);	PrintPieceList(FirstPawn[WHITE]);	printf("\n");
}

/********************************************************************************************************
 * Muestra el numero de cada pieza.																		*
 ********************************************************************************************************/
inline void		PrintPiecesNumber(void)			{
	int			PieceType;
	printf("%s", "\n\np P n N b B r R q Q k K\n");

	for (PieceType = BLACK_PAWN; PieceType <= WHITE_KING; PieceType++)
		if (PieceValue2[PieceType])
			printf("%u ", PiecesNumber[PieceType]);

    printf("%c", '\n');
}

/********************************************************************************************************
 *	Imprime la lista de movimientos realizados en la partida.               							*
 ********************************************************************************************************/
inline void		PrintMoves(void)				{
	if (NmoveInit == Nmove)
		return;

    if (!(NmoveInit & BLACK))	{
		printf(" %u.  \t\t", (Oi + 1) / 2);
		PrintMove(Moves[Oi++].Move);
    }

	printf("\n");

	/** Recorre la lista de movimientos imprimiéndolos. */
	for (Oi = NmoveInit; Oi < Nmove && Moves[Oi].Move.ID; Oi++)
		if (Oi & BLACK)		{
			printf(" %u.  ", Oi / 2 + 1);
			PrintMove(Moves[Oi].Move);
			printf("\t");
		}
		else	{
			PrintMove(Moves[Oi].Move);
			printf("\n");
		}

	if (NmoveInit < Nmove)
		printf("\n\n");
}

/********************************************************************************************************
 * Si es final de partida indica el resultado de la misma.                              				*
 ********************************************************************************************************/
inline void		PrintResult(void)				{
	Ply			= 0;
	int			Result = NONE;

	if (LegalsGen())			{                                      			/** Comprueba los movimientos legales. */
		if (RepeatPos(3))		{
			printf("1/2-1/2 {Draw by repetition}\n");							/** Tablas por repetición de 3 veces la misma posición. */
			Result = DRAW;
		}
		else if (Rule50 >= 100)	{
			printf("1/2-1/2 {Draw by 50 move rule}\n");							/** Tablas por regla de los 50 movimientos. */
			Result = DRAW;
		}
		else if (MatInsuf())	{
			printf("1/2-1/2 {Draw by insufficient material}\n");				/** Tablas por que no hay suficiente material para dar mate. */
			Result = DRAW;
		}
	}
	else if (InCheck(Turn))		{
		if (Turn & WHITE)		{
			printf("0-1 {White mates}\n");										/** Si toca mover a negras y están en jaque han perdido. */
			Result = BLACK_WIN;
		}
		else	{
			printf("1-0 {Black mates}\n");										/** Si toca mover a blancas y están en jaque han perdido. */
			Result = WHITE_WIN;
		}
	}
	else		{
		printf("1/2-1/2 {Stalemate}\n");										/** Tablas porque quien mueve está ahogado. */
		Result = DRAW;
	}

	if (Result)	{
		AddGame(&Book1, Result, EngineTurn);
		AddGame(&Book2, Result, EngineTurn);
		MoveLearnSize = 0;
	}
}

/********************************************************************************************************
 *  Muestra información sobre el movimiento seleccionado, profundidad, puntuación, tiempo empleado y    *
 *  nodos examinados.                                                 									*
 ********************************************************************************************************/
inline void		PrintPV(int Depth, int Score)	{
	if (PVLenght[Ply] < 1 || !(EngineConfig & POST))
		return;

	if (EngineState & ENGINE_PONDERING)											/** Envía profundidad, puntos, tiempo(centisegundos), nodos. */
		printf("%d %d %d %d ", Depth, -Score, (ClockTime() - TimeLeft) / 10, Nodes);

	else printf("%d %d %d %d ", Depth, Score,  (ClockTime() - TimeLeft) / 10, Nodes);

	for (Oi = 0; (Oi < PVLenght[Ply]) && PV[0][Oi].ID; Oi++, printf(" "))		/** Imprime la mejor linea. */
		PrintMove(PV[0][Oi]);

	printf("\n");		                                						/** Indica el fin de linea. */
	fflush(stdout);		                                						/** Obligado por el protocolo xboard. */
}

#define DONE	"||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define NO_DONE "                                                                                                    "

inline void		PrintProgress(float Percent)	{
	Percent = MAX(0, MIN(100, Percent));
	printf("\r[%s%s %.2f%c]", (DONE + 100 - (int)Percent), (NO_DONE + (int)Percent), Percent, '%');
}


/// 200
