#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <dos.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <mem.h>
#include "keycodes.h"

#define espera_tecla if ( getch ( ) == 0 ) getch ( )
#define COORD_X 6
#define COORD_Y 6
#define HISCORES "PUZZLE.TXT"

/*------------------------------ Variaveis -------------------------------*/
char autor[] = "Alexsander da Rosa",
     internet[] = "alexsand@attila.urcamp.tche.br";
struct hi_score
    {
    char nome[21];
    unsigned int pontos, lances;
    } tabela [11];
int cor_tecla, cor_fundo, cor_letra, buraco;
int inicial[20], pos[20];
unsigned movimentos, segundos;
unsigned long score, goal, km;
char tecla[20][10];

/*---------------------------- Prototipos ---------------------------------*/
int des_tecla ( int x, int y, char *conteudo );
int desenha_tabuleiro ( int x, int y );
int move_tecla ( int i, int j, int dir );
int move_tecla_dir ( int num );
int move_tecla_esq ( int num );
int move_tecla_cima ( int num );
int move_tecla_baixo ( int num );
int esculhamba ( int vezes );
int inic_records ( void );
int save_records ( void );
int disk_error ( void );
int verif_record ( void );
int entra_textoxy ( int x, int y, char *dest, char *formato );

/*-------------------------------------------------------------------------*/
int des_tecla ( int x, int y, char *conteudo )
{
textcolor ( cor_letra );
textbackground ( cor_tecla );
gotoxy ( x, y );
cputs ( "      " );
gotoxy ( x, y+1 );
cputs ( conteudo );
gotoxy ( x, y+2 );
cputs ( "      " );
textcolor ( BLACK );
textbackground ( cor_fundo );
gotoxy ( x+6, y );
cputs ( "Ü" );
gotoxy ( x+6, y+1 );
cputs ( "Û" );
gotoxy ( x+6, y+2 );
cputs ( "Û" );
gotoxy ( x, y+3 );
cputs ( " ßßßßßß" );
return ( 0 );
}

/*-------------------------------------------------------------------------*/
int desenha_tabuleiro ( int x, int y )
{
int i, j;

for ( i = 0; i < 4; i++ )
    for ( j = 0; j < 4; j++ )
	if ( pos[i+4*j] >= 0 )
	    des_tecla ( x+i*8, y+j*4, tecla[pos[i+4*j]] );
return ( 0 );
}

/*-------------------------------------------------------------------------*/
int move_tecla_dir ( int num )
{
int i, j, a, b;

if ( num < 0 || num % 4 == 3 || pos[num+1] != -1 )
    return ( 1 );

a = COORD_X + ( num % 4 ) * 8;
b = COORD_Y + ( num / 4 ) * 4;
for ( i = 1; i < 9; i++ )
    {
    des_tecla ( a + i, b, tecla[pos[num]] );
    for ( j = 0; j < 4; j++ )
	{
	gotoxy ( a + ( i - 1 ), b + j );
	cputs ( " " );
	}
    delay ( 5 );
    }
pos[num+1] = pos[num];
pos[num] = -1;
buraco--;
return ( 0 );
}

/*-------------------------------------------------------------------------*/
int move_tecla_esq ( int num )
{
int i, j, a, b;

if ( num > 15 || num % 4 == 0 || pos[num-1] != -1 )
    return ( 1 );

a = COORD_X + ( num % 4 ) * 8;
b = COORD_Y + ( num / 4 ) * 4;
for ( i = -1; i > -9; i-- )
    {
    des_tecla ( a + i, b, tecla[pos[num]] );
    for ( j = 0; j < 4; j++ )
	{
	gotoxy ( a + i + 7, b + j );
	cputs ( " " );
	}
    delay ( 5 );
    }
pos[num-1] = pos[num];
pos[num] = -1;
buraco++;
return ( 0 );
}

/*-------------------------------------------------------------------------*/
int move_tecla_baixo ( int num )
{
int j, a, b;

if ( num < 0 || num > 11 || pos[num+4] != -1 )
    return ( 1 );

a = COORD_X + ( num % 4 ) * 8;
b = COORD_Y + ( num / 4 ) * 4;
for ( j = 1; j < 5; j++ )
    {
    des_tecla ( a, b + j, tecla[pos[num]] );
    gotoxy ( a, b + (j-1) );
	cputs ( "        " );
    delay ( 10 );
    }
pos[num+4] = pos[num];
pos[num] = -1;
buraco -= 4;
return ( 0 );
}

