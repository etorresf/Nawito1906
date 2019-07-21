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

int			SliderType[32]		= {0x0,0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x2,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
			PawnOffset[4][4]	= {{15,17,0,0},{15,17,0,0},{-15,-17,0,0},{-15,-17,0,0}},
			Value[32], Ato, Adir, Aside, *Adelta, SlideType, Asq, Ad, Again, AminorPiece, Acturn;
TYPE_SEE	*LastAttacker[4], Attackers[4][32];
PIECE		*Apiece, *OriginPiece;
TYPE_SEE	*MinorAttacker, *ActAttacker[4], *Am;

/********************************************************************************************************
 *	'TRUE' si 'From' es atacado por 'Side', 'FALSE' en otro caso.										*
 ********************************************************************************************************/
inline int		Attacked(int Side, int From)	{
	for (Apiece = FirstPiece[Side]; Apiece->Type; Apiece++)
		if ((Adir = AttackTable[Apiece->Type][0xBB + From - Apiece->Square]))	{
			if (NoMovSlide[Apiece->Type])
				return TRUE;

			for (Ato = From + Adir; Board[Ato]->Type & EMPTY; Ato += Adir);
			if (Ato == Apiece->Square)
				return TRUE;
		}

	return ATTACKED_BY_PAWN(Side, From);
}

/********************************************************************************************************
 *	'TRUE' si movimiento de mala captura, 'FALSE' en otro caso. Evita realizar un caro SEE.				*
 ********************************************************************************************************/
inline int		BadCapture(int From, int To)	{
	Ad			= PieceValue2[Board[To]->Type] - PieceValue2[Board[From]->Type];

	if (Ad < 0)	{
		if (ATTACKED_BY_PAWN(Turn ^ 3, To))
			return TRUE;

		for (Apiece = FirstPiece[Turn ^ 3]; Apiece->Type; Apiece++)
			if ((Adir = AttackTable[Apiece->Type][0xBB + To - Apiece->Square]))	{
				if (!NoMovSlide[Apiece->Type])	{
					for (Ato = To + Adir; Board[Ato]->Type & EMPTY; Ato += Adir);
					if (Ato != Apiece->Square)
						continue;
				}

				if (Ad + PieceValue2[Apiece->Type] < 0)
					return TRUE;
			}
	}

	return FALSE;
}

/********************************************************************************************************
 *	Calcula información de ataque-defensa para 'From'. No tiene ataques o defensas rayos x.				*
 ********************************************************************************************************/
inline void		CalculateAttackers(int From)	{
	memset(Attackers, 0, sizeof(Attackers));	/**	Vacía las listas de ataques. */

	for (Aside = BLACK; Aside <= WHITE; Aside++){
		LastAttacker[Aside] = Attackers[Aside];	/**	Inicializa la listas de ataques. */

		/**	Calcula ataques de peones. */
		for (Adelta = PawnOffset[Aside]; *Adelta; Adelta++)
			if (Board[From + *Adelta]->Type == (PAWN | Aside))	{
				LastAttacker[Aside]->Square		= From + *Adelta;
				LastAttacker[Aside]->Slide		= 1;
				LastAttacker[Aside]->Direction	= *Adelta;
				LastAttacker[Aside]++;
			}

		/**	Calcula los ataques de las restantes piezas. */
		for (Apiece = FirstPiece[Aside]; Apiece->Type; Apiece++)
			if ((Adir = AttackTable[Apiece->Type][0xBB + From - Apiece->Square]))	{
				if (NoMovSlide[Apiece->Type])	{
					LastAttacker[Aside]->Square = Apiece->Square;
					LastAttacker[Aside]++;
					continue;
				}

				for (Ato = From + Adir; Board[Ato]->Type & EMPTY; Ato += Adir);
				if (Ato == Apiece->Square)	{
					LastAttacker[Aside]->Square		= Ato;
					LastAttacker[Aside]->Direction	= Adir;
					LastAttacker[Aside]->Slide		= SliderType[abs(Adir)];
					LastAttacker[Aside]++;
				}
			}
	}
}

/********************************************************************************************************
 *	SEE, Static Exchange Evaluator.																		*
 ********************************************************************************************************/
inline int		SEE(int Side, int To, int From, int SEEtype)		{
	if (PieceValue2[Board[To]->Type] >= PieceValue2[Board[From]->Type])
		return 0;														/** Salida rápida. */

	/**	Elimina la pieza original del tablero. */
	OriginPiece = Board[From];
	Board[From]	= &E;

	CalculateAttackers(To);

	if (!Attackers[Side ^ 3]->Square)	{								/** ¿Es una captura libre? */
		Board[From] = OriginPiece;
		return PieceValue2[Board[To]->Type];
	}

	memset(Value, 0, sizeof(Value));
	Ad = 1;

	/** Inicia la primera recaptura. */
	Value[0] = PieceValue2[Board[To]->Type];							/** Inicialmente e gana la pieza capturada. */
	Again 	 = PieceValue2[OriginPiece->Type];
	Side    ^= 3;

	ActAttacker[BLACK]	= Attackers[BLACK];
	ActAttacker[WHITE]	= Attackers[WHITE];

	while(ActAttacker[Side] != LastAttacker[Side])	{
		MinorAttacker = ActAttacker[Side];

		/**	Busca al atacante de menor valor. */
		for(Am = MinorAttacker + 1; Am->Square; Am++)
			if (PieceValue2[Board[Am->Square]->Type] < PieceValue2[Board[MinorAttacker->Square]->Type])
				MinorAttacker = Am;

		/**	Suma piezas que atacan y defienden mediante rayos x. */
		if (MinorAttacker->Slide & 3)	{
			for (Asq = MinorAttacker->Square + MinorAttacker->Direction; Board[Asq]->Type & EMPTY; Asq += MinorAttacker->Direction);

			/**	Comprobando ataques rayos X para las piezas con movimientos en slide. */
			SlideType = MovSlide[Board[Asq]->Type] & MinorAttacker->Slide;
			if (SlideType)	{
				Aside = Board[Asq]->Type & GET_COLOR;

				LastAttacker[Aside]->Square		= Asq;
				LastAttacker[Aside]->Slide		= SlideType;
				LastAttacker[Aside]->Direction	= MinorAttacker->Direction;
				LastAttacker[Aside]++;
			}
		}

		Value[Ad] = Again - Value[Ad - 1];
		if (SEEtype && -MIN(-Value[Ad], Value[Ad - 1]) < 0)
            break;

		Again = PieceValue2[Board[MinorAttacker->Square]->Type];
		*MinorAttacker = *ActAttacker[Side];							/**	Intercambio de la posición de la pieza utilizada, con la comparada, para no volver a utilizarla. */

		ActAttacker[Side]++;
		Side ^= 3;
		Ad++;
	}

	while(--Ad)
		Value[Ad - 1] = MIN(-Value[Ad], Value[Ad - 1]);

	Board[From] = OriginPiece;											/**	Se Restaura el capturador. */
	return Value[0];
}

///	150
