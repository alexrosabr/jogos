/*-------------------------------------------------------------------------*/
#include <dos.h>
#include <mem.h>
#include <bios.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keycodes.h"
#include "superkey.h"
		      
/*-------------------------------------------------------------------------*/
#define PRETO		0x2100
#define AZUL		0x2200
#define CIANO		0x2400
#define VERMELHO	0x2500
#define MAGENTA		0x2600
#define	MARROM		0x2700
#define BRANCO		0x2800

#define	MOLD_SIMPLES		"Ú¿ÀÙÄ³"
#define MOLD_DUPLA		"É»È¼Íº"

#define INTR		0x09	/* interrupcao do teclado */

/*-------------------------------------------------------------------------*/
char autor[20] = "Alexsander da Rosa";
unsigned long checksum;
unsigned int ( far *video )[80], cronos;
char goleiro[] = " ÛÛÛÛÛÛÛÛ ", buffer[80], moldura[7] = MOLD_SIMPLES;
int speed, dur_jogo, num_times, colorido, apito, play_off;
struct
    {
    int x, y, numero, dx;
    unsigned cor;
    } bj[10];

struct        
    {
    unsigned placar, col;
    } jog[2];

struct
    {
    unsigned char jg, v, e, pg;
    unsigned camisa, reserva, gp, gc;
    char nome[16];
    struct
	{
	unsigned char adver, em_casa;
	unsigned fez, levou;
	} retro[20];
    } team[10];

struct
    {
    unsigned char local, visit;
    unsigned gloc, gvis;
    } history[60];

unsigned char tabela[30], num_jogos, aux;

unsigned char
    tab2 [2] = { 12, 21 },
    tab3 [3] = { 12, 23, 31 },
    tab4 [6] = { 12, 34, 31, 42, 14, 23 },
    tab5[10] = { 12, 34, 51, 23, 45, 13, 24, 35, 41, 52 },
    tab6[15] = { 12, 34, 56, 23, 61, 45, 26, 14, 35, 63, 51, 42, 31, 52, 64 },

    tab7[21] = { 12, 34, 56, 71, 23, 45, 76,
		 13, 57, 24, 61, 35, 27, 46,
		 41, 74, 62, 15, 36, 73, 52 },

    tab8[28] = { 12, 34, 56, 78, 23, 45, 67, 81,
		 75, 24, 31, 68, 53, 64, 17, 82,
		 41, 25, 47, 26, 58, 16, 72, 38,
		 36, 51, 84, 73 };


/*-------------------------------------------------------------------------*/
void interrupt ( *oldhandler ) ( void );

extern void inicializa_superkey ( void );

void desenha_goleiro ( int x, int y, unsigned cor );
void desenha_jogador ( int x, int y, unsigned cor, int numero );
void mostra_tabela ( void );
void show_bar ( int x, int y, int tam, int cor );
int menu_princ ( void );
int desenha_campo ( unsigned corp1, unsigned corp2 );
int start_game ( char *player1, unsigned c1, char *player2, unsigned c2, int duracao );
int entra_textoxy ( int x, int y, char *dest, char *formato );
void campeonato ( void );
void pausa ( int tempo );
void show_retro ( int t1, int t2 );
void config ( int x, int y );
int desenha_moldura ( int x1, int y1, int x2, int y2 );
void historico ( void );

/*-------------------------------------------------------------------------*/
void interrupt handler ( void )
{
superkey;
aux = inportb ( 0x61 );
outportb ( 0x61, aux | 0x80 );
outportb ( 0x61, aux );
outportb ( 0x20, 0x20 );
}

/*-------------------------------------------------------------------------*/
void desenha_goleiro ( int x, int y, unsigned cor )
{
int i;

for ( i = 0; i < 10; i++ )
    video[y][x+i] = goleiro[i] + cor;
return;
}

/*-------------------------------------------------------------------------*/
void desenha_jogador ( int x, int y, unsigned cor, int numero )
{
video[y][x] = ' ' + cor;
video[y][x+1] = 'Û' + cor;
video[y][x+2] = '0' + numero + cor * 16 + 0x200 - 0x1000;
video[y][x+3] = 'Û' + cor;
video[y][x+4] = ' ' + cor;
return;
}

