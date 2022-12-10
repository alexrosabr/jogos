#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <bios.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <ctype.h>
#include <mem.h>

#include "keycodes.h"
#include "superkey.h"

#define INTR  9

#define PLACA 8*BARRA
#define HISCORES "TARUMA.TXT"

/*------------------------------ Variaveis -------------------------------*/
void interrupt ( *oldhandler ) ( void );
struct REGPACK reg;
char *pista = "Û                       Û";
char *autor = "Alexsander da Rosa";
char *email = "alexsand@attila.urcamp.tche.br";
unsigned long int score, km, goal, next, bonus;
int lives, vivo, counter, posicao, metade, died, largura, quarto;
void *chao, *tela;
int speed, snake, silencio, restart;
unsigned char aux, level, BARRA = 50;
struct hi_score
    {
    char nome[21];
    unsigned long int pontos;
    } tabela [11];

/*----------------------------- Prototipos -------------------------------*/
int draw_car ( int x, int y, int cor );
int erase_car ( int x, int y );
int add_line ( int x );
int bateu ( void );
int morre ( int x, int y );
int inic_records ( void );
int save_records ( void );
int disk_error ( void );
int verif_record ( void );
int entra_textoxy ( int x, int y, char *dest, char *formato );

/*-------------------------------------------------------------------------*/
void interrupt handler ( void )
{
superkey;
aux = inportb ( 0x61 );
outportb ( 0x61, aux | 0x80 );
outportb ( 0x61, aux );
outportb ( 0x20, 0x20 );
}

/*------------------------------------------------------------------------*/
int draw_car ( int x, int y, int cor )
{
textcolor ( cor );
gotoxy ( x+1, y );
cputs ( "ÜÅÜ" );
gotoxy ( x+1, y + 1 );
cputs ( "ÚÅ¿" );
gotoxy ( x+1, y + 2 );
cputs ( "ÛÍÛ" );
return ( 0 );
}

/*------------------------------------------------------------------------*/
int erase_car ( int x, int y )
{
textcolor ( BLACK );
gotoxy ( x+1, y );
cputs ( "   " );
gotoxy ( x+1, y + 1 );
cputs ( "   " );
gotoxy ( x+1, y + 2 );
cputs ( "   " );
return ( 0 );
}

/*------------------------------------------------------------------------*/
int add_line ( int x )
{
int carro, col;

reg.r_ax = 0x0701;
reg.r_cx = 0x0106;
reg.r_dx = 0x194e;
reg.r_bx = 0x2000;
intr ( 0x10, &reg );

textcolor ( YELLOW );
gotoxy ( x, 2 );
cputs ( pista );

switch (goal-km-18)
    {
    case 1:
	gotoxy ( 7, 2 );
	textbackground ( GREEN );
	textcolor ( BLACK );
	cputs ( "º º" );
	break;
    case 0:
	gotoxy ( 7, 2 );
	textcolor ( LIGHTRED );
	if ( quarto == 1 || quarto == 3 )
	    cputs ( " ¬ " );
	else
	    if ( quarto == 2 )
		cputs ( " « " );
	    else
		cputs ( " ¯ " );
	quarto++;
	break;
    case -1:
	goal += PLACA;
	break;
    default:;
    };
textbackground ( BLACK );

if ( km >= next )
    {
    next += BARRA;
    textbackground ( BLUE );
    textcolor ( LIGHTMAGENTA );
    if ( metade )
	{
	gotoxy ( 2, posicao );
	cputs ( "Û" );
	metade = 0;
	}
    else
	{
	posicao--;
	gotoxy ( 2, posicao );
	cputs ( "Ü" );
	metade = 1;
	}
    textbackground ( BLACK );
    if ( posicao < 9 )
	return ( 1 );
    }

if ( restart )
    return ( 0 );

carro = random ( 5 );
if ( ( counter > 20 && carro > 3 ) || counter > 50 )
    {
    counter = 0;
    carro = 9 + random ( 6 );
    col = x + 3 + random ( largura );
    draw_car ( col, 2, carro );
    }
else
    if ( random ( 1000 ) < 1 )
	{
	col = x + 3 + random ( largura );
	gotoxy ( col, 2 );
	textcolor ( LIGHTRED );
	cputs ( "\003" );
	}

return ( 0 );
}

