//---------------------------------------------------------------------------
// Corrida (por enquanto sem nome melhor)
//---------------------------------------------------------------------------
#include "13hlib.h"
#include "superkey.h"
#include "keycodes.h"

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>

#define TIMEINTR     8
#define KEYBINTR     9

#define NUM_CARS     4
#define NUM_PISTAS   2

#define BLOCK_SIZE  10
#define CENTROVERT  60
#define CENTROHOR1  80
#define CENTROHOR2 240

//---------------------------------------------------------------------------
unsigned char syncro, aux;
Mode13hLib Video;

//---------------------------------------------------------------------------
void interrupt ( *oldkeyboard ) ( ... );
void interrupt ( *oldtimer ) ( ... );
void interrupt newkeyb ( ... );
void interrupt newtimer ( ... );
void ErroFatal ( int numero );
void DesenhaPista ( int x1, int y1, int x2, int y2 );
void GeraTabelaIA ( char *nome_arq );

//---------------------------------------------------------------------------
char buffer[30], nome_pista[NUM_PISTAS][9] = { "FIRST", "SECOND" };
Sprite IMG_Carro[NUM_CARS*32];
Sprite Pista[640/BLOCK_SIZE][400/BLOCK_SIZE];
double x[NUM_CARS], y[NUM_CARS], dx[NUM_CARS], dy[NUM_CARS], veloc[NUM_CARS];
int voltas[NUM_CARS], cp1[NUM_CARS], cp2[NUM_CARS], frame[NUM_CARS],
    Posicao[NUM_CARS], terminou[NUM_CARS];
int num_voltas = 50, tabIA[16][20]; // 20x16
/*-------------------------------------------------------------------------*/
void interrupt newkeyb ( ... )
{
superkey;
aux = inportb ( 0x61 );
outportb ( 0x61, aux | 0x80 );
outportb ( 0x61, aux );
outportb ( 0x20, 0x20 );
}

/*-------------------------------------------------------------------------*/
void interrupt newtimer ( ... )
{
syncro++;
oldtimer ( );
}

//---------------------------------------------------------------------------
void ErroFatal ( int numero )
{
Video.CloseMode13h ( );
printf ( "Erro %02d: ", numero );
switch ( numero )
    {
    case 0:
	puts ( "Placa VGA n„o detectada!\n" );
	break;
    case 1:
	puts ( "Sem mem¢ria dispon¡vel! " );
	printf ( "(mem¢ria livre: %lu)\n", farcoreleft ( ) );
	break;
    default:
	puts ( "Desconhecido!" );
	break;
    }
exit ( EXIT_FAILURE );
}

