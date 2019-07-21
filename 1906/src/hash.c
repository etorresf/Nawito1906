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
 *	Funciones para manejar las tablas hash y evaluación cache.											*
 *																										*
 *	Las tablas hash actúan durante la búsqueda, cuando una posición se visita, se guarda información de	*
 *	ella, no se sabe el valor exacto en ese momento de la evaluación, pero si que se tiene algo	de		*
 *	información, se puede cortar con las tablas hash muchas ramas de los árboles ahorrando muchos nodos,*
 *	se consigue con ellas de 1 a 2 plys más en el medio juego y más de 4 en el final.					*
 *																										*
 *	En el caso de la evaluación caché, actúan en el momento que se evalúa una posición, se guarda la	*
 *	llave de la posición y la evaluación, si se vuelve a visitar esa posición no se tendrá que evaluar	*
 *	sino retornar ya su valor, la evaluación cache no ahorra nodos pero si un poco de trabajo.			*
 ********************************************************************************************************/

BUCKET			*TTindex, *TTindex2;
TTYPE			*TThash;
ETYPE			*EThash, *ETindex;
int				Hi, Hj;

/********************************************************************************************************
 *	Calcula la llave hash material para la actual posición.												*
 ********************************************************************************************************/
inline void		PutHashMat(void){
	HashMat 	= HASH_MAT(BLACK_PAWN) + HASH_MAT(BLACK_KNIGHT) + HASH_MAT(BLACK_BISHOP) + HASH_MAT(BLACK_ROOK) + HASH_MAT(BLACK_QUEEN) +
				  HASH_MAT(WHITE_PAWN) + HASH_MAT(WHITE_KNIGHT) + HASH_MAT(WHITE_BISHOP) + HASH_MAT(WHITE_ROOK) + HASH_MAT(WHITE_QUEEN);
}

/********************************************************************************************************
 *	Calcula la llave hash para la actual posición.														*
 ********************************************************************************************************/
inline uint64_t	PutHash(void)	{
	PIECE*		Piece;
	int			Side;
	uint64_t	h = HashTurn[Turn] ^ HashCasttle[Casttle];

	for (Side = BLACK; Side <= WHITE; Side++)	{
		for (Piece = FirstPawn[Side]; Piece->Type; Piece++)
			h ^= HashPiece[Piece->Type][Piece->Square];

		for (Piece = FirstPiece[Side]; Piece->Type; Piece++)
			h ^= HashPiece[Piece->Type][Piece->Square];
	}

	if (EnPassant)				{
		if (Board[EnPassant + LeftPawnCapture[Turn ^ 3]]->Type == (PAWN | Turn))
			return h ^ HashEnPassant[EnPassant];

		if (Board[EnPassant + RightPawnCapture[Turn ^ 3]]->Type == (PAWN | Turn))
			return h ^ HashEnPassant[EnPassant];
    }

    return h;
}

/********************************************************************************************************
 *	Borra las tablas hash y evaluación caché.															*
 ********************************************************************************************************/
inline void		ClearHash(void)	{
	memset(TThash,	0, sizeof(TTYPE) * TTsize);
	memset(EThash,	0, sizeof(ETYPE) * ETsize);
}

/********************************************************************************************************
 *	Se almacena información en las tablas hash.															*
 ********************************************************************************************************/
inline void		StoreHash(int Type, int Depth, int NumMove, int Value, int MoveID)			{
	TTindex2	= TThash[Hash & TTMask].Bucket;

	for (Hi = 0; (Hi < NUM_BUCKETS - 1) && (Depth + NumMove < TTindex2[Hi].Depth1 + TTindex2[Hi].Nmove) && TTindex2[Hi].Nmove; Hi++);
	for (Hj = NUM_BUCKETS - 1; Hj > Hi; Hj--)
		TTindex2[Hj] = TTindex2[Hj - 1];

	TTindex		= &TTindex2[Hi];

	/**	Se almacena la información. */
	TTindex->Hash	= Hash;
	TTindex->Type	= Type;
	TTindex->Depth1	= Depth;
	TTindex->Nmove	= NumMove;
	TTindex->MoveID	= MoveID;

	/**	Si es MATE se ajusta el valor de la evaluación. */
	if (Value >= MATE_SCORE)
		TTindex->Value	= Value + Ply;

	else if (Value <= -MATE_SCORE)
		TTindex->Value	= Value - Ply;

	else TTindex->Value	= Value;
}

/********************************************************************************************************
 *	Prueba si la posición esta almacenada en las tablas hash.											*
 ********************************************************************************************************/
inline int		ProbeHash(int* Type, int Depth, int NumMove, int Beta, int* MoveID, int* MakeNull)			{
	*Type		= HASF_NULL;
	*MoveID		= 0;
	*MakeNull	= TRUE;

	for (Hi = 0; Hi < NUM_BUCKETS; Hi++)	{
		TTindex	= &TThash[Hash & TTMask].Bucket[Hi];

		if ((HashAge - TTindex->Nmove) / 2 > 9)
			TTindex->Nmove = 0;

		if (TTindex->Hash == Hash)	{
			*MoveID = TTindex->MoveID;

			if (TTindex->Depth1 +  TTindex->Nmove >= DepthNull && TTindex->Value < Beta && TTindex->Type == HASF_ALPHA)
				*MakeNull = FALSE;

			if (TTindex->Depth1 +  TTindex->Nmove >= Depth + NumMove)	{
				*Type = TTindex->Type;

				/**	Si es MATE se ajusta el valor de la evaluación. */
				if (TTindex->Value >= MATE_SCORE)
					return TTindex->Value - Ply;

				if (TTindex->Value <= -MATE_SCORE)
					return TTindex->Value + Ply;

				return TTindex->Value;
			}
		}
	}

	return VAL_UNKNOWN;
}

/********************************************************************************************************
 *	Almacena información sobre la evaluación de una posición.											*
 ********************************************************************************************************/
inline int		StoreEval(int Value)			{
	ETindex		= &EThash[Hash & ETMask];		/**	Se obtiene la direccion . */
	ETindex->Hash			= Hash;
	return ETindex->Value	= Value;
}

/********************************************************************************************************
 *	Comprueba si una posición ya se ha evaluado.														*
 ********************************************************************************************************/
inline int		ProbeEval(void)	{
	ETindex		= &EThash[Hash & ETMask];		/**	Se obtiene la direccion . */

	if (ETindex->Hash == Hash)
		return ETindex->Value;

	return -MATE;
}

/********************************************************************************************************
 *	Asigna memoria para las tablas hash y evaluación caché.												*
 ********************************************************************************************************/
inline void		AllocHash(void)	{
	TThash	= (TTYPE*) malloc(sizeof(TTYPE) * TTsize);
	EThash	= (ETYPE*) malloc(sizeof(ETYPE) * ETsize);

	if (TThash == NULL || EThash == NULL)	{
		printf("# Out of memory allocating hash or evalcache.\n");
		exit(1);
	}
	return;
}

/********************************************************************************************************
 *	Libera memoria de las tablas hash y de la evaluación caché.											*
 ********************************************************************************************************/
inline void		FreeHash(void)	{
	free(TThash);
	free(EThash);
	return;
}

/********************************************************************************************************
 * Devuelve si la posición se ha repetido 'NRepeat' veces.												*
 ********************************************************************************************************/
inline int		RepeatPos(int NRepeat)			{
	Hj			= Nmove - Rule50;

	for (Hi = Nmove - 2, NRepeat--; Hi >= Hj; Hi -= 2)
		if (Moves[Hi].Hash == Hash && !(--NRepeat))
			return TRUE;

	return FALSE;
}

///	200