/*------------------------------------------------------------------------*/
int bateu ( void )
{
int i, resp;
char *oops;

resp = 0;
oops = ( char * ) chao;
for ( i = 0; i < 3; i++ )
    {
    if ( *oops != ' ' )
	switch ( *oops )
	{
	case '\003':
	    if ( lives < 6 )
		{
		lives++;
		textcolor ( LIGHTRED );
		textbackground ( BLUE );
		gotoxy ( lives, 4 );
		cputs ( "\003" );
		textbackground ( BLACK );
		}
	    else
		score += 2500;

	    if ( !silencio )
		sound ( 640 );
	    delay ( 50 );
	    nosound ( );
	    break;
	default:
	    resp = 1;
	}
    oops++;
    oops++;
    }
return ( resp );
}

/*------------------------------------------------------------------------*/
int morre ( int x, int y )
{
int i, tone, tempo, mili;

tone = 300;
tempo = mili = 1;
for ( i = 0; i < 30; i++ )
    {
    if ( !silencio )
	sound ( tone );
    textcolor ( WHITE );
    gotoxy ( x+1, y );
    cputs ( "°±°" );
    gotoxy ( x+1, y+1 );
    cputs ( "±²±" );
    gotoxy ( x+1, y+2 );
    cputs ( "°±°" );
    nosound ( );
    delay ( tempo );

    if ( !silencio )
	sound ( tone );
    textcolor ( YELLOW );
    gotoxy ( x+1, y );
    cputs ( "±²±" );
    gotoxy ( x+1, y+1 );
    cputs ( "²°²" );
    gotoxy ( x+1, y+2 );
    cputs ( "±²±" );
    nosound ( );
    delay ( tempo );

    if ( !silencio )
	sound ( tone );
    textcolor ( LIGHTRED );
    gotoxy ( x+1, y );
    cputs ( "²°²" );
    gotoxy ( x+1, y+1 );
    cputs ( "°±°" );
    gotoxy ( x+1, y+2 );
    cputs ( "²°²" );
    nosound ( );
    delay ( tempo );

    tone -= 3;
    mili++;
    if ( mili > 5 )
	{
	tempo++;
	mili = 0;
	}
    }

nosound ( );
textbackground ( BLUE );
gotoxy ( lives--, 4 );
cputs ( " " );
textbackground ( BLACK );
if ( lives < 1 )
   vivo = 0;
died = 1;
return ( 0 );
}