//---------------------------------------------------------------------------
void DesenhaPista ( int x1, int y1, int x2, int y2 )
{
int tx, ty, xd, yd, i, j, a, b, col, lin;

Video.Bar ( 0, 0, 319, 148, 0x04 );

//----------------------------- Vermelho
tx = x1/BLOCK_SIZE;
xd = x1%BLOCK_SIZE;
ty = y1/BLOCK_SIZE;
yd = y1%BLOCK_SIZE;
a = 0;
for ( i = tx; i < tx+16; i++ )
    {
    b = 0;
    for ( j = ty; j < ty+14; j++ )
	{
	col = BLOCK_SIZE*a-xd;
	lin = BLOCK_SIZE*b-yd;
	if ( col >= 0 && lin >= 0 && i >= 0 && i < 64 && j >=0 && j < 40 )
	    Video.DrawSpriteNoTrans ( col, lin, BLOCK_SIZE, BLOCK_SIZE,
		Pista[i][j] );
	b++;
	}
    a++;
    }

for ( i = 0; i < NUM_CARS; i++ )
    {
    if ( i == 0 ) continue;
    xd = x[i]-x1;
    yd = y[i]-y1;
    if ( xd >= 0 && xd < 149 && yd > 0 && yd < 125 )
	Video.DrawSprite ( xd, yd, 24, 24, IMG_Carro[32*i+frame[i]] );
    }

if ( Video.GetPixel ( CENTROHOR1+4 , CENTROVERT+4  ) > 0x7f ||
    Video.GetPixel ( CENTROHOR1+19, CENTROVERT+4  ) > 0x7f ||
    Video.GetPixel ( CENTROHOR1+4 , CENTROVERT+19 ) > 0x7f ||
    Video.GetPixel ( CENTROHOR1+19, CENTROVERT+19 ) > 0x7f )
    {
    if ( Video.GetPixel ( CENTROHOR1+4 , CENTROVERT+4  ) < 0xa0 ||
	Video.GetPixel ( CENTROHOR1+19, CENTROVERT+4  ) < 0xa0 ||
	Video.GetPixel ( CENTROHOR1+4 , CENTROVERT+19 ) < 0xa0 ||
	Video.GetPixel ( CENTROHOR1+19, CENTROVERT+19 ) < 0xa0 )
	veloc[0] = -(veloc[0]/1.5);
    else
	veloc[0] = 0;
    }

//----------------------------- Azul
tx = x2/BLOCK_SIZE;
xd = x2%BLOCK_SIZE;
ty = y2/BLOCK_SIZE;
yd = y2%BLOCK_SIZE;
a = 0;
for ( i = tx; i < tx+16; i++ )
    {
    b = 0;
    for ( j = ty; j < ty+14; j++ )
	{
	col = 160+BLOCK_SIZE*a-xd;
	lin = BLOCK_SIZE*b-yd;
	if ( col < 310 && lin >= 0 && i >= 0 && i < 64 && j >=0 && j < 40 )
	    Video.DrawSpriteNoTrans ( col, lin, BLOCK_SIZE, BLOCK_SIZE,
		Pista[i][j] );
	b++;
	}
    a++;
    }

for ( i = 0; i < NUM_CARS; i++ )
    {
    if ( i == 1 ) continue;
    xd = x[i]-x2;
    yd = y[i]-y2;
    if ( xd >= 0 && xd < 149 && yd > 0 && yd < 125 )
	Video.DrawSprite ( 160+xd, yd, 24, 24, IMG_Carro[32*i+frame[i]] );
    }

if ( Video.GetPixel ( CENTROHOR2+4 , CENTROVERT+4  ) > 0x7f ||
    Video.GetPixel ( CENTROHOR2+19, CENTROVERT+4  ) > 0x7f ||
    Video.GetPixel ( CENTROHOR2+4 , CENTROVERT+19 ) > 0x7f ||
    Video.GetPixel ( CENTROHOR2+19, CENTROVERT+19 ) > 0x7f )
    {
    if ( Video.GetPixel ( CENTROHOR2+4 , CENTROVERT+4  ) < 0xa0 ||
	Video.GetPixel ( CENTROHOR2+19, CENTROVERT+4  ) < 0xa0 ||
	Video.GetPixel ( CENTROHOR2+4 , CENTROVERT+19 ) < 0xa0 ||
	Video.GetPixel ( CENTROHOR2+19, CENTROVERT+19 ) < 0xa0 )
	veloc[1] = -(veloc[1]/1.5);
    else
	veloc[1] = 0;
    }

//----------------------------- Molduras
Video.Bar ( 0, 0, 319, 10, 0xF2 );
Video.Bar ( 0, 131, 319, 17, 0xF2 );
Video.Bar ( 0, 0, 10, 148, 0xF2 );
Video.Bar ( 150, 0, 20, 148, 0xF2 );
Video.Bar ( 310, 0, 10, 148, 0xF2 );
Video.Rectangle ( 0, 0, 319, 148, 0x11 );
Video.Rectangle ( 10, 9, 140, 122, 0x11 );
Video.Rectangle ( 170, 9, 140, 122, 0x11 );

return;
}