/*-------------------------------------------------------------------------*/
int move_tecla_cima ( int num )
{
int j, a, b;

if ( num > 15 || num < 4 || pos[num-4] != -1 )
    return ( 1 );

a = COORD_X + ( num % 4 ) * 8;
b = COORD_Y + ( num / 4 ) * 4;
for ( j = -1; j > -5; j-- )
    {
    des_tecla ( a, b + j, tecla[pos[num]] );
    gotoxy ( a, b + j + 4 );
	cputs ( "        " );
    delay ( 10 );
    }
pos[num-4] = pos[num];
pos[num] = -1;
buraco += 4;
return ( 0 );
}

/*-------------------------------------------------------------------------*/
int esculhamba ( int vezes )
{
int i, val, last, key;

randomize ( );
textcolor ( cor_tecla );
textbackground ( BLACK );
gotoxy ( COORD_X, 3 );
cputs ( "Embaralhando... " );
last = 0;
for ( i = 0; i < vezes; i++ )
    {
denovo:
    if ( kbhit ( ) )
	{
	key = getch ( );
	if ( key == 0 )
	    getch ( );
	if ( key == ESC )
	    return ( 1 );
	}
    val = random ( 4 );
    switch ( val )
	{
	case 0:
	    if ( last != 1 )
		if ( move_tecla_dir ( buraco - 1 ) )
		    goto denovo;
	    break;
	case 1:
	    if ( last != 0 )
		if ( move_tecla_esq ( buraco + 1 ) )
		    goto denovo;
	    break;
	case 2:
	    if ( last != 3 )
		if ( move_tecla_cima ( buraco + 4 ) )
		    goto denovo;
	    break;
	case 3:
	    if ( last != 2 )
		if ( move_tecla_baixo ( buraco - 4 ) )
		    goto denovo;
	    break;
	default:
	    break;
	}
    last = val;
    }
textbackground ( BLACK );
gotoxy ( COORD_X, 3 );
cputs ( "                " );
return ( 0 );
}

/*------------------------------------------------------------------------*/
int inic_records ( void )
{
FILE *hst;
int i, j, ok, erro;

for ( i = 0; i < 10; i++ )
    {
    strcpy ( tabela[i].nome, autor );
    tabela[i].pontos = 60+i*60;
    tabela[i].lances = 999;
    }

_chmod ( HISCORES, 1, FA_ARCH );
maisum:

hst = fopen ( HISCORES, "rb" );
if ( hst == NULL )
    return ( 0 );

erro = 0;
score = 0;
km = 1;
for ( i = 0; i < 10; i++ )
    {
    ok = fscanf ( hst, "%20s %d %d", tabela[i].nome,
        &(tabela[i].pontos), &(tabela[i].lances) );
    score += tabela[i].pontos * km;
    if ( ok == EOF )
	{
	erro = 1;
	break;
	}
    for ( j = 0; j < strlen ( tabela[i].nome ); j++ )
	{
	score += tabela[i].nome[j] * km;
	if ( tabela[i].nome[j] == 'ú' )
	    tabela[i].nome[j] = ' ';
	}
    km += 3;
    }
ok = fscanf ( hst, "%ld ", &goal );
if ( ok == EOF)
    erro = 1;
fclose ( hst );
if ( erro )
    if ( !disk_error ( ) )
	goto maisum;
_chmod ( HISCORES, 1, FA_ARCH|FA_RDONLY|FA_HIDDEN );
if ( goal != score )
    return ( 1 );
return ( 0 );
}

