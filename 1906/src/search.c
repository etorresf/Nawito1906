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

int DepthNullTable[128] = {
	0,0,0,0,0,0,0,2,4,6,8,10,12,10,12,14,16,18,20,22,24,26,28,26,28,30,32,34,36,38,40,42,44,
	42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,
	100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,182,184,186,
	188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,224,226,228,230,
};

/** Utilizado para extensiones fracciónales. */
typedef		enum	PLY_TYPES {QUARTER_PLY = 1, HALF_PLY = 2, PLY = 4, PLY2 = 8, PLY3 = 12, PLY4 = 16, PLY5 = 20, PLY6 = 24, PLY7 = 28, PLY11 = 44, PLY16 = 64}	PLY_TYPE;

#define	QS_LIMITE		-7									/** Para prevenir la qsearch exploxión. */
#define	MARGEN			20									/** Utilizado en la Aspiration Windows. */

int				Si, StimeUsed;
const int		Margen[4]      			= {0, 120, 140, 160};
const int		PawnEndgameExt[32]		= {0,0,0,0,0, 0,0,0,0, PLY,PLY,0,0, PLY,PLY,0,0, PLY3>>1,PLY3>>1,0,0, PLY2,PLY2,0,0, 0,0,0,0, 0,0,0};
const int		ContemFactor[4]			= {-536, 536, 536, 536};
HIST_T			*Sstack, *Send, *Sbi;

/********************************************************************************************************
 *	Retorna el mejor movimiento para una profundidad, usando de forma iterativa al algoritmo alphabeta.	*
 *	Este podria ser un movimiento de libro.																*
 ********************************************************************************************************/
inline MOVE		EngineThink(void)				{
	TimeLeft	= ClockTime();

	MOVE		BestMove;
	int			Alpha, Beta, NumMoves, i, j,
				WindowAlpha = 16,
				WindowBeta	= 16;

	/** Se consulta al libro de aperturas. */
	if (EngineState & ENGINE_SEARCHING)			{
		BestMove = GetBookMove(&Book1, Hash);
		if (BestMove.Type)		{
			BookMoveLearn[MoveLearnSize].Key	= Hash;
			BookMoveLearn[MoveLearnSize].Side	= Turn;
			BookMoveLearn[MoveLearnSize++].Move = BestMove;
			return BestMove;
		}

		BestMove = GetBookMove(&Book2, Hash);
		if (BestMove.Type)		{
			BookMoveLearn[MoveLearnSize].Key	= Hash;
			BookMoveLearn[MoveLearnSize].Side	= Turn;
			BookMoveLearn[MoveLearnSize++].Move = BestMove;
			return BestMove;
		}

		OutOfBook++;
	}

	BestMove	 = NoMove;
	SearchState	^= (SearchState & (ABORT_SEARCH | STOP_SEARCH));
	Ply = Nodes	 = 0;

	if (MatSum < END_GAME_MATERIAL)
		Cknodes = 0x3FF;

	else Cknodes = 0x3FFF;

	memset(PV,			0, sizeof(PV));
	memset(MateKiller,	0, sizeof(MateKiller));
	memset(Killer1,		0, sizeof(Killer1));
	memset(Killer2,		0, sizeof(Killer2));

	OldPieceMat[BLACK]	= PieceMat[BLACK];
	OldPieceMat[WHITE]	= PieceMat[WHITE];
	OldPawnMat[BLACK]	= PawnMat[BLACK];
	OldPawnMat[WHITE]	= PawnMat[WHITE];

	/**	Actualiza la historia. */
	if (MaxHistory[BLACK] || MaxHistory[WHITE])		{
		MaxHistory[BLACK] >>= 3;
		MaxHistory[WHITE] >>= 3;

		for (j = GET_ID(A1, B1); j < GET_ID(H8, H8); j++)	{
			History[BLACK][j] >>= 3;
			History[WHITE][j] >>= 3;
		}
	}

	NumMoves = LegalsGen();

	/********************************************************************************************************
	 *	LLama a la búsqueda de forma interativa, esto tiene 2 ventajas. Si el tiempo se termina al menos se	*
	 *	tiene una búsqueda completa en una profundidad. Además cada profundidad se puede resolver mas rápido*
	 *	que la anterior debido al uso de las tablas hash y a una mejor ordenación de la variante principal.	*
	 ********************************************************************************************************/
	for (i = 1; i <= MaxDepth;)		{
		Li = i;
		SearchState |= FOLLOW_PV;			   	 						/**	Se pone a seguir la variante principal. */

		if (i < 3)	{													/**	Se realiza una búsqueda completa en las primeras profundidades. */
			Alpha = -MATE;
			Beta  = MATE;
		}

		Value1[i] = PVS(Alpha, Beta, i * PLY, TRUE);					/**	La profundidad que se utiliza es fraccional, permite un mejor control. */
		BestMove = PV[0][0];

		if (SearchState & STOP_SEARCH)
			break;

		/**	Se abren nuevas ventanas si se produce un fallo en la búsqueda. */
		if (Value1[i] <= Alpha)		{
			if (WindowAlpha & 0xFE00)
				Alpha = -MATE;

			else Alpha = Value1[i] - (WindowAlpha <<= 1);
			continue;
		}

		if (Value1[i] >= Beta)	{
			if (WindowBeta & 0xFE00)
				Beta = MATE;

			else Beta = Value1[i] + (WindowBeta <<= 1);
			continue;
		}

		if ((EngineState & ENGINE_SEARCHING) && i > 1)		{			/**	Si la cosa es clara, reducir el tiempo de búsqueda */
			SearchState |= ABORT_SEARCH;								/**	Se tiene una profundidad máxima para abandonar la búsqueda. */
			if (NumMoves == 1 || ClockTime() - TimeLeft > NoNewIteration)
				break;													/**	No comenzar una nueva iteración si ya ha consumido al menos la mitad de tiempo. */
		}

		WindowAlpha	= WindowBeta = 16;
		Alpha		= Value1[i] - WindowAlpha;							/**	Se configura las ventanas para el siguiente ciclo. */
		Beta		= Value1[i] + WindowBeta;
		i++;
	}

	HashAge = Nmove;
	return BestMove;
}

