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

MOVE*		Mmove;
HIST_T		*MHPtr;
PIECE		*Mpiece, *Mcapture;
int			Mcturn, Mto, Mfrom, *Mdelta, MoldEnPassant, MoldCasttle;
const int	PromotionScore[32]	= {0,0,0,0,0,0,0,0,0,230000000,230000000,0,0,210000000,210000000,0,0,220000000,220000000,0,0,400000000,400000000,0,0,000,000,0,0,0,0,0};
int			Rank2[4]			= {RANK_7,RANK_7,RANK_2,RANK_2};
int			Rank7[4]			= {RANK_2,RANK_2,RANK_7,RANK_7};

/********************************************************************************************************
 *	Genera los pseudos movimientos del turno que le toca jugar.											*
 ********************************************************************************************************/
inline void		MoveGen(void)			{
	Mcturn		= Turn ^ 3;
	MHPtr		= FirstMove[Ply];									/** Se pone donde empiezan los movimientos del próximo nivel. */

	if (EnPassant)				{									/** Se genera los movimientos de capturas al paso. */
		if (Board[(Mfrom = EnPassant + LeftPawnCapture[Mcturn])]->Type == (PAWN | Turn))
			PushMove(Mfrom, EnPassant, EN_PASSANT_CAPTURE);

		if (Board[(Mfrom = EnPassant + RightPawnCapture[Mcturn])]->Type == (PAWN | Turn))
			PushMove(Mfrom, EnPassant, EN_PASSANT_CAPTURE);
	}

	if (Casttle)				{									/** Se genera los movimientos de enroque. */
		Mfrom = KING_SQUARE(Turn);

		if ((Casttle & KingSideCasttle[Turn]) && (Board[Mfrom + 1]->Type & Board[Mfrom + 2]->Type & EMPTY) && !Attacked(Mcturn, Mfrom + 1))
			PushMove(Mfrom, Mfrom + 2, CASTTLE);

		if ((Casttle & QueenSideCasttle[Turn]) && (Board[Mfrom - 1]->Type & Board[Mfrom - 2]->Type & Board[Mfrom - 3]->Type & EMPTY) && !Attacked(Mcturn, Mfrom - 1))
			PushMove(Mfrom, Mfrom - 2, CASTTLE);
	}

	/** Se genera los movimientos de las piezas. */
	for (Mpiece = FirstPiece[Turn]; Mpiece->Type; Mpiece++)		{
		Mfrom = Mpiece->Square;

		if (MovSlide[Mpiece->Type])
			for (Mdelta = Offset[Mpiece->Type]; *Mdelta; Mdelta++)	{
				for (Mto = Mfrom + *Mdelta; Board[Mto]->Type & EMPTY; Mto += *Mdelta)
					PushMove(Mfrom, Mto, NORMAL);

				if (Board[Mto]->Type & Mcturn)
					PushMove(Mfrom, Mto, CAPTURE);
			}
		else for (Mdelta = Offset[Mpiece->Type]; *Mdelta; Mdelta++)
			if (Board[Mto = Mfrom + *Mdelta]->Type & EMPTY)
				PushMove(Mfrom, Mto, NORMAL);

			else if (Board[Mto]->Type & Mcturn)
				PushMove(Mfrom, Mto, CAPTURE);
	}

	/** Se genera los movimientos de peones. */
	for (Mpiece = FirstPawn[Turn]; Mpiece->Type; Mpiece++)		{
		Mfrom = Mpiece->Square;

		if (Board[Mfrom + LeftPawnCapture[Turn]]->Type & Mcturn)
			PushPawnMove(Mfrom, Mfrom + LeftPawnCapture[Turn], CAPTURE | Turn);	/** Captura a la izquierda. */

		if (Board[Mfrom + RightPawnCapture[Turn]]->Type & Mcturn)
			PushPawnMove(Mfrom, Mfrom + RightPawnCapture[Turn], CAPTURE | Turn);/** Captura a la derecha. */

		if (Board[Mfrom + Front[Turn]]->Type & EMPTY)	{						/** Avance de un paso. */
			PushPawnMove(Mfrom, Mfrom + Front[Turn], NORMAL | Turn);

			if (RANK(Mfrom) == Rank2[Turn] && (Board[Mfrom + 2 * Front[Turn]]->Type & EMPTY))
				PushMove(Mfrom, Mfrom + 2 * Front[Turn], PAWN_PUSH_2_SQUARE);	/** Avance de dos pasos. */
		}
	}

	FirstMove[Ply + 1] = MHPtr;
}

