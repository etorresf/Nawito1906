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

/**	hash.c */
void		AllocHash(void);
void		FreeHash(void);
void		ClearHash(void);
uint64_t	PutHash(void);
int			RepeatPos(int NRepeat);
int			ProbeEval(void);
int			StoreEval(int Value);
int			ProbeHash(int* Type, int Depth, int NumMove, int Beta, int* MoveID, int* MakeNull);
void		StoreHash(int Type, int Depth, int NumMove, int Value, int MoveID);
void		PutHashMat(void);

/**	util.c */
void		ClearBoard(void);
void		ClearPieceList(PIECE* Plist);
void		SortPieceList(PIECE* Plist);
void		MatInit(void);
void		Init(void);
void		Finalize(void);
int			Get_Pow_2(int Value);

/**	time.c */
int			ClockTime(void);

/**	config.c */
void		PrintConfig(void);
int			FindProfile(char* Profile);
int			LoadProfile(char* Profile);
void		CompleteProfile(int m);

/**	output.c */
void		PrintMoves(void);
void		PrintBoard(void);
void		PrintPiecesNumber(void);
void		PrintPieceList(PIECE* Plist);
void		PrintAllPieceList(void);
void		PrintLastMove(int Side, char* Info);
void		PrintMove(MOVE Move);
void		PrintPV(int depth, int score);
void		PrintResult(void);
void		PrintProgress(float Percent);

/**	input.c */
int			Bioskey(void);
MOVE		CheckMove(char *s, int* l);
MOVE		CheckOneMove(char *s, int* l);
int			CheckMoveType(int From, int* To, char *s, int* l);
int			CheckMoves(char *s, MOVE* Mlist, int* l);
int			LoadEPD(char* EPD, MOVE* MoveList);

/**	fen.c */
int 		LoadFEN(char* Buffer);

/**	attack.c */
int			Attacked(int Side, int From);
#define 	InCheck(Side)		Attacked((Side) ^ 3, KingPos[Side]->Square)
int			BadCapture(int From, int To);
int			SEE(int Side, int To, int From, int SEEtype);
void		CalculateAttackers(int From);

/**	move_gen.c */
void		PushPawnMove(int From, int To, int Type);
void		PushPawnCapture(int From, int To, int Type);
void		PushMove(int From, int To, int Type);
void		PushCapture(int From, int To, int Type);
int			MakeMove(MOVE* Move);
void		UnMakeMove(void);
int			LegalsGen(void);
void		MoveGen(void);
void		CaptureGen(void);

/**	test.c */
uint64_t	PerftAux(int Depth);
void		Perft(int Depth);
void		Bench(void);

/**	sts.c */
void		STS(int ThemeBegin, int ThemeEnd);
int			STSeval(int STSnum, int Seg);
int			LoadEPDfile(char* STSname, int STSnum, char* EPD);
void		PrintSTStheme(int ThemeBegin, int ThemeEnd);
void		PrintSTSposition(int ThemeBegin, int ThemeEnd);
void		PrintSTSall(int ThemeBegin, int ThemeEnd);
float		STSthemeEval(unsigned Theme);
void		SaveSTS(void);
void		LoadSTS(void);

/**	eval.c */
int			Eval(int Alpha, int Beta);
int			EvalMiddleGame(void);
int			EvalEndGame(void);
int			EvalMiddleEndGame(void);

int			DrawEndGame(void);
void		EvalInit(void);
void		EvalParamIni(void);

void		KingShield(void);
void		KingSafety(void);

void		KnightMobility(void);
void		BishopMobility(void);
void		RookMobility(void);
void		QueenMobility(void);

int			PawnBackward(void);
int			IsPawnPassedBloqued(int BloquedSquare);
int			PawnCandidate(void);

void		ExchangeIni(void);
void		MiddleGameExchange(void);
void		EndGameExchange(void);

int			KnightTrapped(void);
int			BishopTrapped(void);
int			RookTrapped(void);

void		EvalCenterControl(void);

int			IsKnightOutpost(void);

void		BlockedPawns(void);

void		BadBishop(void);
void		BishopPair(void);

int			MatInsuf(void);
int			SupportedEndGames(void);

/**	search.c */
MOVE		EngineThink(void);
int			PVS(int Alpha, int Beta, int Depth, int MakeNullMove);
int			QuiesenceSearch(int Alpha, int Beta, int Depth);
void		SortMove(HIST_T* j);
void		UpdatePV(MOVE* PVmov);
int			Checkup(int Score);
int			Extensions(int PVnode, int CheckMateThreat);
void		SortHmovePV(int MoveID);

/**	book.c */
void		IntToFile(FILE* File, uint64_t r, int l);
int			IntFromFile(uint64_t* r, FILE* File, int l);
int			BookMoveFromFile(BOOK_MOVE* BookMove, FILE* File);
int			BookHeaderFromFile(BOOK* Book, FILE* File);
int			FindKey(BOOK* Book, uint64_t MoveKey);
int			LoadBook(BOOK* Book);
void		SaveBook(BOOK* Book);
int 		DepositedPherom(char GameResult, int NumMove);
uint16_t	ScanBookMove(char* c);
void		UpdateBook(BOOK* Book);
int			UpdMove(BOOK* Book, BOOK_MOVE* BookMove, uint64_t MoveKey);
int			AddGame(BOOK* Book, int Result, int Option);
uint16_t	SelectBookMove(BOOK_MOVE_LIST* MoveList);
void		PrintBookMove(char* Buffer, uint16_t IDmove);
MOVE		GetBookMove(BOOK* Book, uint64_t MoveKey);
void		CLOSE_BOOK(BOOK* Book);
int			LoadBookHeader(BOOK* Book, FILE* File);
int			BookMoveListFromFile(BOOK_MOVE_LIST* MoveList, FILE* File);
void		SaveBookHeader(FILE* File, BOOK* Book);
void		BookMoveListToFile(FILE* File, BOOK_MOVE_LIST* MoveList);
void		BookMoveToFile(FILE* File, BOOK_MOVE* BookMove);

/**	protocol.c */
void		PrintMenu(void);
void		Xboard(void);

///	180