//---------------------------------------------------------------------------
void GeraTabelaIA ( char *nome_arq )
{
FILE *arq;
int i, j, xi, yi, a, b, tecla, ext, lin, col;

lin = col = 0;
xi = yi = 0;
tecla = 1;
while ( tecla != 27 )
    {
    a = 0;
    for ( i = xi; i < xi+32; i++ )
	{
	b = 0;
	for ( j = yi; j < yi+20; j++ )
	    {
	    Video.DrawSpriteNoTrans ( 10*a, 10*b, BLOCK_SIZE, BLOCK_SIZE,
		Pista[i][j] );
	    b++;
	    }
	a++;
	}

    for ( i = 0; i < 10; i++ )
	for ( j = 0; j < 8; j++ )
	    {
	    a = ((yi>0)?8:0)+j;
	    b = ((xi>0)?10:0)+i;
	    Video.DrawSprite ( i*32, j*25, 24, 24,
		IMG_Carro[tabIA[a][b]] );
	    }

    a = ((yi>0)?8:0)+lin;
    b = ((xi>0)?10:0)+col;

    Video.DrawSprite ( col*32, lin*25, 24, 24,
	IMG_Carro[32+tabIA[a][b]] );

    Video.CopyPageToScreen ( );
    ext = 1;
    tecla = getch ( );
    if ( tecla == 0 ) ext = getch ( );

    if ( tecla == '+' )
	{
	tabIA[a][b]++;
	if ( tabIA[a][b] > 31 )
	    tabIA[a][b] = 0;
	}

    if ( tecla == '-' )
	{
	tabIA[a][b]--;
	if ( tabIA[a][b] < 0 )
	    tabIA[a][b] = 31;
	}

    if ( ext == LEFT && col > 0 )
	col--;

    if ( ext == RIGHT && col < 9 )
	col++;

    if ( ext == UP && lin > 0 )
	lin--;

    if ( ext == DOWN && lin < 7 )
	lin++;

    if ( ext == HOME ) { xi =  0; yi =  0; }
    if ( ext == END )  { xi =  0; yi = 20; }
    if ( ext == PGUP ) { xi = 32; yi =  0; }
    if ( ext == PGDN ) { xi = 32; yi = 20; }

    }

arq = fopen ( nome_arq, "wb" );
if ( fwrite ( tabIA, sizeof(int), 320, arq ) != 320 )
    {
    cout << "Erro gravando arquivo \"" << nome_arq << "\" !\n";
    return;
    }
fclose ( arq );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void main ( int argc, char *argv[] )
{
clock_t largada[NUM_CARS], chegada[NUM_CARS],
    v1[NUM_CARS], v2[NUM_CARS] /*, v3[NUM_CARS]*/;
double TotalTime[NUM_CARS], BestLap[NUM_CARS], AvgLap[NUM_CARS], Tempo;
int lider = 1, num_pista = 0, ext, tecla;
Sprite IMG_Letra[100];
Sprite Mapa[NUM_PISTAS], Velocimetro;
Sprite Frase[2];
FILE *arq;
unsigned long int mem_antes, mem_depois;
int i, j, ndx = 0, direcao;
double angulo, velx, vely;

mem_antes = farcoreleft ( );
if ( mem_antes < 442096L )
    {
    cout << "Sem mem¢ria convencional dispon¡vel!\n";
    cout << "M¡nimo necess rio: 442096 bytes (mais o espa‡o para o programa).\n";
    cout << "Vocˆ tem apenas " << mem_antes << " bytes.\n";
    return;
    }

for ( i = 0; i < 16; i++ )
    for ( j = 0; j < 20; j++ )
	tabIA[i][j] = 0;

if ( !Video.DetectVGA ( ) )
    ErroFatal ( 0 );

Video.SetMode13h ( );

Video.SetUpPage ( );
if ( !Video.PageActive ( ) )
    ErroFatal ( 1 );

//--------------------------------------- Abertura
Video.LoadPCXFile ( "painel.pcx" );
Velocimetro = Video.GetSprite ( 6, 154, 40, 40 );
Video.CopyPageToScreen ( );

//--------------------------------------- Vermelho
Video.LoadPCXFile ( "carro1.pcx" );
for ( i = 0; i <= 8; i++ )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 0, 24, 24 );
for ( i = 7; i >= 0; i-- )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 30, 24, 24 );
for ( i = 7; i >= 0; i-- )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 90, 24, 24 );
for ( i = 1; i <= 7; i++ )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 60, 24, 24 );

//--------------------------------------- Azul
Video.LoadPCXFile ( "carro2.pcx" );
for ( i = 0; i <= 8; i++ )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 0, 24, 24 );
for ( i = 7; i >= 0; i-- )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 30, 24, 24 );
for ( i = 7; i >= 0; i-- )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 90, 24, 24 );
for ( i = 1; i <= 7; i++ )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 60, 24, 24 );

//--------------------------------------- Verde
Video.LoadPCXFile ( "carro3.pcx" );
for ( i = 0; i <= 8; i++ )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 0, 24, 24 );
for ( i = 7; i >= 0; i-- )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 30, 24, 24 );
for ( i = 7; i >= 0; i-- )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 90, 24, 24 );
for ( i = 1; i <= 7; i++ )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 60, 24, 24 );

//--------------------------------------- Amarelo
Video.LoadPCXFile ( "carro4.pcx" );
for ( i = 0; i <= 8; i++ )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 0, 24, 24 );
for ( i = 7; i >= 0; i-- )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 30, 24, 24 );
for ( i = 7; i >= 0; i-- )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 90, 24, 24 );
for ( i = 1; i <= 7; i++ )
    IMG_Carro[ndx++] = Video.GetSprite ( i*24, 60, 24, 24 );

//--------------------------------------- Mapas das pistas
Video.LoadPCXFile ( "pistas.pcx" );
for ( i = 0; i < NUM_PISTAS; i++ )
    Mapa[i] = Video.GetSprite ( 80*i, 50*(i/4), 80, 50 );

//--------------------------------------- Letras
Video.LoadPCXFile ( "letras.pcx" );
for ( i = 0; i < 10; i++ )
    IMG_Letra[i] = Video.GetSprite ( 9+i*8, 100, 8, 12 );
IMG_Letra[10] = Video.GetSprite ( 120, 100, 8, 12 );
for ( i = 0; i < 10; i++ )
    IMG_Letra[11+i] = Video.GetSprite ( 10+i*7, 120, 7, 10 );