/*------------------------------------------------------------------------*/
int save_records ( void )
{
FILE *hst;
int i, j, ok, erro;

_chmod ( HISCORES, 1, FA_ARCH );
denovo:
hst = fopen ( HISCORES, "wb" );
if ( hst == NULL )
    {
    if ( disk_error ( ) )
	{
	fclose ( hst );
	return ( 1 );
	}
    else
	goto denovo;
    }
erro = 0;
score = 0;
km = 1;
for ( i = 0; i < 10; i++ )
    {
    score += tabela[i].pontos * km;
    for ( j = 0; j < strlen ( tabela[i].nome ); j++ )
	{
	if ( tabela[i].nome[j] == ' ' )
	    tabela[i].nome[j] = 'ú';
	score += tabela[i].nome[j] * km;
	}
    km += 3;
    ok = fprintf ( hst, "%s\n%d\n%d\n", tabela[i].nome,
        tabela[i].pontos, tabela[i].lances );
    if ( ok == EOF )
	{
	erro = 1;
	break;
	}
    for ( j = 0; j < strlen ( tabela[i].nome ); j++ )
	if ( tabela[i].nome[j] == 'ú' )
	    tabela[i].nome[j] = ' ';
    }
ok = fprintf ( hst, "%ld\n", score );
if ( ok == EOF )
    erro = 1;
fclose ( hst );
if ( erro )
    {
    if ( disk_error ( ) )
       return ( 1 );
    else
       goto denovo;
    }
_chmod ( HISCORES, 1, FA_ARCH|FA_RDONLY|FA_HIDDEN );
return ( 0 );
}

/*------------------------------------------------------------------------*/
int disk_error ( void )
{
int key;

textcolor ( LIGHTRED+BLINK );
textbackground ( BLACK );
gotoxy ( 21, 12 );
cputs ( " Erro de disco. Tento de novo? (S/N) " );
key = getch ( );
if ( key == 0 ) getch ( );
if ( toupper ( key ) == 'N' || key == ESC )
    return ( 1 );
return ( 0 );
}

/*------------------------------------------------------------------------*/
int verif_record ( void )
{
int i, j, k, erro;
struct hi_score aux;
char temp[25] = "";

tabela[10].lances = movimentos;
tabela[10].pontos = segundos;
tabela[10].nome[0] = '@';
tabela[10].nome[1] = NULL;

for ( i = 0; i < 11; i++ )
    {
    k = i;
    for ( j = i+1; j < 11; j++ )
        {
	if ( tabela[j].pontos < tabela[k].pontos )
            {
	    k = j;
            continue;
            }
	if ( tabela[j].pontos == tabela[k].pontos )
            if ( tabela[j].lances <= tabela[k].lances )
	        k = j;
        }
    if ( k != i )
	{
	aux = tabela[i];
	tabela[i] = tabela[k];
	tabela[k] = aux;
	}
    }
if ( tabela[10].nome[0] != '@' )
    {
    k = 0;
    for ( j = 0; j < 10; j++ )
	if ( tabela[j].nome[0] == '@' )
	    k = j;
    textbackground ( BLACK );
    textcolor ( LIGHTGRAY );
    for ( i = 0; i < 10; i++ )
	{
	gotoxy ( 45, 9+i );
	cprintf ( "%-20s", tabela[i].nome );
	gotoxy ( 69, 9+i );
	cprintf ( "%02d:%02d",tabela[i].pontos/60, tabela[i].pontos%60 );
        gotoxy ( 75, 9+i );
        cprintf ( "%4d", tabela[i].lances );
	}
    textcolor ( WHITE );
    gotoxy ( 42, 9+k );
    cprintf ( "%2d>", k+1 );
    gotoxy ( 69, 9+k );
    cprintf ( "%02d:%02d", segundos / 60, segundos % 60 );
    gotoxy ( 75, 9+k );
    cprintf ( "%4d", movimentos );
    textbackground ( BLUE );
    _setcursortype ( _NORMALCURSOR );
    erro = entra_textoxy ( 45, 9+k, temp, "%20s" );
    if ( erro == -1 || temp[0] == '\0' )
	strcpy ( temp, "An“nimo" );
    _setcursortype ( _NOCURSOR );
    textbackground ( BLACK );
    strcpy ( tabela[k].nome, temp );
    save_records ( );
    }
return ( 0 );
}