/********************************************************************************************************
 *	Genera los movimientos de captura y promociones. Básicamente una copia de Generar_Movimientos.		*
 ********************************************************************************************************/
inline void		CaptureGen(void){
	Mcturn		= Turn ^ 3;
	MHPtr		= FirstMove[Ply];												/** Se pone donde empiezan los movimientos del próximo nivel. */

	if (EnPassant)				{												/** Se genera los movimientos de capturas al paso. */
		if (Board[(Mfrom = EnPassant + LeftPawnCapture[Mcturn])]->Type == (PAWN | Turn))
			PushCapture(Mfrom, EnPassant, EN_PASSANT_CAPTURE);

		if (Board[(Mfrom = EnPassant + RightPawnCapture[Mcturn])]->Type == (PAWN | Turn))
			PushCapture(Mfrom, EnPassant, EN_PASSANT_CAPTURE);
	}

	/** Se genera las promociones y los movimientos de capturas con peon. */
	for (Mpiece = FirstPawn[Turn]; Mpiece->Type; Mpiece++)		{
		Mfrom = Mpiece->Square;

		if (Board[Mfrom + LeftPawnCapture[Turn]]->Type & Mcturn)
			PushPawnCapture(Mfrom, Mfrom + LeftPawnCapture[Turn], CAPTURE | Turn);			/** Captura a la izquierda. */

		if (Board[Mfrom + RightPawnCapture[Turn]]->Type & Mcturn)
			PushPawnCapture(Mfrom, Mfrom + RightPawnCapture[Turn], CAPTURE | Turn);			/** Captura a la derecha. */

		if (RANK(Mfrom) == Rank7[Turn] && (Board[Mfrom + Front[Turn]]->Type & EMPTY))		/** Promocionees. */
			PushCapture(Mfrom, Mfrom + Front[Turn], QUEEN | Turn);
	}

	/** Se genera los movimientos de capturas con piezas. */
	for (Mpiece = FirstPiece[Turn]; Mpiece->Type; Mpiece++)		{
		Mfrom = Mpiece->Square;

		if (MovSlide[Mpiece->Type])
			for (Mdelta = Offset[Mpiece->Type]; *Mdelta; Mdelta++)	{
				for (Mto = Mfrom + *Mdelta; Board[Mto]->Type & EMPTY; Mto += *Mdelta);

				if (Board[Mto]->Type & Mcturn)
					PushCapture(Mfrom, Mto, CAPTURE);
			}
		else for (Mdelta = Offset[Mpiece->Type]; *Mdelta; Mdelta++)
			if (Board[Mto = Mfrom + *Mdelta]->Type & Mcturn)
				PushCapture(Mfrom, Mto, CAPTURE);
	}

	FirstMove[Ply + 1] = MHPtr;
}

/********************************************************************************************************
 *	Retorna el número de movimientos legales(<= 3).														*
 ********************************************************************************************************/
inline int		LegalsGen(void)	{
	int			Cont = 0;

	HIST_T* HPtr = FirstMove[Ply + 1], *FirstPtr, *LastPtr;
	MoveGen();

	for (FirstPtr = FirstMove[Ply], LastPtr = FirstMove[Ply + 1]; (FirstPtr < LastPtr) && (Cont < 3); FirstPtr++)
		if (MakeMove(&FirstPtr->Move))	{
			Cont++;
			UnMakeMove();
		}

	FirstMove[Ply + 1] = HPtr;
	return Cont;
}

/********************************************************************************************************
 *	Funcion intermedia para guardar movimientos de peones blancos.										*
 ********************************************************************************************************/