/********************************************************************************************************
 *	Algoritmo alphabeta encargado de la búsqueda, basado en un algoritmo de Bruce Moreland conocido como*
 *	variante principal.																					*
 ********************************************************************************************************/
inline int		PVS(int Alpha, int Beta, int Depth, int MakeNullMove)		{
	int			Htipo, InCheck, ev, Ext, j, MoveID,
				CheckMateThreat = FALSE,
				WeCanCut	= FALSE,
				Score		= Alpha,
				PVnode		= (Beta - Alpha) > 1,
				NumMoves	= 0,
				BestEval	= -MATE,
				HashFlag	= HASF_ALPHA;
	MOVE		BestMov = NoMove;
	HIST_T		*Htmp,*i, *e;

	if (Depth < PLY || (Ply & MAX_DEPHT_1))
		return QuiesenceSearch(Alpha, Beta, 0);							/**	Si la profundidad es menor que PLY o se ha ido muy profundo, se llama al buscador de capturas y promociones. */

	if (!((++Nodes) % Cknodes) && (Checkup(Score) & STOP_SEARCH))		/**	Comprueba cada un cierto numero de nodos que tenemos tiempo. */
		return 0;

	if (Ply)	{
		if ((Alpha = MAX(Alpha, Ply - MATE)) >= (Beta = MIN(Beta, MATE - Ply)))
			return Alpha;												/**	Mate-distance pruning */

		if (RepeatPos(2))
			return MatSum / ContemFactor[Turn & EngineTurn];			/**	Si la posición se repite 1 vez valora como tablas, aunque no las reclama. */

		Score = ProbeHash(&Htipo, Depth, Nmove, Beta, &MoveID, &MakeNullMove);
		if (Score != VAL_UNKNOWN)	 									/**	Comprueba las tablas hash. */
			switch (Htipo)	{
				case HASF_EXACT:
					if (!PVnode)
						return Score;
					break;
				case HASF_ALPHA:
					if (Score <= Alpha)
						return Alpha;
					break;
				case HASF_BETA:
					if (Score >= Beta)
						return Beta;
					break;
				default:;
			}

		if (!MoveID && (Depth >= PLY7 || (PVnode && Depth >= PLY4)))	{/** Internal Iterative Deepening. */
			Score = PVS(Alpha, Beta, Depth - PLY2, TRUE);
			ProbeHash(&Htipo, Depth, Nmove, Beta, &MoveID, &MakeNullMove);
		}
	}

	InCheck = InCheck(Turn);
	if (Ply && !(InCheck || PVnode) && Beta < MATE - MAX_DEPHT)	{
		ev = Eval(-MATE, MATE);

		if (MakeNullMove && PieceMat[Turn])		{
			if (Depth <= PLY)	{
				if (ev >= Beta + 150)
					return Beta + 150;									/**	Static null move prunning. */

				if (ev + 125 < Beta)	{								/**	Razoring */
					Score = QuiesenceSearch(Alpha, Beta, 0);
					return MAX(Score, ev + 125);
				}
			}
			else if (Depth <= PLY3 && ev + 300 < Beta)			{		/**	Razoring */
				if ((Score = QuiesenceSearch(Alpha, Beta, 0)) < ev + 300)
					return MAX(Score, ev + 300);
			}

			/********************************************************************************************************
			 *	Movimiento nulo, solo hacer si: nos deja después de comprobar las tablas hash, no se esta en jaque y*
			 *	si el lado que mueve tiene al menos una pieza.														*
			 ********************************************************************************************************/
			if (Depth > PLY && ev >= Beta)		{
				DepthNull = DepthNullTable[Depth >> 1];

				Htmp = FirstMove[Ply + 1];
				FirstMove[Ply + 1] = FirstMove[Ply];			/**	Se pone donde empiezan los movimientos del próximo Nivel. */

				/**	Realiza el movimiento nulo, actualizama la llave hash. */
				Moves[Nmove].Move		= NoMove;
				Moves[Nmove].Casttle	= Casttle;
				Moves[Nmove].EnPassant	= EnPassant;
				Moves[Nmove].Rule50		= Rule50;
				Moves[Nmove].Hash		= Hash;

				Hash	 ^= HashEnPassant[EnPassant];
				EnPassant = 0;

				Rule50++;
				Ply++;
				Nmove++;
				Hash ^= HashTurn[Turn];
				Hash ^= HashTurn[Turn ^= 3];

				Score = -PVS(-Beta, 1 - Beta, DepthNull, FALSE);

				/**	Deshace el movimiento nulo. */
				Turn ^= 3;
				Nmove--;
				Ply--;
				Casttle		= Moves[Nmove].Casttle;
				EnPassant	= Moves[Nmove].EnPassant;
				Rule50		= Moves[Nmove].Rule50;
				Hash		= Moves[Nmove].Hash;

				FirstMove[Ply + 1] = Htmp;

				if (SearchState & STOP_SEARCH)
					return 0;

				if (Score >= Beta)	{
					if (MatSum < END_GAME_MATERIAL)		{
						if ((Depth -= PLY5) < PLY)						/**	Null reduction */
							return QuiesenceSearch(Alpha, Beta, 0);
					}
					else	{
						StoreHash(HASF_BETA, Depth, Nmove, Beta, BestMov.ID);
						if (Score >= MATE - MAX_DEPHT)
							Score = Beta;	  							/**	No devuelve un mate */

						return Score;									/**	Cutoff **/
					}
				}
				else CheckMateThreat = Score <= -MATE / 2 && Beta > -MATE / 2;
			}
		}

		if (Depth < PLY4 && Alpha > MAX_DEPHT - MATE)					/**	Comprueba si es posible hacer futility prunning. */
			WeCanCut = ev + Margen[Depth / PLY] < Beta;
	}

	PVLenght[Ply] = Ply;
	MoveGen();															/**	Genera todos los movimientos para la posición actual. */

	SortHmovePV(MoveID);												/**	Ordena primero el movimiento de la hash, luego el de la variante principal. */

	for (i = FirstMove[Ply], e = FirstMove[Ply + 1]; i != e; i++)	{
		SortMove(i);

		if (!MakeMove(&i->Move))
			continue;

		NumMoves++;														/**	Numero de movimientos legales. */
		Ext = Extensions(PVnode, CheckMateThreat);

		/**	Futility prunning */
		if (WeCanCut && !Ext && NumMoves > 1 && i->Move.Score < FUTILITY_PRUNNING_SCORE)	{
			UnMakeMove();
			continue;
		}

		/**	Para el primer movimiento realiza una búsqueda completa. */
		if (NumMoves == 1)
			Score = -PVS(-Beta, -Alpha, Depth + Ext - PLY, TRUE);

		else	{
			if (NumMoves > 12 && Depth < PLY2 && !(InCheck || PVnode || Ext) && BestEval > -MATE / 2 &&
			(NumMoves > 20 || i->Move.Score < FUTILITY_PRUNNING_SCORE))	{
				UnMakeMove();											/**	LMR prunning en las terminaciones. */
				continue;
			}

			/********************************************************************************************************
			 *	Prueba hacer LMR si no hay extensiones, si hay un numero máximo de movimientos legales, si se tiene	*
			 *	una determinada profundidad, si el valor del movimiento no es grande, que no sea el movimiento de la*
			 *	tabla hash, principal variante, matekiller, capturas, promociones y killers y que su historia no sea*
			 *	elevada, si todo esto se cumple se prueba a reducir la profundidad.									*
			 ********************************************************************************************************/
			if (NumMoves >= 4 && Depth >= PLY2 && !(Ext || InCheck || (SearchState & FOLLOW_PV)) && BestEval > -MATE / 2 &&
			(i->Move.Score < MaxHistory[Turn ^ 3] * 66 / 100 || ((Moves[Nmove - 1].Move.Type & CAPTURE) && i->Move.Score < HIGH_HISTORY_SCORE)))	{
				if (NumMoves > 20 && Depth >= PLY4 && i->Move.Score < MaxHistory[Turn ^ 3] * 16 / 100)
					Score = -PVS(-Alpha - 1, -Alpha, Depth - PLY4, TRUE);

				else if (NumMoves > 12 && Depth >= PLY3 && i->Move.Score < MaxHistory[Turn ^ 3] * 33 / 100)
					Score = -PVS(-Alpha - 1, -Alpha, Depth - PLY3, TRUE);

				else Score = -PVS(-Alpha - 1, -Alpha, Depth - PLY2, TRUE);
			}
			else Score = Alpha + 1;	   									/**	Hack to ensure that full-depth search is done. */

			if (Score > Alpha)		{
				Score = -PVS(-Alpha - 1, -Alpha, Depth + Ext - PLY, TRUE);

				if (!(SearchState & STOP_SEARCH) && Score > Alpha && Score < Beta)
					Score = -PVS(-Beta, -Alpha, Depth + Ext - PLY, TRUE);
			}
		}

		UnMakeMove();

		if (SearchState & STOP_SEARCH)
			return 0;

		if (Score > BestEval)	{
			BestEval = Score;
			BestMov = i->Move;

			if (Score > Alpha)	{
				if (!(BestMov.Type & CAPTURE) && !(BestMov.Type & PROMOTION))	{				/**	Actualiza la historia. */
					History[Turn][BestMov.ID] += (Depth * Depth) / PLY4;
					if (History[Turn][BestMov.ID] > MaxHistory[Turn])
						MaxHistory[Turn] = History[Turn][BestMov.ID];

					if (MaxHistory[Turn] > HIGH_HISTORY_SCORE)		{
						MaxHistory[BLACK] >>= 1;
						MaxHistory[WHITE] >>= 1;

						for (j = GET_ID(A1, B1); j < GET_ID(H8, H8); j++){
							History[BLACK][j] >>= 1;
							History[WHITE][j] >>= 1;
						}
					}
				}

				if (Score >= Beta)	{
					/**	Los movimientos matekiller y killer son movimientos buenos, aprovechamos para guardar información de ellos. */
					if (Score > MATE - MAX_DEPHT)
						MateKiller[Ply] = i->Move.ID;					/**	Mate killer, es un movimiento killer(asesino) que es un mate. */

					else if (!(i->Move.Type & CAPTURE_PROMOTION) && i->Move.ID != Killer1[Ply])	{
						Killer2[Ply] = Killer1[Ply];
						Killer1[Ply] = i->Move.ID;
					}

					StoreHash(HASF_BETA, Depth, Nmove, Beta, BestMov.ID);/**	TTables */
					return BestEval;									/**	Cutoff del alphabeta */
				}

				Alpha	 = BestEval;
				HashFlag = HASF_EXACT;
				UpdatePV(&BestMov);										/**	Actualiza la variante principal. */

				if (!Ply && Nodes > Noise)
					PrintPV(Li, Score);									/**	Mostrar valoración. */
			}
		}
	}

	if (NumMoves)	{
		if (Rule50 >= 100)
			return MatSum / ContemFactor[Turn & EngineTurn];			/**	Tablas por repeticion. */
	}
	else if (InCheck)													/**	Mate */
		BestEval = Ply - MATE;

	else BestEval = MatSum / ContemFactor[Turn & EngineTurn];			/**	Ahogado */

	StoreHash(HashFlag, Depth, Nmove, BestEval, BestMov.ID);
	return BestEval;
}