/*------------------------------------------------------------------------*/
int entra_textoxy ( int x, int y, char *dest, char *formato )
/*
Bits em "tipo":
+-----+-------+----------------------------------+
| Bit | Valor | Caracteres que aceita            |
+-----+-------+----------------------------------+
| 0   |   1   | letras                           |
| 1   |   2   | digitos                          |
| 2   |   4   | ponto decimal                    |
| 3   |   8   | sinal                            |
| 4   |  16   | caracteres de path ( \ : . * ? ) |
| 5   |  32   | digitos hexadecimais             |
| 6   |  64   | minusculas                       |
| 7   | 128   | todos                            |
+-----+------+-----------------------------------+
*/
{
	int i, xis, yps, ndx, maxtam, casas, ponto, numcasas,
		apaga, carac_ok, insmode, tamanho, primeira;
	char ext, tecla, *scan, tam [3], *curr, *next, *barra, buffer [80];
	unsigned char tipo;

	i = strlen ( formato );
	if ( i < 2 || i > 7 || formato [ 0 ] != '%' )
		return ( 1 );
	scan = formato;
	tipo = i = ponto = numcasas = 0;
	scan++;
	if ( *scan == '-' )
		{
		scan++;
		tipo |= 8;
		}
        strcpy ( tam, "" );
	while ( isdigit (*scan) && i < 2 )
		{
		tam [i++] = *scan;
		tam [i]='\0';
		scan++;
		}
	if ( isdigit (*scan) )
		return ( 2 );
        casas = 0;
	if ( *scan == '.' )
		{
		tipo |= 4;
		scan++;
		if ( !isdigit (*scan) )
			return ( 1 );
		casas = *scan - '0';
		scan++;
		}
	if ( isdigit (*scan) )
		return ( 2 );
	maxtam = atoi ( tam );
	if ( maxtam < 1 || maxtam > 79 )
		return ( 1 );
	if ( islower (*scan) )
		tipo |= 64;
	switch ( toupper (*scan) )
		{
		case 'N':tipo |= 3;
			break;
		case 'S':tipo |= 1;
			break;
		case 'D':tipo |= 2;
			break;
		case 'A':tipo |= 16;
			break;
		case 'H':tipo |= 32;
			break;
		case 'X':tipo |= 128;
			break;
		default: return ( 3 );
		}
	if ( casas != 0 && ( tipo & 2 ) == 0 )
		return ( 1 );

	xis = x;
	yps = y;
	insmode = 1;
	tecla = 1;
	apaga = 0;

	barra = calloc ( maxtam + 2, sizeof ( char ) );
	setmem ( barra, maxtam + 1, 32 );
	gotoxy ( x, y );
	cputs ( barra );
	free ( barra );

	strcpy ( buffer, dest );
	gotoxy ( x, y );
	cputs ( buffer );
	ndx = tamanho = strlen ( buffer );
	xis += ndx;
	primeira = 1;

	while ( tecla != 13 )
		{
                gotoxy ( xis, yps );
		ext = 1;
		tecla = getch ( );
		if ( tecla == 0 )
			ext = getch ( );
		if ( tecla == 13 )
			primeira = 0;
		if ( tecla == 8 && ndx > 0 )
			{
			primeira = 0;
			ndx--;
			xis--;
			apaga = 1;
                        }
		if ( ext == RIGHT && ndx > 0 )
			{
			primeira = 0;
			ndx--;
			xis--;
                        }
		if ( ext == LEFT && ndx < tamanho )
			{
			primeira = 0;
			ndx++;
			xis++;
			}
		if ( ext == HOME && ndx > 0 )
			{
			primeira = 0;
			ndx = 0;
			xis = x;
			}
                if ( ext == END && ndx < tamanho )
                	{
			primeira = 0;
			ndx = tamanho;
			xis = x + ndx;
			}
		if ( ext == DEL && ndx < tamanho )
			{
			primeira = 0;
			apaga = 1;
			}
		if ( ext == INS )
			insmode = !insmode;
                if ( apaga )
                	{
			next = &buffer [ndx+1];
			curr = &buffer [ndx];
			movmem ( next, curr, tamanho - ndx );
			gotoxy ( x, yps );
			strcat ( buffer, " " );
			cputs ( buffer );
			apaga = 0;
			buffer [ --tamanho ] = '\0';
                        if ( ponto )
                        	{
				numcasas--;
				if ( numcasas < 0 )
					ponto = 0;
				}
			}

		if ( primeira )
			{
			barra = calloc ( maxtam + 2, sizeof ( char ) );
			setmem ( barra, maxtam + 1, 32 );
			gotoxy ( x, y );
			cputs ( barra );
			free ( barra );
			ndx = tamanho = 0;
			xis = x;
			buffer[0] = '\0';
			primeira = 0;
			}

		carac_ok = 0;
                if ( ! ( tipo & 64 ) )
                	tecla = toupper ( tecla );

		if ( tecla == '-' && ndx == 0 && tipo & 8 )
			carac_ok = 1;

		if ( tecla == '.' && !ponto && tipo & 4 && !carac_ok )
			{
			numcasas = 0;
			ponto = 1;
			carac_ok = 1;
			}

		if ( (tecla == '.') || (tecla == '\\') || (tecla == ':') ||
			(tecla == '*') || (tecla == '?') )
			if ( tipo & 16 && !carac_ok )
				carac_ok = 1;

		if ( isalpha ( tecla ) && !carac_ok )
			if ( tipo & 1 || tipo & 16 )
				carac_ok = 1;

		if ( isdigit ( tecla ) && !carac_ok )
			if ( tipo & 2 || tipo & 16 || tipo & 32 )
				{
				carac_ok = 1;
				if ( ponto )
					{
					numcasas++;
					if ( numcasas > casas )
						{
						numcasas--;
						carac_ok = 0;
						}
					}
				}

		if ( isxdigit ( tecla ) && !carac_ok )
			if ( tipo & 32 )
				carac_ok = 1;

		if ( tecla == 32 && tipo & 1 && !carac_ok )
			carac_ok = 1;

		if ( tipo & 128 && tecla > 31 && !carac_ok )
			carac_ok = 1;

		if ( insmode || buffer [ ndx ] == '\0' )
                	{
			if ( carac_ok && tamanho < maxtam )
                		{
				next = &buffer [ndx+1];
				curr = &buffer [ndx];
                                movmem ( curr, next, tamanho - ndx + 1 );
                                tamanho++;
				buffer [ ndx++ ] = tecla;
        	                gotoxy ( xis, yps );
                        	cputs ( curr );
                                xis++;
                                }
                        }
                else
                	{
                        if ( carac_ok )
                        	{
				curr = &buffer [ndx];
				buffer [ ndx++ ] = tecla;
                                gotoxy ( xis, yps );
                        	cputs ( curr );
                                xis++;
                                }
			}
 		if ( tecla == 27 ) break;
		}
	if ( tecla == 27 )
		{
		strcpy ( dest, "" );
		return ( -1 );
		}
	strcpy ( dest, buffer );
	return ( 0 );
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
int main ( void )
{
unsigned long int checksum;
int i, key, ext, ok, z, tamanho;
time_t tempo1;
char *trecho, aux, msg[550]; /*536*/

textmode ( C80 );
_setcursortype ( _NOCURSOR );

inic_records ( );
cor_tecla = LIGHTGRAY;
cor_letra = BLUE;
cor_fundo = GREEN;

strcpy ( msg, "                              " );
strcat ( msg, "                                                          " );
strcat ( msg, "   PUZZLE Vers„o 1.1 - O objetivo do jogo ‚" );
strcat ( msg, " recolocar as pe‡as nos seus lugares originais, ou seja, em" );
strcat ( msg, " ordem crescente, com o 1 no canto superior esquerdo -" );
strcat ( msg, " Use as teclas setas para mover as pe‡as para o lugar vazio -" );
strcat ( msg, " Escrito por " );
strcat ( msg, autor );
strcat ( msg, " - Pressione qualquer tecla para come‡ar " );
strcat ( msg, "                                                          " );
strcat ( msg, "                                                          " );
tamanho = strlen ( msg );

checksum = 0UL;
km = 4;
for ( i = 0; i < strlen ( autor ); i++ )
    {
    checksum += autor[i] * km;
    km += i % 9;
    }
km = 2;
for ( i = 0; i < strlen ( internet ); i++ )
    {
    checksum += internet[i] * km;
    km += i % 13;
    }
if ( checksum != 270561L )
    return ( 0 );

textcolor ( LIGHTGRAY );
textbackground ( BLACK );
textmode ( C80 );
clrscr ( );
gotoxy ( 1, 1 );
cputs ( "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿" );
for ( i = 2; i < 22; i++ )
    {
    gotoxy ( 1, i );
    cputs ( "³" );
    gotoxy ( 80, i );
    cputs ( "³" );
    }
gotoxy ( 1, 24 ); cputs ( "Â" );
gotoxy ( 80, 24 ); cputs ( "Â" );
gotoxy ( 1, 22 ); cputs ( "Á" );
gotoxy ( 80, 22 ); cputs ( "Á" );
gotoxy ( 1, 25 );
cputs ( "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ" );
pokeb ( 0xb800, 3998, 217 );

textcolor ( LIGHTBLUE );
gotoxy ( 30, 1 ); cputs ( "ÛÛÛÛÜÜ  ÛÛ   ÛÛ ÛÛÛÛÛÛÛÛÛß" );
gotoxy ( 30, 2 ); cputs ( "ÛÛ   ÛÛ ÛÛ   ÛÛ     ÜÛÛß ÜÜÜÜÜÜÜÜÜÜ ÜÜ     ÜÜÜÜÜÜ" );
gotoxy ( 30, 3 ); cputs ( "ÛÛÛÛßß  ÛÛ   ÛÛ   ÜÛÛß   ßßßßßÛÛÛß  ÛÛ     ÛÛßßßß" );
gotoxy ( 30, 4 ); cputs ( "ÛÛ      ßÛÛÛÛÛß ÜÛÛÛÛÛÛÛÛÛ  ÜÛÛß    ÛÛ     ÛÛÛÛ" );
gotoxy ( 30, 5 ); cputs ( "                          ÜÛÛÛÜÜÜÜÜ ÛÛÜÜÜÜ ÛÛÜÜÜÜ" );
gotoxy ( 30, 6 ); cputs ( "                         ßßßßßßßßßß ßßßßßß ßßßßßß" );

textcolor ( WHITE );
gotoxy ( 50, 6 ); cputs ( "v1.1" );

textcolor ( YELLOW );
gotoxy ( 55, 20 ); cputs ( "Escrito por:" );
gotoxy ( 52, 21 ); cputs ( autor );

textcolor ( LIGHTGRAY );
gotoxy ( 36, 1 ); cputs ( "ÄÄ" );
gotoxy ( 40, 1 ); cputs ( "ÄÄÄ" );
gotoxy ( 45, 1 ); cputs ( "Ä" );

textcolor ( BLACK );
textbackground ( cor_fundo );
gotoxy ( COORD_X-1, COORD_Y-1 );
cputs ( "ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß" );
for ( i = 0; i < 16; i++ )
    {
    gotoxy ( COORD_X-1, COORD_Y+i );
    cprintf ( "%32c ", ' ' );
    }

inicio:

for ( i = 0; i < 16; i++ )
    {
    pos[i] = inicial [i] = i;
    sprintf ( tecla[i], "  %2d  ", i+1 );
    }
pos[15] = -1;
for ( i = 16; i < 20; i++ )
    inicial[i] = pos[i] = 255;

desenha_tabuleiro ( COORD_X, COORD_Y );

textbackground ( BLACK );
textcolor ( WHITE );
gotoxy ( 45, 8 ); cputs ( "Nome" );
gotoxy ( 69, 8 ); cputs ( "Tempo" );
gotoxy ( 75, 8 ); cputs ( "Movs" );
textcolor ( cor_tecla );
for ( i = 0; i < 10; i++ )
    {
    gotoxy ( 45, 9+i );
    cprintf ( "%-20s", tabela[i].nome );
    gotoxy ( 69, 9+i );
    cprintf ( "%02d:%02d", tabela[i].pontos / 60, tabela[i].pontos % 60 );
    gotoxy ( 75, 9+i );
    cprintf ( "%4d", tabela[i].lances );
    }

gotoxy ( COORD_X, 3 );
cputs ( "Posi‡„o inicial:" );

z = 80;
while ( !kbhit ( ) )
    {
    aux = msg[z];
    msg[z] = NULL;
    trecho = &msg[z-80];
    gotoxy ( 1, 23 );
    cputs ( trecho );
    delay ( 100 );
    msg[z] = aux;
    z++;
    if ( z > tamanho )
       z = 80;
    }
gotoxy ( 1, 23 );
cprintf ( "%30s%30s%20s", "", "", "" );

key = getch ( );
if ( key == 0 ) getch ( );
if ( key == ESC )
    goto final;

buraco = 15;

if ( esculhamba ( 200 ) )
    goto final;

sound ( 440 );
delay ( 100 );
nosound ( );

desenha_tabuleiro ( COORD_X, COORD_Y );

movimentos = 0;

textcolor ( WHITE );
textbackground ( BLACK );
gotoxy ( COORD_X+15, 3 );
cputs ( "Tempo" );
gotoxy ( COORD_X-1, 3 );
cputs ( "Movimentos" );

key = 1;
tempo1 = time ( NULL );
ok = 0;
while ( key != ESC && !ok )
    {
    segundos = ( int ) difftime ( time ( NULL ), tempo1 );
    if ( segundos > 9999 )
	break;

    if ( kbhit ( ) )
	{
	key = getch ( );
	if ( key == 0 )
	    ext = getch ( );
	}
    else
	ext = key = 1;

    switch ( ext )
	{
	case RIGHT:
	    if ( !move_tecla_dir ( buraco - 1 ) )
		{
		sound ( 880 );
		movimentos++;
		}
	    else
		sound ( 262 );
	    delay ( 10 );
	    nosound ( );
	    break;
	case LEFT:
	    if ( !move_tecla_esq ( buraco + 1 ) )
		{
		sound ( 880 );
		movimentos++;
		}
	    else
		sound ( 262 );
	    delay ( 10 );
	    nosound ( );
	    break;
	case UP:
	    if ( !move_tecla_cima ( buraco + 4 ) )
		{
		sound ( 880 );
		movimentos++;
		}
	    else
		sound ( 262 );
	    delay ( 10 );
	    nosound ( );
	    break;
	case DOWN:
	    if ( !move_tecla_baixo ( buraco - 4 ) )
		{
		sound ( 880 );
		movimentos++;
		}
	    else
		sound ( 262 );
	    delay ( 10 );
	    nosound ( );
	    break;
	default:
	    break;
	}
    ok = 1;
    for ( i = 0; i < 15; i++ )
	if ( pos[i] != inicial[i] )
	    ok = 0;

    textcolor ( cor_tecla );
    textbackground ( BLACK );
    gotoxy ( COORD_X-1, 4 );
    cprintf ( "%04d", movimentos );
    gotoxy ( COORD_X+15, 4 );
    cprintf ( "%02d:%02d", segundos / 60, segundos % 60 );

    }

if ( ok )
    {
    for ( i = 0; i < 3; i++ )
	{
	sound ( 990 );
	delay ( 70 );
	nosound ( );
	delay ( 70 );
	}
    verif_record ( );
    }

final:

textcolor ( cor_tecla + BLINK );
textbackground ( BLACK );
gotoxy ( 33, 23 );
cputs ( "Mais uma? (S/N)" );

key = 1;
while ( key != ESC && !isalpha ( key ) )
    {
    key = getch ( );
    if ( key == 0 )
	ext = getch ( );
    }

if ( toupper ( key ) != 'N' && key != ESC )
    goto inicio;

_setcursortype ( _NORMALCURSOR );
textcolor ( LIGHTGRAY );
textbackground ( BLACK );
clrscr ( );
cputs ( "\n\r    Este programa ‚ um " );textcolor ( YELLOW );
cputs ( "freeware" );textcolor ( LIGHTGRAY );
cputs ( ". Isto significa que tu podes copi -lo quantas\n\r" );
cputs ( "vezes quiser, para quem quiser. Caso tenhas interesse em entrar em contato com\n\r" );
cputs ( "o autor, para dar alguma id‚ia, sugest„o ou algo parecido, manda um e-mail para\n\r" );
textcolor ( YELLOW ); cputs ( internet ); textcolor ( LIGHTGRAY );
autor[10] = NULL;
cprintf ( ", ou procura pelo Prof. %s no Curso de\n\r", autor );
cputs ( "Inform tica da URCAMP (Universidade da Regi„o da Campanha) em BAG-RS - Brasil.\n\r" );

return ( 0 );
}
