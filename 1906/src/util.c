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

#include <time.h>
#include <math.h>
#include "defs.h"
#include "data.h"
#include "protos.h"

/********************************************************************************************************
 *	Inicializa algunos datos del motor.																	*
 ********************************************************************************************************/
inline void		Init(void)						{
	srand(ClockTime());
	ProfileTurn = NONE;

	FindProfile(ProfileName);
	LoadProfile(ProfileName);

	AllocHash();														/** Reserva memoria para las tablas hash. */
	EvalParamIni();

	Book1.State |= USE_BOOK;
	if (LoadBook(&Book1))
		Book1.State ^= USE_BOOK;

	Book2.State |= USE_BOOK;
	if (LoadBook(&Book2))
		Book2.State ^= USE_BOOK;

	LoadSTS();
	PrintConfig();

	printf("\n#\n#\n# \"--help\" o \"-h\" print a list of commands.\n#\n");
}

/********************************************************************************************************
 *	Se devuelve la memoria utilizada por el motor.														*
 ********************************************************************************************************/
inline void		Finalize(void)					{
	if (Book1.State & USE_BOOK)
		CLOSE_BOOK(&Book1);

	if (Book2.State & USE_BOOK)
		CLOSE_BOOK(&Book2);

	FreeHash();															/**	Se libera la memoria de las tablas hash. */
}

/********************************************************************************************************
 *	Ordena una lista de piezas.																			*
 ********************************************************************************************************/
inline void		SortPieceList(PIECE* Plist)		{
	PIECE		PieceTmp, *Piece, *Piece2;

	for (Piece = Plist; (Piece + 1)->Type; Piece++)
		for (Piece2 = Piece + 1; Piece2->Type; Piece2++)
			if (PieceValue2[Piece->Type] > PieceValue2[Piece2->Type])	{	/** Se intercambian las piezas en caso que la primera sea menor. */
				PieceTmp				= *Piece;
				*Piece					= *Piece2;
				*Piece2					=  PieceTmp;

				Board[Piece->Square]	= Piece;
				Board[Piece2->Square]	= Piece2;
			}
}

/********************************************************************************************************
 *	Vacía una lista de piezas.																			*
 ********************************************************************************************************/
inline void		ClearPieceList(PIECE* Plist)	{
	PIECE*		Piece;

	for (Piece = Plist; Piece->Type; Piece++)	{
		Board[Piece->Square]	= &E;
		*Piece					= N;
	}
}

/********************************************************************************************************
 *	Actualiza los datos correspondientes al material y al numero de piezas.								*
 ********************************************************************************************************/
inline void		MatInit(void)					{
	PIECE*		Piece;
	int			Side;
	memset(PiecesNumber, 0, sizeof(PiecesNumber));						/**	Se inicializa el numero de piezas. */

	for (Side = BLACK; Side <= WHITE; Side++)	{
		for (Piece = FirstPawn[Side]; Piece->Type; Piece++)
			PiecesNumber[Piece->Type]++;

		for (Piece = FirstPiece[Side]; Piece->Type; Piece++)
			PiecesNumber[Piece->Type]++;

		PawnMat[Side]	= PiecesNumber[PAWN | Side]		* PAWN_VALUE;
		PieceMat[Side]	= PiecesNumber[KNIGHT | Side]	* KNIGHT_VALUE	+ PiecesNumber[BISHOP | Side]	* BISHOP_VALUE +
						  PiecesNumber[ROOK | Side]		* ROOK_VALUE	+ PiecesNumber[QUEEN | Side]	* QUEEN_VALUE;
	}

	MatSum = PawnMat[BLACK] + PawnMat[WHITE] + PieceMat[BLACK] + PieceMat[WHITE];
}

/********************************************************************************************************
 *	Inicializa los datos correspondientes a la representacion del tablero.								*
 ********************************************************************************************************/
inline void		ClearBoard(void)				{
	memset(PiecesNumber,	0,	sizeof(PiecesNumber));			/**	Se inicializa el numero de piezas. */
	memset(CasttleMask,		15,	sizeof(CasttleMask));
	memset(Moves,			0,	sizeof(Moves));
	memset(BlackHistory,	0,	sizeof(BlackHistory));
	memset(WhiteHistory,	0,	sizeof(WhiteHistory));
	memset(MaxHistory,		0,	sizeof(MaxHistory));

	/**	Se inicializa la mascara del enroque. */
	CasttleMask[E1] = 12;
	CasttleMask[E8] = 3;
	CasttleMask[A1] = 13;
	CasttleMask[H1] = 14;
	CasttleMask[A8] = 7;
	CasttleMask[H8] = 11;

	/**	Se inicializan algunas variables globales. */
	MaxDepth		= MAX_DEPHT;
	PawnMat[BLACK]	= PieceMat[BLACK] = Casttle = Rule50 = EnPassant = Ply =
	PawnMat[WHITE]	= PieceMat[WHITE] = MatSum = EngineTurn = Turn = OutOfBook = NONE;
	Nmove			= NmoveInit = Li  = 1;
	HashAge 		= 1;
	MoveLearnSize   = 0;

	FirstMove[0]	= MoveStack;
	LastCapture		= CaptureStack;

	/**	Se vacían las listas de piezas. */
	ClearPieceList(FirstPawn[BLACK]);
	ClearPieceList(FirstPawn[WHITE]);
	ClearPieceList(FirstPiece[BLACK]);
	ClearPieceList(FirstPiece[WHITE]);

	FirstPiece[BLACK]	= BlackPieceList + KING + 1;
	FirstPiece[WHITE]	= WhitePieceList + KING + 1;
	FirstPawn[BLACK]	= BlackPawnList  + 15;
	FirstPawn[WHITE]	= WhitePawnList  + 15;
}

/********************************************************************************************************
 * Obtine el logaritmo(entero positivo) en base 2 de un numero(entero positivo).						*
 ********************************************************************************************************/
inline int		Get_Pow_2(int Value)			{
	int			m, M;

	if (Value < 1)
		return 1;

	if (Value > 512)
		return 512;

	for (m = 1, M = 1; M < Value; M <<= 1)
		m = M;

	if ((M + m) / 2 < Value)
		return M;
	return m;
}


///	150