/**	Extensiones */
inline int		Extensions(int PVnode, int CheckMateThreat)		{
	if (CheckMateThreat || InCheck(Turn))								/**	Amenaza de MATE o Se da jaque  */
		return HALF_PLY << PVnode;

	if (PVnode && Moves[Nmove - 1].Move.Type & CAPTURE)			{
		if (MatSum < END_GAME_MATERIAL && !(PieceMat[BLACK] || PieceMat[WHITE]) && PawnEndgameExt[LastCapture->Type])
			return PawnEndgameExt[LastCapture->Type];					/**	Entrando en un final de peones. */

		if (Moves[Nmove - 2].Move.Type & CAPTURE)				{
			if (Moves[Nmove - 1].Move.To == Moves[Nmove - 2].Move.To && PieceValue2[LastCapture->Type] <= PieceValue2[(LastCapture - 1)->Type])
				return PLY;												/** Recaptura. */
		}
	}

	return NONE;
}

/********************************************************************************************************
 *  Extensiones(similar a alphabeta) busca capturas y promociones, no lleva profundidad.				*                                                                                       *
 ********************************************************************************************************/
inline int		QuiesenceSearch(int Alpha, int Beta, int Depth)	{
	int			Score = Alpha, BestEval = -MATE, StaticEval = -MATE, delta = 0, NumMoves = 0,
				InCheck, MoveID = 0;
	HIST_T		*i, *e;

	if (!((++Nodes) % Cknodes) && (Checkup(Score) & STOP_SEARCH))
		return 0;

	if (RepeatPos(2))
		return MatSum / ContemFactor[Turn & EngineTurn];

	if (Ply & MAX_DEPHT_1)
		return Eval(Alpha, Beta);

	PVLenght[Ply] = Ply;

	if ((InCheck = InCheck(Turn)))
		MoveGen();														/** Se generan evasiones. */

	else	{
		BestEval	= StaticEval = Eval(Alpha, Beta);
		Alpha		= MAX(Alpha, BestEval);

      	if (Alpha >= Beta)	{
			return Alpha;
      	}

		/** delta prunning */
		if (MatSum > MIDDLE_GAME_MATERIAL)
			delta = Alpha - 200 - StaticEval;

		else if (MatSum > END_GAME_MATERIAL)
			delta = Alpha - 500 - StaticEval;

		CaptureGen();
	}

	SortHmovePV(MoveID);

    for (i = FirstMove[Ply], e = FirstMove[Ply + 1]; i != e; i++)	{
		SortMove(i);

		if (!(InCheck || (i->Move.Type & PROMOTION)))	{
			if (i->Move.Score == BAD_CAPTURE2_SCORE || PieceValue2[Board[i->Move.To]->Type] < delta || SEE(Turn, i->Move.To, i->Move.From, 1) < 0)
				continue;												/** Mala captura y delta prunning. */
		}

		if (Depth <= QS_LIMITE && !InCheck)
        	Score = StaticEval + SEE(Turn, i->Move.To, i->Move.From, 0);/** Previene qsearch exploxion. */

		else	{
			if (!MakeMove(&i->Move))
				continue;

			NumMoves++;
			Score = -QuiesenceSearch(-Beta, -Alpha, Depth - 1);
			UnMakeMove();
		}

		if (SearchState & STOP_SEARCH)
			return 0;

		BestEval = MAX(BestEval, Score);

		if (Score > Alpha)		{
			Alpha = Score;
			UpdatePV(&i->Move);
		}

		if (Alpha >= Beta)
			return Alpha;
	}

	if (InCheck && !NumMoves)
		return Ply - MATE;

	return BestEval;
}

