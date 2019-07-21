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
#include "eval.h"

#define		NEAR_KING(Side, SQ)					(NearKing[Side][0xBB + (KingPos[Side]->Square) - (SQ)])
#define		PAWN_ATTACK(Side, Square)			(COUNT_PAWN_ATTACK(Attack[Side][Square]))
#define		KING_ATTACK(Side, Square)			(COUNT_KING_ATTACK(Attack[Side][Square]))
#define		ROOK_ATTACK(Side, Square)			(COUNT_ROOK_ATTACK(Attack[Side][Square]))
#define		PAWN_PASSED_CONTROLLED(Side, Square)((Board[Square]->Type & BLACK_WHITE) || (Attack[(Side) ^ 3][Square] && !Attack[Side][Square]))
#define 	OpenFile(File)						(OpenFile[WHITE][File] && OpenFile[BLACK][File])

/**	Variables globales que se tiene su valor todo el tiempo. */
int			Esquare, ErelSquare, Eview, Eside, Ecside, CasttleFlag;

/**	Variables globales. */
int			AttackWeight[4], AttackCount[4], BishopColor[4], OpenFile[4][16], Attack[4][256], PawnInRank[4][16], BackwardPawn[4][16], ForwardPawn[4][16],
			PawnImp[4], ErookSQ[4], EpawnSQ[4], EknightSQ[4],
			ERank, EFile, EPawnOpened, EPawnPassed, EPawnDoubled, EPawnBackward, EMatDif, EMobility, EHashMatWhitOutPawn, *EDelta, ETmp, ETmp1, ETmp2, EdistDiff,
			AttackDif, ETo, ESupportedEndGame;
int			PawnMidGamePST[256], KnightMidGamePST[256], BishopMidGamePST[256], RookMidGamePST[256], QueenMidGamePST[256], KingMidGamePST[256],
			PawnEndGamePST[256], KnightEndGamePST[256], BishopEndGamePST[256], RookEndGamePST[256], QueenEndGamePST[256], KingEndGamePST[256],
			PawnPassedProtectedMidGame[256], PawnBackwardMidGame[256], PawnUnitedMidGame[256], PawnIsolatedMidGame[256],
			PawnPassedProtectedEndGame[256], PawnBackwardEndGame[256], PawnUnitedEndGame[256], PawnIsolatedEndGame[256],
			PawnDoubledMidGame[256], PawnCandidateMidGame[256], PawnPassedMidGame[256], PassedPawnPotential[256],
			PawnDoubledEndGame[256], PawnCandidateEndGame[256], PawnPassedEndGame[256],
			SRKingCount[256], KnightMoveImport[256], KnightOutpost[256], RookCompelling[256], QueenCompelling[256], CenterTable[256], PawnPassedUnstopped[256],
			PawnImport[256], SquareToPromotion[256];