Frase[0] = Video.GetSprite ( 9, 140, 230, 12 );
Frase[1] = Video.GetSprite ( 9, 160, 160, 12 );

Restart:

//--------------------------------------- Painel
Video.LoadPCXFile ( "painel.pcx" );

mem_depois = farcoreleft ( );

//--------------------------------------- Escolhe a pista
tecla = 1;
while ( tecla != ENTER )
    {
    Video.Rectangle ( 149, 123, 18, 14, 4 );
    Video.Bar ( 150, 124, 16, 13, 134 );
    Video.DrawSprite (  150, 125, 12, 8, IMG_Letra[num_voltas/10] );
    Video.DrawSprite ( 158, 125, 12, 8, IMG_Letra[num_voltas%10] );
    Video.DrawSpriteNoTrans ( 120, 149, 80, 50, Mapa[num_pista] );
    Video.CopyPageToScreen ( );

    ext = 1;
    tecla = getch ( );
    if ( tecla == 0 ) ext = getch ( );

    if ( ext == UP )
	{
	num_voltas++;
	if ( num_voltas > 99 )
	    num_voltas = 99;
	}

    if ( ext == DOWN )
	{
	num_voltas--;
	if ( num_voltas < 5 )
	    num_voltas = 5;
	}

    if ( ext == PGUP )
	{
	num_voltas += 10;
	if ( num_voltas > 99 )
	    num_voltas = 99;
	}

    if ( ext == PGDN )
	{
	num_voltas -= 10;
	if ( num_voltas < 5 )
	    num_voltas = 5;
	}

    if ( ext == LEFT && num_pista > 0 )
	num_pista--;
    if ( ext == RIGHT && num_pista < NUM_PISTAS-1 )
	num_pista++;

    if ( tecla == ESC ) goto Fim;
    }

//--------------------------------------- Pista NW
sprintf ( buffer, "%s1.PCX", nome_pista[num_pista] );
Video.LoadPCXFile ( buffer );
for ( i = 0; i < 32; i++ )
    for ( j = 0; j < 20; j++ )
	Pista[i][j] = Video.GetSprite ( BLOCK_SIZE*i, BLOCK_SIZE*j, BLOCK_SIZE, BLOCK_SIZE );

//--------------------------------------- Pista NE
sprintf ( buffer, "%s2.PCX", nome_pista[num_pista] );
Video.LoadPCXFile ( buffer );
for ( i = 0; i < 32; i++ )
    for ( j = 0; j < 20; j++ )
	Pista[32+i][j] = Video.GetSprite ( BLOCK_SIZE*i, BLOCK_SIZE*j, BLOCK_SIZE, BLOCK_SIZE );

//--------------------------------------- Pista SW
sprintf ( buffer, "%s3.PCX", nome_pista[num_pista] );
Video.LoadPCXFile ( buffer );
for ( i = 0; i < 32; i++ )
    for ( j = 0; j < 20; j++ )
	Pista[i][20+j] = Video.GetSprite ( BLOCK_SIZE*i, BLOCK_SIZE*j, BLOCK_SIZE, BLOCK_SIZE );

//--------------------------------------- Pista SE
sprintf ( buffer, "%s4.PCX", nome_pista[num_pista] );
Video.LoadPCXFile ( buffer );
for ( i = 0; i < 32; i++ )
    for ( j = 0; j < 20; j++ )
	Pista[32+i][20+j] = Video.GetSprite ( BLOCK_SIZE*i, BLOCK_SIZE*j, BLOCK_SIZE, BLOCK_SIZE );

//--------------------------------------- Carrega dados da IA
sprintf ( buffer, "%s.DAT", nome_pista[num_pista] );
arq = fopen ( buffer, "rb" );
if ( arq != NULL )
    {
    if ( fread ( tabIA, sizeof(int), 320, arq ) != 320 )
	{
	cout << "Erro lendo arquivo! \"" << buffer << "\" !\n";
	return;
	}
    fclose ( arq );
    }

//--------------------------------------- Parƒmetros
if ( argc == 2 )
    {
    if ( strcmp ( argv[1], "-ia" ) == 0 )
	GeraTabelaIA ( buffer );
    }

//--------------------------------------- Dados basicos dos carros
for ( i = 0; i < NUM_CARS; i++ )
    {
    cp1[i] = cp2[i] = 0;
    voltas[i] = 0;
    frame[i] = 8;
    dx[i] = 0.0; dy[i] = 0.0; veloc[i] = 0.0;
    Posicao[i] = 4;
    terminou[i] = 0;
    }
x[0] = 235.0; y[0] = 342.0;
x[1] = 255.0; y[1] = 315.0;
x[2] = 260.0; y[2] = 342.0;
x[3] = 280.0; y[3] = 315.0;

