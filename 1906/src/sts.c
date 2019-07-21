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

int		Seg = 4,
		NawitoEval[1300];
MOVE	STSMoves[1300][8];

/** Numero en string de las posiciones de la STS de un tema. */
char*	STSid[100]			=	{
		"001","002","003","004","005","006","007","008","009","010","011","012","013","014","015","016","017","018","019","020",
		"021","022","023","024","025","026","027","028","029","030","031","032","033","034","035","036","037","038","039","040",
		"041","042","043","044","045","046","047","048","049","050","051","052","053","054","055","056","057","058","059","060",
		"061","062","063","064","065","066","067","068","069","070","071","072","073","074","075","076","077","078","079","080",
		"081","082","083","084","085","086","087","088","089","090","091","092","093","094","095","096","097","098","099","100",
};

/** Nombres de los temas de la STS. */
char*	STSname[16]			=	{
		"Undermine","Open Files and Diagonals","Knight Outposts/Repositioning/Centralization","Square Vacancy","Bishop vs Knight",
		"Recapturing","Simplification","AKPC","Advancement of a/b/c pawns","Simplification","King Activity","Center Control","Pawn Play in the Center",
};

/********************************************************************************************************
 * Realiza el Strategical Test Suit desde un tema a otro.												*
 ********************************************************************************************************/
inline void		STS(int ThemeBegin, int ThemeEnd)				{
	int 		i, Tmp;

	Tmp				 = EngineConfig;
	EngineState		|= ENGINE_SEARCHING;
	NoNewIteration	 = NoNewMov = TimeLimit = TimeTotal = Seg * 1000;
	ThemeEnd		 = MAX(0, MIN(ThemeEnd, 13));

	if (EngineConfig & POST)
		EngineConfig ^= POST;

	for (i = ThemeBegin * 100; i < ThemeEnd * 100; i++)		{
		PrintProgress((i - ThemeBegin * 100.00 + 1.00) / (ThemeEnd - ThemeBegin));
		STSeval(i, Seg);
	}

	printf("\n\n");
	EngineConfig = Tmp;
	SaveSTS();
}

/********************************************************************************************************
 * Muestra el resultado por posicion del Strategical Test Suit desde un tema a otro.					*
 ********************************************************************************************************/
inline void		PrintSTSposition(int ThemeBegin, int ThemeEnd)	{
	int 		i;

	ThemeEnd	= MAX(0, MIN(ThemeEnd, 13));
	printf("\n\n\t\t\tSTS(%i) Statistics By Position\n", Seg);

	for (i = ThemeBegin * 100; i < ThemeEnd * 100; i++)		{
		PrintProgress((float)NawitoEval[i] * 10.00);
		printf("\t%s.%s\n", STSname[i / 100], STSid[i % 100]);
	}

	printf("\n\n");
}

/********************************************************************************************************
 * Muestra el resultado por tema del Strategical Test Suit desde un tema a otro.						*
 ********************************************************************************************************/
inline void		PrintSTStheme(int ThemeBegin, int ThemeEnd)		{
	int 		i;

	ThemeEnd	= MAX(0, MIN(ThemeEnd, 13));
	printf("\n\n\t\t\tSTS(%i) Statistics By Themes\n", Seg);

	for (i = ThemeBegin; i < ThemeEnd; i++)		{
		PrintProgress(STSthemeEval(i) / 10.00);
		printf("\t%s\n", STSname[i]);
	}

	printf("\n\n");
}

/********************************************************************************************************
 * Muestra el resultado total del Strategical Test Suit desde un tema a otro.							*
 ********************************************************************************************************/
inline void		PrintSTSall(int ThemeBegin, int ThemeEnd)		{
	int 		i;
	float		sum = 0;

	ThemeEnd	= MAX(0, MIN(ThemeEnd, 13));
	printf("\n\n\t\t\tSTS(%i) Statistics By Total Score\n", Seg);

	for (i = ThemeBegin; i < ThemeEnd; i++)
		sum += STSthemeEval(i);

	PrintProgress(sum / (10.00 * (ThemeEnd - ThemeBegin)));
	printf("\tTotal Score\n\n");
}

/********************************************************************************************************
 * Calcula la evaluacion para un tema del STS.															*
 ********************************************************************************************************/
inline float	STSthemeEval(unsigned Theme)	{
	int			i;
	float		Sum = 0;

	for (i = 100 * Theme; i < 100 * (Theme + 1) ; i++)
		Sum += NawitoEval[i];

	return Sum;
}

/********************************************************************************************************
 * Evalua una posicion STS durante n segundos.															*
 ********************************************************************************************************/
inline int		STSeval(int STSnum, int Seg)	{
	int			i;
	MOVE		BestMove;
	char		EPD[256];

	LoadEPDfile(ENGINE_EPD_FILE, STSnum, EPD);
	LoadFEN(EPD);
	LoadEPD(EPD, STSMoves[STSnum]);

	BestMove		= EngineThink();
	BestMove.Score	= 0;

	for (i = 0; STSMoves[STSnum][i].ID; i++)
		if (STSMoves[STSnum][i].ID == BestMove.ID && STSMoves[STSnum][i].Type == BestMove.Type)
			return (NawitoEval[STSnum] = STSMoves[STSnum][i].Score);

	return (NawitoEval[STSnum] = BestMove.Score);
}

/********************************************************************************************************
 * Carga una posicion de un archivo STS.																*
 ********************************************************************************************************/
inline int		LoadEPDfile(char* STSname, int STSnum, char* EPD)				{
	FILE		*EPDFile	= fopen(STSname, "r");
	char		Temp[256];
	int			i;

	EPD[0] = '\0';

	if (EPDFile == NULL)
		return 1;

	if (STSnum < 0 || STSnum >= 1300)			{
		fclose(EPDFile);
		return 2;
	}

	for (i = 0; fgets(Temp, 256, EPDFile) && i < STSnum; i++);
	if (i == STSnum)
		strcpy(EPD, Temp);

	fclose(EPDFile);
	return 0;
}

/********************************************************************************************************
 * Salva los resultados del STS.																		*
 ********************************************************************************************************/
inline void		SaveSTS(void)	{
	FILE		*IniFile = fopen(ENGINE_STS_FILE,  "w");
	int			i;

	fseek(IniFile, 0, SEEK_SET);

	for (i = 0; i < 1300; i++)
		fprintf(IniFile, "%i\n", NawitoEval[i]);

	fclose(IniFile);
}

/********************************************************************************************************
 * Carga un archivo STS.																				*
 ********************************************************************************************************/
inline void		LoadSTS(void)	{
	FILE		*IniFile = fopen(ENGINE_STS_FILE,  "r");
	int			i;

	if (IniFile == NULL)
		return;

	fseek(IniFile, 0, SEEK_SET);

	for (i = 0; i < 1300 && fgets(Line, 256, IniFile); i++)
		sscanf(Line, "%i", &NawitoEval[i]);

	fclose(IniFile);
}

///	200
