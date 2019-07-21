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
 *  Protocolo xboard mas otros commandos.																*
 ********************************************************************************************************/
inline void		Xboard(void)					{
	char		Minutes[4],												/** Recibe el número de minutos base para el control de tiempo. */
				Args[6][64],											/** Para el comando setboard. */
				FEN[256],
				ResultChar[16],
				profile[16];
	int	 		MoveEntry = 0,											/** Recibe el número de movimientos para el control de tiempo. */
				Increment,												/** Recibe el número de segundos de incremento para el control de tiempo. */
				n, b, e, Result,
				PonderFlag = 1;
	float		factor,
				target;
	MOVE		BestMove;
																		/** indica si se pondera o no. */
	setbuf(stdout, NULL);												/** En windows o linux podemos intentar eliminar el problema de buffering al enviar algo a xboard. */
    LoadFEN(START_POS);
    NoNewIteration = NoNewMov = TimeLimit = TimeTotal = 8000;			/**	Por defecto 8 seg para pensar el motor. */

	for (;;)	{														/** bucle esperando recibir un comando del GUI. */
		if (EngineConfig & SHOW_BOARD)	{
			PrintBoard();
            Li = 0;
		}

        if (Turn & EngineTurn)	{										/** Turno del motor. */
        	EngineState	|= ENGINE_SEARCHING;
			BestMove	 = EngineThink();

			MakeMove(&BestMove);										/** Guarda el movimiento en la lista de jugadas. */

            /** Envía el movimiento al GUI. */
            printf("move ");
            PrintMove(BestMove);
            printf("\n");

            PrintResult();                    							/** Si es final de partida imprime el resultado. */

			EngineState	^= ENGINE_SEARCHING;
			PonderFlag	^= 1;											/** Indica que puede ponderar. */
			continue;
        }
		else if (EngineTurn)	{
			if ((PonderFlag & 1) && (EngineConfig & MAKE_PONDER))		{	/** Pondera si el GUI le envía una señal. */
				if (OutOfBook)		{
					EngineState		|= ENGINE_PONDERING;
					BestMove		 = EngineThink();
					EngineState		^= ENGINE_PONDERING;
				}

				PonderFlag ^= 1;										/** Deja de ponderar. */
				continue;
			}
        }

		if (!fgets(Line, 256, stdin))
			return;
		if (Line[0] == '\n')
			continue;

		sscanf(Line, "%s", Command);									/** Almacena lo que llega y lo guarda en la variable Command. */

		if (!strcmp(Command, "xboard"));								/** Si llega xboard simplemente continua. */
		else if (!strcmp(Command, "name"));								/** Nombre del oponente. */
		else if (!strcmp(Command, "rating"));							/** Rating del oponente cuando estamos en modo ICS. */
		else if (!strcmp(Command, "ics"));								/** Nombre del servidor de Internet donde jugamos. */
		else if (!strcmp(Command, "computer"));							/** El oponente es un motor. */
		else if (!strcmp(Command, "variant"));							/** Nos indica si jugamos una variante que no es la normal. */
		else if (!strcmp(Command, "random"));							/** Comando que en GNUChess añadía un pequeño valor aleatorio a la evaluación. */
		else if (!strcmp(Command, "otim"));								/** Tiempo del oponente, no utilizado de momento. */
		else if (!strcmp(Command, "edit"));								/** Viejo comando edit de winboard para configurar una posición. */
		else if (!strcmp(Command, "."));
		else if (!strcmp(Command, "exit"));
		else if (!strcmp(Command, "bk"));								/** Si el usuario selecciona Book en el menú. */

		else if (!strcmp(Command,"protover"))							/** Comprueba si tiene protocolo winboard 2 y define algunas características. */
			printf("feature colors=0 draw=0 ics=1 myname=\"%s\" ping=1 setboard=1 sigint=0 sigterm=0 variants=\"normal,nocastle\"\nfeature done=1\n", ENGINE_NAME);

		else if (!strcmp(Command, "accepted"))							/** Indica si las características definidas son aceptadas. */
			printf("feature accepted\n");

		else if (!strcmp(Command, "rejected"))
			printf("feature rejected\n");

		else if (!strcmp(Command, "quit"))								/** Comando para cerrar el motor . */
			return;

		else if (!strcmp(Command, "force"))								/** El motor deja de jugar. */
			EngineTurn = NONE;

		else if (!strcmp(Command, "result"))	{						/** Resultado de un juego el motor debe parar. */
			sscanf(Line, "result %s", ResultChar);
			Result = NONE;

			if (!strcmp(ResultChar, "1/2-1/2"))
				Result = DRAW;

			else if (!strcmp(ResultChar, "1-0"))
				Result = WHITE_WIN;

			else if (!strcmp(ResultChar, "0-1"))
				Result = BLACK_WIN;

			if (Result)	{
				AddGame(&Book1, Result, EngineTurn);
				AddGame(&Book2, Result, EngineTurn);
				MoveLearnSize = 0;
			}

			EngineTurn = NONE;
		}
		else if (!strcmp(Command, "?"))
			EngineTurn = NONE;

		else if (!strcmp(Command, "hard"))								/** Ponder, pensar con el tiempo del contrario. */
			EngineConfig |= MAKE_PONDER;

		else if (!strcmp(Command, "easy"))								/** No ponder. */
			EngineConfig ^= (MAKE_PONDER & EngineConfig);

		else if (!strcmp(Command, "post"))								/** Imprimir el pensamiento. */
			EngineConfig |= POST;

		else if (!strcmp(Command, "nopost"))							/** No imprimir el pensamiento. */
			EngineConfig ^= (POST & EngineConfig);

		else if (!strcmp(Command, "new"))		{						/** Comprueba si hay que empezar nueva partida. */
			LoadFEN(START_POS);
			NoNewIteration = NoNewMov = TimeLimit = TimeTotal = 8000;	/**	Por defecto 5 seg para pensar el motor. */
		}
		else if (!strcmp(Command, "go"))		{						/** Fuerza al motor a jugar. */
			EngineTurn	= Turn;
			ProfileTurn	= Turn ^ 3;
		}
		else if (!strcmp(Command, "white"))		{						/** Turno para las blancas, el motor juega negras. */
			ProfileTurn = Turn = WHITE;
			EngineTurn  = BLACK;
		}
		else if (!strcmp(Command, "black"))		{						/** Turno para las negras, el motor juega blancas. */
			ProfileTurn = Turn = BLACK;
			EngineTurn  = WHITE;
		}
		else if (!strcmp(Command, "level"))		{						/** Parámetros para configurar el nivel de juego del programa level 40 5 0 --> 40 movimientos 5 minutos incremento 0 si el número de movimientos es 0 la partida es a finish. */
			sscanf(Line, "level %d %s %d", &MoveEntry, Minutes, &Increment);
			Mps = MoveEntry;											/** Interesa saber cuantos movimientos para el control. */
			Inc = Increment * 1000;
		}
		else if (!strcmp(Command, "st"))		{						/** Configura el nivel de juego indicando cuantos segundos por movimiento	st 10 --> 10 segundos por jugada. */
			sscanf(Line, "st %d", &MaxTime);
			NoNewIteration = NoNewMov = TimeLimit = TimeTotal = MaxTime *= 1000;/** Convierte a milisegundos y no gestiona el tiempo para cambios de iteración o nuevos movimientos. */
			MaxDepth = MAX_DEPHT;
		}
		else if (!strcmp(Command, "sd"))		{						/** Configura el nivel de juego indicando la profundidad a jugar sd 8 --> juega hasta profundidad principal 8. */
			sscanf(Line, "sd %d", &n);

			MaxDepth		= n & MAX_DEPHT;
			NoNewIteration	= NoNewMov = TimeLimit = TimeTotal = 1<<25;	/** Es como poner tiempo infinito y no gestionamos el tiempo. */
		}
		else if (!strcmp(Command, "time"))		{						/** Recibe del GUI el tiempo que nos queda. */
			sscanf(Line, "time %d", &MaxTime);

			TimeTotal  = MaxTime *= 10;									/** Pasa a milisegundos que es como trabajamos internamente. */
			TimeTotal -= 100;											/** Mantiene un margen de seguridad de 100 ms. */
			if (TimeTotal < 10)											/** Mínimo */
                TimeTotal = 10;

			if (!Mps && Inc > 0){			    						/** Partidas con incremento de tiempo. */
				if (TimeTotal < 3 * Inc)
					NoNewMov	= TimeLimit = TimeTotal = 3 * Inc / 4;

				else			{
					TimeTotal	= TimeTotal / 30 + Inc;
					TimeLimit	= (4 * TimeTotal);
					NoNewMov	= (3 * TimeTotal / 2);
				}

				NoNewIteration	= TimeTotal / 2;
				continue;
			}

			/** Movimientos que quedaran para llegar al control si no es partida con incremento. */
			NextTimeControl = Mps - Nmove / 2 + BLACK - EngineTurn;
			if (Mps)
				while (NextTimeControl <= 0)
					NextTimeControl += Mps;

			else				{
				if (OutOfBook <= 35)
					NextTimeControl = (175 - OutOfBook) / 5;

				else if (OutOfBook <= 60)
					NextTimeControl = (204 - OutOfBook) / 6;

				else NextTimeControl = 24;
			}

			factor	= 2.0 - (float)MIN(10, OutOfBook) / 10.0;
			target 	= (float)TimeTotal / (float)NextTimeControl;

			TimeLimit	 	= factor * target;
			NoNewIteration	= TimeLimit / 2.0;
			NoNewMov		= (3.0 * TimeLimit) / 4.0;
		}
		else if (!strcmp(Command, "ping"))		{						/** Control para winboard, recibe un ping y envía un pong. */
			sscanf(Line, "ping %d", &n);
			printf("pong %d\n", n);
		}
		else if (!strcmp(Command, "setboard"))	{						/** Configura el tablero con una posición en formato FEN. */
			strcpy(FEN, "");
			sscanf(Line, "setboard %s %s %s %s %s %s", Args[0],Args[1],Args[2],Args[3],Args[4],Args[5]);

			strcat(FEN, Args[0]); strcat(FEN, " ");
			strcat(FEN, Args[1]); strcat(FEN, " ");
			strcat(FEN, Args[2]); strcat(FEN, " ");
			strcat(FEN, Args[3]); strcat(FEN, " ");
			strcat(FEN, Args[4]); strcat(FEN, " ");
			strcat(FEN, Args[5]);

			LoadFEN(FEN);
			NoNewIteration = NoNewMov = TimeLimit = TimeTotal = 8000;
		}
		else if (!strcmp(Command, "hint"))		{						/** Da una pista del mejor movimiento al usuario si nos lo pide. */
			NoNewIteration	 = NoNewMov = TimeLimit = TimeTotal = 8000;
			MaxDepth		 = MAX_DEPHT;

			EngineState		|= ENGINE_SEARCHING;
			BestMove		 = EngineThink();

			if (BestMove.ID)	{
				printf("Hint: ");
            	PrintMove(BestMove);
                printf("\n");
            }

            EngineState		^= ENGINE_SEARCHING;
		}
		else if (!strcmp(Command, "undo"))		{						/** Deshace un movimiento. */
			if (Nmove > 1)	{
				UnMakeMove();
				if (Hash == BookMoveLearn[MoveLearnSize - 1].Key)
					MoveLearnSize--;

				Ply = 0;
			}
		}
		else if (!strcmp(Command, "remove"))	{						/** Deshace los 2 últimos movimientos y continua el mismo color. */
			if (Nmove > 2)		{
				UnMakeMove();
				if (Hash == BookMoveLearn[MoveLearnSize - 1].Key)
					MoveLearnSize--;

				UnMakeMove();
				if (Hash == BookMoveLearn[MoveLearnSize - 1].Key)
					MoveLearnSize--;

				Ply = 0;
			}
		}
		else if (!strcmp(Command, "analyze"))	{						/** Modo de análisis con GUI. */
			NoNewIteration	 = NoNewMov = TimeLimit = TimeTotal = 1 << 25;
			MaxDepth	   	 = MAX_DEPHT;
			EngineState		|= ENGINE_ANALYZING;

			BestMove		 = EngineThink();

			EngineState		^= ENGINE_ANALYZING;
			EngineTurn		 = NONE;
		}
		/**	LOS SIGUIENTES COMANDOS NO PERTENECEN A XBOARD. SON PARA USARSE SOLO CON NAWITO. */
		else if (!(strcmp(Command, "--STS") && strcmp(Command, "-sts")))	{				/** Perft: comprueba el correcto funcionamiento del generador de movimientos. */
			b = 0; e = 1;
			sscanf(Line,"%*s %i %i", &b, &e);
			STS(b % 14, e % 14);
		}
		else if (!(strcmp(Command, "--STStheme") && strcmp(Command, "-STST")))	{			/** Perft: comprueba el correcto funcionamiento del generador de movimientos. */
			b = 0; e = 1;
			sscanf(Line,"%*s %i %i", &b, &e);
			PrintSTStheme(b % 14, e % 14);
		}
		else if (!(strcmp(Command, "--STSposition") && strcmp(Command, "-STSP")))	{		/** Perft: comprueba el correcto funcionamiento del generador de movimientos. */
			b = 0; e = 1;
			sscanf(Line,"%*s %i %i", &b, &e);
			PrintSTSposition(b % 14, e % 14);
		}
		else if (!(strcmp(Command, "--STSall") && strcmp(Command, "-STSA")))	{			/** Perft: comprueba el correcto funcionamiento del generador de movimientos. */
			b = 0; e = 1;
			sscanf(Line,"%*s %i %i", &b, &e);
			PrintSTSall(b % 14, e % 14);
		}
		else if (!(strcmp(Command, "--perft") && strcmp(Command, "-p")))		{			/** Perft: comprueba el correcto funcionamiento del generador de movimientos. */
			n = 5;
			sscanf(Line,"%*s %i", &n);
			Perft(n % MAX_DEPHT_1);
		}
		else if (!(strcmp(Command, "--bench") && strcmp(Command, "-b")))		/**	Bench: comprueba la velocidad del equipo. */
			Bench();

		else if (!(strcmp(Command, "--board") && strcmp(Command, "-d")))		/** Se muestra la posición actual del tablero. */
			PrintBoard();

		else if (!(strcmp(Command, "--pieces") && strcmp(Command, "-l")))	{	/** Se muestra la lista de piezas actual. */
			printf("\n");
			PrintAllPieceList();
			printf("\n");
		}
		else if (!(strcmp(Command, "--moves") && strcmp(Command, "-m")))		/** Se muestra los movimientos realizados. */
			PrintMoves();

		else if (!(strcmp(Command, "--piece_number") && strcmp(Command, "-n")))	/** Se muestra el numero de piezas actual, según su tipo. */
			PrintPiecesNumber();

		else if (!(strcmp(Command, "--config") && strcmp(Command, "-c")))		/** Se muestra la configuración actual. */
			PrintConfig();

		else if (!strcmp(Command, "--profile"))							{		/** Carga un perfil en especifico. */
		    sscanf(Line, "--profile %s", profile);
		    LoadProfile(profile);
		    PrintConfig();
		}
		else if (!(strcmp(Command, "--version") && strcmp(Command, "-v")))
			printf("%s", ENGINE_NAME);

		else if (!(strcmp(Command, "--help") && strcmp(Command, "-h")))			/**	Ayuda en pantalla. */
			PrintMenu();

		else if (!(strcmp(Command, "--quit") && strcmp(Command, "-q")))			/**	Sale del programa. */
			break;

		else	{
			BestMove = CheckOneMove(Command, &n);
			if (BestMove.ID)	{												/** Comprueba si llega un movimiento del usuario y es legal. */
				MakeMove(&BestMove);
				PrintResult();													/** Si es final de partida imprime el resultado. */
				Ply = 0;
			}
			else printf("Error (unknown command): %s\n", Command);
		}
	}
}