/*-------------------------------------------------------------------------*/
void mostra_tabela ( void )
{
struct
    {
    int nro, saldo;
    double percent;
    unsigned gols, vit, pontos;
    } rasc[10], aux;
char pos;
int i, j, k;

for ( i = 0; i < num_times; i++ )
    {
    rasc[i].nro = i;
    rasc[i].saldo = team[i].gp - team[i].gc;
    rasc[i].percent = 0.0;
    if ( team[i].jg > 0 )
	rasc[i].percent = (double) (100 * (double)(team[i].pg) / (double)(team[i].jg*2) );
    rasc[i].gols = team[i].gp;
    rasc[i].vit = team[i].v;
    rasc[i].pontos = team[i].pg;
    }

for ( i = 0; i < num_times; i++ )
    {
    k = i;
    for ( j = i+1; j < num_times; j++ )
	{
	if ( rasc[j].pontos > rasc[k].pontos )
	    k = j;
	if ( rasc[j].pontos == rasc[k].pontos )
	    {
	    if ( rasc[j].vit > rasc[k].vit )
		k = j;
	    if ( rasc[j].vit == rasc[k].vit )
		{
		if ( rasc[j].saldo > rasc[k].saldo )
		    k = j;
		if ( rasc[j].saldo == rasc[k].saldo )
		    {
		    if ( rasc[j].gols > rasc[k].gols )
			k = j;
		    if ( rasc[j].gols == rasc[k].gols )
			if ( random ( 2 ) )
			   k = j;
		    }
		}
	    }
	}
    if ( k != i )
	{
	aux = rasc[i];
	rasc[i] = rasc[k];
	rasc[k] = aux;
	}
    }

textbackground ( LIGHTGRAY );
textcolor ( BLACK );
clrscr ( );
gotoxy ( 33, 2 );
cputs ( "Classifica‡„o:" );
gotoxy ( 1, 4 );
cputs ( "   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\r" );
cputs ( "       Equipe                PG  JG   V   E   D     GP     GC      S       %\n\r" );
cputs ( "   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\r" );

for ( i = 0; i < num_times; i++ )
    {
    j = rasc[i].nro;
    gotoxy ( 4, 7+i );
    pos = '1' + i;
    if ( ( i > 0 ) && ( team[rasc[i].nro].pg == team[rasc[i-1].nro].pg ) )
	pos = ' ';
    cprintf ( " %c  %-20s  %2d  %2d  %2d  %2d  %2d  %5d  %5d  %5d  %6.2f",
		pos, team[j].nome, team[j].pg, team[j].jg, team[j].v,
		team[j].e, (team[j].jg - (team[j].v+team[j].e)),
		team[j].gp, team[j].gc, rasc[i].saldo, rasc[i].percent );
    }
cputs ( "\n\r   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\r" );
return;
}

/*-------------------------------------------------------------------------*/
void show_bar ( int x, int y, int tam, int cor )
{
int i;

for ( i = 0; i < tam; i++ )
    {
    video[y][x+i] &= 0x0fff;
    video[y][x+i] |= cor;
    }
return;
}

/*-------------------------------------------------------------------------*/
int menu_princ ( void )
{
int key, ext, ret, dado, i, j, moveu, test;
int xis, yps, xant, yant, vx, vy;
unsigned suit;
unsigned char cont, mask, ciclo;

textmode ( C80 );
textcolor ( YELLOW );
textbackground ( CYAN );
_setcursortype ( _NOCURSOR );
clrscr ( );

textcolor ( BLACK );
gotoxy ( 14, 2 );
cputs ( "²²²²²²Ü ²Ü   ²Ü ²²²²²²²Ü ²²²²²²Ü ²²²²²Ü   ²²²²Ü  ²Ü" );
gotoxy ( 14, 3 );
cputs ( "²Ûßßßßß ²Û   ²Û  ßß²Ûßßß ²Ûßßßßß ²Ûßßß²Ü ²Üßßß²Ü ²Û" );
gotoxy ( 14, 4 );
cputs ( "²²²²Ü   ²Û   ²Û    ²Û    ²²²²Ü   ²²²²²Üß ²Û   ²Û ²Û" );
gotoxy ( 14, 5 );
cputs ( "²Ûßßß   ²Û   ²Û    ²Û    ²Ûßßß   ²Ûßßß²Ü ²Û   ²Û ²Û" );
gotoxy ( 14, 6 );
cputs ( "²Û      ²²²²²²Û    ²Û    ²²²²²²Ü ²²²²²Üß  ²²²²Üß ²²²²²²Ü" );
gotoxy ( 14, 7 );
cputs ( " ß       ßßßßßß     ß     ßßßßßß  ßßßßß    ßßßß   ßßßßßß" );

for ( i = 0; i < 7; i++ )
    for ( j = 0; j < 80; j++ )
	if ( video[i][j] == ( 178 + 0x3000 ) )
	    video[i][j] = 'Û' + 0x7f00;

for ( i = 34; i < 47; i++ )
    for ( j = 9; j < 18; j += 2 )
	video[j][i] = 0x3700;

gotoxy ( 38, 10 );
cputs ( "mistoso" );
gotoxy ( 38, 12 );
cputs ( "lay-off" );
gotoxy ( 37, 14 );
cputs ( "ampeonato" );
gotoxy ( 37, 16 );
cputs ( "nstru‡”es" );
gotoxy ( 36, 18 );
cputs ( "c nfigurar" );
textcolor ( RED );
gotoxy ( 37, 10 );
cputs ( "A" );
gotoxy ( 37, 12 );
cputs ( "P" );
gotoxy ( 36, 14 );
cputs ( "C" );
gotoxy ( 36, 16 );
cputs ( "I" );
gotoxy ( 37, 18 );
cputs ( "O" );

textcolor ( BLACK );
gotoxy ( 47, 10 );
cputs ( "Ü" );
gotoxy ( 36, 11 );
cputs ( "ßßßßßßßßßßßß" );
gotoxy ( 47, 12 );
cputs ( "Ü" );
gotoxy ( 36, 13 );
cputs ( "ßßßßßßßßßßßß" );
gotoxy ( 47, 14 );
cputs ( "Ü" );
gotoxy ( 36, 15 );
cputs ( "ßßßßßßßßßßßß" );
gotoxy ( 47, 16 );
cputs ( "Ü" );
gotoxy ( 36, 17 );
cputs ( "ßßßßßßßßßßßß" );
gotoxy ( 47, 18 );
cputs ( "Ü" );
gotoxy ( 36, 19 );
cputs ( "ßßßßßßßßßßßß" );

show_bar ( 34, 9, 12, 0x2000 );
show_bar ( 34, 11, 12, 0x7000 );
show_bar ( 34, 13, 12, 0x700