oldtimer = getvect ( TIMEINTR );
setvect ( TIMEINTR, newtimer );

oldkeyboard = getvect ( KEYBINTR );
setvect ( KEYBINTR, newkeyb );

inicializa_superkey ( );
if ( kbhit ( ) ) if ( getch ( ) == 0 ) getch ( );

//--------------------------------------- Painel
Video.LoadPCXFile ( "painel.pcx" );

DesenhaPista ( x[0]-CENTROHOR1, y[0]-CENTROVERT,
    x[1]-CENTROHOR1, y[1]-CENTROVERT );

//----------------------------- Desenha os carrinhos
Video.DrawSprite ( CENTROHOR1, CENTROVERT, 24, 24, IMG_Carro[frame[0]] );
Video.DrawSprite ( CENTROHOR2, CENTROVERT, 24, 24, IMG_Carro[32+frame[1]] );

//----------------------------- Mostra o Mapa
Video.Rectangle ( 119, 148, 81, 51, 0x11 );
Video.DrawSpriteNoTrans ( 120, 149, 80, 50, Mapa[num_pista] );
for ( i = 0; i < NUM_CARS; i++ )
    {
    Video.PlotPixel ( 120+x[i]/8, 149+y[i]/8, 0x04 );
    Video.PlotPixel ( 121+x[i]/8, 149+y[i]/8, 0x04 );
    Video.PlotPixel ( 119+x[i]/8, 150+y[i]/8, 0x04 );
    Video.PlotPixel ( 120+x[i]/8, 150+y[i]/8, 130+i*4 );
    Video.PlotPixel ( 121+x[i]/8, 150+y[i]/8, 129+i*4 );
    Video.PlotPixel ( 122+x[i]/8, 150+y[i]/8, 0x04 );
    Video.PlotPixel ( 119+x[i]/8, 151+y[i]/8, 0x04 );
    Video.PlotPixel ( 120+x[i]/8, 151+y[i]/8, 129+i*4 );
    Video.PlotPixel ( 121+x[i]/8, 151+y[i]/8, 128+i*4 );
    Video.PlotPixel ( 122+x[i]/8, 151+y[i]/8, 0x04 );
    Video.PlotPixel ( 120+x[i]/8, 152+y[i]/8, 0x04 );
    Video.PlotPixel ( 121+x[i]/8, 152+y[i]/8, 0x04 );
    }
Video.CopyPageToScreen ( );
delay ( 1000 );

//---------------------------------- Laco principal
mem_depois = farcoreleft ( );

for ( i = 0; i < NUM_CARS; i++ )
    {
    v1[i] = largada[i] = clock ( );
    BestLap[i] = 99.999;
    }
