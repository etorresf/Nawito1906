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

#include <stdint.h>
#include <inttypes.h>

int				HashKey[32];

typedef	enum	HASH_MATERIAL_TYPE{
	KK=0x0, KKP=0xF0000000,KKN=0xE00000, KKB=0x38000, KKR=0xE00, KKQ=0x38, KPK=0xF000000, KNK=0x1C0000, KBK=0x7000, KRK=0x1C0, KQK=0x7,KQKQ=KQK+KKQ,KRKR=KRK+KKR,
	KBKB=KBK+KKB, KNKN=KNK+KKN, KPKP=KPK+KKP, KQKR=KQK+KKR, KRKQ=KRK+KKQ, KRRK=KRK+KRK, KKRR=KKR+KKR,KNKR=KNK+KKR, KBKR=KBK+KKR, KNKB=KNK+KKB,KPKN=KPK+KKN,
	KPKQ=KPK+KKQ, KPKB=KPK+KKB, KKNN=KKN+KKN, KKBN=KKB+KKN, KKBP=KKB+KKP, KPKR=KPK+KKR, KRKN=KRK+KKN, KRKB=KRK+KKB, KBKN=KBK+KKN, KNKP=KNK+KKP,KQKP=KQK+KKP,
	KBKP=KBK+KKP, KNNK=KNK+KNK, KBNK=KBK+KNK, KBPK=KBK+KPK, KRKP=KRK+KKP, KKQB=KKQ+KKB, KQBK=KQK+KBK, KRBK=KRK+KBK, KKBB=KKB+KKB, KKQN=KKQ+KKN, KKRB=KKR+KKB,
	KQKRR=KQK+KKRR, KQKQB=KQK+KKQB, KRBKQ=KRBK+KKQ, KBKBB=KBK+KKBB, KQKQN=KQK+KKQN, KRKBN=KRK+KKBN, KBKBN=KBK+KKBN, KNKBB=KNK+KKBB, KBBK=KBK+KBK,KQNK=KQK+KNK,
	KRRKQ=KRRK+KKQ, KQBKQ=KQBK+KKQ, KQKRB=KQK+KKRB, KBBKB=KBBK+KKB, KQNKQ=KQNK+KKQ, KBNKR=KBNK+KKR, KBNKB=KBNK+KKB, KBBKN=KBBK+KKN, KRNK=KRK+KNK, KKRN=KKR+KKN,
	KRNKQ=KRNK+KKQ, KRKRB=KRK+KKRB, KRKBB=KRK+KKBB, KRKRN=KRK+KKRN, KRKNN=KRK+KKNN, KNKBN=KNK+KKBN, KNKNN=KNK+KKNN, KBKNN=KBK+KKNN, KKNP=KKN+KKP, KKRP=KKR+KKP,
	KQKRN=KQK+KKRN, KRBKR=KRBK+KKR, KBBKR=KBBK+KKR, KRNKR=KRNK+KKR, KNNKR=KNNK+KKR, KBNKN=KBNK+KKN, KNNKN=KNNK+KKN, KNNKB=KNNK+KKB, KKPP=KKP+KKP, KRPK=KRK+KPK,
	KRPKR=KRPK+KKR, KRKBP=KRK+KKBP, KBKBP=KBK+KKBP, KNKBP=KNK+KKBP, KBKNP=KBK+KKNP, KRKNP=KRK+KKNP, KNKPP=KNK+KKPP, KPKNN=KPK+KKNN, KPPK=KPK+KPK, KNPK=KNK+KPK,
	KRKRP=KRK+KKRP, KBPKR=KBPK+KKR, KBPKB=KBPK+KKB, KBPKN=KBPK+KKN, KNPKB=KNPK+KKB, KNPKR=KNPK+KKR, KPPKN=KPPK+KKN, KNNKP=KNNK+KKP, KKPPP=KKPP+KKP, KPPPK=KPPK+KPK,
	KBKPP=KBK+KKPP, KPKPP=KPK+KKPP, KNNKNN=KNNK+KKNN, KRRKRR=KRRK+KKRR, KPPKPP=KPPK+KKPP, KPPKB=KPPK+KKB, KPPKP=KPPK+KKP, KBBKBB=KBBK+KKBB, KRBNK=KRBK+KNK,
	KPPPKPPP=KPPPK+KKPPP, KRBKBB=KRBK+KKBB, KQNKRR=KQNK+KKRR, KRBKNN=KRBK+KKNN, KBNKRB=KBNK+KKRB, KRBNKQ=KRBNK+KKQ, KBBKRN=KBBK+KKRN, KKBNN=KKBN+KKN, KKRBN=KKRB+KKN,
	KBBKRB=KBBK+KKRB, KRRKQN=KRRK+KKQN, KNNKRB=KNNK+KKRB, KRBKBN=KRBK+KKBN, KQKRBN=KQK+KKRBN, KRNKBB=KRNK+KKBB, KBNNK=KBNK+KNK, KKQP=KKQ+KKP, KQPK=KQK+KPK,
	KBBNK=KBBK+KNK, KQPKQ=KQPK+KKQ, KBNKNN=KBNK+KKNN, KKBBN=KKBB+KKN, KQKQP=KQK+KKQP, KNNKBN=KNNK+KKBN, KKRNN=KKRN+KKN, KRNNK=KRNK+KNK, KKRBB= KKRB+KKB,
	KQKRNN=KQK+KKRNN, KQKBNN=KQK+KKBNN, KRKBNN=KRK+KKBNN, KBBNKQ=KBBNK+KKQ, KBNKRN=KBNK+KKRN, KNNKRN=KNNK+KKRN, KBNKBB=KBNK+KKBB, KPKRN=KPK+KKRN, KRBBK=KRBK+KBK,
	KRNNKQ=KRNNK+KKQ, KBNNKQ=KBNNK+KKQ, KBNNKR=KBNNK+KKR, KQKBBN=KQK+KKBBN, KRNKBN=KRNK+KKBN, KRNKNN=KRNK+KKNN, KBBKBN=KBBK+KKBN, KRNKP=KRNK+KKP, KKRBP=KKRB+KKP,
	KQPKQB=KQPK+KKQB, KRPKRB=KRPK+KKRB, KBBKRP=KBBK+KKRP, KBPKBB=KBPK+KKBB, KBBKNP=KBBK+KKNP, KQPKQN=KQPK+KKQN, KBBKNN=KBBK+KKNN, KRPKRN=KRPK+KKRN, KQBPK=KQBK+KPK,
	KQBKQP=KQBK+KKQP, KRBKRP=KRBK+KKRP, KRPKBB=KRPK+KKBB, KBBKBP=KBBK+KKBP, KNPKBB=KNPK+KKBB, KQNKQP=KQNK+KKQP, KNNKBB=KNNK+KKBB, KRNKRP=KRNK+KKRP, KKBBP=KKBB+KKP,
	KBNKRP=KBNK+KKRP, KBPKBN=KBPK+KKBN, KNPKBN=KNPK+KKBN, KNNKRP=KNNK+KKRP, KNNKBP=KNNK+KKBP, KNPKNN=KNPK+KKNN, KPPKNN=KPPK+KKNN, KQKRBB=KQK+KKRBB, KBNPK=KBNK+KPK,
	KRPKBN=KRPK+KKBN, KBNKBP=KBNK+KKBP, KBNKNP=KBNK+KKNP, KRPKNN=KRPK+KKNN, KBPKNN=KBPK+KKNN, KNNKNP=KNNK+KKNP, KNNKPP=KNNK+KKPP, KRBBKQ=KRBBK+KKQ, KKQBP=KKQB+KKP,
	KRBKRB=KRBK+KKRB, KBBNKRR=KBBNK+KKRR, KBNNKRR=KBNNK+KKRR, KQBPKQB=KQBPK+KKQB, KRBKRBP=KRBK+KKRBP, KBBKBBP=KBBK+KKBBP, KBNPKNN=KBNPK+KKNN, KRBPK=KRBK+KPK,
	KBBPK=KBBK+KPK, KKBNP=KKBN+KKP, KKQNP=KKQN+KKP, KRNPK=KRNK+KPK, KQNPK=KQNK+KPK, KQBBK=KQBK+KBK, KKRNP=KKRN+KKP, KNNPK=KNNK+KPK, KKNNP=KKNN+KKP, KRPPK=KRPK+KPK,
	KQBKQB=KQBK+KKQB, KRRKBBN=KRRK+KKBBN, KRRKBNN=KRRK+KKBNN, KQBKQBP=KQBK+KKQBP, KRBPKRB=KRBPK+KKRB, KBBPKBB=KBBPK+KKBB, KNNKBNP=KNNK+KKBNP, KBPPK=KBPK+KPK,
	KQBKQNP=KQBK+KKQNP, KRNPKRB=KRNPK+KKRB, KBNPKBB=KBNPK+KKBB, KQNKQBP=KQNK+KKQBP, KRNKRBP=KRNK+KKRBP, KRNKBBP=KRNK+KKBBP, KQNPKQN=KQNPK+KKQN, KQPPK=KQPK+KPK,
	KQNPKQB=KQNPK+KKQB, KRBKRNP=KRBK+KKRNP, KBBKBNP=KBBK+KKBNP, KQBPKQN=KQBPK+KKQN, KRBPKRN=KRBPK+KKRN, KBBPKRN=KBBPK+KKRN, KQNKQNP=KQNK+KKQNP, KKRPP=KKRP+KKP,
	KRNPKRN=KRNPK+KKRN, KBNPKRN=KBNPK+KKRN, KBNPKBN=KBNPK+KKBN, KNNPKRN=KNNPK+KKRN, KNNPKBN=KNNPK+KKBN, KBBPKNN=KBBPK+KKNN, KRPKBNN=KRPK+KKBNN, KKQPP=KKQP+KKP,
	KRNKRNP=KRNK+KKRNP, KRNKBNP=KRNK+KKBNP, KBNKBNP=KBNK+KKBNP, KRNKNNP=KRNK+KKNNP, KBNKNNP=KBNK+KKNNP, KNNKBBP=KNNK+KKBBP, KBNNKRP=KBNNK+KKRP, KKBPP=KKBP+KKP,
	KNNPKNN=KNNPK+KKNN, KQPPKQB=KQPPK+KKQB, KRPPKRB=KRPPK+KKRB, KBPPKBB=KBPPK+KKBB, KQPPKQN=KQPPK+KKQN, KRPPKRN=KRPPK+KKRN, KBPPKBN=KBPPK+KKBN, KNPPK=KNPK+KPK,
	KNNKNNP=KNNK+KKNNP, KQBKQPP=KQBK+KKQPP, KRBKRPP=KRBK+KKRPP, KBBKBPP=KBBK+KKBPP, KQNKQPP=KQNK+KKQPP, KRNKRPP=KRNK+KKRPP, KBNKBPP=KBNK+KKBPP, KKNPP=KKNP+KKP,
	KNPPKBN=KNPPK+KKBN, KRPPKBNN=KRPPK+KKBNN, KNPPKNN=KNPPK+KKNN, KPPPKNN=KPPPK+KKNN, KRBKBBP=KRBK+KKBBP, KBNKBBP=KBNK+KKBBP, KBBKNNP=KBBK+KKNNP, KBNKNPP=KBNK+KKNPP,
	KBNNKRPP=KBNNK+KKRPP, KNNKNPP=KNNK+KKNPP, KNNKPPP=KNNK+KKPPP, KBBPKRB=KBBPK+KKRB, KBBPKBN=KBBPK+KKBN, KNNPKBB=KNNPK+KKBB, KPPPKPP=KPPPK+KKPP, KPPKPPP=KPPK+KKPPP,
}	HASH_MATERIAL;

uint64_t*		HashTurn;
uint64_t*		HashCasttle;
uint64_t*		HashEnPassant;
uint64_t*		HashPiece[32];

///	60