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

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include "defs.h"
#include "random.h"

extern char				ProfileName[128];

extern int				EngineTurn;
extern int				EngineConfig;
extern int				EngineState;
extern int				SearchState;

extern BOOK				Book1;
extern BOOK				Book2;
extern int				OutOfBook;

extern BOOK_MOVE_LEARN	BookMoveLearn[128];
extern int				MoveLearnSize;

extern char				Command[256];
extern char				Line[256];

extern int				Turn;
extern int				Casttle;
extern int				EnPassant;
extern int				Rule50;

extern int				MaxDepth;
extern int				DepthNull;
extern int				Li;
extern int				Ply;
extern int				Nodes;
extern int				Cknodes;

extern int				MaxTime;
extern int				Mps;
extern int				Base;
extern int				Inc;
extern int				NextTimeControl;
extern int				TimeLeft;
extern int				TimeLimit;
extern int				NoNewMov;
extern int				NoNewIteration;
extern int				TimeTotal;

extern int				PiecesNumber[33];
#define					COUNT(Piece)		(PiecesNumber[Piece])

extern HIST_T			Moves[HIST_STACK];
extern int				Nmove;
extern int				NmoveInit;

extern HIST_T			MoveStack[GEN_STACK];
extern HIST_T*			FirstMove[MAX_DEPHT_1];

extern int				Killer1[MAX_DEPHT_1];
extern int				Killer2[MAX_DEPHT_1];
extern int				MateKiller[MAX_DEPHT_1];

extern int				CasttleMask[256];

extern MOVE				PV[MAX_DEPHT_1][MAX_DEPHT_1];
extern int				PVLenght[MAX_DEPHT_1];

extern PIECE			E;
extern PIECE			N;

extern PIECE*			Board[256];

extern PIECE			BlackPieceList[32];
extern PIECE			WhitePieceList[32];
extern PIECE			BlackPawnList[16];
extern PIECE			WhitePawnList[16];
extern PIECE*			FirstPawn[4];
extern PIECE*			FirstPiece[4];
extern PIECE*			KingPos[4];

extern PIECE			CaptureStack[64];
extern PIECE*			LastCapture;

extern char				PiecesChar[33];
extern char				PromotionChar[33];

extern unsigned			BlackHistory[65536];
extern unsigned			WhiteHistory[65536];
extern unsigned*		History[4];
extern unsigned			MaxHistory[4];

extern int				Value1[MAX_DEPHT_1];

extern int				KingAttackTable[512];
extern int				QueenAttackTable[512];
extern int				RookAttackTable[512];
extern int				BishopAttackTable[512];
extern int				KnightAttackTable[512];
extern int				EmptyAttackTable[512];
extern int*				AttackTable[33];
extern int				ChebyshevDistance[512];
#define					DISTANCE(from, to)				ChebyshevDistance[0xBB + (from) - (to)]

extern int				TTsize;
extern int				ETsize;
extern int				ETMask;
extern int				TTMask;

extern uint16_t			HashAge;
extern uint64_t			Hash;
extern uint32_t			HashMat;

extern MOVE				NoMove;

extern int				Noise;

extern int* 			Offset[33];

extern int				PieceValue2[33];

extern int				MovSlide[33];
extern int*				NoMovSlide;

extern int				PawnMat[4];
extern int				PieceMat[4];
extern int				MatSum;
extern int				OldPieceMat[4];
extern int				OldPawnMat[4];

extern int				Square64To256[64];

extern int				SquareColor[256];

extern int				ProfileTurn;

extern char*			SquareChar[256];

extern int*				NearKing[4];

extern int				LeftPawnCapture[4];
extern int				RightPawnCapture[4];
extern int     			QueenSideCasttle[4];
extern int     			KingSideCasttle[4];
extern int				Front[4];
extern int				Backward[4];

#define 				HASH_MAT(Piece)				(HashKey[Piece] * PiecesNumber[Piece])
#define 				ATTACKED_BY_PAWN(Side, SQ)	(Board[(SQ) - LeftPawnCapture[Side]]->Type == (PAWN | (Side)) || Board[(SQ) - RightPawnCapture[Side]]->Type == (PAWN | (Side)))
#define					KING_SQUARE(Side)			(KingPos[Side]->Square)
#define					KING_FILE(Side)				(FILE(KingPos[Side]->Square))
#define					KING_RANK(Side)				(RANK(KingPos[Side]->Square))



#endif // DATA_H_INCLUDED

/// 150
