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

#include <math.h>
#include "defs.h"
#include "data.h"
#include "protos.h"

#define	CHECK_PHEROM(p)		MAX(0x1, MIN(0xFFFF, p))
#define SQUARE_256(s)		(Square64To256[s] ^ 0xF0)

float	P	= 0.97;	///	0 <= p <= 1
//float	Q	= 0.10;	///	0 <= q <= 1
float	B	= 1.75;	///	0 <= b <= 32
float	A	= 1.00;	///	0 <= b <= 32
int		M	= 12;	///	0 <= b <= 32

inline	int		IntFromFile(uint64_t* r, FILE* File, int l)		{
    int			i, c;
    *r			= 0;

    for(i = 0; i < l; i++)		{
        c = fgetc(File);

        if(c == EOF)
			return 1;

        (*r) = ((*r) << 8) + c;
    }

    return 0;
}

inline int		LoadBookHeader(BOOK* Book, FILE* File)			{
	uint64_t	r;

	if (IntFromFile(&r, File, 4))
		return 1;
	Book->NumKey = r;

	if (IntFromFile(&r, File, 4))
		return 1;
	Book->LeftGames = r;

	if (IntFromFile(&r, File, 4))
		return 1;
	Book->Iteration = r;

	return 0;
}

inline	int		BookMoveFromFile(BOOK_MOVE* BookMove, FILE* File)				{
    uint64_t	r;

	if (IntFromFile(&r, File, 2))
		return 1;
	BookMove->Move = r;

	if (IntFromFile(&r, File, 2))
		return 1;
	BookMove->T = r;

	if (IntFromFile(&r, File, 2))
		return 1;
	BookMove->N = r;

	if (IntFromFile(&r, File, 2))
		return 1;
	BookMove->t = r;

	return 0;
}

inline	int		BookMoveListFromFile(BOOK_MOVE_LIST* MoveList, FILE* File)		{
	uint32_t	i;
    uint64_t	r;
    BOOK_MOVE	BookMove;

	if (IntFromFile(&r, File, 8))
		return 1;
	MoveList->Key = r;

	if (IntFromFile(&r, File, 1))
		return 1;
	MoveList->Size = r;

	MoveList->Item = (BOOK_MOVE*) malloc(sizeof(BOOK_MOVE) * MoveList->Size);

	for (i = 0; i < MoveList->Size; i++)		{
		if (BookMoveFromFile(&BookMove, File))
			return 1;

		MoveList->Item[i] = BookMove;
	}

	return 0;
}

inline	int		LoadBook(BOOK* Book)			{
	uint32_t	i = 0;
	FILE* 		File = fopen(Book->Name, "rb");
	BOOK_MOVE_LIST	MoveList;
	Book->NumKey = 1;

	if (!File)
		return 1;

	if (!LoadBookHeader(Book, File))		{
		Book->Moves	= (BOOK_MOVE_LIST*) malloc(sizeof(BOOK_MOVE_LIST) * (int)Book->NumKey);

		for (i = 0; i < Book->NumKey; i++)		{
			if (BookMoveListFromFile(&MoveList, File))
				break;

			Book->Moves[i]	 = MoveList;
			Book->Size		+= MoveList.Size;
		}
	}

	M = MIN(255, MAX(4, sqrt(Book->Size) / 4));

	fclose(File);
	return i != Book->NumKey;
}

inline	void	IntToFile(FILE* File, uint64_t r, int l)		{
    int			i, c;

    for(i = 0; i < l; i++)		{
        c = (r >> 8 * (l - i - 1)) & 0xFF;
        fputc(c, File);
    }
}

inline void		SaveBookHeader(FILE* File, BOOK* Book)			{
	IntToFile(File, Book->NumKey,		4);
	IntToFile(File, Book->LeftGames,	4);
	IntToFile(File, Book->Iteration,	4);
}

inline	void	BookMoveToFile(FILE* File, BOOK_MOVE* BookMove)				{
	IntToFile(File, BookMove->Move, 2);
	IntToFile(File, BookMove->T, 	2);
	IntToFile(File, BookMove->N, 	2);
	IntToFile(File, BookMove->t, 	2);
}