int			EndGameScore[4], MidGameScore[4], MidEndGameScore[4];
int			EmptyPieceTable[33]			= {0,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
			EmptyFileTable[16]			= {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
			WhiteFianchetto[34]			= {0,0,0,0,0, -10,0,0,0, -10,-10,0,0, -10,10,0,0, -10,-10,0,0, -10,-10,0,0, -10,5,0,0, -10,-10,-10,-10,-10},
			*Fianchetto[4]				= {EmptyPieceTable, WhiteFianchetto + 1, WhiteFianchetto, EmptyPieceTable},
			WhitePawnPassedSoported[34]	= {0,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, -20,70,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,0},
			*PawnPassedSoported[4]		= {EmptyPieceTable,WhitePawnPassedSoported + 1,WhitePawnPassedSoported,EmptyPieceTable},
			WhiteFriendPiece[34]		= {0,0,0,0,0, 0,0,0,0, 0,2,0,0, 0,2,0,0, 0,2,0,0, 0,2,0,0, 0,2,0,0, 2,2,2,2,2},
			*FriendPiece[4]				= {EmptyPieceTable, WhiteFriendPiece + 1, WhiteFriendPiece, EmptyPieceTable},
			WhiteKingWithOutFriend[34]	= {0,0,0,0,0, 2,0,0,0, 2,1,0,0, 2,1,0,0, 2,1,0,0, 2,1,0,0, 2,1,0,0, 2,2,2,2,2},
			*KingWithOutFriend[4]		= {EmptyPieceTable, WhiteKingWithOutFriend + 1, WhiteKingWithOutFriend, EmptyPieceTable},
			WhiteKingWithEnemy[34]		= {0,0,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,1,1,1,1},
			*KingWithEnemy[4]			= {EmptyPieceTable, WhiteKingWithEnemy + 1, WhiteKingWithEnemy, EmptyPieceTable},
			WhitePawnPassedBloqued[34]	= {0,0,0,0,0, 0,0,0,0, 15,0,0,0, 10,0,0,0, 0,0,0,0, 0,0,0,0, 25,0,0,0, 0,0,0,0,0},
			*PawnPassedBloqued[4]		= {EmptyPieceTable, WhitePawnPassedBloqued + 1, WhitePawnPassedBloqued, EmptyPieceTable},
			WhiteAttackingPiece[34]		= {0,0,0,0,0, 2,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 0,0,0,0, 0,0,0,0,0},
			*AttackingPiece[4]			= {EmptyPieceTable, WhiteAttackingPiece + 1, WhiteAttackingPiece, EmptyPieceTable},
			WhiteRookSupport[34]		= {0,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,4,0,0, 0,2,0,0, 0,0,0,0, 0,0,0,0,0},
			*RookSupport[4]				= {EmptyPieceTable, WhiteRookSupport + 1, WhiteRookSupport, EmptyPieceTable},
			KingFrontZone[4][4]			= {{0,0,0,0}, {-15,-16,-17,0}, {15,16,17,0}, {0,0,0,0}},
			KingPatternZone[4][4]		= {{0,0,0,0}, {-31,-32,-33,0}, {31,32,33,0}, {0,0,0,0}},
			KingNoFrontZone[4][8]		= {{0,0,0,0,0,0,0,0}, {17,16,15,1,-1,0,0,0}, {-17,-16,-15,-1,1,0,0,0}, {0,0,0,0,0,0,0,0}},
			AttackerType[32]			= {0,0,0,0,4096,0,0,0,256,0,0,0,256,0,0,0,32,0,0,0,2,0,0,0,1,0,0,0,0,0,0,0},
			IsPawn[33]					= {0,0,0,0,0, 1,1,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
			OneColor[4]					= {0, 1, 1, 0},
			LazyScore[128]				= {
				0,990,955,920,885,850,815,780,745,710,675,640,605,570,535,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,
				500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,		///	final
				478,456,435,413,391,369,347,325,303,281,260,238,216,194,183,172,		///	mediojuego - final	(OK)
				150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,///	mediojuego
				150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,
			};

PIECE		*EPiece;
int			BlackPawnShieldAdvanced[16]	= {0,0,0,0, 30,24,24,24,24,12,0,0, 0,0,0,0},
			WhitePawnShieldAdvanced[16]	= {0,0,0,0, 0, 0,12,24,24,24,24,30, 0,0,0,0},
			*PawnShieldAdvanced[4]		= {EmptyFileTable,BlackPawnShieldAdvanced,WhitePawnShieldAdvanced,EmptyFileTable},
			BlackPawnShieldAttacked[16]	= {0,0,0,0, 0, 0, 0, 0, 6,12, 0,18, 0,0,0,0},
			WhitePawnShieldAttacked[16]	= {0,0,0,0,18, 0,12, 6, 0, 0, 0, 0, 0,0,0,0},
			*PawnShieldAttacked[4]		= {EmptyFileTable,BlackPawnShieldAttacked,WhitePawnShieldAttacked,EmptyFileTable};
int			CenterSquare[32]			= {B3,B4,B5,B6,C3,C4,C5,C6,D3,D4,D5,D6,E3,E4,E5,E6,F3,F4,F5,F6,G3,G4,G5,G6,0,0,0,0,0,0,0,0};
int			RookFileOffset[4]			= {16, -16, 0,0},
			RookRankOffset[4]			= {1, -1, 0,0};

#define BPF(f)  (PawnShieldAdvanced[Eside][BackwardPawn[Eside][f]] + PawnShieldAttacked[Eside][ForwardPawn[Ecside][f]])
/********************************************************************************************************
 *	Evaluación completa de la posición con salida mediante lazy eval. Dividida en 3 partes mediojuego,	*
 *	final y mediojuego-final.																			*
 ********************************************************************************************************/
inline int		Eval(int Alpha, int Beta)		{
	EMatDif		= PieceMat[Turn] + PawnMat[Turn] - PieceMat[Turn ^ 3] - PawnMat[Turn ^ 3];

	/********************************************************************************************************
	 *	Si se tiene mucha ventaja o desventaja no hace falta seguir evaluando, cuanto menor es el valor de	*
	 *	LazyScore más rápido sera el programa, con riesgo de dejar algún movimiento súper bueno. 			*
	 ********************************************************************************************************/
	if (EMatDif - LazyScore[MatSum >> 6] >= Beta)
		return Beta;

	if (EMatDif + LazyScore[MatSum >> 6] <= Alpha)
		return Alpha;

	if ((ETmp = ProbeEval()) != -MATE)								/** Comprueba si la posición esta evaluada. */
		return ETmp;

	EvalInit();

	MidEndGameScore[BLACK]	+= RookAdjust[PawnImp[BLACK]] * COUNT(BLACK_ROOK) + KnightAdjust[PawnImp[BLACK]] * COUNT(BLACK_KNIGHT) - KnightPair[COUNT(BLACK_KNIGHT)] - RookPair[COUNT(BLACK_ROOK)] - PawnAdjust[PawnImp[BLACK]];
	MidEndGameScore[WHITE]	+= RookAdjust[PawnImp[WHITE]] * COUNT(WHITE_ROOK) + KnightAdjust[PawnImp[WHITE]] * COUNT(WHITE_KNIGHT) - KnightPair[COUNT(WHITE_KNIGHT)] - RookPair[COUNT(WHITE_ROOK)] - PawnAdjust[PawnImp[WHITE]];
	MidEndGameScore[Turn]	+= 10;

	if (PieceMat[Turn] > PieceMat[Turn ^ 3] + 20)	{
		if(OldPieceMat[Turn] > PieceMat[Turn])
			MidEndGameScore[Turn] += (OldPieceMat[Turn] - PieceMat[Turn]) / PieceSimp[MatSum >> 6];

		if (OldPawnMat[Turn] > PawnMat[Turn])
			MidEndGameScore[Turn] += PawnSimp[MatSum >> 6];
	}

	if (MatSum > MIDDLE_GAME_MATERIAL)
		return StoreEval(EvalMiddleGame());

	if (MatSum < END_GAME_MATERIAL)
		return StoreEval(EvalEndGame());

	return StoreEval(EvalMiddleEndGame());
}

/********************************************************************************************************
 *	Evaluación correspondiente al mediojuego. Tiene en cuenta los siguientes parámetros:				*
 ********************************************************************************************************/
inline int		EvalMiddleGame(void)			{
	for (Eside = BLACK, Eview = 0xF0; Eside <= WHITE; Eside++, Eview ^= 0xF0)	{
		Ecside = Eside ^ 3;

		for (EPiece = FirstPiece[Eside]; EPiece->Type; EPiece++)				{
			Esquare			= EPiece->Square;
			ErelSquare		= Esquare ^ Eview;

			switch (EPiece->Type)	{
				case BLACK_KNIGHT:	case WHITE_KNIGHT:
					KnightMobility();
					MidGameScore[Eside]	+= KnightMidGamePST[ErelSquare]	+ KnightMidGameMob[EMobility]	- KnightTrapped();

					if (IsKnightOutpost())
						MidGameScore[Eside]	+= PAWN_ATTACK(Eside, Esquare) * KnightOutpost[ErelSquare];
					break;
				case BLACK_BISHOP:	case WHITE_BISHOP:
					BishopMobility();
					MidGameScore[Eside]	+= BishopMidGamePST[ErelSquare]	+ BishopMidGameMob[EMobility]	- BishopTrapped();
					break;
				case BLACK_ROOK:	case WHITE_ROOK:
					RookMobility();
					MidGameScore[Eside]	+= RookMidGamePST[ErelSquare]	+ RookMidGameMob[EMobility]		- RookTrapped();
					break;
				case BLACK_QUEEN:	case WHITE_QUEEN:
					QueenMobility();
					MidGameScore[Eside]	+= QueenMidGamePST[ErelSquare]	+ QueenMidGameMob[EMobility];

					if (ErelSquare != D1 && (Board[B1 ^ Eview]->Type == (KNIGHT | Eside) || Board[G1 ^ Eview]->Type == (KNIGHT | Eside) ||
					Board[C1 ^ Eview]->Type == (BISHOP | Eside) || Board[F1 ^ Eview]->Type == (BISHOP | Eside)))
						MidGameScore[Eside]	-= 20;
					break;
				case BLACK_KING: case WHITE_KING:
					MidGameScore[Eside]	+= KingMidGamePST[ErelSquare];
					break;
				default:;
			}
		}
	}

	for (Eside = BLACK, Eview = 0xF0; Eside <= WHITE; Eside++, Eview ^= 0xF0)	{
		Ecside = Eside ^ 3;

		BishopPair();
		KingSafety();
		KingShield();
		BlockedPawns();

		for (EPiece = FirstPawn[Eside]; EPiece->Type; EPiece++)	{
			Esquare 		= EPiece->Square;
			ErelSquare		= Esquare ^ Eview;
			ERank			= RANK(Esquare);
			EFile			= FILE(Esquare);
			EPawnBackward	= 0;

			if (Eside & BLACK)	{
				EPawnDoubled	= BackwardPawn[Eside][EFile] > ERank;
				EPawnOpened		= BackwardPawn[Ecside][EFile] > ERank;
				EPawnPassed		= EPawnOpened && (BackwardPawn[Ecside][EFile - 1] >= ERank && BackwardPawn[Ecside][EFile + 1] >= ERank);
			}
			else {
				EPawnDoubled	= BackwardPawn[Eside][EFile] < ERank;
				EPawnOpened		= BackwardPawn[Ecside][EFile] < ERank;
				EPawnPassed		= EPawnOpened && (BackwardPawn[Ecside][EFile - 1] <= ERank && BackwardPawn[Ecside][EFile + 1] <= ERank);
			}

			MidGameScore[Eside]	+= PawnMidGamePST[ErelSquare];

			if (OpenFile[Eside][EFile - 1] && OpenFile[Eside][EFile + 1])
				MidGameScore[Eside]	-= PawnIsolatedMidGame[ErelSquare] * (EPawnOpened + 1);

			else if (Board[Esquare - 1]->Square == (PAWN | Eside))
				MidGameScore[Eside]	+= PawnUnitedMidGame[ErelSquare];

			else if ((EPawnBackward = PawnBackward()))
				MidGameScore[Eside]	-= PawnBackwardMidGame[ErelSquare] * (EPawnOpened + 1);

			if (EPawnDoubled)
				MidGameScore[Eside]	-= PawnDoubledMidGame[ErelSquare];

			if (EPawnOpened)	{
				if (EPawnPassed)				{
					MidGameScore[Eside]	+= PawnPassedMidGame[ErelSquare];

					if (PAWN_ATTACK(Eside, Esquare + Front[Eside]))
						MidGameScore[Eside]	+= PawnPassedProtectedMidGame[ErelSquare];

					else if (PAWN_ATTACK(Eside, Esquare))
						MidGameScore[Eside]	+= PawnPassedProtectedMidGame[ErelSquare] / 2;

					else if (PAWN_ATTACK(Eside, Esquare + Backward[Eside]))
						MidGameScore[Eside]	+= PawnPassedProtectedMidGame[ErelSquare] / 3;
				}
				else if (!EPawnBackward && PawnCandidate())
					MidGameScore[Eside]	+= PawnCandidateMidGame[ErelSquare];
			}
		}
	}

	ExchangeIni();
	MiddleGameExchange();
	EvalCenterControl();

	return MidGameScore[Turn] - MidGameScore[Turn ^ 3] + MidEndGameScore[Turn] - MidEndGameScore[Turn ^ 3];
}

/********************************************************************************************************
 *	Evaluación correspondiente al final. Tiene en cuenta los siguientes parámetros:						*
 ********************************************************************************************************/
inline int		EvalEndGame(void)				{
	for (Eside = BLACK, Eview = 0xF0; Eside <= WHITE; Eside++, Eview ^= 0xF0)	{
		Ecside = Eside ^ 3;

		for (EPiece = FirstPiece[Eside]; EPiece->Type; EPiece++){
			Esquare			= EPiece->Square;
			ErelSquare		= Esquare ^ Eview;

			switch (EPiece->Type)	{
				case BLACK_KNIGHT:	case WHITE_KNIGHT:
					EknightSQ[Eside] = Esquare;
					KnightMobility();
					EndGameScore[Eside]	+= KnightEndGamePST[ErelSquare] + KnightEndGameMob[EMobility]	- KnightTrapped();
					break;
				case BLACK_BISHOP:	case WHITE_BISHOP:
					BishopMobility();
					EndGameScore[Eside]	+= BishopEndGamePST[ErelSquare] + BishopEndGameMob[EMobility]	- BishopTrapped();
					break;
				case BLACK_ROOK:	case WHITE_ROOK:
					ErookSQ[Eside] = Esquare;
					RookMobility();
					EndGameScore[Eside]	+= RookEndGamePST[ErelSquare]	+ RookEndGameMob[EMobility]		- RookTrapped();
					break;
				case BLACK_QUEEN:	case WHITE_QUEEN:
					QueenMobility();
					EndGameScore[Eside]	+= QueenEndGamePST[ErelSquare]	+ QueenEndGameMob[EMobility];
					break;
				case BLACK_KING: case WHITE_KING:
					EndGameScore[Eside]	+= KingEndGamePST[ErelSquare];
					break;
				default:;
			}
		}
	}

	for (Eside = BLACK, Eview = 0xF0; Eside <= WHITE; Eside++, Eview ^= 0xF0)	{
		Ecside = Eside ^ 3;

		BadBishop();
		BishopPair();

		EpawnSQ[Eside]	= FirstPawn[Eside]->Square;

		for (EPiece = FirstPawn[Eside]; EPiece->Type; EPiece++)	{
			Esquare			= EPiece->Square;
			ErelSquare		= Esquare ^ Eview;
			ERank	 		= RANK(Esquare);
			EFile			= FILE(Esquare);
			EPawnBackward	= 0;

			if (Eside & BLACK)	{
				EPawnDoubled	= BackwardPawn[Eside][EFile] > ERank;
				EPawnOpened		= BackwardPawn[Ecside][EFile] > ERank;
				EPawnPassed		= EPawnOpened && (BackwardPawn[Ecside][EFile - 1] >= ERank && BackwardPawn[Ecside][EFile + 1] >= ERank);
			}
			else {
				EPawnDoubled	= BackwardPawn[Eside][EFile] < ERank;
				EPawnOpened		= BackwardPawn[Ecside][EFile] < ERank;
				EPawnPassed		= EPawnOpened && (BackwardPawn[Ecside][EFile - 1] <= ERank && BackwardPawn[Ecside][EFile + 1] <= ERank);
			}

			EndGameScore[Eside]	+= PawnEndGamePST[ErelSquare];

			if (OpenFile[Eside][EFile - 1] && OpenFile[Eside][EFile + 1])
				EndGameScore[Eside] -= PawnIsolatedEndGame[ErelSquare];

			else if (Board[Esquare - 1]->Square == (PAWN | Eside))
				EndGameScore[Eside] += PawnUnitedEndGame[ErelSquare];

			else if ((EPawnBackward = PawnBackward()))
				EndGameScore[Eside] -= PawnBackwardEndGame[ErelSquare];

			if (EPawnDoubled)
				EndGameScore[Eside]	-= PawnDoubledEndGame[ErelSquare];

			if (EPawnOpened)	{
				if (EPawnPassed)				{
					EndGameScore[Eside] += PawnPassedEndGame[ErelSquare] * (12 + DISTANCE(Esquare + Front[Eside], KING_SQUARE(Ecside)) * 2 - DISTANCE(Esquare + Front[Eside], KING_SQUARE(Eside)) / 2) + 20;

					if (PAWN_ATTACK(Eside, Esquare + Front[Eside]))
						EndGameScore[Eside]	+= PawnPassedProtectedEndGame[ErelSquare];

					else if (PAWN_ATTACK(Eside, Esquare))
						EndGameScore[Eside]	+= PawnPassedProtectedEndGame[ErelSquare] / 2;

					else if (PAWN_ATTACK(Eside, Esquare + Backward[Eside]))
						EndGameScore[Eside]	+= PawnPassedProtectedEndGame[ErelSquare] / 3;

					if (PAWN_PASSED_CONTROLLED(Eside, Esquare + Front[Eside]))
						EndGameScore[Eside]	+= IsPawnPassedBloqued(Esquare + Front[Eside]);

					else		{
						EndGameScore[Eside]	+= PassedPawnPotential[ErelSquare] * 3;

						if (PAWN_PASSED_CONTROLLED(Eside, Esquare + 2 * Front[Eside]))
							EndGameScore[Eside]	+= IsPawnPassedBloqued(Esquare + 2 * Front[Eside]);

						else	{
							EndGameScore[Eside]	+= PassedPawnPotential[ErelSquare] * 2;

							if (PAWN_PASSED_CONTROLLED(Eside, Esquare + 3 * Front[Eside]))
								EndGameScore[Eside] += IsPawnPassedBloqued(Esquare + 3 * Front[Eside]);

							else EndGameScore[Eside]+= PassedPawnPotential[ErelSquare];
						}
					}

					if (ROOK_ATTACK(BLACK, Esquare) || ROOK_ATTACK(WHITE, Esquare))	{
						for (ETo = Esquare + Backward[Eside]; Board[ETo]->Type & EMPTY; ETo += Backward[Eside]);
						EndGameScore[Eside] += PawnPassedSoported[Eside][Board[ETo]->Type];
					}

					if (!PieceMat[Ecside])	{
						ETo = SquareToPromotion[Esquare] ^ Eview;

						if (DISTANCE(Esquare, ETo) < DISTANCE(KING_SQUARE(Ecside), ETo) + (Turn == Eside))
							EndGameScore[Eside] += PawnPassedUnstopped[ErelSquare];
					}
				}
				else if (!EPawnBackward && PawnCandidate())
					EndGameScore[Eside] += PawnCandidateEndGame[ErelSquare];
			}
		}
	}

	PutHashMat();
	if ((ESupportedEndGame = DrawEndGame()))
		return (EndGameScore[Turn] - EndGameScore[Turn ^ 3]) / ESupportedEndGame;

	ExchangeIni();
	EndGameExchange();

	if ((ESupportedEndGame = SupportedEndGames()))
		return 16 * (EndGameScore[Turn] - EndGameScore[Turn ^ 3] + MidEndGameScore[Turn] - MidEndGameScore[Turn ^ 3]) / ESupportedEndGame;

	return EndGameScore[Turn] - EndGameScore[Turn ^ 3] + MidEndGameScore[Turn] - MidEndGameScore[Turn ^ 3];
}

/********************************************************************************************************
 *	Evaluación correspondiente al mediojuego-final. Tiene en cuenta los siguientes parámetros:			*
 ********************************************************************************************************/
inline int		EvalMiddleEndGame(void)			{
	for (Eside = BLACK, Eview = 0xF0; Eside <= WHITE; Eside++, Eview ^= 0xF0)	{
		Ecside = Eside ^ 3;

		for (EPiece = FirstPiece[Eside]; EPiece->Type; EPiece++){
			Esquare		= EPiece->Square;
			ErelSquare	= Esquare ^ Eview;

			switch (EPiece->Type)	{
				case BLACK_KNIGHT:	case WHITE_KNIGHT:
					KnightMobility();
					MidEndGameScore[Eside]	-= KnightTrapped();
					MidGameScore[Eside] 	+= KnightMidGamePST[ErelSquare]	+ KnightMidGameMob[EMobility];
					EndGameScore[Eside] 	+= KnightEndGamePST[ErelSquare]	+ KnightEndGameMob[EMobility];

					if (IsKnightOutpost())
						MidGameScore[Eside]	+= PAWN_ATTACK(Eside, Esquare) * KnightOutpost[ErelSquare];
					break;
				case BLACK_BISHOP:	case WHITE_BISHOP:
					BishopMobility();
					MidEndGameScore[Eside]	-= BishopTrapped();
					MidGameScore[Eside]		+= BishopMidGamePST[ErelSquare]	+ BishopMidGameMob[EMobility];
					EndGameScore[Eside]		+= BishopEndGamePST[ErelSquare]	+ BishopEndGameMob[EMobility];
					break;
				case BLACK_ROOK:	case WHITE_ROOK:
					RookMobility();
					MidEndGameScore[Eside]	-= RookTrapped();
					MidGameScore[Eside]		+= RookMidGamePST[ErelSquare]	+ RookMidGameMob[EMobility];
					EndGameScore[Eside] 	+= RookEndGamePST[ErelSquare]	+ RookEndGameMob[EMobility];
					break;
				case BLACK_QUEEN:	case WHITE_QUEEN:
					QueenMobility();
					MidGameScore[Eside]		+= QueenMidGamePST[ErelSquare]	+ QueenMidGameMob[EMobility];
					EndGameScore[Eside]		+= QueenEndGamePST[ErelSquare]	+ QueenEndGameMob[EMobility];

					if (ErelSquare != D1 && (Board[B1 ^ Eview]->Type == (KNIGHT | Eside) || Board[G1 ^ Eview]->Type == (KNIGHT | Eside) ||
					Board[C1 ^ Eview]->Type == (BISHOP | Eside) || Board[F1 ^ Eview]->Type == (BISHOP | Eside)))
						MidGameScore[Eside]	-= 20;
					break;
				case BLACK_KING: case WHITE_KING:
					MidGameScore[Eside]		+= KingMidGamePST[ErelSquare];
					EndGameScore[Eside]		+= KingEndGamePST[ErelSquare];
					break;
				default:;
			}
		}
	}

	for (Eside = BLACK, Eview = 0xF0; Eside <= WHITE; Eside++, Eview ^= 0xF0)	{
		Ecside = Eside ^ 3;

		BishopPair();
		KingSafety();
		BadBishop();
		KingShield();
		BlockedPawns();

		for (EPiece = FirstPawn[Eside]; EPiece->Type; EPiece++)	{
			Esquare			= EPiece->Square;
			ErelSquare		= Esquare ^ Eview;
			ERank			= RANK(Esquare);
			EFile			= FILE(Esquare);
			EPawnBackward	= 0;

			if (Eside & BLACK)	{
				EPawnDoubled	= BackwardPawn[Eside][EFile] > ERank;
				EPawnOpened		= BackwardPawn[Ecside][EFile] > ERank;
				EPawnPassed		= EPawnOpened && (BackwardPawn[Ecside][EFile - 1] >= ERank && BackwardPawn[Ecside][EFile + 1] >= ERank);
			}
			else {
				EPawnDoubled	= BackwardPawn[Eside][EFile] < ERank;
				EPawnOpened		= BackwardPawn[Ecside][EFile] < ERank;
				EPawnPassed		= EPawnOpened && (BackwardPawn[Ecside][EFile - 1] <= ERank && BackwardPawn[Ecside][EFile + 1] <= ERank);
			}

			MidGameScore[Eside]	+= PawnMidGamePST[ErelSquare];
			EndGameScore[Eside]	+= PawnEndGamePST[ErelSquare];

			if (OpenFile[Eside][EFile - 1] && OpenFile[Eside][EFile + 1])	{
				MidGameScore[Eside]	-= PawnIsolatedMidGame[ErelSquare] * (EPawnOpened + 1);
				EndGameScore[Eside]	-= PawnIsolatedEndGame[ErelSquare];
			}
			else if (Board[Esquare - 1]->Type == (PAWN | Eside))	{
				MidGameScore[Eside]	+= PawnUnitedMidGame[ErelSquare];
				EndGameScore[Eside]	+= PawnUnitedEndGame[ErelSquare];
			}
			else if ((EPawnBackward = PawnBackward()))		{
				MidGameScore[Eside]	-= PawnBackwardMidGame[ErelSquare] * (EPawnOpened + 1);
				EndGameScore[Eside]	-= PawnBackwardEndGame[ErelSquare];
			}

			if (EPawnDoubled)	{
				MidGameScore[Eside]	-= PawnDoubledMidGame[ErelSquare];
				EndGameScore[Eside]	-= PawnDoubledEndGame[ErelSquare];
			}

			if (EPawnOpened)	{
				if (EPawnPassed)	{
					MidGameScore[Eside]	+= PawnPassedMidGame[ErelSquare];
					EndGameScore[Eside]	+= PawnPassedEndGame[ErelSquare] * (12 + DISTANCE(Esquare + Front[Eside], KING_SQUARE(Ecside)) * 2 - DISTANCE(Esquare + Front[Eside], KING_SQUARE(Eside)) / 2) + 20;

					if (PAWN_ATTACK(Eside, Esquare + Front[Eside]))	{
						MidGameScore[Eside]	+= PawnPassedProtectedMidGame[ErelSquare];
						EndGameScore[Eside]	+= PawnPassedProtectedEndGame[ErelSquare];
					}
					else if (PAWN_ATTACK(Eside, Esquare))	{
						MidGameScore[Eside]	+= PawnPassedProtectedMidGame[ErelSquare] / 2;
						EndGameScore[Eside]	+= PawnPassedProtectedEndGame[ErelSquare] / 2;
					}
					else if (PAWN_ATTACK(Eside, Esquare + Backward[Eside]))	{
						MidGameScore[Eside]	+= PawnPassedProtectedMidGame[ErelSquare] / 3;
						EndGameScore[Eside]	+= PawnPassedProtectedEndGame[ErelSquare] / 3;
					}

					if (PAWN_PASSED_CONTROLLED(Eside, Esquare + Front[Eside]))
						EndGameScore[Eside]	+= IsPawnPassedBloqued(Esquare + Front[Eside]);

					else			{
						EndGameScore[Eside]			+= PassedPawnPotential[ErelSquare] * 3;

						if (PAWN_PASSED_CONTROLLED(Eside, Esquare + 2 * Front[Eside]))
							EndGameScore[Eside]		+= IsPawnPassedBloqued(Esquare + 2 * Front[Eside]);

						else	{
							EndGameScore[Eside]		+= PassedPawnPotential[ErelSquare] * 2;

							if (PAWN_PASSED_CONTROLLED(Eside, Esquare + 3 * Front[Eside]))
								EndGameScore[Eside]	+= IsPawnPassedBloqued(Esquare + 3 * Front[Eside]);

							else EndGameScore[Eside]+= PassedPawnPotential[ErelSquare];
						}
					}

					if (ROOK_ATTACK(BLACK, Esquare) || ROOK_ATTACK(WHITE, Esquare))	{
						for (ETo = Esquare + Backward[Eside]; Board[ETo]->Type & EMPTY; ETo += Backward[Eside]);
						EndGameScore[Eside] += PawnPassedSoported[Eside][Board[ETo]->Type];
					}

					if (!PieceMat[Ecside])	{
						ETo = SquareToPromotion[Esquare] ^ Eview;

						if (DISTANCE(Esquare, ETo) < DISTANCE(KING_SQUARE(Ecside), ETo) + (Turn == Eside))
							EndGameScore[Eside] += PawnPassedUnstopped[ErelSquare];
					}
				}
				else if (!EPawnBackward && PawnCandidate())	{
					MidGameScore[Eside]	+= PawnCandidateMidGame[ErelSquare];
					EndGameScore[Eside]	+= PawnCandidateEndGame[ErelSquare];
				}
			}
		}
	}

	ExchangeIni();
	MiddleGameExchange();
	EndGameExchange();
	EvalCenterControl();

	return (MidEndGameScore[Turn] - MidEndGameScore[Turn ^ 3]) + ((MidGameScore[Turn] - MidGameScore[Turn ^ 3]) * (MatSum - END_GAME_MATERIAL) + (EndGameScore[Turn] - EndGameScore[Turn ^ 3]) * (MIDDLE_GAME_MATERIAL - MatSum)) / MIDDLE_END_GAME_MATERIAL;
}

inline int		IsKnightOutpost(void)			{
	if (KnightOutpost[ErelSquare] && PAWN_ATTACK(Eside, Esquare))				{
		if (PAWN_ATTACK(Ecside, Esquare))
			return FALSE;

		if (PAWN_ATTACK(Ecside, Esquare + Front[Eside]))
			return FALSE;

		if (PAWN_ATTACK(Ecside, Esquare + 2 * Front[Eside]))
			return FALSE;

		if (PAWN_ATTACK(Ecside, Esquare + 3 * Front[Eside]))
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************************
 *	Calcula algunos datos necesarios para evaluar la posicion.											*
 ********************************************************************************************************/
inline void		EvalInit(void)					{
	memset(Attack,				0,		sizeof(Attack));
	memset(PawnInRank,			0,		sizeof(PawnInRank));
	memset(BishopColor,			0,		sizeof(BishopColor));
	memset(AttackWeight,		0,		sizeof(AttackWeight));
	memset(PawnImp,				0,		sizeof(PawnImp));
	memset(MidGameScore,		0,		sizeof(MidGameScore));
	memset(EndGameScore,		0,		sizeof(EndGameScore));
	memset(MidEndGameScore,		0,		sizeof(MidEndGameScore));
	memset(AttackCount,			0,		sizeof(AttackCount));
	memset(EknightSQ,			0,		sizeof(EknightSQ));

	for (ETo = 0; ETo < 16; ETo++)		{
		BackwardPawn[BLACK][ETo]	= ForwardPawn[WHITE][ETo]	= RANK_1;
		BackwardPawn[WHITE][ETo]	= ForwardPawn[BLACK][ETo]	= RANK_8;
		OpenFile[BLACK][ETo]		= OpenFile[WHITE][ETo]		= 1;
	}

	for (Eside = BLACK; Eside <= WHITE; Eside++)
		for (EPiece = FirstPawn[Eside]; EPiece->Type;  EPiece++){
			EFile = FILE(EPiece->Square);
			ERank = RANK(EPiece->Square);

			OpenFile[Eside][EFile] = 0;
			PawnImp[Eside] += PawnImport[EPiece->Square];
			PawnInRank[Eside][ERank]++;

			Attack[Eside][EPiece->Square + Front[Eside] - 1] += AttackerType[PAWN];
			Attack[Eside][EPiece->Square + Front[Eside] + 1] += AttackerType[PAWN];

			if (Eside & BLACK)	{
				if (BackwardPawn[Eside][EFile] < ERank)
					BackwardPawn[Eside][EFile] = ERank;

				if (ForwardPawn[Eside][EFile] > ERank)
					ForwardPawn[Eside][EFile] = ERank;
			}
			else		{
				if (BackwardPawn[Eside][EFile] > ERank)
					BackwardPawn[Eside][EFile] = ERank;

				if (ForwardPawn[Eside][EFile] < ERank)
					ForwardPawn[Eside][EFile] = ERank;
			}
		}

	for (Eside = BLACK; Eside <= WHITE; Eside++)
		for (EDelta = Offset[KING | Eside]; *EDelta; EDelta++)
			if (Board[ETo = KING_SQUARE(Eside) + *EDelta]->Type){
				Attack[Eside][ETo] += AttackerType[KING];

				if (Board[ETo]->Type == (PAWN | (Eside ^ 3)) && !PAWN_ATTACK(Eside ^ 3, ETo))
					EndGameScore[Eside] += 5;
			}
}

inline void		EvalCenterControl(void)			{
	if (COUNT(BLACK_PAWN) <= 4 || COUNT(WHITE_PAWN) <= 4)
		return;

	for (EDelta = CenterSquare; *EDelta; EDelta++)
		if (Attack[WHITE][*EDelta] > Attack[BLACK][*EDelta])
			MidGameScore[WHITE] += CenterTable[*EDelta];

		else if (Attack[BLACK][*EDelta] > Attack[WHITE][*EDelta])
			MidGameScore[BLACK] += CenterTable[*EDelta ^ 0xF0];

		else if (Board[*EDelta]->Type & WHITE)
			MidGameScore[WHITE] += CenterTable[*EDelta];

		else if (Board[*EDelta]->Type & BLACK)
			MidGameScore[BLACK] += CenterTable[*EDelta ^ 0xF0];
}

/********************************************************************************************************
 *	Inicializa la tabla hash material para el final, útil en la detección de finales tablas.			*
 ********************************************************************************************************/
inline int		DrawEndGame(void)				{
	switch (HashMat)			{
		/**	Insuficiencia material.									0 -> 0 */
		case KK: case KNK: case KKN: case KBK: case KKB:			return MATE;

		/**	1 pieza por bando.										2 -> 12 */
		case KNKN: case KBKB: case KNKB: case KBKN:					return 73;
		case KRKR:													return 33;
		case KQKQ:													return 23;
		case KBKP: case KPKB:										return 40;
		case KNKP: case KPKN:										return 32;
		case KNKR: case KRKN:										return 29;

		/**	2 piezas por bando.										14 -> 14 */
		case KNNK: case KKNN:										return 53;

		/**	2 piezas para un bando, 1 para el otro					16 -> 32 */
		case KNKNN: case KNNKN: case KBKNN: case KNNKB: case KNKBN: case KBNKN:
		case KBKBN: case KBNKB: case KBKBB: case KBBKB: case KBBKN: case KNKBB:				return 29;
		case KNNKP: case KPKNN:										return 53;
		case KRKNN: case KNNKR: case KRKBN: case KBNKR: case KRKBB: case KBBKR:				return 12;
		case KNPKR: case KRKNP: case KBPKR: case KRKBP:				return 11;
		case KRKRN: case KRNKR: case KRKRB: case KRBKR:				return 19;
		case KQKRN: case KRNKQ: case KQKRB: case KRBKQ:				return 11;
		case KQNKQ: case KQKQN: case KQBKQ: case KQKQB:				return 16;
		case KPPKN: case KNKPP: case KPPKB: case KBKPP:				return 7;
		case KQKRR: case KRRKQ:										return 7;

		/**	2 piezas por bando.										34 -> 46 */
		case KNNKPP: case KPPKNN:									return 16;
		case KNPKNN: case KNNKNP: case KBPKNN: case KNNKBP: case KNPKBN: case KBNKNP:
		case KBPKBN: case KBNKBP: case KNPKBB: case KBBKNP: case KBPKBB: case KBBKBP:		return 10;
		case KRPKNN: case KNNKRP: case KRPKBN: case KBNKRP: case KRPKBB: case KBBKRP:		return 4;
		case KRNKNN: case KNNKRN: case KRNKBN: case KBNKRN: case KRNKBB: case KBBKRN: case KRBKBB: case KBBKRB:				return 10;
		case KRBKBN: case KBNKRB:									return 8;
		case KRBKNN: case KNNKRB:									return 2;
		case KRPKRN: case KRNKRP: case KRPKRB: case KRBKRP:			return 8;
		case KQNKQP: case KQPKQN: case KQBKQP: case KQPKQB:			return 10;
		case KQNKRR: case KRRKQN:									return 8;

		/**	3 piezas para un bando, 1 para el otro.					48 -> 52 */
		case KBNNKR: case KRKBNN:									return 12;
		case KQKBNN: case KBNNKQ: case KQKBBN: case KBBNKQ:			return 4;
		case KQKRNN: case KRNNKQ: case KQKRBN: case KRBNKQ: case KQKRBB: case KRBBKQ:		return 6;

		/**	3 piezas para un bando, 2 para el otro.					54 ->  72 */
		case KRNKNNP: case KNNPKRN: case KRNKBNP: case KBNPKRN: case KRNKBBP: case KBBPKRN: case KRBKBBP: case KBBPKRB:		return 5;
		case KRRKBNN: case KBNNKRR: case KRRKBBN: case KBBNKRR:		return 5;
		case KNPPKNN: case KNNKNPP: case KNPPKBN: case KBNKNPP: case KBPPKBN: case KBNKBPP: case KBPPKBB: case KBBKBPP:		return 4;
		case KNNKPPP: case KPPPKNN:									return 7;
		case KBNNKRP: case KRPKBNN:									return 7;
		case KRPPKRN: case KRNKRPP: case KRPPKRB: case KRBKRPP:		return 4;
		case KNNKNNP: case KNNPKNN: case KBNKNNP: case KNNPKBN: case KBBKNNP: case KNNPKBB: case KNNKBNP: case KBNPKNN: case KBNKBNP:
		case KBNPKBN: case KBBKBNP: case KBNPKBB: case KBBKBBP: case KBBPKBB: case KNNKBBP: case KBBPKNN: case KBNKBBP: case KBBPKBN:		return 4;
		case KRNKRNP: case KRNPKRN: case KRBKRNP: case KRNPKRB: case KRNKRBP: case KRBPKRN: case KRBKRBP: case KRBPKRB:		return 4;
		case KQNKQPP: case KQPPKQN: case KQBKQPP: case KQPPKQB:		return 3;
		case KQNKQNP: case KQNPKQN: case KQBKQNP: case KQNPKQB: case KQNKQBP: case KQBPKQN: case KQBKQBP: case KQBPKQB:		return 4;

		/**	3 piezas por bando.										74 - > 74 */
		case KBNNKRPP: case KRPPKBNN:								return 7;
		default: return 0;
	}
}

inline void		EvalParamIni(void)				{
	for (ETo = 0; ETo < 64; ETo++)				{
		Esquare = Square64To256[ETo];

		PawnMidGamePST[Esquare]		= PawnMidGamePSTini[ETo];
		KnightMidGamePST[Esquare]	= KnightMidGamePSTini[ETo];
		BishopMidGamePST[Esquare]	= BishopMidGamePSTini[ETo];
		RookMidGamePST[Esquare]		= RookMidGamePSTini[ETo];
		QueenMidGamePST[Esquare]	= QueenMidGamePSTini[ETo];
		KingMidGamePST[Esquare]		= KingMidGamePSTini[ETo];

		PawnEndGamePST[Esquare]		= PawnEndGamePSTini[ETo];
		KnightEndGamePST[Esquare]	= KnightEndGamePSTini[ETo];
		BishopEndGamePST[Esquare]	= BishopEndGamePSTini[ETo];
		RookEndGamePST[Esquare]		= RookEndGamePSTini[ETo];
		QueenEndGamePST[Esquare]	= QueenEndGamePSTini[ETo];
		KingEndGamePST[Esquare]		= KingEndGamePSTini[ETo];

		SRKingCount[Esquare]		= SRKingCountIni[ETo];

		KnightMoveImport[Esquare]	= KnightMoveImportIni[ETo];
		KnightOutpost[Esquare]		= KnightOutpostIni[ETo];

		PawnPassedProtectedEndGame[Esquare]	= PawnPassedProtectedEndGameIni[ETo];
		PawnPassedProtectedMidGame[Esquare]	= PawnPassedProtectedMidGameIni[ETo];
		PawnPassedEndGame[Esquare]			= PawnPassedEndGameIni[ETo];
		PawnPassedUnstopped[Esquare]		= PawnPassedUnstoppedIni[ETo];
		PawnPassedMidGame[Esquare]			= PawnPassedMidGameIni[ETo];
		PassedPawnPotential[Esquare]		= PassedPawnPotentialIni[ETo];
		PawnCandidateMidGame[Esquare]		= PawnCandidateMidGameIni[ETo];
		PawnCandidateEndGame[Esquare]		= PawnCandidateEndGameIni[ETo];

		PawnBackwardEndGame[Esquare]		= PawnBackwardEndGameIni[ETo];
		PawnBackwardMidGame[Esquare]		= PawnBackwardMidGameIni[ETo];
		PawnUnitedEndGame[Esquare]			= PawnUnitedEndGameIni[ETo];
		PawnUnitedMidGame[Esquare]			= PawnUnitedMidGameIni[ETo];
		PawnIsolatedEndGame[Esquare]		= PawnIsolatedEndGameIni[ETo];
		PawnIsolatedMidGame[Esquare]		= PawnIsolatedMidGameIni[ETo];
		PawnDoubledEndGame[Esquare]			= PawnDoubledEndGameIni[ETo];
		PawnDoubledMidGame[Esquare]			= PawnDoubledMidGameIni[ETo];

		RookCompelling[Esquare]				= RookCompellingIni[ETo];
		QueenCompelling[Esquare]			= QueenCompellingIni[ETo];

		CenterTable[Esquare]				= CenterTableIni[ETo];
		PawnImport[Esquare]					= PawnImportIni[ETo];
		SquareToPromotion[Esquare]			= SquareToPromotionIni[ETo];
	}
}

/********************************************************************************************************
 *	Calcula la mobilidad del caballo.																	*
 ********************************************************************************************************/
inline void		KnightMobility(void)			{
	EMobility	= 2 * PAWN_ATTACK(Eside, Esquare);
	ETmp		= 1;

	for (EDelta = Offset[BLACK_KNIGHT]; *EDelta; EDelta++)
		if (Board[ETo = Esquare + *EDelta]->Type)		{
			Attack[Eside][ETo] += AttackerType[KNIGHT];

			if (ETmp && NEAR_KING(Ecside, ETo))
				ETmp = 0;

			if ((Board[ETo]->Type & Eside) || PAWN_ATTACK(Ecside, ETo))
				continue;

			EMobility += KnightMoveImport[ETo] + AttackingPiece[Eside][Board[ETo]->Type];
		}

	if (!ETmp)
		AttackCount[Eside]++;
}

/********************************************************************************************************
 *	Calcula la mobilidad del alfil.																		*
 ********************************************************************************************************/
inline void		BishopMobility(void)			{
	EMobility			= 3 * PAWN_ATTACK(Eside, Esquare) / 2;
	BishopColor[Eside]   |= SquareColor[Esquare];
	ETmp		= 1;

	for (EDelta = Offset[BLACK_BISHOP]; *EDelta; EDelta++)
		for (ETo = Esquare + *EDelta; Board[ETo]->Type; ETo += *EDelta, EMobility += 2)		{
			Attack[Eside][ETo] += AttackerType[BISHOP];

			if (ETmp && NEAR_KING(Ecside, ETo))
				ETmp = 0;

			if (Board[ETo]->Type ^ EMPTY)	{
				if (!PAWN_ATTACK(Ecside, ETo))
					EMobility += AttackingPiece[Eside][Board[ETo]->Type];

				for (; FriendPiece[Eside][Board[ETo]->Type]; ETo += *EDelta)
					EMobility++;
				break;
			}
		}

	if (!ETmp)
		AttackCount[Eside]++;
}

/********************************************************************************************************
 *	Calcula la mobilidad del torre.																		*
 ********************************************************************************************************/
inline void		RookMobility(void)				{
	EMobility	= 0;
	ETmp		= 1;

	for (EDelta = RookFileOffset; *EDelta; EDelta++)		{
		for (ETo = Esquare + *EDelta; Board[ETo]->Type; ETo += *EDelta, EMobility += 2)	{
			Attack[Eside][ETo] += AttackerType[ROOK];

			if (ETmp && NEAR_KING(Ecside, ETo))
				ETmp = 0;

			if (Board[ETo]->Type ^ EMPTY)		{
				if (OpenFile[BLACK][FILE(ETo)] && OpenFile[WHITE][FILE(ETo)])
					EMobility += RookSupport[Eside][Board[ETo]->Type];

				if (!PAWN_ATTACK(Ecside, ETo))
					EMobility += AttackingPiece[Eside][Board[ETo]->Type];
				break;
			}
		}
	}

	for (EDelta = RookRankOffset; *EDelta; EDelta++)		{
		for (ETo = Esquare + *EDelta; Board[ETo]->Type; ETo += *EDelta, EMobility++)	{
			Attack[Eside][ETo] += AttackerType[ROOK];

			if (ETmp && NEAR_KING(Ecside, ETo))
				ETmp = 0;

			if (Board[ETo]->Type ^ EMPTY)		{
				if (!PAWN_ATTACK(Ecside, ETo))
					EMobility += AttackingPiece[Eside][Board[ETo]->Type];
				break;
			}
		}
	}

	if (!ETmp)
		AttackCount[Eside]++;

	if (OpenFile[Eside][FILE(Esquare)])	{
		if (OpenFile[Ecside][FILE(Esquare)])	{
			EMobility += 5;

			if (abs(KING_FILE(Ecside) - FILE(Esquare)) < 2)
				AttackWeight[Eside]++;
		}
		else 	{
			EMobility += 3;

			if (abs(KING_FILE(Ecside) - FILE(Esquare)) < 2)
				AttackWeight[Eside] += 2;
		}
	}

	if (RANK(Esquare ^ Eview) == RANK_7 && (PawnInRank[Ecside][RANK(Esquare ^ Eview)] || (RANK(KING_SQUARE(Ecside) ^ Eview) == RANK_8)))
		EMobility += 10;
}

/********************************************************************************************************
 *	Calcula la mobilidad de la dama.																	*
 ********************************************************************************************************/
inline void		QueenMobility(void)				{
	EMobility	= 0;
	ETmp		= 1;

	for (EDelta = Offset[BLACK_QUEEN]; *EDelta; EDelta++)
		for (ETo = Esquare + *EDelta; Board[ETo]->Type; ETo += *EDelta)			{
			Attack[Eside][ETo] += AttackerType[QUEEN];

			if (ETmp && NEAR_KING(Ecside, ETo))
				ETmp = 0;

			if (Board[ETo]->Type ^ EMPTY)
				break;

			EMobility++;
		}

	if (!ETmp)
		AttackCount[Eside]++;
}

/********************************************************************************************************
 *	Evalua los peones avanzados del enroque o hacia el enroque enemigo.									*
 ********************************************************************************************************/
inline void		KingShield(void){
	EFile = KING_FILE(Eside);

	if (EFile < FILE_D)			{
		ETmp = BPF(FILE_A) + BPF(FILE_B) + BPF(FILE_C) / 2;

		if (PiecesNumber[QUEEN | Ecside])
			MidGameScore[Eside] += Fianchetto[Eside][Board[B2 ^ Eview]->Type];

		if (Board[A1 ^ Eview]->Type != (ROOK | Eside))
			MidGameScore[Eside] += 40;
	}
	else if (EFile > FILE_E)	{
		ETmp = BPF(FILE_H) + BPF(FILE_G) + BPF(FILE_F) / 2;

		if (PiecesNumber[QUEEN | Ecside])
			MidGameScore[Eside] += Fianchetto[Eside][Board[G2 ^ Eview]->Type];

		if (Board[H1 ^ Eview]->Type != (ROOK | Eside))
			MidGameScore[Eside] += 40;
	}
	else	{
		ETmp = 12 * (OpenFile(EFile - 1) + OpenFile(EFile) + OpenFile(EFile + 1));

		if (PiecesNumber[QUEEN | Ecside] && !((KingSideCasttle[Eside] | QueenSideCasttle[Eside]) & Casttle))
			MidGameScore[Eside] -= 40;
	}

	if (abs(EFile - KING_FILE(Ecside) >= 3))
		ETmp *= 2;

	MidGameScore[Eside] -= ETmp * PieceMat[Ecside] / INITIAL_MATERIAL;
}

/********************************************************************************************************
 *	Evalua el ataque de piezas enemigas al rey.															*
 ********************************************************************************************************/
inline void		KingSafety(void){
	if (PiecesNumber[QUEEN | Ecside] && AttackCount[Ecside] > 1){
		AttackWeight[Ecside] += SRKingCount[KING_SQUARE(Eside) ^ Eview] + ATTACKS_SUM(Attack[Ecside][KING_SQUARE(Eside)]) / 2;

		for (EDelta = KingPatternZone[Eside]; *EDelta; EDelta++)
			if (Board[ETo = KING_SQUARE(Eside) + *EDelta]->Type)
				AttackWeight[Ecside] += ATTACKS_SUM(Attack[Ecside][ETo]);

		for (EDelta = KingNoFrontZone[Eside]; *EDelta; EDelta++)
			if (Board[ETo = KING_SQUARE(Eside) + *EDelta]->Type && Attack[Ecside][ETo])
				AttackWeight[Ecside] += ATTACKS_SUM(Attack[Ecside][ETo]) + !ATTACK_WITHOUT_KING(Attack[Eside][ETo]);

		for (EDelta = KingFrontZone[Eside]; *EDelta; EDelta++)
			if (Board[ETo = KING_SQUARE(Eside) + *EDelta]->Type)				{
				if (Attack[Ecside][ETo])
					AttackWeight[Ecside] += ATTACKS_SUM(Attack[Ecside][ETo]) + KingWithOutFriend[Eside][Board[ETo]->Type] + !ATTACK_WITHOUT_KING(Attack[Eside][ETo]);

				else AttackWeight[Ecside] += KingWithEnemy[Eside][Board[ETo]->Type];
			}

		MidGameScore[Eside] -= SafetyTable[AttackWeight[Ecside]];
	}
}

/********************************************************************************************************
 *	Determina si un peon es semi-pasado.																*
 ********************************************************************************************************/
inline int	    PawnCandidate(void)				{
	AttackDif	= PAWN_ATTACK(Eside, Esquare) - PAWN_ATTACK(Ecside, Esquare);

	if (AttackDif < 0)
		return FALSE;

	if (PAWN_ATTACK(Ecside, Esquare + Front[Eside]) > PAWN_ATTACK(Eside, Esquare + Front[Eside]))
		return FALSE;

	ETo = Esquare;
	if (Board[ETo += Front[Eside]]->Type)		{
		AttackDif -= PAWN_ATTACK(Ecside, ETo);

		if (Board[ETo += Front[Eside]]->Type)	{
			AttackDif -= PAWN_ATTACK(Ecside, ETo);

			if (Board[ETo += Front[Eside]]->Type)				{
				AttackDif -= PAWN_ATTACK(Ecside, ETo);

				if (Board[ETo += Front[Eside]]->Type)
					AttackDif -= PAWN_ATTACK(Ecside, ETo);
			}
		}
	}

	AttackDif += PAWN_ATTACK(Eside, Esquare + Front[Eside]);

	ETo = Esquare;
	if (Board[ETo += Backward[Eside]]->Type)	{
		AttackDif += PAWN_ATTACK(Eside, ETo);

		if (Board[ETo += Backward[Eside]]->Type)	{
			AttackDif += PAWN_ATTACK(Eside, ETo);

			if (Board[ETo += Backward[Eside]]->Type)	{
				AttackDif += PAWN_ATTACK(Eside, ETo);

				if (Board[ETo += Backward[Eside]]->Type)
					AttackDif += PAWN_ATTACK(Eside, ETo);
			}
		}
	}

	return AttackDif >= 0;
}

/********************************************************************************************************
 *	Determina si un peon negro es retrasado.															*
 ********************************************************************************************************/
inline int		PawnBackward(void)				{
	if (Eside & WHITE)			{
		ETmp1	= BackwardPawn[WHITE][EFile - 1] - ERank;
		ETmp2	= BackwardPawn[WHITE][EFile + 1] - ERank;
	}
	else{
		ETmp1	= ERank - BackwardPawn[BLACK][EFile - 1];
		ETmp2	= ERank - BackwardPawn[BLACK][EFile + 1];
	}

	if (ETmp1 <= 0 || ETmp2 <= 0)
		return FALSE;

	if (PAWN_ATTACK(Ecside, Esquare + Front[Eside]) > PAWN_ATTACK(Eside, Esquare + Front[Eside]))
		return TRUE;

	if (RANK(ErelSquare) != RANK_2)				{
		if (ETmp1 > 1 && ETmp2 > 1)
			return TRUE;

		if ((ETmp1 == 1 || ETmp2 == 1) && IsPawn[Board[Esquare + Front[Eside]]->Type])
			return TRUE;

		if (ETmp1 == 1 && (ETmp2 == 1 || ETmp2 == 2))
			return TRUE;

		return ETmp2 == 1 && ETmp1 == 2;
	}

	if (ETmp1 > 2 && ETmp2 > 2)
		return TRUE;

	if (ETmp1 > 1 && ETmp2 > 1 && PAWN_ATTACK(Ecside, Esquare + 2 * Front[Eside]) > PAWN_ATTACK(Eside, Esquare + 2 * Front[Eside]))
		return TRUE;

	if ((ETmp1 == 2 || ETmp2 == 2) && (IsPawn[Board[Esquare + Front[Eside]]->Type] || IsPawn[Board[Esquare + 2 * Front[Eside]]->Type]))
		return TRUE;

	if (ETmp1 == 2 && (ETmp2 > 0 && ETmp2 < 4))
		return TRUE;

	return ETmp2 == 2 && (ETmp1 == 1 || ETmp1 == 3);
}

inline int		IsPawnPassedBloqued(int BloquedSquare)			{
	ETmp		= Board[BloquedSquare]->Type;

	if (ETmp == (ROOK | Eside))
		return -30;

	if (ETmp == (ROOK | Ecside))
		return RookCompelling[ErelSquare];

	if (ETmp == (QUEEN | Ecside))
		return QueenCompelling[ErelSquare];

	if (ETmp == (KNIGHT | Ecside) || ETmp == (BISHOP | Ecside) || ETmp == (KING | Ecside))		{
		if (PiecesNumber[BISHOP | Eside] && !PiecesNumber[KNIGHT | Eside] && !(SquareColor[BloquedSquare] & BishopColor[Eside]))
			return (3 * (DISTANCE(Esquare, BloquedSquare) - 4) * PawnPassedBloqued[Eside][ETmp]) >> 1;

		return (DISTANCE(Esquare, BloquedSquare) - 4) * PawnPassedBloqued[Eside][ETmp];
	}

	return 0;
}

inline void		ExchangeIni(void)				{
	ETmp1		= PieceMat[WHITE] - PieceMat[BLACK];
	ETmp2		= COUNT(WHITE_KNIGHT) + COUNT(WHITE_BISHOP) - COUNT(BLACK_KNIGHT) - COUNT(BLACK_BISHOP);

	if (ETmp1 < 0)				{
		Eside = BLACK;
		ETmp1 = -ETmp1;
	}
	else Eside = WHITE;
}

/********************************************************************************************************
 *	Algunos valores para los intercambios de piezas en el mediojuego.									*
 ********************************************************************************************************/
inline void		MiddleGameExchange(void)		{
	if (COUNT(WHITE_QUEEN))			{
		if (!COUNT(BLACK_QUEEN))
			MidGameScore[WHITE] += 50;											/** Bonus por tener dama. */
	}
    else if(COUNT(BLACK_QUEEN))
		MidGameScore[BLACK] += 50;

	if (ETmp1 >= ROOK_VALUE)
        MidGameScore[Eside] += 200;												/** Mayor contra peones. */

	else if (ETmp1 >= KNIGHT_VALUE)
		MidGameScore[Eside] += 100;   											/** Menor contra peones. */

	else if (ETmp1 >= ROOK_VALUE - BISHOP_VALUE)   									/** valor_torre - valor_alfil */
		MidGameScore[Eside]	+= 50;												/** Menor contra torre. */

	else if (ETmp2 > 1)
		MidGameScore[WHITE] += 50;												/** Dos menores contra torre. */

	else if (ETmp2 < -1)
		MidGameScore[BLACK] += 50;
}

/********************************************************************************************************
 *	Algunos valores para los intercambios de piezas en el final.										*
 ********************************************************************************************************/
inline void		EndGameExchange(void)			{
	if (COUNT(WHITE_QUEEN))			{
		if (!COUNT(BLACK_QUEEN))
			EndGameScore[WHITE] += 25;
	}
    else if(COUNT(BLACK_QUEEN))
		EndGameScore[BLACK] += 25;

    if (ETmp1 >= KNIGHT_VALUE)
		EndGameScore[Eside] += 50;

	else if (ETmp1 >= ROOK_VALUE - BISHOP_VALUE)
		EndGameScore[Eside] += 25;

	else if (ETmp2 > 2)															/**	Tres menores contra dama o dos torres. */
		EndGameScore[WHITE] += 25;

	else if (ETmp2 < -2)
		EndGameScore[BLACK] += 25;
}

/********************************************************************************************************
 *	Determina si un caballo esta atrapado.																*
 ********************************************************************************************************/
inline int		KnightTrapped(void)				{
	if (EMobility < 7 && PAWN_ATTACK(Ecside, ErelSquare ^ 33))
		switch (ErelSquare)		{
			case A8: case H8:	return 150;
			case A7: case H7:	return 100 * PAWN_ATTACK(Ecside, ErelSquare ^ 50);
			default:			return 0;
		}

	return 0;
}

/********************************************************************************************************
 *	Determina si un alfil esta atrapado.																*
 ********************************************************************************************************/
inline int		BishopTrapped(void)				{
	if (EMobility < 7)
		switch (ErelSquare)		{
			case A7: case H7:		return 50 * (Board[ErelSquare ^ 49]->Type == (PAWN | Ecside));
			case A6: case H6:		return 50 * (Board[ErelSquare ^ 17]->Type == (PAWN | Ecside) && Board[ErelSquare ^ 2]->Type == (PAWN | Ecside));
			case B8: case G8:		return 150 * (Board[ErelSquare ^ 35]->Type == (PAWN | Ecside));
			default:				return 0;
		}

	return 0;
}

/********************************************************************************************************
 *	Determina si una torre esta atrapada.																*
 ********************************************************************************************************/
inline int		RookTrapped(void)				{
	if (EMobility < 7)
		switch (KING_SQUARE(Eside) ^ Eview)		{
			case F1: case G1:		return 75 * (ErelSquare == H1 || ErelSquare == H2 || ErelSquare == G1);
			case H1: case A1:		return 75 * (Esquare == KING_SQUARE(Eside) + Front[Eside]);
			case C1: case B1:		return 75 * (ErelSquare == A1 || ErelSquare == A2 || ErelSquare == B1);
			default:				return 0;
		}

	return 0;
}

/********************************************************************************************************
 *	Evalua el desarrollo.																				*
 ********************************************************************************************************/
inline void		BlockedPawns(void)				{
	if (Board[C3 ^ Eview]->Type == (KNIGHT | Eside) && Board[C2 ^ Eview]->Type == (PAWN | Eside) &&
	Board[D4 ^ Eview]->Type == (PAWN | Eside) && Board[E4 ^ Eview]->Type != (PAWN | Eside))
		MidGameScore[Eside] -= 15;

	if (Board[E2 ^ Eview]->Type == (PAWN | Eside) && (Board[E3 ^ Eview]->Type ^ EMPTY) && Board[F1 ^ Eview]->Type == (BISHOP | Eside))
		MidGameScore[Eside] -= 20;

	if (Board[D2 ^ Eview]->Type == (PAWN | Eside) && (Board[D3 ^ Eview]->Type ^ EMPTY) && Board[C1 ^ Eview]->Type == (BISHOP | Eside))
		MidGameScore[Eside] -= 20;
}

/********************************************************************************************************
 *	Evalua el alfil blanco.																				*
 ********************************************************************************************************/
inline void		BadBishop(void)	{
	if (OneColor[BishopColor[Eside]])			{
		for (EPiece = FirstPawn[Eside], ETmp = 0; EPiece->Type; EPiece++)
			if (BishopColor[Eside] & SquareColor[EPiece->Square])
				ETmp += PawnImport[EPiece->Square];

		EndGameScore[Eside] -= BadBishopScore[ETmp];
	}
}

/********************************************************************************************************
 *	Evalua la pareja de alfiles.																		*
 ********************************************************************************************************/
inline void		BishopPair(void){
	if (BishopColor[Eside] != 3);

	else if (PiecesNumber[BISHOP ^ Eside])
		MidEndGameScore[Eside]	+= BishopPairScore[(PawnImp[BLACK] + PawnImp[WHITE]) >> 1];

	else if (PiecesNumber[KNIGHT ^ Eside])
		MidEndGameScore[Eside]	+= BishopPairScore[(PawnImp[BLACK] + PawnImp[WHITE]) >> 1] + WithOutBishop;

	else MidEndGameScore[Eside]	+= BishopPairScore[(PawnImp[BLACK] + PawnImp[WHITE]) >> 1] + 3 * (WithOutBishop >> 1);
}

/********************************************************************************************************
 *	Permite reclamar tablas por insuficiencia material.													*
 ********************************************************************************************************/
inline int		MatInsuf(void)	{
	if (MatSum > END_GAME_MATERIAL || COUNT(BLACK_PAWN) || COUNT(WHITE_PAWN))
		return FALSE;

	if (MatSum < ROOK_VALUE)
		return TRUE;

	if ((COUNT(BLACK_BISHOP) + COUNT(WHITE_BISHOP)) * BISHOP_VALUE == MatSum)				{
		ETmp = 0;

		for (Eside = BLACK; Eside <= WHITE; Eside++)
			for (EPiece = FirstPiece[Eside]; EPiece->Type; EPiece++)
				if (EPiece->Type == (BISHOP | Eside))
					ETmp |= SquareColor[EPiece->Square];

		return ETmp != BLACK_WHITE;
	}

	return FALSE;
}

/********************************************************************************************************
 *	Algunos finales soportados.																			*
 ********************************************************************************************************/
#define		KING_SQUARE_REL(Side)				(KING_SQUARE(Side) ^ Eview)
#define		KING_RANK_REL(Side)					RANK(KING_SQUARE_REL(Side))
#define		ROOK_FILE(Side)						FILE(ErookSQ[Side])
#define		ROOK_RANK(Side)						RANK(ErookSQ[Side])
#define		ROOK_RANK_REL(Side)					RANK(ErookSQ[Side] ^ Eview)
#define		PAWN_RANK(Side)						RANK(EpawnSQ[Side])
#define		PAWN_FILE(Side)						FILE(EpawnSQ[Side])
#define		PAWN_RANK_REL(Side)					RANK(EpawnSQ[Side] ^ Eview)
#define		PROMOTION_SQUARE(Side)				(SquareToPromotion[EpawnSQ[Side]] ^ Eview)

int			EnemyKingFrontPawn;
inline int 		SupportedEndGames(void)			{
	if (COUNT(BLACK_PAWN))			{
		Eview = 0xF0;
		Eside = BLACK;
	}
	else if (COUNT(WHITE_PAWN))		{
		Eview = 0;
		Eside = WHITE;
	}
	else if (COUNT(BLACK_BISHOP))
		Eside = BLACK;

	else Eside = WHITE;

	Ecside = Eside ^ 3;

	EnemyKingFrontPawn = 0;
	if (PAWN_FILE(Eside) == KING_FILE(Ecside))		///	mejorar
		for (Esquare = EpawnSQ[Eside] + Front[Eside]; Board[Esquare]->Type; Esquare += Front[Eside])
			if (Board[Esquare]->Type == (KING | Ecside))		{
				EnemyKingFrontPawn = TRUE;
				break;
			}

	switch (HashMat)	{
		case KKBN:	case KBNK:
			EndGameScore[Eside] += 150 / DISTANCE(KING_SQUARE(Eside), KING_SQUARE(Ecside)) + 125 / DISTANCE(KING_SQUARE(Ecside), EknightSQ[Eside]);

			if (SquareColor[EknightSQ[Eside]] & BishopColor[Eside])
				EndGameScore[Eside] += 32;

			if (BishopColor[Eside] & BLACK)
				return 16 * MIN(DISTANCE(KING_SQUARE(Ecside), A1), DISTANCE(KING_SQUARE(Ecside), H8)) + 4;

			return 16 * MIN(DISTANCE(KING_SQUARE(Ecside), H1), DISTANCE(KING_SQUARE(Ecside), A8)) + 4;
		case KBKR:	case KRKB:
			if (BishopColor[Eside] & BLACK)
				return 64 * MIN(DISTANCE(KING_SQUARE(Eside), A1), DISTANCE(KING_SQUARE(Eside), H8)) + 16;

			return 64 * MIN(DISTANCE(KING_SQUARE(Eside), H1), DISTANCE(KING_SQUARE(Eside), A8)) + 16;
		case KKP:	case KPK:
			if (DISTANCE(PROMOTION_SQUARE(Eside), EpawnSQ[Eside]) + (Turn == Ecside) < DISTANCE(PROMOTION_SQUARE(Eside), KING_SQUARE(Ecside)))
				return 2;

			if (DISTANCE(KING_SQUARE(Eside), EpawnSQ[Eside]) + (Turn == Ecside) > DISTANCE(EpawnSQ[Eside], KING_SQUARE(Ecside)))
				return 128 - 21 * DISTANCE(KING_SQUARE(Ecside), EpawnSQ[Eside]);

			if (PAWN_FILE(Eside) == FILE_A)
				return 128 - 21 * DISTANCE(KING_SQUARE(Ecside), B7);

			if (PAWN_FILE(Eside) == FILE_H)
				return 128 - 21 * DISTANCE(KING_SQUARE(Ecside), G7);

			if (PAWN_RANK_REL(Eside) == RANK_2 || PAWN_RANK_REL(Eside) == RANK_3 || PAWN_RANK_REL(Eside) == RANK_4)
				ETo = EpawnSQ[Eside] + 2 * Front[Eside];

			else ETo = EpawnSQ[Eside] + Front[Eside];

			if (KING_ATTACK(Eside, ETo - 1) || KING_ATTACK(Eside, ETo) || KING_ATTACK(Eside, ETo + 1))
				return DISTANCE(PROMOTION_SQUARE(Eside), EpawnSQ[Eside]);

			return 128;
		case KKBP:	case KBPK:
			if ((PAWN_FILE(Eside) == FILE_A || PAWN_FILE(Eside) == FILE_H) && (SquareColor[PROMOTION_SQUARE(Eside)] ^ BishopColor[Eside]))
				return 725 - 120 * DISTANCE(KING_SQUARE(Ecside), PROMOTION_SQUARE(Eside) ^ 17);
			break;
		case KBKNP:	case KNPKB:
			if (EnemyKingFrontPawn)
				return 239;
			break;
		case KNKBP:	case KBKBP:	case KBPKN:	case KBPKB:
			if (EnemyKingFrontPawn)
				return 239;

			for (ETo = EpawnSQ[Eside];; ETo += Front[Eside])	{
				if (!(SquareColor[ETo] & BishopColor[Eside]))	{
					if (ATTACK_WITHOUT_KING(Attack[Ecside][ETo]))
						return 239;

					if (Board[ETo]->Type & Ecside)
						return 239;
				}

				if (Board[ETo]->Type ^ EMPTY)
					break;
			}
			break;
		case KRKRP:	case KRPKR:
			if ((PAWN_FILE(Eside) > FILE_C || PAWN_FILE(Eside) < FILE_F) && ROOK_RANK_REL(Ecside) == RANK_8 && KING_RANK(Ecside) == ROOK_RANK(Ecside) &&
			(KING_SQUARE(Ecside) == EpawnSQ[Eside] + Front[Eside] || KING_SQUARE(Ecside) == EpawnSQ[Eside] + 2 * Front[Eside]))
				return 170;								  						/** Back-rank defense */

			if (PAWN_FILE(Eside) == FILE_A || PAWN_FILE(Eside) == FILE_H){		/** Pawn in rook file */
				if (PAWN_RANK_REL(Eside) == RANK_7)			{
					if (KING_SQUARE(Eside) == EpawnSQ[Eside] + Front[Eside] && abs(PAWN_FILE(Eside) - ROOK_FILE(Ecside)) == 1 && abs(PAWN_FILE(Eside) - KING_FILE(Ecside) < 5))
						return 170; 											/** King in front of pawn */

					if (ErookSQ[Eside] == EpawnSQ[Eside] + Front[Eside] && PAWN_FILE(Eside) == ROOK_FILE(Ecside) &&
					(DISTANCE(KING_SQUARE(Ecside), ErookSQ[Eside]) < 3 || (KING_RANK_REL(Ecside) == RANK_7 && DISTANCE(KING_SQUARE(Ecside), EpawnSQ[Eside]) > 5)))
						return 170;												/** Rook in front of pawn */
				}
				else if (!(PAWN_RANK_REL(Eside) == RANK_7 || PAWN_RANK_REL(Eside) == RANK_8) && ROOK_RANK(Ecside) == PAWN_RANK(Eside) &&
				MIN(DISTANCE(KING_SQUARE_REL(Ecside), H8), DISTANCE(KING_SQUARE_REL(Ecside), A8)) < 2 &&
				RookAttackTable[0xBB + EpawnSQ[Eside] - PROMOTION_SQUARE(Eside)] == RookAttackTable[0xBB + EpawnSQ[Eside] - ErookSQ[Eside]])
					return 170;							  						/** Vancura position */

				if (EnemyKingFrontPawn)
					return 170; 												/** Enemy king in front of pawn */
			}

			if (EnemyKingFrontPawn || KING_ATTACK(Ecside, EpawnSQ[Eside] + Front[Eside]))		{		/** Philidor position */
				if (!(PAWN_RANK_REL(Eside) == RANK_6 || PAWN_RANK_REL(Eside) == RANK_7 || PAWN_RANK_REL(Eside) == RANK_8))		{
					 if (!(KING_RANK_REL(Eside) == RANK_6 || KING_RANK_REL(Eside) == RANK_7 || KING_RANK_REL(Eside) == RANK_8) && ROOK_RANK_REL(Ecside) == RANK_6)
						return 133;
				}
				else if (!(KING_RANK_REL(Eside) == RANK_1 || KING_RANK_REL(Eside) == RANK_2))
					return 170;

				return 73;
			}
			if ((PAWN_FILE(Eside) == FILE_B || PAWN_FILE(Eside) == FILE_G) && DISTANCE(KING_SQUARE(Eside), EpawnSQ[Eside]) == 1 &&
			abs(PAWN_FILE(Eside) - ROOK_FILE(Ecside)) > 3 && abs(PAWN_FILE(Eside) - KING_FILE(Ecside)) < 3 && DISTANCE(KING_SQUARE(Ecside), ErookSQ[Ecside]) > 5)
				return 170;								  /** Short side defense */
			break;
		case KQKP:	case KPKQ:
			if (PROMOTION_SQUARE(Eside) == A1 || PROMOTION_SQUARE(Eside) == H1)
				ETmp1 = 4;

			else if (PROMOTION_SQUARE(Eside) == C1 || PROMOTION_SQUARE(Eside) == F1)
				ETmp1 = 3;

			else break;

			if (DISTANCE(KING_SQUARE(Ecside), PROMOTION_SQUARE(Eside)) <= ETmp1)
				return 3 * DISTANCE(KING_SQUARE(Ecside), PROMOTION_SQUARE(Eside)) + 11;

			return 67 * DISTANCE(KING_SQUARE(Ecside), PROMOTION_SQUARE(Eside));
		case KRKP:	case KPKR:
			if (DISTANCE(KING_SQUARE(Eside), EpawnSQ[Eside]) > 2)
				break;

			if ((Turn & Ecside) && DISTANCE(KING_SQUARE(Eside), EpawnSQ[Eside]) == 2 && abs(KING_RANK(Eside) - PAWN_RANK(Eside)) < 2 &&
			ROOK_FILE(Ecside) != (KING_FILE(Eside) + PAWN_FILE(Eside)) / 2)
				break;

			EdistDiff	= DISTANCE(KING_SQUARE(Eside), PROMOTION_SQUARE(Eside)) + DISTANCE(EpawnSQ[Eside], PROMOTION_SQUARE(Eside));

			if (KING_SQUARE(Eside) == PROMOTION_SQUARE(Eside))
				EdistDiff++;

			if (KING_SQUARE(Eside) + Backward[Eside] == EpawnSQ[Eside])	{
				if (PAWN_FILE(Eside) == FILE_A || PAWN_FILE(Eside) == FILE_H)
					break;

				EdistDiff++;
			}

			if (ROOK_FILE(Ecside) != PAWN_FILE(Eside) && ROOK_RANK_REL(Ecside) != RANK_8)
				EdistDiff--;

			if (Turn & Eside)
				EdistDiff--;

			return 75 * MAX(0, DISTANCE(KING_SQUARE(Ecside), PROMOTION_SQUARE(Eside)) - EdistDiff) + 1;
		default:;
	}

	EHashMatWhitOutPawn = HashMat - HASH_MAT(BLACK_PAWN) - HASH_MAT(WHITE_PAWN);

	if (EHashMatWhitOutPawn == KBKB)			{
		if ((BishopColor[BLACK] ^ BishopColor[WHITE]) && (abs(EMatDif) < 4 * PAWN_VALUE))
			return DiferentBishop[COUNT(BLACK_PAWN) + COUNT(WHITE_PAWN)];

		if (abs(EMatDif) <= PAWN_VALUE)
			return EqualBishop[COUNT(BLACK_PAWN) + COUNT(WHITE_PAWN)];
	}

	if (!EHashMatWhitOutPawn || EHashMatWhitOutPawn == KNKN || EHashMatWhitOutPawn == KNNKNN)
		return 14;

	if (abs(EMatDif) <= PAWN_VALUE)
		switch (EHashMatWhitOutPawn)			{
			case KRKR:			return 19;
			case KQKQ:			return 18;
			case KBBKBB:
				if (COUNT(BLACK_PAWN) + COUNT(WHITE_PAWN))
					return 128 / (COUNT(BLACK_PAWN) + COUNT(WHITE_PAWN));

				return MATE;
			case KRBKRB:
				if (BishopColor[BLACK] ^ BishopColor[WHITE])
					return 22;

				return 18;
			case KRRKRR:	return 17;
			case KQBKQB:
				if (BishopColor[BLACK] ^ BishopColor[WHITE])
					return 20;
				return 17;
			default:;
		}

    /********************************************************************************************************
     *  Regla general(no perfecta) para identificar posiciones tablas:               						*
     *  si ambos bandos no tienen peones se necesita el equivalente a casi 4 peones para poder ganar.  		*
     ********************************************************************************************************/
	if (!COUNT(BLACK_PAWN) && !COUNT(WHITE_PAWN) && abs(EMatDif) < PAWN_VALUE * 15 / 4)
		return 64;

	return 0;
}

///	1300	1350	1400	1450