syncro = 0;
while ( lider < 4 )
    {
    //----------------------------- Mostra o relogio "real time"
/*    for ( i = 0; i < 2; i++ )
	{
	v3[i] = clock( );
	Tempo = (v3[i]-v1[i])/CLK_TCK;
	Video.Bar ( 150*i+55, 155, 60, 10, 31 );
	sprintf ( buffer, "%6.3lf", Tempo );
	for ( j = 0; j < strlen(buffer); j++ )
	    if ( isdigit(buffer[j]) )
		Video.DrawSprite ( 150*i+55+j*8, 155,
		10, 7, IMG_Letra[buffer[j]-37] );
	}*/

    //----------------------------- Testa se alguem cruzou a linha
    for ( i = 0; i < NUM_CARS; i++ )
	{
	if ( !cp1[i] && x[i] > 420 && y[i] < 150 )
	    cp1[i] = 1;
	if ( !cp2[i] && x[i] < 200 && y[i] < 150 )
	    cp2[i] = 1;
	}
    for ( i = 0; i < NUM_CARS; i++ )
	if ( cp1[i] && cp2[i] && dx[i] > 0.5
	    && x[i] > 310 && x[i] < 330 && y[i] > 310 && y[i] < 390 )
	    {
	    v2[i] = clock ( );
	    Tempo = (v2[i]-v1[i])/CLK_TCK;
	    if ( i < 2 )
		{
		Video.Bar ( 150*i+55, 170, 62, 10, 31 );
		sprintf ( buffer, "%8.3lf", Tempo );
		for ( j = 0; j < strlen(buffer); j++ )
		    if ( isdigit(buffer[j]) )
			Video.DrawSprite ( 150*i+55+j*8, 170,
			10, 7, IMG_Letra[buffer[j]-37] );
		Video.Bar ( 150*i+90, 178, 1, 2, 0x04 );
		Video.Bar ( 150*i+89, 177, 1, 2, 0x01 );
		}
	    if ( Tempo < BestLap[i] )
		{
		BestLap[i] = Tempo;
		if ( i < 2 )
		    {
		    Video.Bar ( 150*i+55, 185, 62, 10, 31 );
		    sprintf ( buffer, "%8.3lf", BestLap[i] );
		    for ( j = 0; j < strlen(buffer); j++ )
			if ( isdigit(buffer[j]) )
			    Video.DrawSprite ( 150*i+55+j*8, 185,
			    10, 7, IMG_Letra[buffer[j]-37] );
		    Video.Bar ( 150*i+90, 193, 1, 2, 0x04 );
		    Video.Bar ( 150*i+89, 192, 1, 2, 0x01 );
		    }
		}
	    v1[i] = v2[i];
	    voltas[i]++;
	    if ( voltas[i] > 99 ) voltas[i] = 99;
	    if ( voltas[i] >= num_voltas && !terminou[i] )
		{
		chegada[i] = v2[i];
		Posicao[i] = lider++;
		terminou[i] = 1;
		}
	    cp1[i] = cp2[i] = 0;
	    }

    //----------------------------- Desenha a pista
    DesenhaPista ( x[0]-CENTROHOR1, y[0]-CENTROVERT,
	x[1]-CENTROHOR1, y[1]-CENTROVERT );

    //----------------------------- Total de voltas
    Video.DrawSprite (  152, 135, 12, 8, IMG_Letra[num_voltas/10] );
    Video.DrawSprite ( 160, 135, 12, 8, IMG_Letra[num_voltas%10] );

    //----------------------------- Contador de voltas
    for ( i = 0; i < NUM_CARS; i++ )
	{
	switch ( i )
	    {
	    case 0: j = 70; break;
	    case 1: j = 230; break;
	    case 2: j = 130; break;
	    case 3: j = 172; break;
	    default:;break;
	    }
	Video.Rectangle ( j, 133, 18, 14, 4 );
	Video.Bar ( j+1, 134, 16, 13, 130+i*4 );
	if ( terminou[i] )
	    {
	    Video.DrawSprite ( j+1, 135, 12, 8, IMG_Letra[Posicao[i]] );
	    Video.DrawSprite ( j+9, 135, 12, 8, IMG_Letra[10] );
	    }
	else
	    {
	    Video.DrawSprite (  j+1, 135, 12, 8, IMG_Letra[voltas[i]/10] );
	    Video.DrawSprite ( j+9, 135, 12, 8, IMG_Letra[voltas[i]%10] );
	    }
	}

    //----------------------------- Desenha os carrinhos
    Video.DrawSprite ( CENTROHOR1, CENTROVERT, 24, 24, IMG_Carro[frame[0]] );
    Video.DrawSprite ( CENTROHOR2, CENTROVERT, 24, 24, IMG_Carro[32+frame[1]] );

    //----------------------------- Mostra o Mapa
    Video.DrawSpriteNoTrans ( 120, 149, 80, 50, Mapa[num_pista] );
    for ( i = 0; i < NUM_CARS; i++ )
	{
	Video.PlotPixel ( 120+x[i]/8, 149+y[i]/8, 0x04 );
	Video.PlotPixel ( 121+x[i]/8, 149+y[i]/8, 0x04 );
	Video.PlotPixel ( 119+x[i]/8, 150+y[i]/8, 0x04 );
	Video.PlotPixel ( 120+x[i]/8, 150+y[i]/8, 130+i*4 );
	Video.PlotPixel ( 121+x[i]/8, 150+y[i]/8, 129+i*4 );
	Video.PlotPixel ( 122+x[i]/8, 150+y[i]/8, 0x04 );
	Video.PlotPixel ( 119+x[i]/8, 151+y[i]/8, 0x04 );
	Video.PlotPixel ( 120+x[i]/8, 151+y[i]/8, 129+i*4 );
	Video.PlotPixel ( 121+x[i]/8, 151+y[i]/8, 128+i*4 );
	Video.PlotPixel ( 122+x[i]/8, 151+y[i]/8, 0x04 );
	Video.PlotPixel ( 120+x[i]/8, 152+y[i]/8, 0x04 );
	Video.PlotPixel ( 121+x[i]/8, 152+y[i]/8, 0x04 );
	}

    //-------------------------------------- Velocimetros
    Video.DrawSpriteNoTrans ( 6, 154, 40, 40, Velocimetro );
    Video.DrawSpriteNoTrans ( 272, 154, 40, 40, Velocimetro );
    for ( i = 0; i < 2; i++ )
	{
	angulo = M_PI - 1.0 + fabs(veloc[i]) / 2.5;
	velx = 26 + i*266;
	vely = 174;
	Video.Line ( velx, vely,
	    velx+cos(angulo)*18, vely+sin(angulo)*18, 0xFF );
	}

    Video.CopyPageToScreen ( );
    while ( syncro < 1 );
    syncro = 0;

    if ( teclando[1] )
	{
	lider = 99;
	break;
	}

    if ( voltas[1] >= num_voltas ) goto ProxCarro;

    //-------------------------------------- Carro Azul
    if ( teclando[UP] )
	{
	frame[1]--;
	if ( frame[1] < 0 ) frame[1] = 31;
	}
    if ( teclando[PGUP] )
	{
	frame[1]++;
	if ( frame[1] > 31 ) frame[1] = 0;
	}
    if ( teclando[HOME] && veloc[1] < 12.0 ) // acelerador
	veloc[1] += 0.40;
    if ( teclando[LEFT] && veloc[1] > -2.0 ) // freio/marcha a re'
	veloc[1] -= 0.40;

ProxCarro:
    if ( !teclando[HOME] && veloc[1] > 0.0 )
	{
	veloc[1] -= 0.30;
	if ( veloc[1] < 0.0 ) veloc[1] = 0.0;
	}
    if ( !teclando[LEFT] && veloc[1] < 0.0 )
	{
	veloc[1] += 0.30;
	if ( veloc[1] > 0.0 ) veloc[1] = 0.0;
	}

    if ( voltas[0] >= num_voltas ) goto CarrosCPU;

    //-------------------------------------- Carro Vermelho
    if ( teclando[17] ) // era 30 (A)
	{
	frame[0]--;
	if ( frame[0] < 0 ) frame[0] = 31;
	}
    if ( teclando[18] ) // era 32 (D)
	{
	frame[0]++;
	if ( frame[0] > 31 ) frame[0] = 0;
	}
    if ( teclando[16] && veloc[0] < 12.0 ) // acelerador
	veloc[0] += 0.40;
    if ( teclando[30] && veloc[0] > -2.0 ) // freio/marcha a re'
	veloc[0] -= 0.40;

CarrosCPU:
    if ( !teclando[16] && veloc[0] > 0.0 ) // era 17 (W)
	{
	veloc[0] -= 0.30;
	if ( veloc[0] < 0.0 ) veloc[0] = 0.0;
	}
    if ( !teclando[30] && veloc[0] < 0.0 ) // era 31 (S)
	{
	veloc[0] += 0.30;
	if ( veloc[0] > 0.0 ) veloc[0] = 0.0;
	}

    //---------------------------- IA dos carros verde e amarelo
    for ( i = 2; i < NUM_CARS; i++ )
	{
	direcao = tabIA[(y[i]+12)/25][(x[i]+12)/32];
	if ( frame[i] != direcao )
	    {
	    if ( frame[i] > direcao )
		{
		if ( frame[i]-direcao > 16 )
		    {
		    frame[i]++;
		    if ( frame[i] > 31 ) frame[i] = 0;
		    }
		else
		    frame[i]--;
		}
	    else
		{
		if ( direcao-frame[i] > 16 )
		    {
		    frame[i]--;
		    if ( frame[i] < 0 ) frame[i] = 31;
		    }
		else
		    frame[i]++;
		}
	    }
	switch ( i )
	    {
	    case 2:
		if ( veloc[i] < 10.0 ) veloc[i] += 0.25;
		break;
	    default:
		if ( veloc[i] < 12.0 ) veloc[i] += 0.25;
		break;
	    }
	}

    //---------------------------- Atualiza posicoes
    for ( i = 0; i < NUM_CARS; i++ )
	{
	if ( voltas[i] >= num_voltas ) frame[i] = 8;
	switch ( frame[i] )
	    {
	    case  0: dx[i] =  0.000; dy[i] = -1.000; break;
	    case  1: dx[i] =  0.125; dy[i] = -0.875; break;
	    case  2: dx[i] =  0.250; dy[i] = -0.750; break;
	    case  3: dx[i] =  0.375; dy[i] = -0.625; break;
	    case  4: dx[i] =  0.500; dy[i] = -0.500; break;
	    case  5: dx[i] =  0.625; dy[i] = -0.375; break;
	    case  6: dx[i] =  0.750; dy[i] = -0.250; break;
	    case  7: dx[i] =  0.875; dy[i] = -0.125; break;

	    case  8: dx[i] =  1.000; dy[i] =  0.000; break;
	    case  9: dx[i] =  0.875; dy[i] =  0.125; break;
	    case 10: dx[i] =  0.750; dy[i] =  0.250; break;
	    case 11: dx[i] =  0.625; dy[i] =  0.375; break;
	    case 12: dx[i] =  0.500; dy[i] =  0.500; break;
	    case 13: dx[i] =  0.375; dy[i] =  0.625; break;
	    case 14: dx[i] =  0.250; dy[i] =  0.750; break;
	    case 15: dx[i] =  0.125; dy[i] =  0.875; break;

	    case 16: dx[i] =  0.000; dy[i] =  1.000; break;
	    case 17: dx[i] = -0.125; dy[i] =  0.875; break;
	    case 18: dx[i] = -0.250; dy[i] =  0.750; break;
	    case 19: dx[i] = -0.375; dy[i] =  0.625; break;
	    case 20: dx[i] = -0.500; dy[i] =  0.500; break;
	    case 21: dx[i] = -0.625; dy[i] =  0.375; break;
	    case 22: dx[i] = -0.750; dy[i] =  0.250; break;
	    case 23: dx[i] = -0.875; dy[i] =  0.125; break;

	    case 24: dx[i] = -1.000; dy[i] =  0.000; break;
	    case 25: dx[i] = -0.875; dy[i] = -0.125; break;
	    case 26: dx[i] = -0.750; dy[i] = -0.250; break;
	    case 27: dx[i] = -0.625; dy[i] = -0.375; break;
	    case 28: dx[i] = -0.500; dy[i] = -0.500; break;
	    case 29: dx[i] = -0.375; dy[i] = -0.625; break;
	    case 30: dx[i] = -0.250; dy[i] = -0.750; break;
	    case 31: dx[i] = -0.125; dy[i] = -0.875; break;
	    default: break;
	    }
	}

    //----------------------------- Atualiza posicoes
    for ( i = 0; i < NUM_CARS; i++ )
	{
	x[i] += dx[i] * veloc[i];
	if ( x[i] < 0 || x[i] > 616 ) x[i] -= dx[i] * veloc[i];
	y[i] += dy[i] * veloc[i];
	if ( y[i] < 0 || y[i] > 376 ) y[i] -= dy[i] * veloc[i];
	}


    }