/********************************************************************************************************
 *  Muestra el menu o la ayuda del motor.																*
 ********************************************************************************************************/
inline void		PrintMenu(void)					{
	printf("\n#");
	printf("\n# --perft $1, -p $1\t\t\tTotal nodes account for a given depth $1 from the perft position");
	printf("\n# --bench, -b\t\t\t\tRun the built in benchmark");

	printf("\n# --board, -s\t\t\t\tPrint the board");
	printf("\n# --fen, -f\t\t\t\tPrint the board in FEN format");
	printf("\n# --moves, -m\t\t\t\tPrint the move list");

	printf("\n# --profile $1\t\t\t\tLoad the profile name $1");

	printf("\n# --pieces, -l\t\t\t\tPrint the piece list");
	printf("\n# --piece_number, -n\t\t\tPrint the piece number");

	printf("\n# --config, -c\t\t\t\tPrint the engine config");
	printf("\n# --version, -v\t\t\t\tPrint the engine version");

	printf("\n# --STS $1 $2, -sts $1 $2\t\tRun the Strategist Test Suit from theme $1 to $2");
	printf("\n# --STStheme $1 $2, -STST $1 $2\t\tPrint the STS result from theme $1 to $2");
	printf("\n# --STSposition $1 $2, -STSP $1 $2\tPrint the STS result from position $1 to $2");
	printf("\n# --STSall $1 $2, -STSA $1 $2\t\tPrint the STS average result from theme $1 to $2");

	printf("\n# --help, -h\t\t\t\tPrint a list of commands");
	printf("\n# --quit, -q\t\t\t\tExit the program");
	printf("\n#");
}

/// 350