inline	void	BookMoveListToFile(FILE* File, BOOK_MOVE_LIST* MoveList)		{
	uint32_t	i;

    IntToFile(File, MoveList->Key,	8);
    IntToFile(File, MoveList->Size, 1);

    for (i = 0; i < MoveList->Size; i++)
		BookMoveToFile(File, &(MoveList->Item[i]));
}

inline	void	SaveBook(BOOK* Book)			{
	if (Book->State & USE_BOOK)	{
		uint32_t	i;
		FILE* 		File = fopen(Book->Name, "wb");

		SaveBookHeader(File, Book);

		for (i = 0; i < Book->NumKey; i++)
			BookMoveListToFile(File, &(Book->Moves[i]));

		fclose(File);
	}
}

inline	int		FindKey(BOOK* Book, uint64_t MoveKey)			{
   int			Midd, Inf = 0,
				Sup = Book->NumKey - 1;

	while(Inf <= Sup)			{
		Midd = ((Sup - Inf) / 2) + Inf;

		if(MoveKey == Book->Moves[Midd].Key)
			return Midd;

		if(MoveKey < Book->Moves[Midd].Key)
			Sup = Midd - 1;

		else Inf = Midd + 1;
	}

	return -1;
}

inline void		CLOSE_BOOK(BOOK* Book)			{
	if (Book->State & USE_BOOK)	{
		int		KeyInd;

		if (Book->State & BOOK_LEARN)
			SaveBook(Book);

		for (KeyInd = 0; KeyInd < Book->NumKey; KeyInd++)
			free(Book->Moves[KeyInd].Item);

		free(Book->Moves);
	}
}

inline uint16_t	SelectBookMove(BOOK_MOVE_LIST* MoveList)		{
	int			i;
	float		MovesProb[MoveList->Size], Sum = 0;
	float		j = rand() % 10000;

	/// se calcula la probabilidad
	for (i = 0; i < MoveList->Size; i++)		{
		MovesProb[i] = pow((float)MoveList->Item[i].T, A) * pow((float)MoveList->Item[i].N, B);
		Sum += MovesProb[i];
	}

	for (i = 0; i < MoveList->Size; i++)		{
		j -= 10000.00 * MovesProb[i] / Sum;

		if (j <= 0)
			return MoveList->Item[i].Move;
	}

	return MoveList->Item[MoveList->Size - 1].Move;
}

inline uint16_t	ScanBookMove(char* c)			{
	int			From	= (c[0] - 'a') + ((c[1] - '1') << 3),
				To		= (c[2] - 'a') + ((c[3] - '1') << 3);
	uint16_t	Move;

	if (SQUARE_256(From) == KING_SQUARE(BLACK) || SQUARE_256(From) == KING_SQUARE(WHITE))
		switch (SQUARE_256(To))				{
			case H1: case H8:	To--;	break;
			case A1: case A8:	To++;	break;
			default:;
		}

	Move = To + (From << 6);
	switch (c[4])				{
		case 'n':	return Move + (1 << 12);
		case 'b':	return Move + (2 << 12);
		case 'r':	return Move + (3 << 12);
		case 'q':	return Move + (4 << 12);
		default:	return Move;
	}
}

inline void		PrintBookMove(char* Buffer, uint16_t IDmove)	{
	IDmove		&= 0x7FFF;
	char*		PromoteChar = " nbrq   ";
	int			From	= (IDmove >> 6) & 077,
				To		= IDmove & 077,
				Promote	= (IDmove >> 12) & 0x7;

	if (SQUARE_256(From) == KING_SQUARE(BLACK) || SQUARE_256(From) == KING_SQUARE(WHITE))
		switch (SQUARE_256(To))				{
			case H1: case H8:	To--;	break;
			case A1: case A8:	To++;	break;
			default:;
		}

	Buffer += sprintf(Buffer, "%s%s", SquareChar[SQUARE_256(From)], SquareChar[SQUARE_256(To)]);

	if (Promote)
		Buffer += sprintf(Buffer, "%c", PromoteChar[Promote]);
}