setvect ( KEYBINTR, oldkeyboard );
setvect ( TIMEINTR, oldtimer );

//------------------------------ Escreve mensagens
Video.DrawSprite ( 45, 90, 12, 230, Frase[0] );
Video.DrawSprite ( 75, 110, 12, 160, Frase[1] );
Video.CopyPageToScreen ( );

//------------------------------ Libera memoria
for ( i = 0; i < 64; i++ )
    for ( j = 0; j < 40; j++ )
	KillSprite ( Pista[i][j] );

tecla = 1;
while ( tecla != ESC )
    {
    ext = 1;
    tecla = getch ( );
    if ( tecla == 0 ) ext = getch ( );

    if ( tecla == ENTER )
	{
	lider = 1;
	goto Restart;
	}
    }

Fim:

Video.ClosePage ( );
Video.CloseMode13h ( );

cout << "Vers„o BETA v0.72 - 23/05/97\n\n";

cout << "Obrigado por experimentar este jogo! Se vocˆ encontrou algum \"bug\" ou\n";
cout << "tem alguma sugest„o (at‚ para o nome...), entre em contato com o autor:\n\n";

cout << "  alexsand@attila.urcamp.tche.br\n\n";

cout << "Novas vers”es estar„o dispon¡veis para download na minha Home Page:\n\n";
cout << "  http://www.urcamp.tche.br/~alexsand/\n\n";