inline void		PushPawnMove(int From, int To, int Type)		{
	if (To >= A8 || To <= H1)	{
		PushMove(From, To, Type | QUEEN);
		PushMove(From, To, Type | ROOK);
		PushMove(From, To, Type | KNIGHT);
		PushMove(From, To, Type | BISHOP);
	}
	else PushMove(From, To, Type);
}

/********************************************************************************************************
 *	Funcion intermedia para guardar movimientos de captura con los peones.								*
 ********************************************************************************************************/
inline void		PushPawnCapture(int From, int To, int Type)		{
	if (To >= A8 || To <= H1)
		PushCapture(From, To, Type | QUEEN);

	else PushCapture(From, To, Type);
}

/********************************************************************************************************
 *	Agrega un movimiento a la lista de movimientos.														*
 ********************************************************************************************************/
inline void		PushMove(int From, int To, int Type)			{
	MHPtr->Move.From	= From;
	MHPtr->Move.To		= To;
	MHPtr->Move.Type	= Type;
	MHPtr->Move.ID		= GET_ID(From, To);

	if (Type & PROMOTION)
		MHPtr->Move.Score = PromotionScore[Type & PROMOTION_TYPE];

	else if (Type & CAPTURE)	{
		if (BadCapture(From, To))
			MHPtr->Move.Score = BAD_CAPTURE_SCORE + 1000 * Board[To]->Type - 10 * Board[From]->Type;	/** Ordena por MVV/LVA. */

		MHPtr->Move.Score = GOOD_CAPTURE_SCORE + 1000 * Board[To]->Type - 10 * Board[From]->Type;		/** Ordena por MVV/LVA. */
	}
	else if (MHPtr->Move.ID == MateKiller[Ply])
		MHPtr->Move.Score = MATE_KILLER_SCORE;

	else if (MHPtr->Move.ID == Killer1[Ply])
		MHPtr->Move.Score = KILLER_SCORE;

	else if (Ply > 2 && MHPtr->Move.ID == Killer1[Ply - 2])
		MHPtr->Move.Score = KILLER1_SCORE;

	else if (MHPtr->Move.ID == Killer2[Ply])
		MHPtr->Move.Score = KILLER2_SCORE;

	else if (Ply > 2 && MHPtr->Move.ID == Killer2[Ply - 2])
		MHPtr->Move.Score = KILLER3_SCORE;

	else MHPtr->Move.Score = History[Turn][MHPtr->Move.ID];

	MHPtr++;
}

/********************************************************************************************************
 *	Agrega un movimiento de captura o promoción a dama a la lista de movimientos.						*
 ********************************************************************************************************/
inline void		PushCapture(int From, int To, int Type)			{
	MHPtr->Move.From	= From;
	MHPtr->Move.To		= To;
	MHPtr->Move.Type	= Type;
	MHPtr->Move.ID		= GET_ID(From, To);

	if (Type & PROMOTION)
		MHPtr->Move.Score = PromotionScore[BLACK_QUEEN];						/** Promoción a dama. */

	else if (BadCapture(From, To))
		MHPtr->Move.Score = BAD_CAPTURE2_SCORE;									/** Malas capturas por detrás. */

	else MHPtr->Move.Score = GOOD_CAPTURE_SCORE + 1000 * Board[To]->Type - 10 * Board[From]->Type;	/** Se ordena por MVV/LVA. */

	MHPtr++;
}

/********************************************************************************************************
 *	Se encarga de realizar un movimiento. Devuelve TRUE si el movimiento es legal, sino FALSE.			*
 ********************************************************************************************************/