inline int		DepositedPherom(char GameResult, int OutBook)	{
	OutBook	= MAX(0, MIN(160, OutBook));

	switch (GameResult)			{
		case '-':
			return 1 + OutBook / 40;			///	[1, 5]
		case '+':
			return 255 - (32 * OutBook) / 40;	///	[127, 255]
		default:
			return 6 + (30 * OutBook) / 40;		///	[6, 126]
	}
}

inline MOVE		GetBookMove(BOOK* Book, uint64_t MoveKey)		{
	if (!(Book->State & USE_BOOK))
		return NoMove;

	int			KeyInd	= FindKey(Book, MoveKey);
	char		MoveStr[8];

	if (KeyInd == -1)
		return NoMove;

	PrintBookMove(MoveStr, SelectBookMove(&(Book->Moves[KeyInd])));
	return CheckOneMove(MoveStr, &KeyInd);
}

inline void		UpdateBook(BOOK* Book)				{
	int			KeyInd, i;
	float		t, T, bound;
	BOOK_MOVE *BookMovePtr;
	BOOK_MOVE_LIST *BookMoveList;

	if (Book->LeftGames < M)	{
		Book->LeftGames++;
		return;
	}

	for (KeyInd = 0; KeyInd < Book->NumKey; KeyInd++)			{
		BookMoveList	= &(Book->Moves[KeyInd]);

		for (i = 0; i < BookMoveList->Size; i++){
			BookMovePtr	= &(BookMoveList->Item[i]);
			T			= (float)(BookMovePtr->T);
			t			= (float)(BookMovePtr->t);

			if (!t)
				t = 10;		///	la idea con esto es darle valor negativo a las partidas perdidas

			bound = P * T + t;
			T = CHECK_PHEROM(bound);

			BookMovePtr->T	= (uint16_t)T;
			BookMovePtr->t	= 0;
		}
	}

	Book->LeftGames = 0;
	Book->Iteration++;
	M = MIN(255, MAX(4, sqrt(Book->Size) / 4));
}

inline int		UpdMove(BOOK* Book, BOOK_MOVE* BookMove, uint64_t MoveKey)		{
	BOOK_MOVE_LIST* MoveList;
	int		KeyInd = FindKey(Book, MoveKey), i;

	if (KeyInd != -1)	{
		MoveList = &(Book->Moves[KeyInd]);

		for (i = 0; i < MoveList->Size; i++)
			if (MoveList->Item[i].Move == BookMove->Move)	{
				MoveList->Item[i].t += BookMove->t;
				return 1;
			}
	}

	return 0;
}

inline int	AddGame(BOOK* Book, int Result, int Option)			{
	if (!(Book->State & (USE_BOOK | BOOK_LEARN)))
		return 0;

	int	 		Score[4]	= {0,0,0,0};
	BOOK_MOVE	BookMove;
	char		MoveStr[8];
	int			Cont = 0, i;

	if (Result == DRAW)
		Score[BLACK]		= Score[WHITE] = DepositedPherom('=', Nmove / 2);

	else 	{
		Score[Result]		= DepositedPherom('+', Nmove / 2);
		Score[Result ^ 3]	= DepositedPherom('-', Nmove / 2);
	}

	for (i = MoveLearnSize - 1; i >= 0; i--)		{
		strcpy(MoveStr, "");
		strcat(MoveStr, SquareChar[BookMoveLearn[i].Move.From]);
		strcat(MoveStr, SquareChar[BookMoveLearn[i].Move.To]);

		if (BookMoveLearn[i].Move.Type & PROMOTION)
			MoveStr[4] = PromotionChar[BookMoveLearn[i].Move.Type & PROMOTION_TYPE];

		BookMove.t		= Score[BookMoveLearn[i].Side];
		BookMove.Move	= ScanBookMove(MoveStr);
		Cont 		   += UpdMove(Book, &BookMove, BookMoveLearn[i].Key);
	}

	if (Cont)	{
		UpdateBook(Book);
		SaveBook(Book);
	}

	return Cont;
}

/// 300 350	400