cout << "Classifica‡„o ap¢s " << num_voltas << " voltas:\n";
cout << " Pos  Carro       Total      M‚dia      Melhor\n";
for ( i = 0; i < NUM_CARS; i++ )
    {
    TotalTime[i] = (chegada[i]-largada[i])/CLK_TCK;
    AvgLap[i] = TotalTime[i] / num_voltas;
    }
for ( i = 1; i <= NUM_CARS; i++ )
    {
    for ( j = 0; j < NUM_CARS; j++ )
	{
	if ( Posicao[j] == i )
	    {
	    cout << "  " << i << "§  ";
	    switch ( j )
		{
		case 0: cout << "VERMELHO"; break;
		case 1: cout << "AZUL    "; break;
		case 2: cout << "VERDE   "; break;
		case 3: cout << "AMARELO "; break;
		default:;
		}
	    if ( terminou[j] )
		sprintf ( buffer, " %8.3lfs    %6.3lfs    %6.3lfs",
		    TotalTime[j], AvgLap[j], BestLap[j] );
	    else
		sprintf ( buffer, "        -          -     %6.3lfs",
		    BestLap[j] );
	    cout << buffer << "\n";
	    }
	}
    }
cout << "\n";

cout << "Mem¢ria inicial (A)....: " << mem_antes << " bytes\n";
cout << "Mem¢ria final (B)......: " << mem_depois << " bytes\n";
cout << "Mem¢ria utilizada (A-B): " << (mem_antes - mem_depois) << " bytes\n\n";
return;
}