inline int		MakeMove(MOVE* Move)			{
	/** Se guarda el movimiento, enroque, al_paso, regla_50 movimientos, captura y la llave hash. */
	Moves[Nmove].Move		= *Move;
	Moves[Nmove].Casttle	= MoldCasttle = Casttle;
	Mcapture				= Board[Move->To];
	Moves[Nmove].EnPassant	= MoldEnPassant = EnPassant;
	Moves[Nmove].Rule50		= Rule50;
	Moves[Nmove].Hash		= Hash;
	Mcturn					= Turn ^ 3;

	Hash ^= HashPiece[Board[Move->From]->Type][Move->From];			/** Elimina de la llave a la pieza de la casilla 'de'. */

	if (Move->Type & PAWN_MOVE_CAPTURE)
		Rule50 = 0;

	else Rule50++;

	if (Move->Type == EN_PASSANT_CAPTURE)		{					/** Borra el peón si estamos capturando al_paso. */
		Mcapture			= Board[Move->To ^ 16];
		Board[Move->To ^ 16]= &E;
	}
	else if (Move->Type == CASTTLE)		{
		if (InCheck(Turn))		{
			Hash ^= HashPiece[Board[Move->From]->Type][Move->From];

			return FALSE;
		}

		switch (Move->To)				{							/** Se mueve la torre del enroque. */
			case G1:
				Board[H1]->Square	= F1;
				Board[F1]			= Board[H1];
				Board[H1]			= &E;
				Hash			   ^= HashPiece[WHITE_ROOK][H1] ^ HashPiece[WHITE_ROOK][F1];
				break;
			case C1:
				Board[A1]->Square	= D1;
				Board[D1]			= Board[A1];
				Board[A1]			= &E;
				Hash			   ^= HashPiece[WHITE_ROOK][A1] ^ HashPiece[WHITE_ROOK][D1];
				break;
			case G8:
				Board[H8]->Square	= F8;
				Board[F8]			= Board[H8];
				Board[H8]			= &E;
				Hash			   ^= HashPiece[BLACK_ROOK][H8] ^ HashPiece[BLACK_ROOK][F8];
				break;
			case C8:
				Board[A8]->Square	= D8;
				Board[D8]			= Board[A8];
				Board[A8]			= &E;
				Hash			   ^= HashPiece[BLACK_ROOK][A8] ^ HashPiece[BLACK_ROOK][D8];
				break;
			default:;
		}
	}

	if (Move->Type & PROMOTION)		{
		/** Agrega la promocion a la lista de piezas. */
		Board[Move->To]			= --FirstPiece[Turn];
		Board[Move->To]->Type	= Move->Type & PROMOTION_TYPE;

		/** Elimina el peon de la lista de peones. */
		if (Board[Move->From] != FirstPawn[Turn])	{
			*Board[Move->From] 				= *FirstPawn[Turn];
			Board[FirstPawn[Turn]->Square]	= Board[Move->From];
		}
		FirstPawn[Turn]++;

		/** Actualiza los datos referente al numero de piezas. */
		PiecesNumber[PAWN | Turn]--;
		PiecesNumber[Move->Type & PROMOTION_TYPE]++;

		/** Actualiza los datos referente al material. */
		PawnMat[Turn]	-= PAWN_VALUE;
		PieceMat[Turn]	+= PieceValue2[Move->Type & PROMOTION_TYPE];
		MatSum			+= PieceValue2[Move->Type & PROMOTION_TYPE] - PAWN_VALUE;
	}
	else Board[Move->To] = Board[Move->From];

	Board[Move->To]->Square = Move->To;
	Board[Move->From] = &E;

	if (Move->Type & CAPTURE)		{
		LastCapture++;
		*LastCapture = *Mcapture;

		PiecesNumber[Mcapture->Type]--;
		MatSum	-= PieceValue2[Mcapture->Type];
		Hash	^= HashPiece[Mcapture->Type][Mcapture->Square];			/** Elimina la pieza capturada de la llave. */

		/** Elimina la pieza capturada de la lista de piezas o peones. */
		if (Mcapture->Type & NOT_PAWN)	{
			PieceMat[Mcturn] -= PieceValue2[Mcapture->Type];

			if (Mcapture != FirstPiece[Mcturn])	{
				*Mcapture 				= *FirstPiece[Mcturn];
				Board[Mcapture->Square]	= Mcapture;
			}
			FirstPiece[Mcturn]++;
		}
		else	{
			PawnMat[Mcturn]	-= PAWN_VALUE;

			if (Mcapture != FirstPawn[Mcturn])	{
				*Mcapture				= *FirstPawn[Mcturn];
				Board[Mcapture->Square]	= Mcapture;
			}
			FirstPawn[Mcturn]++;
		}
	}

	Casttle	&= CasttleMask[Move->From] & CasttleMask[Move->To];			/** Se actualiza los enroques después de mover el rey o una torre. */
	Hash	^=	HashPiece[Board[Move->To]->Type][Move->To] ^ HashTurn[WHITE] ^ HashCasttle[MoldCasttle] ^
				HashCasttle[Casttle] ^ HashEnPassant[MoldEnPassant];
	EnPassant = 0;

	if ((Move->Type == PAWN_PUSH_2_SQUARE) && (Board[Move->To - 1]->Type == (PAWN | Mcturn) || Board[Move->To + 1]->Type == (PAWN | Mcturn)))				{
		EnPassant = Move->To ^ 16;
		Hash	 ^= HashEnPassant[EnPassant];
	}

	Ply++;
	Nmove++;															/** Prepara para guardar la siguiente jugada. */
	Turn ^= 3;															/** Da el turno al oponente. */

	if (InCheck(Turn ^ 3))	{
		UnMakeMove();
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************************************
 *	Lo contrario a hacer un movimiento.																	*
 ********************************************************************************************************/
inline void		UnMakeMove(void){
	Turn ^= 3;
	Mcturn = Turn ^ 3;
	Nmove--;
	Ply--;

	MOVE* Move	= &Moves[Nmove].Move;
	Casttle		= Moves[Nmove].Casttle;
	EnPassant	= Moves[Nmove].EnPassant;
	Rule50		= Moves[Nmove].Rule50;
	Hash		= Moves[Nmove].Hash;

	if (Move->Type & PROMOTION)		{
		/** Agrega el peon de la lista de peones. */
		Board[Move->From]		= --FirstPawn[Turn];
		Board[Move->From]->Type	= PAWN | Turn;

		/** Actualiza los datos referente al material. */
		PawnMat[Turn]	+= PAWN_VALUE;
		PieceMat[Turn]	-= PieceValue2[Move->Type & PROMOTION_TYPE];
		MatSum			+= PAWN_VALUE - PieceValue2[Move->Type & PROMOTION_TYPE];

		/** Elimina la promoción de la lista de piezas. */
		*Board[Move->To]				= *FirstPiece[Turn];
		Board[FirstPiece[Turn]->Square] = Board[Move->To];
		FirstPiece[Turn]++;

		/** Actualiza los datos referente al numero de piezas. */
		PiecesNumber[Move->Type & PROMOTION_TYPE]--;
		PiecesNumber[PAWN | Turn]++;
	}
	else	{
		if (Move->Type == CASTTLE)										/** Movemos la torre. */
			switch (Move->To)	{
				case G1:
					Board[H1]			= Board[F1];
					Board[H1]->Square	= H1;
					Board[F1]			= &E;
					break;
				case C1:
					Board[A1]			= Board[D1];
					Board[A1]->Square	= A1;
					Board[D1]			= &E;
					break;
				case G8:
					Board[H8]			= Board[F8];
					Board[H8]->Square	= H8;
					Board[F8]			= &E;
					break;
				case C8:
					Board[A8]			= Board[D8];
					Board[A8]->Square	= A8;
					Board[D8]			= &E;
					break;
				default:;
			}

		Board[Move->From]		= Board[Move->To];
	}

	Board[Move->From]->Square	= Move->From;
	Board[Move->To] = &E;

	if (Move->Type & CAPTURE)		{
		MatSum += PieceValue2[LastCapture->Type];
		PiecesNumber[LastCapture->Type]++;

		/** Agrega la pieza capturada de la lista de piezas o peones. */
		if (LastCapture->Type & NOT_PAWN)		{
			PieceMat[Mcturn]	+= PieceValue2[LastCapture->Type];
			Board[LastCapture->Square]	 = --FirstPiece[Mcturn];
		}
		else	{
			PawnMat[Mcturn]		+= PAWN_VALUE;
			Board[LastCapture->Square]	 = --FirstPawn[Mcturn];
		}

		*Board[LastCapture->Square] = *LastCapture;
		LastCapture--;
	}
}

///	450
