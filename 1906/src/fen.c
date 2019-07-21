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

/********************************************************************************************************
 *	Carga una posición(formato FEN) de la cadena 'Buffer'.												*
 ********************************************************************************************************/
inline int		LoadFEN(char* Buffer)			{
	char*		c;
	PIECE*		PieceList;
	int			PieceTypeList[17] = {0xC,0,0,0,0,0,0,0,0,0x18,0,0,0x8,0,0x4,0x14,0x10},		/**	Nombre de pieza según la letra que se recibe. */
				PieceType, Square;

	ClearBoard();																/**	Se vacía el tablero. */

	for (c = Buffer, Square = 0; Square < 64 && *c != '\0' && *c != '\n'; c++)	/**	Recorre la cadena Buffer, rellenando el tablero. */
		if (*c != '/')		{
			if ('0' < *c && *c < '9')	{
				Square += *c - '0';												/** Se incrementa el número de casillas vacías. */
				continue;
			}

			/** Detecta que tipo de pieza está en la casilla. */
			PieceType = 0;
			if ('a' < *c && *c < 's')
				PieceType = PieceTypeList[*c - 'b'] | BLACK;

			else if ('A' < *c && *c < 'S')
				PieceType = PieceTypeList[*c - 'B'] | WHITE;

			/**	Guarda la pieza en la lista de piezas y actualiza el tablero. */
			if (PieceType)	{
				if (PieceType & NOT_PAWN)
					PieceList = --FirstPiece[PieceType & GET_COLOR];

				else PieceList = --FirstPawn[PieceType & GET_COLOR];

				PieceList->Type					= PieceType;
				PieceList->Square				= Square64To256[Square];
				Board[Square64To256[Square]]	= PieceList;
			}
			Square++;
		}

	/**	Flag turn. */
	for (; *c == ' '; c++);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c == 'w')
			Turn = WHITE;

		else if (*c == 'b')
			Turn = BLACK;

		c++;
	}

	/**	Flag casttle. */
	for (; *c == ' '; c++);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c == '-')
			c++;

		else for (; *c != ' '; c++)
			switch (*c)		{
			case 'K':	Casttle |= WK;	break;
			case 'Q':	Casttle |= WQ;	break;
			case 'k':	Casttle |= BK;	break;
			case 'q':	Casttle |= BQ;	break;
			default:;
			}
		c++;
	}

	/**	Flag en_passant. */
	for (; *c == ' '; c++);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c != '-')	{
			if (*c >= 'a' && *c <= 'h' && *(c + 1) >= '1' && *(c + 1) <= '8')
				EnPassant = A1 + (*c - 'a') + 16 * (*(c + 1) - '1');

			c += 2;
		}
		c++;
	}

	/**	Flag Halfmove clock. */
	for (; *c == ' '; c++);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c >= '0' && *c <= '9')
			for (Rule50 = 0; *c >= '0' && *c <= '9'; c++)
				Rule50 = 10 * Rule50 + (int)(*c - '0');

		else c++;
	}

	/**	Flag Fullmove number. */
	for (; *c == ' '; c++);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c >= '0' && *c <= '9')
			for (Nmove = 0; *c >= '0' && *c <= '9'; c++)
				Nmove = 10 * Nmove + (int)(*c - '0');

		NmoveInit = Nmove = MAX(1, 2 * Nmove - Turn + BLACK);
		c++;
	}
	if (Turn == BLACK && (Nmove & 1))
		NmoveInit = ++Nmove;

	/**	Ordena las listas de piezas. */
	SortPieceList(FirstPawn[BLACK]);
	SortPieceList(FirstPawn[WHITE]);
	SortPieceList(FirstPiece[BLACK]);
	SortPieceList(FirstPiece[WHITE]);

	/**	Calcula los restantes datos básicos. */
	MatInit();
	ClearHash();
	Hash = PutHash();

	/**	Por defecto: turno del oponente. */
	MaxDepth		= MAX_DEPHT;
	EngineTurn		= Turn ^ 3;
	ProfileTurn		= Turn;
	HashAge			= Nmove;

	return c - Buffer + 1;
}

/// 100