/*------------------------------------------------------------------------*/
int inic_records ( void )
{
FILE *hst;
int i, j, ok, erro;

for ( i = 0; i < 10; i++ )
    {
    strcpy ( tabela[i].nome, "Red Naxel Software" );
    tabela[i].pontos = 10000 - 1000*i;
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
    ok = fscanf ( hst, "%20s %ld ", tabela[i].nome, &(tabela[i].pontos) );
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
    ok = fprintf ( hst, "%s\n%ld\n", tabela[i].nome, tabela[i].pontos );
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
gettext ( 21, 12, 57, 12, tela );
gotoxy ( 21, 12 );
cputs ( " Erro de disco. Tento de novo? (S/N) " );
key = getch ( );
if ( key == 0 ) getch ( );
puttext ( 21, 12, 57, 12, tela );
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

tabela[10].pontos = score;
tabela[10].nome[0] = '@';
tabela[10].nome[1] = NULL;

for ( i = 0; i < 11; i++ )
    {
    k = i;
    for ( j = i+1; j < 11; j++ )
	if ( tabela[j].pontos > tabela[k].pontos )
	    k = j;
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
    textcolor ( WHITE );
    gotoxy ( 17, 13 );
    cputs ( " Parab‚ns! Vocˆ acaba de quebrar um recorde! " );
    gotoxy ( 32, 14 );
    cputs ( " Digite seu nome: " );
    gotoxy ( 22, 15+k );
    cprintf ( "%2d>", k+1 );
    textbackground ( BLUE );
    _setcursortype ( _NORMALCURSOR );
    erro = entra_textoxy ( 25, 15+k, temp, "%20s" );
    if ( erro == -1 || temp[0] == '\0' )
	strcpy ( temp, "An“nimo" );
    temp[0] = toupper(temp[0]);
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

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
int main ( int argc, char *argv[] )
{
int i, key, muda;
int colp, xis, yps, dx, xant;

if ( argc == 3 && !strcmp(argv[1],"-warp") )
    {
    i = atoi(argv[2]);
    if ( i > 0 && i < 500 )
	BARRA = i;
    }

score = 0;
km = 13;
for ( i = 0; i < 18; i++ )
    {
    score += autor[i] * km;
    km += 13;
    }
if ( score != 205504L || inic_records ( ) )
    return ( 1 );

chao = ( char * ) malloc ( 25 );
tela = ( char * ) malloc ( 80 );

_setcursortype ( _NOCURSOR );
textmode ( C80 );
textbackground ( BLUE );
clrscr ( );

/* Opening screen */
textcolor ( YELLOW );

gotoxy ( 15, 2 );
cputs ( "                                           ÜßÜß" );
gotoxy ( 15, 3 );
cputs ( "                                            °°°°" );
gotoxy ( 15, 4 );
cputs ( "ÛÛÛÛÛÛÛ  ÜÛÛÛÛÜ  ÛÛÛÛÛÜ  Û    Û  Û     Û  ÜÛÛÛÛÜ" );
gotoxy ( 15, 5 );
cputs ( " °°Û°°°° Û°°°°Û° Û°°°°Û° Û°   Û° ÛÛ   ÛÛ° Û°°°°Û°" );
gotoxy ( 15, 6 );
cputs ( "   Û°    Û°   Û° Û°   Û° Û°   Û° Û°Û Û Û° Û°   Û°" );
gotoxy ( 15, 7 );
cputs ( "   Û°    ÛÛÛÛÛÛ° ÛÛÛÛÛ ° Û°   Û° Û° Û °Û° ÛÛÛÛÛÛ°" );
gotoxy ( 15, 8 );
cputs ( "   Û°    Û°°°°Û° Û°°°Û°  Û°   Û° Û°  ° Û° Û°°°°Û°" );
gotoxy ( 15, 9 );
cputs ( "   Û°    Û°   Û° Û°   Û   ÛÛÛÛ ° Û°    Û° Û°   Û°" );
gotoxy ( 15, 10 );
cputs ( "    °     °    °  °    °   °°°°   °     °  °    °  v1.0b" );

gotoxy ( 25, 12 );
cprintf ( "Escrito por %s", autor );
gotoxy ( 25, 13 );
cprintf ( "%s", email );

textcolor ( WHITE );
gotoxy ( 20, 15 );
cputs ( "Use as teclas setas para mover seu carro" );
gotoxy ( 20, 17 );
cputs ( "ESC Aborta     S Som on/off      P Pausa" );

gotoxy ( 18, 20 );
cputs ( "°¯° Fim de N¡vel þ °«° Metade þ * Vida Extra" );
gotoxy ( 18, 21 );
cputs ( "B“nus 5000 x n§ do n¡vel + Vida Extra em °¯°" );

gotoxy ( 22, 24 );
cputs ( "Pressione qualquer tecla pra come‡ar" );

textcolor ( LIGHTRED );
gotoxy ( 20, 17 ); cputs ( "ESC" );
gotoxy ( 35, 17 ); cputs ( "S" );
gotoxy ( 53, 17 ); cputs ( "P" );
gotoxy ( 50, 20 ); cputs ( "\003" );
textbackground ( BLACK );
gotoxy ( 18, 20 ); cputs ( " ¯ " );
gotoxy ( 37, 20 ); cputs ( " « " );
gotoxy ( 59, 21 ); cputs ( " ¯ " );

if ( getch ( ) == 0 ) getch ( );

silencio = 0;

oldhandler = getvect ( INTR );

again:

randomize ( );

textbackground ( BLUE );
clrscr ( );

lives = 5;
counter = 0;
score = km = 0L;
goal = PLACA;
next = BARRA;
vivo = died = 1;
metade = 1;
posicao = 24;
largura = 15;
level = quarto = 1;

textcolor ( WHITE );
gotoxy ( 1, 1 );
cputs ( "Pontos:" );

gotoxy ( 1, 3 );
cputs ( "Vidas" );

gotoxy ( 1, 5 );
cputs ( "v1.0b" );
gotoxy ( 2, 7 );
cputs ( "Fim" );
gotoxy ( 1, 25 );
cputs ( "Sa¡da" );

textcolor ( LIGHTRED );
gotoxy ( 1, 4 );
cputs ( "\003\003\003\003\003" );

textcolor ( YELLOW );
gotoxy ( 3, 8 );
cputs ( "Ü" );
gotoxy ( 3, 24 );
cputs ( "ß" );

for ( i = 9; i < 24; i++ )
    {
    gotoxy ( 3, i );
    if ( ( i % 4 ) == 0 )
	cputs ( "´¬" );
    else
	cputs ( "³" );
    }
gotoxy ( 3, 16 );
cputs ( "Å«" );

textcolor ( LIGHTMAGENTA );
gotoxy ( 2, posicao );
cputs ( "Ü" );

gotoxy ( 1, 6 );
textcolor ( WHITE );
if ( silencio )
    cputs ( "Quieto" );
else
    cputs ( "      " );
textbackground ( BLACK );

speed = 30;		/* Velocidade ( quanto maior, mais lento ) */
snake = 79;		/* Curvas: Quanto maior, menos curvas faz */
strcpy ( pista, "Û                       Û" );

setvect ( INTR, handler );

inicio:

if ( vivo )
    {
    textcolor ( WHITE );
    gotoxy ( 32, 10 );
    cprintf ( "    N¡vel %02d    ", level );
    delay ( 1500 );
    gotoxy ( 33, 10 );
    cputs ( "              " );
    textbackground ( BLUE );
    gotoxy ( 35, 1 );
    cprintf ( " N¡vel %02d ", level );
    textbackground ( BLACK );
    }

if ( died )
    {
    colp = 32;
    xis = xant = 37;
    yps = 20;
    dx = 0;
    }

restart = 1;
if ( vivo && died )
    for ( i = 0; i < 24; i++ )
	add_line ( colp );
restart = 0;

key = died = 0;

inicializa_superkey ( );

while ( vivo )
    {
    textbackground ( BLUE );
    textcolor ( WHITE );
    gotoxy ( 9, 1 );
    cprintf ( "%10lu", score );
    textbackground ( BLACK );
    score += ( 31 - speed );
    counter++;
    km++;

    muda = random ( 100 );
    if ( muda > snake )
	dx = random ( 3 ) - 1;

    colp += dx;
    if ( colp < 7 || colp > largura+39 )
	colp -= dx;

    erase_car ( xant, yps );
    muda = add_line ( colp );

    gettext ( xis+1, yps, xis+3, yps, chao );
    if ( bateu ( ) )
	{
	morre ( xis, yps );
	goto inicio;
	}

    draw_car ( xis, yps, WHITE );
    xant = xis;
    delay ( speed );

    if ( muda )			/* passou de fase ? */
	{
	bonus = 5000L * (unsigned long)level;
	textbackground ( BLUE );
	for ( i = 8; i < 25; i++ )
	    {
	    gotoxy ( 2, i );
	    cputs ( " " );
	    }
	posicao = 24;
	textcolor ( LIGHTMAGENTA );
	gotoxy ( 2, posicao );
	cputs ( "Ü" );
	textbackground ( BLACK );
	if ( lives < 6 )
	    {
	    lives++;
	    textcolor ( LIGHTRED );
	    textbackground ( BLUE );
	    gotoxy ( lives, 4 );
	    cputs ( "\003" );
	    textbackground ( BLACK );
	    }
	else
	    bonus += 2000L * (unsigned long)level;

	if ( !silencio )
	    sound ( 640 );
	delay ( 100 );
	nosound ( );

	textcolor ( WHITE );
	gotoxy ( 32, 10 );
	cprintf ( " B“nus %8lu ", bonus );
	score += bonus;
	level++;
	speed --;
	if ( speed < 1 )
	    speed = 1;
	snake -= 5;
	if ( snake < 5 )
	    snake = 5;
	km = 0;
	next = BARRA;
	goal = PLACA;
	if ( level % 2 == 0 && largura > 10 )
	    {
	    pista [ largura+8 ] = 'Û';
	    pista [ largura+9 ] = NULL;
	    largura--;
	    }
	quarto = 1;
	delay ( 1500 );
	goto inicio;
	}

    if ( teclando[_ESC] )
	{
	vivo = 0;
	break;
	}

    if ( teclando[31] && !key )
	{
	key = 1;
	silencio = !silencio;
	gotoxy ( 1, 6 );
	textbackground ( BLUE );
	textcolor ( WHITE );
	if ( silencio )
	    cputs ( "Quieto" );
	else
	    cputs ( "      " );
	textbackground ( BLACK );
	continue;
	}

    if ( !teclando[31] && key )
	key = 0;

    if ( teclando[25] )
	{
	gettext ( 38, 12, 42, 12, chao );
	gotoxy ( 38, 12 );
	textcolor ( WHITE + BLINK );
	cputs ( "PAUSA" );
	while ( teclando[25] );		/* espera soltar */
	while ( !teclando[25] );	/* espera reapertar */
	while ( teclando[25] );		/* espera soltar */
	puttext ( 38, 12, 42, 12, chao );
	}

    if ( teclando[_RIGHT] )		/* Shift DIREITO */
	{
	gettext ( xis+4, yps, xis+4, yps+2, chao );
	if ( bateu ( ) )
	    {
	    morre ( xis, yps );
	    goto inicio;
	    }
	xant = xis;
	xis++;
	continue;
	}

    if ( teclando[_LEFT] )		/* Shift ESQUERDO */
	{
	gettext ( xis, yps, xis, yps+2, chao );
	if ( bateu ( ) )
	    {
	    morre ( xis, yps );
	    goto inicio;
	    }
	xant = xis;
	xis--;
	}

    }

setvect ( INTR, oldhandler );

while ( kbhit ( ) )
    getch ( );

textbackground ( BLUE );
textcolor ( WHITE );
gotoxy ( 9, 1 );
cprintf ( "%10lu", score );
textbackground ( BLACK );

textcolor ( LIGHTMAGENTA );
gotoxy ( 29 , 10 );
cputs ( " F I M   D E   J O G O " );

verif_record ( );

textcolor ( WHITE );

for ( i = 0; i < 10; i++ )
    {
    gotoxy ( 25, 15+i );
    cprintf ( "%-20s", tabela[i].nome );
    gotoxy ( 45, 15+i );
    cprintf ( "%10ld", tabela[i].pontos );
    }

textcolor ( WHITE+BLINK );
gotoxy ( 32, 12 );
cputs ( " Mais uma? (S/N) " );

key = getch ( );
if ( key == 0 )
    key = getch ( );
if ( toupper ( key ) != 'N' && key != 27 )
    goto again;

textcolor ( LIGHTGRAY );
clrscr ( );
_setcursortype ( _NORMALCURSOR );
return ( 0 );
}
