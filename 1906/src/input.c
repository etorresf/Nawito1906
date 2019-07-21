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

#if defined (_WIN32) || defined(_WIN64)
	#ifndef WINDOWS
		#define WINDOWS
	#endif
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <string.h>
	struct timeval tv;
	struct timezone tz;
#endif

/********************************************************************************************************
 *  Permite conocer el tipo de movimiento que llega. Pasa por referencia la posicion del ultimo caracter*
 *	analizado de ´c´.																					*
 ********************************************************************************************************/
inline int		CheckMoveType(int From, int* To, char* s, int* l){
	int			Type;
	*l			= 4;

	if ((Board[From]->Type ^ (Board[From]->Type & GET_COLOR)) == KING)			{
		if (s[0] == 'e' && (s[1] == '1' || s[1] == '8') && (s[2] == 'g' || s[2] == 'c' || s[2] == 'h' || s[2] == 'a') && (s[3] == '1' || s[3] == '8'))	{

			if (s[2] == 'h')
				*To = From + 2;

			else if (s[2] == 'a')
				*To = From - 2;

			return CASTTLE;														/** Movimiento de enroque. */
		}
	}

	if (Board[*To]->Type & EMPTY)
		Type = NORMAL;

	else Type = CAPTURE;

	if (Board[From]->Type & NOT_PAWN)
		return Type;

	if (*To == EnPassant)
		return EN_PASSANT_CAPTURE;												/** Movimiento de captura al paso. */

	if ((s[1] == '2' && s[3] == '4') || (s[1] == '7' && s[3] == '5'))
		return PAWN_PUSH_2_SQUARE;												/** Movimiento de avance de peón dos pasos. */

	Type |= Turn;

	*l = 5;
	switch (s[4]) 		{														/** Movimientos de promociones. */
		case 'q':	return	Type | QUEEN;
		case 'r':	return	Type | ROOK;
		case 'b':	return	Type | BISHOP;
		case 'n':	return	Type | KNIGHT;
		default:	*l = 4;	return	Type;
	}
}

/********************************************************************************************************
 *  Dado un movimiento(formato SAN) determina si es legal o no. Retornando el movimiento(formato nawito)*
 *	si es legal, en otro caso retorna un movimiento vacío.												*
 ********************************************************************************************************/
inline MOVE		CheckMove(char* s, int* l)		{
	int			From	= A1 + (s[0] - 'a') + 16 * (s[1] - '1'),
				To		= A1 + (s[2] - 'a') + 16 * (s[3] - '1'),
				Sum		= 0,
				Ssize	= 0,
				Type, ID;
	MOVE 		m;
	char* 		c;
	*l			= 0;
	HIST_T		*i, *End;

	if ((int)*s < 32)
		return NoMove;

	if (From < A1 || From > H8 || To < A1 || To > H8 || !(Board[From]->Type && Board[To]->Type))
		return NoMove;

	Type   	= CheckMoveType(From, &To, s, l),						/** Obtiene el tipo de movimiento. */
	ID		= GET_ID(From, To);
	m		= NoMove;
	End		= FirstMove[Ply + 1];

	for (i = FirstMove[Ply]; i != End; i++)								/** Recorre todos los movimientos para determinar si es legal. */
		if (i->Move.ID == ID && i->Move.Type == Type)	{
			if (MakeMove(&i->Move))		{
				UnMakeMove();
				m = i->Move;
			}
			break;
		}

	for (c = s + *l; *c == ' '; c++, Ssize++);							/** Consume los espacios. */

	/** Asigna la evaluacion del movimiento. */
	if (*c == '=')	{
		for (Ssize++, c++; *c == ' '; c++, Ssize++);

		for (; *c >= '0' && *c <= '9'; c++, Ssize++)
			Sum = Sum * 10 +  *c - '0';
	}

	m.Score  = Sum;
	*l		+= Ssize;

	return m;
}

/********************************************************************************************************
 *  Comprueba si un movimiento es legal.																*
 ********************************************************************************************************/
inline MOVE		CheckOneMove(char* s, int* l)	{
	MOVE		m;
	HIST_T*		Temp = FirstMove[Ply + 1];

	MoveGen();
	m = CheckMove(s, l);
	FirstMove[Ply + 1] = Temp;

	return m;
}

/********************************************************************************************************
 *  Comprueba si varios movimientos son legales. En caso de serlo se almacena en ´MoveList´.			*
 ********************************************************************************************************/
inline int		CheckMoves(char* s, MOVE* MoveList, int* l)		{
	int			ListSize= 0, i = 0;
	HIST_T*		Temp	= FirstMove[Ply + 1];
	char*		c		= s;
	MOVE		m;

	MoveGen();

	while (TRUE)	{
		while (*c == ' ' || *c == ',')			/** Consume los espacios y separador. */
			c++;

		if (*c == '\0' || *c == '\n' || *c == ';')				/** Romper si se encuentra el final. */
			break;

		m = CheckMove(c, &ListSize);
		c += ListSize;

		if (m.ID)
			MoveList[i++] = m;

		while (*c != ' ' && *c != ',' && *c != ';' && *c != '\0' && *c != '\n')
			c++;
	}

	FirstMove[Ply + 1] = Temp;
	*l = (c - s) + 1;
	return i;
}

/********************************************************************************************************
 *  Carga un archivo en formato EPD. No se reconocen todos los campos de esta notaion.					*
 ********************************************************************************************************/
inline int		LoadEPD(char* EPD, MOVE* MoveList)				{
	int		l;
	return	CheckMoves(EPD + LoadFEN(EPD), MoveList, &l);
}

/********************************************************************************************************
 *	La siguiente función permite salir de una búsqueda si llega una interrupción por parte del GUI o	*
 *	permite saber cuando tenemos que dejar de ponderar y empezar a pensar, esta función está basada en	*
 *	el programa Olithink y hay cantidad de programas que la utilizan.									*
 ********************************************************************************************************/
inline int		Bioskey(void)					{
	#ifndef WINDOWS
		fd_set readfds;
		FD_ZERO (&readfds);
		FD_SET (fileno(stdin), &readfds);
		tv.tv_sec = 0; tv.tv_usec=0;
		select(16, &readfds, 0, 0, &tv);

		return (FD_ISSET(fileno(stdin), &readfds));
	#else
		static int init = 0, pipe;
		static HANDLE inh;
		DWORD dw;

		if (!init)				{
			init = 1;
			inh = GetStdHandle(STD_INPUT_HANDLE);
			pipe = !GetConsoleMode(inh, &dw);

			if (!pipe)			{
				SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
				FlushConsoleInputBuffer(inh);
			}
		}
		if (pipe)				{
			if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
				return 1;
			return dw;
		}
		else	{
			GetNumberOfConsoleInputEvents(inh, &dw);
			return dw <= 1 ? 0 : dw;
		}
	#endif
}

/// 200
