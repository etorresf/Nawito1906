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
 *	En este archivo se encuentran funciones para realizar test.											*
 ********************************************************************************************************/

/********************************************************************************************************
 *	Muestra los datos obtenidos por Perft_Aux.															*
 ********************************************************************************************************/
inline void		Perft(int Depth)				{
	int			i;

	LoadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ; D1 48; D2 2039; D3 97862; D4 4085603; D5 193690690; D6 8031647685;");
	PrintBoard();

	printf("  Performance Test\n  Ply\tSeconds\tNodes\n");
	for (i = 0; i <= Depth; i++)	{
		TimeLeft = ClockTime();												/** Se almacena el tiempo con el que se empieza el nuevo nivel. */
		printf("  %d:\t%d\t%" PRIu64 "\n", i, (ClockTime() - TimeLeft) / 1000, PerftAux(i));
	}
}

/********************************************************************************************************
 *	Comprueba el generador de movimientos, contando el número de nodos totales para una determinada		*
 *  profundidad.																						*
 ********************************************************************************************************/
inline uint64_t PerftAux(int Depth)				{
	if (!Depth)
		return 1;

	HIST_T* i, *e;
	uint64_t NodesNum = 0;
	MoveGen();																	/** Genera todos los movimientos para la posición actual. */

	for (i = FirstMove[Ply], e = FirstMove[Ply + 1]; i != e; i++)		{		/** Recorre la lista de movimientos para la profundidad actual. */
		if (!MakeMove(&i->Move))
			continue;

		NodesNum += PerftAux(Depth - 1);										/** Va más profundo en el árbol recursivamente. */
		UnMakeMove();															/** Deshace el movimiento para ir al próximo. */
	 }

	return NodesNum;
}

/********************************************************************************************************
 *	Resuelve un mate en 7, sirve para comprobar la velocidad relativa del ordenador respecto a uno      *
 *  determinado, también para comprobar diferentes compilaciones del código con diferentes compiladores.*
 ********************************************************************************************************/
inline void		Bench(void)						{
	int			i, t;
	double		nps[2], npst;

	printf("\n   Searching Mate in 11... \n");
	//LoadFEN("r6k/2pn1rp1/2qp2Qp/1p2pPP1/4P3/1pP4R/P4RBP/6K1 w - - 2 36; M8;");	///[Event "CCRL 40/40"][Date "2017.11.16"][Round "585.1.845"][White "Nawito 1.1d"][Black "tomitankChess 1.4 64-bit"][Result "1-0"]
	LoadFEN("r4nk1/4brp1/q2p1pQP/3P4/2p1NP2/P7/1B6/1KR3R1 w - - 1 34 ; M11;");	///[Event "CCRL 40/4"][Date "2019.01.24"][Round "464.4.713"][White "Nawito 1812"][Black "Amyan 1.72"][Result "1-0"]
	PrintBoard();

	MaxDepth = 18;
	NoNewIteration = NoNewMov = TimeLimit = TimeTotal = 1 << 25;

	for (i = 0; i < 2; i++)		{
		EngineThink();
		t		= ClockTime() - TimeLeft;
		nps[i]	= ((double)Nodes / ((double)t + 1)) * 1000.0;

		printf("# \n# Time: %d ms\n# Nodes: %d\n# \n", t, Nodes);
	}

	if (nps[0] > nps[1])
		npst = nps[0];

	else npst = nps[1];

	printf("# Best Nodes per second: %d (Score: %.4f)\n", (int)npst, (float)npst / 2077146);
	printf("# Score: 1.000 on Intel(R) Core(TM) i7-6500U CPU @ 2.50GHz 2.60GHz\n\n");
}

/// 80