/** Pone en primer lugar el movimiento con mayor valor. */
inline void		SortMove(HIST_T* j)				{
	Sbi			= j;

	for (Sstack = j + 1, Send = FirstMove[Ply + 1]; Sstack != Send; Sstack++)
		if (Sstack->Move.Score > Sbi->Move.Score)
			Sbi = Sstack;

    if (Sbi != j)	{
	    MOVE m		= j->Move;
        j->Move		= Sbi->Move;
        Sbi->Move	= m;
	}
}

/** Se actualiza la variante principal con un movimiento. */
inline void		UpdatePV(MOVE* Move)			{
	PV[Ply][Ply] = *Move;

	for (Si = Ply + 1; Si < PVLenght[Ply + 1]; Si++)
		PV[Ply][Si] = PV[Ply + 1][Si];

	PVLenght[Ply] = PVLenght[Ply + 1];
}

/** Comprueba si es necesario detener la busqueda. */
inline int		Checkup(int Score)				{
	int			StimeUsed = ClockTime() - TimeLeft;

	if ((EngineState & ENGINE_SEARCHING) && (SearchState & ABORT_SEARCH))	{
		if (Bioskey())	{
			EngineTurn = NONE;
			return SearchState |= STOP_SEARCH;
		}

		if ((StimeUsed >= TimeLimit) || (StimeUsed > NoNewMov && Score + MARGEN > Value1[Li - 1]))
			return SearchState |= STOP_SEARCH;
	}
	else if (EngineState & ENGINE_PONDERING)	{
		if (Bioskey())
			return SearchState |= STOP_SEARCH;
	}
	else if (EngineState & ENGINE_ANALYZING)	{
		if (Bioskey())	{
			if (!fgets(Line, 256, stdin))
				return SearchState;

			sscanf(Line, "%s", Command);

			if (!strcmp(Command, "."))
				return SearchState ^= (SearchState & STOP_SEARCH);

            if (!strcmp(Command, "exit"))
				return SearchState |= STOP_SEARCH;
		}

		if (StimeUsed >= TimeLimit)
			return SearchState |= STOP_SEARCH;
	}

	return SearchState;
}

inline void		SortHmovePV(int MoveID)		{
	if (MoveID)
		for(Sstack = FirstMove[Ply], Send = FirstMove[Ply + 1]; Sstack != Send; Sstack++)
			if (Sstack->Move.ID == MoveID)	{
				Sstack->Move.Score = HASH_SCORE;
				break;
			}

	if (SearchState & FOLLOW_PV)	{
		SearchState = SearchState ^ FOLLOW_PV;

		if ((Si = PV[0][Ply].ID))
			for(Sstack = FirstMove[Ply], Send = FirstMove[Ply + 1]; Sstack != Send; Sstack++)
				if (Sstack->Move.ID == Si)	{
					SearchState	|= FOLLOW_PV;

					if (Sstack->Move.Score == HASH_SCORE)
						Sstack->Move.Score	+= FOLLOW_PV_SCORE;

					else Sstack->Move.Score	= FOLLOW_PV_SCORE;
					return;
				}
	}
}

///	600
