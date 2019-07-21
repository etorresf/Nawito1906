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

int     TTSizeMB, ETSizeMB;

/********************************************************************************************************
 *	Muestra la configuración del motor.																	*
 ********************************************************************************************************/
inline void		PrintConfig(void)				{
	printf("\n#\n#");
	printf("\n# Profile\t\t=\t%s", ProfileName);

	printf("\n#");
	printf("\n# Hash\t\t\t=\t%d Mb.", TTSizeMB);
	printf("\n# Ecache\t\t=\t%d Mb.", ETSizeMB);
	printf("\n#");

	if (Book1.State & USE_BOOK)
		printf("\n# Book1\t\t\t=\t%s", Book1.Name);

	if (Book2.State & USE_BOOK)
		printf("\n# Book2\t\t\t=\t%s", Book2.Name);

	if ((Book1.State | Book2.State) & BOOK_LEARN)
		printf("\n# BookLearn\t\t=\ton");

	else printf("\n# BookLearn\t\t=\toff");

	printf("\n#");
	printf("\n# Noise\t\t\t=\t%i", Noise);

	printf("\n#");
	if (EngineConfig & SHOW_BOARD)
		printf("\n# ShowBoard\t\t=\ton");

	else printf("\n# ShowBoard\t\t=\toff");

	if (EngineConfig & SHOW_MOVES)
		printf("\n# ShowMoves\t\t=\ton");

	else printf("\n# ShowMoves\t\t=\toff");
}

/********************************************************************************************************
 *	Busca el nombre del perfil que se usara en la configuracion del motor.								*
 ********************************************************************************************************/
inline int		FindProfile(char* Profile)		{
	FILE*		IniFile = fopen(ENGINE_CONFIG_FILE, "r");

	if (IniFile == NULL)		{
		printf("\n#");
		return FALSE;
	}

	fseek(IniFile, 0, SEEK_SET);
	while (fgets(Line, 256, IniFile))
		if (Line[0] != '#' && Line[0] != ';' && (int)Line[0] > 31)				{
			sscanf(Line, "%s", Command);

			if (!strcmp(Command, "Profile"))	{
				sscanf(Line, "Profile = %s", Profile);
				fclose(IniFile);
				return TRUE;
			}
		}

	printf("\n# 'Profile =' keyword no found in %s file.\n", ENGINE_CONFIG_FILE);
	fclose(IniFile);
	return FALSE;
}

/********************************************************************************************************
 *	Si falta algun dato en la configuracion pone su valor por defecto.									*
 ********************************************************************************************************/
inline void		CompleteProfile(int m)			{
	printf("\n#");

	if (m & 1)	{
		printf("\n# Noise\t\tconfig no found. Using\t'Noise = 200000'");
		Noise = 200000;
	}
	if (m & 2)
		printf("\n# Book1\t\tconfig no found. Using\t'Book1 = Null'");

	if (m & 256)
		printf("\n# Book2\t\tconfig no found. Using\t'Book2 = Null'");

	if (m & 4)
		printf("\n# ShowBoard\tconfig no found. Using\t'ShowBoard = off'");

	if (m & 8)
		printf("\n# ShowMoves\tconfig no found. Using\t'ShowMoves = off'");

	if (m & 16)
		printf("\n# Hash\t\tconfig no found. Using\t'Hash = %i'", TTSizeMB = 128);

	if (m & 32)
		printf("\n# Ecache\tconfig no found. Using\t'Ecache = %i'", ETSizeMB = 32);

	if (m & 64)
		printf("\n# BookLearn\tconfig no found. Using\t'BookLearn = off'");

	TTSizeMB = Get_Pow_2(TTSizeMB);
	ETSizeMB = Get_Pow_2(ETSizeMB);

	TTsize	= (TTSizeMB << 20) / sizeof(TTYPE);
	ETsize	= (ETSizeMB << 20) / sizeof(ETYPE);

	TTMask	= TTsize - 1;
	ETMask	= ETsize - 1;
}

/********************************************************************************************************
 *	Carga la configuracion de un perfil.																*
 ********************************************************************************************************/
inline int		LoadProfile(char* Profile)		{
	char		Temp[256];
	FILE		*IniFile	= fopen(ENGINE_CONFIG_FILE, "r");
	int			ConfigFlag	= 0xFFFF;
	EngineConfig			= POST;

	if (IniFile == NULL)		{
		printf("\n#");
		printf("\n# %s\tfile no found.", ENGINE_CONFIG_FILE);

		CompleteProfile(0xFFFF);
		return FALSE;
	}

	fseek(IniFile, 0, SEEK_SET);
	sprintf(Temp, "[%s]", Profile);

	while (fgets(Line, 256, IniFile))
		if (Line[0] == '[')		{
			sscanf(Line, "%s", Command);

			if (!strcmp(Temp, Command))	{
				while (fgets(Line, 256, IniFile) && (Line[0] != '['))			{
					if (Line[0] == '#' || Line[0] == ';' || (int)Line[0] < 32)
						continue;

					sscanf(Line, "%s", Command);

					if (!strcmp(Command, "Noise"))				{
						ConfigFlag ^= 1;
						sscanf(Line, "Noise = %d", &Noise);
					}
					else if (!strcmp(Command, "Book1"))			{
						ConfigFlag ^= 2;
						sscanf(Line, "Book1 = %s", Book1.Name);
					}
					else if (!strcmp(Command, "Book2"))			{
						ConfigFlag ^= 256;
						sscanf(Line, "Book2 = %s", Book2.Name);
					}
					else if (!strcmp(Command, "BookLearn"))		{
						ConfigFlag ^= 64;
						sscanf(Line, "BookLearn = %s", Temp);
						if (!strcmp(Temp, "on"))	{
							Book1.State |= BOOK_LEARN;
							Book2.State |= BOOK_LEARN;
						}
					}
					else if (!strcmp(Command, "ShowBoard"))		{
						ConfigFlag ^= 4;
						sscanf(Line, "ShowBoard = %s", Temp);
						if (!strcmp(Temp, "on"))
							EngineConfig |= SHOW_BOARD;
					}
					else if (!strcmp(Command, "ShowMoves"))	{
						ConfigFlag ^= 8;
						sscanf(Line, "ShowMoves = %s", Temp);

						if (!strcmp(Temp, "on"))
							EngineConfig |= SHOW_MOVES;
					}
					else if (!strcmp(Command, "Hash"))			{
						ConfigFlag ^= 16;
						sscanf(Line, "Hash = %d", &TTSizeMB);
					}
					else if (!strcmp(Command, "Ecache"))		{
						ConfigFlag ^= 32;
						sscanf(Line, "Ecache = %d", &ETSizeMB);
					}
				}

				fclose(IniFile);
				CompleteProfile(ConfigFlag);
				strcpy(ProfileName, Profile);
				return TRUE;
			}
		}

	fclose(IniFile);
	printf("\n# '%s'\tprofile no found.", Profile);
	CompleteProfile(0xFFFF);
	return FALSE;
}

/// 200
