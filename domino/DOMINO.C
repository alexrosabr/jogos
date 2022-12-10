#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <time.h>

#include "dominet.h"
#include "keycodes.h"

#define MAXX 639
#define MAXY 479

#define LIMITE        100
#define QTD_INICIAL   7
#define NUM_FACES     7
#define NUM_PEDRAS    28
#define NUM_JOGADORES 4
#define NUM_EQUIPES   2

int em_rede, net_quem, net_nodos;

int silencio, primeiro, velocidade, inicio, mao, speed, rodada;
struct
    {
    void *imagem_horiz, *imagem_vert;
    unsigned char ponto[3][3], cor;
    } face[NUM_FACES];
struct player
    {
    char nome[16];
    unsigned char hand[QTD_INICIAL], num_pedras, nivel;
    } jogador[NUM_JOGADORES+1];
struct
    {
    unsigned char valor, dono, soma;
    int x, y, pos;
    } pedra[NUM_PEDRAS];
struct
    {
    unsigned char numero, estado, carreta, pedra;
    int x, y, pos;
    } ponta[2];
void *seta, *fundo_seta;
unsigned char tabela[100];
int pontos[NUM_EQUIPES];
char diff[4][20] = { "Humano", "Iniciante", "Intermedi rio", "Avan‡ado" },
    leitura[30];
/*-------- Variaveis da IA */
int nao_tem[NUM_JOGADORES][NUM_FACES], quantas_tem[NUM_JOGADORES][NUM_FACES],
    posicao[QTD_INICIAL], possivel[QTD_INICIAL], quantas_poss[NUM_FACES],
    cpu_ponta, last_move[NUM_JOGADORES];

/*--------------------------------------------------------------------------*/
void som ( int freq, int dur )
{
if ( !silencio )
    sound ( freq );
delay ( dur );
nosound ( );
}

/*--------------------------------------------------------------------------*/
double entra_string ( int xis, int yps, int tamanho, int flopt, int sinal )
{
	int i, z, point, alt, larg, ok, jatem, maxtam;
	char zyb [ 40 ], zc [ 2 ];
	double result;

	maxtam = tamanho;
	setwritemode ( COPY_PUT );
	alt = textheight ( "H" );
	larg = textwidth ( "H" );
	setfillstyle ( SOLID_FILL, BLACK );
	maxtam += sinal;
	bar ( xis - 1, yps - 1, xis + (maxtam+1) * larg + 1, yps + alt + 1 );
	maxtam -= sinal;
	setcolor ( WHITE );
	i = 0;
	z = 1;
	point = jatem = 0;
	ok = 1;
	outtextxy ( xis, yps, "_" );
	strcpy ( zyb, "" );
	strcpy ( zc, "0" );
	result = -1.0;
	while ( z != ESC )
		{
		z = getch ( );
		if ( z == 0 )
			{
			z = getch ( );
			}
		if ( z == '-' && !jatem && sinal )
			{
			strcat ( zyb, "@" );
			zc [ 0 ] = z;
			setcolor ( BLACK );
			outtextxy ( xis, yps, "_" );
			setcolor ( WHITE );
			outtextxy ( xis, yps, zc );
			xis += larg;
			outtextxy ( xis, yps, "_" );
			zyb [ i++ ] = z;
			if ( !ok ) ok = 1;
			jatem = 1;
			maxtam++;
			}
		if ( ( isdigit ( z ) && i < maxtam ) ||
			( flopt == 2 && isalpha ( z ) && i < maxtam ) )
			{
			strcat ( zyb, "@" );
			zc [ 0 ] = z;
			setcolor ( BLACK );
			outtextxy ( xis, yps, "_" );
			setcolor ( WHITE );
			outtextxy ( xis, yps, zc );
			xis += larg;
			outtextxy ( xis, yps, "_" );
			zyb [ i++ ] = z;
			if ( !ok ) ok = 1;
			jatem = 1;
			}
		if ( ( z == '.' || z == ',' ) &&
			i < maxtam && !point && ( flopt == 1 ) &&
			( i > 1 || zyb [ 0 ] != '-' ) )
			{
			strcat ( zyb, "@" );
			zc [ 0 ] = z;
			setcolor ( BLACK );
			outtextxy ( xis, yps, "_" );
			setcolor ( WHITE );
			outtextxy ( xis, yps, zc );
			xis += larg;
			outtextxy ( xis, yps, "_" );
			zyb [ i++ ] = '.';
			point = 1;
			ok = 0;
			}
		if ( z == 8 && i > 0 )
			{
			zc [ 0 ] = zyb [ i - 1 ];
			setcolor ( BLACK );
			outtextxy ( xis, yps, "_" );
			xis -= larg;
			outtextxy ( xis, yps, zc );
			setcolor ( WHITE );
			outtextxy ( xis, yps, "_" );
			i--;
			if ( i == 0 ) jatem = 0;
			zyb [ i ] = '\0';
			if ( zc [ 0 ] == '.' ) point = 0;
			}
		if ( z == ENTER && !ok ) z = 1;
		if ( z == ENTER ) break;
		}
	if ( z == ESC ) return ( result );
	if ( flopt == 1 ) strcat ( zyb, ".00" );
	if ( flopt < 2 )
		result = atof ( zyb );
		else
		{
		result = 0.0;
		strcpy ( leitura, zyb );
		}
	return ( result );
}

/*--------------------------------------------------------------------------*/
char mensagem ( char *linha1, char *linha2 )
{
char tecla;
void *fundo;

fundo = malloc ( imagesize ( 200, 220, 440, 250 ) );
if ( fundo == NULL )
    return ( -1 );
setfillstyle ( SOLID_FILL, LIGHTGRAY );
bar ( 200, 220, 440, 250 );
setcolor ( BLACK );
rectangle ( 202, 222, 438, 248 );
getimage ( 200, 220, 440, 250, fundo );
settextjustify ( CENTER_TEXT, TOP_TEXT );
if ( linha2[0] == NULL )
    {
    outtextxy ( 320, 231, linha1 );
    }
else
    {
    outtextxy ( 320, 226, linha1 );
    outtextxy ( 320, 236, linha2 );
    }
som ( 640, 50 );
tecla = getch ( );
if ( tecla == 0 ) getch ( );
settextjustify ( LEFT_TEXT, TOP_TEXT );
free ( fundo );
return ( tecla );
}

/*--------------------------------------------------------------------------*/
int inicializa ( void )
{
int driver, mode, erro;

driver = VGA;
mode = VGAHI;
erro = registerbgidriver ( EGAVGA_driver );
if ( erro < 0 )
    return ( 0 );
initgraph ( &driver, &mode, "" );
if ( mode != VGAHI )
    return ( 0 );
setrgbpalette ( BLUE, 63, 32, 0 );      // pedra 2 = laranja
return ( 1 );
}

/*-------------------------------------------------------------------------*/
void cria_pedras ( void )
{
int i, j, k, tamanho, contador;
char linha1[NUM_FACES*3+1] = "000000001001101101101",//101111111",
     linha2[NUM_FACES*3+1] = "000010000010000010101",//111101111",
     linha3[NUM_FACES*3+1] = "000000100100101101101";//101111111";

face[0].cor = BLACK;
face[1].cor = WHITE;
face[2].cor = BLUE;
face[3].cor = LIGHTCYAN;
face[4].cor = LIGHTGREEN;
face[5].cor = YELLOW;
face[6].cor = LIGHTRED;

/*for ( i = 0; i < NUM_FACES; i++ )
    face[i].cor = WHITE;*/

for ( k = 0; k < NUM_FACES; k++ )
    for ( j = 0; j < 3; j++ )
	{
	face[k].ponto[0][j] = linha1[k*3+j] - 0x30;
	face[k].ponto[1][j] = linha2[k*3+j] - 0x30;
	face[k].ponto[2][j] = linha3[k*3+j] - 0x30;
	}

tamanho = imagesize ( 46, 96, 70, 120 );
//------------------------------------ Horizontal
for ( k = 0; k < NUM_FACES; k++ )
    {
    setcolor ( LIGHTGRAY );
    rectangle ( 50*(k+1)-4, 96, 50*(k+1)+20, 120 );
    setfillstyle ( SOLID_FILL, BLACK );
    floodfill ( 50*(k+1), 100, LIGHTGRAY );
    setcolor ( face[k].cor );
    setfillstyle ( SOLID_FILL, face[k].cor );
    for ( i = 0; i < 3; i++ )
	for ( j = 0; j < 3; j++ )
	    if ( face[k].ponto[i][j] )
		{
		circle ( 50*(k+1)+i*8, 100+j*8, 2 );
		floodfill ( 50*(k+1)+i*8, 100+j*8, face[k].cor );
		putpixel ( 50*(k+1)+i*8+1, 101+j*8, BLACK );
		putpixel ( 50*(k+1)+i*8+1, 100+j*8, BLACK );
		putpixel ( 50*(k+1)+i*8, 101+j*8, BLACK );
		}
    face[k].imagem_horiz = malloc ( tamanho );
    if ( face[k].imagem_horiz == NULL )
	return;
    getimage ( 50*(k+1)-4, 96, 50*(k+1)+20, 120, face[k].imagem_horiz );
    }

//------------------------------------ Vertical
for ( k = 0; k < NUM_FACES; k++ )
    {
    setcolor ( LIGHTGRAY );
    rectangle ( 50*(k+1)-4, 196, 50*(k+1)+20, 220 );
    setfillstyle ( SOLID_FILL, BLACK );
    floodfill ( 50*(k+1), 200, LIGHTGRAY );
    setcolor ( face[k].cor );
    setfillstyle ( SOLID_FILL, face[k].cor );
    for ( i = 0; i < 3; i++ )
	for ( j = 0; j < 3; j++ )
	    if ( face[k].ponto[j][2-i] )
		{
		circle ( 50*(k+1)+i*8, 200+j*8, 2 );
		floodfill ( 50*(k+1)+i*8, 200+j*8, face[k].cor );
		putpixel ( 50*(k+1)+i*8+1, 201+j*8, BLACK );
		putpixel ( 50*(k+1)+i*8+1, 200+j*8, BLACK );
		putpixel ( 50*(k+1)+i*8, 201+j*8, BLACK );
		}
    face[k].imagem_vert = malloc ( tamanho );
    if ( face[k].imagem_vert == NULL )
	return;
    getimage ( 50*(k+1)-4, 196, 50*(k+1)+20, 220, face[k].imagem_vert );
    }

//---------------------------- valores
for ( i = 0; i < 100; i++ )
    tabela[i] = 0xFF;

contador = 0;
for ( i = 0; i < NUM_FACES; i++ )
    for ( j = 0; j <= i; j++ )
	{
	tabela[i*10+j] = contador;
	pedra[contador].valor = i*10+j;
	pedra[contador].dono = 0;
	pedra[contador].soma = i+j;
	pedra[contador].x = pedra[contador].y = 0;
	contador++;
	}

seta = malloc ( imagesize ( 12, 12, 27, 27 ) );
if ( seta == NULL )
    return;
fundo_seta = malloc ( imagesize ( 12, 12, 27, 27 ) );
if ( fundo_seta == NULL )
    return;
setcolor ( WHITE );
circle ( 20, 20, 5 );
circle ( 20, 20, 4 );
setfillstyle ( SOLID_FILL, LIGHTRED );
floodfill ( 20, 20, WHITE );
getimage ( 12, 12, 27, 27, seta );

strcpy ( jogador[1].nome, diff[0] );
jogador[1].nivel = 0;
for ( i = 2; i <= NUM_JOGADORES; i++ )
    {
    jogador[i].nivel = 2;
    sprintf ( jogador[i].nome, "CPU #%d", i );
    }

return;
}

/*-------------------------------------------------------------------------*/
void mostra_placar ( void )
{
int i;
char buffer[50];

setfillstyle ( SOLID_FILL, LIGHTGRAY );
bar ( 2, 2, 200, 68 );
setcolor ( BLACK );
rectangle ( 4, 4, 198, 66 );
for ( i = 0; i < NUM_EQUIPES; i++ )
    {
    sprintf ( buffer, "%s/%s", jogador[i+1].nome, jogador[i+3].nome );
    outtextxy ( 8, 10+i*30, buffer );
    sprintf ( buffer, "%d pontos", pontos[i] );
    outtextxy ( 8, 20+i*30, buffer );
    }
setfillstyle ( SOLID_FILL, GREEN );

return;
}

/*-------------------------------------------------------------------------*/
void desenha_pedra ( int qual, int xis, int yps, int horiz )
{
setcolor ( WHITE );
if ( horiz )
    {
    putimage ( xis+1, yps+1, face[qual/10].imagem_horiz, COPY_PUT );
    putimage ( xis+25, yps+1, face[qual%10].imagem_horiz, COPY_PUT );
    rectangle ( xis, yps, xis+50, yps+26 );
    }
else
    {
    putimage ( xis+1, yps+1, face[qual/10].imagem_vert, COPY_PUT );
    putimage ( xis+1, yps+25, face[qual%10].imagem_vert, COPY_PUT );
    rectangle ( xis, yps, xis+26, yps+50 );
    }
return;
}

/*-------------------------------------------------------------------------*/
void apaga_pedra ( int qual )
{
int xis, yps;

xis = pedra[qual].x;
yps = pedra[qual].y;
setfillstyle ( SOLID_FILL, GREEN );
if ( pedra[qual].pos )
    bar ( xis, yps, xis+50, yps+26 );
else
    bar ( xis, yps, xis+26, yps+50 );
return;
}

/*-------------------------------------------------------------------------*/
void reseta_pedras ( void )
{
int i;

for ( i = 0; i < NUM_PEDRAS; i++ )
    {
    pedra[i].dono = 0;
    pedra[i].x = pedra[i].y = 0;
    }
for ( i = 0; i < 2; i++ )
    {
    ponta[i].x = 310;
    ponta[i].y = 210;
    ponta[i].pos = 0;
    ponta[i].numero = 0xFF;
    ponta[i].estado = 0xFF;
    ponta[i].pedra = 0xFF;
    ponta[i].carreta = 0;
    }
return;
}

/*-------------------------------------------------------------------------*/
void move_pedra ( int qual, int horiz, int x0, int y0, int x1, int y1,
    int visivel )
{
int i;
double xis, yps, dx, dy, xis_ant, yps_ant;
void *fundo;

dx = ( x1 - x0 ) / 25.0;
dy = ( y1 - y0 ) / 25.0;
xis = (double)x0;
yps = (double)y0;
fundo = malloc ( sizeof ( x0, y0, x0+50, y0+26 ) );
if ( fundo == NULL )
    return;
for ( i = 0; i < 25; i++ )
    {
    if ( horiz )
	getimage ( xis, yps, xis+50, yps+26, fundo );
    else
	getimage ( xis, yps, xis+26, yps+50, fundo );
    if ( visivel )
	desenha_pedra ( qual, xis, yps, horiz );
    else
	{
	setcolor ( WHITE );
	setfillstyle ( XHATCH_FILL, DARKGRAY );
	bar ( xis, yps, horiz?xis+50:xis+25, horiz?yps+25:yps+50 );
	rectangle ( xis, yps, horiz?xis+50:xis+25, horiz?yps+25:yps+50 );
	}
    xis_ant = xis;
    yps_ant = yps;
    xis += dx;
    yps += dy;
    delay ( velocidade );
    putimage ( xis_ant, yps_ant, fundo, COPY_PUT );
    }
if ( visivel )
    desenha_pedra ( qual, x1, y1, horiz );
else
    {
    setcolor ( WHITE );
    setfillstyle ( XHATCH_FILL, DARKGRAY );
    bar ( x1, y1, horiz?x1+50:x1+25, horiz?y1+25:y1+50 );
    rectangle ( x1, y1, horiz?x1+50:x1+25, horiz?y1+25:y1+50 );
    }
som ( 640, 5 );
free ( fundo );

return;
}

/*-------------------------------------------------------------------------*/
void distrib_pedras ( int local )
{
int i, j, piece, xf, yf, aberta;

for ( i = 0; i <= NUM_JOGADORES; i++ )
    {
    for ( j = 0; j < QTD_INICIAL; j++ )
	{
	jogador[i].hand[j] = 0;
	nao_tem[i][j] = 0;
	quantas_tem[i][j] = 0;
	}
    jogador[i].num_pedras = 0;
    }

setfillstyle ( SOLID_FILL, GREEN );
bar ( 1, 1, MAXX-1, MAXY-1 );
reseta_pedras ( );

randomize ( );
for ( i = 1; i <= NUM_JOGADORES; i++ )
    {
    for ( j = 0; j < QTD_INICIAL; j ++ )
	{
	do  {
	    piece = random ( NUM_PEDRAS );
	    }
	while ( pedra[piece].dono );
	jogador[i].hand[j] = pedra[piece].valor;
	quantas_tem[i][pedra[piece].valor/10]++;
	if ( pedra[piece].valor % 11 )
	    quantas_tem[i][pedra[piece].valor%10]++;
	if ( inicio && pedra[piece].valor == 66 )
	    primeiro = i;
	pedra[piece].dono = i;
	}
    jogador[i].num_pedras = QTD_INICIAL;
    }

for ( j = 0; j < QTD_INICIAL; j++ )
    for ( i = 0; i < NUM_JOGADORES; i++ )
	{
	xf = i%2 ? 5 : 210+j*30;
	yf = i%2 ? 140+j*30 : 5;
	if ( i == 0 ) yf = 425;
	if ( i == 1 ) xf = 585;
	piece = jogador[i+1].hand[j];
	aberta = ( local == i+1 ) ? 1 : 0;
	if ( local == 0 && jogador[i+1].nivel == 0 )
	    aberta = 1;
	move_pedra ( piece, i%2, 320, 240, xf, yf, aberta );
	pedra[tabela[piece]].x = xf;
	pedra[tabela[piece]].y = yf;
	pedra[tabela[piece]].pos = i%2;
	}

return;
}

/*-------------------------------------------------------------------------*/
int escolhe_pedra ( int quem )
{
int numero, x, y, dx, dy, tecla, ext, pos, pos_ant, moveu,
    maior, menor, i, num1, num2, passou,
    cpu_poss, cpu_lance, cpu_prox, cpu_parc, cpu_jogou, na_mesa[NUM_FACES],
    cpu_dono[NUM_FACES];
//char buffer[20];

numero = -1;
x = (quem-1)%2 ? 60 : 215;
y = (quem-1)%2 ? 145 : 60;
if ( quem == 1 ) y = 405;
if ( quem == 2 ) x = 565;
dx = (quem-1)%2 ? 0 : 30;
dy = (quem-1)%2 ? 30 : 0;

pos = moveu = 0;
while ( jogador[quem].hand[pos] == 0xFF && pos <= NUM_FACES )
    pos++;
if ( pos == NUM_FACES )
    return ( numero );
maior = menor = pos_ant = pos;
i = 0;
while ( i < QTD_INICIAL )
    {
    if ( jogador[quem].hand[i] != 0xFF )
	maior = i;
    i++;
    }

// testa se passou
passou = 1;
for ( i = 0; i < QTD_INICIAL; i++ )
    {
    possivel[i] = 0;
    if ( jogador[quem].hand[i] == 0xFF )
	continue;
    num1 = jogador[quem].hand[i]/10;
    num2 = jogador[quem].hand[i]%10;
    if ( ponta[0].numero == num1 || ponta[0].numero == num2 ||
	 ponta[1].numero == num1 || ponta[1].numero == num2 )
	{
	passou = 0;
	break;
	}
    }
if ( !mao && passou )
    {
    nao_tem[quem][ponta[0].numero] = 1;
    nao_tem[quem][ponta[1].numero] = 1;
    getimage ( x+pos*dx, y+pos*dy, x+pos*dx+15, y+pos*dy+15, fundo_seta );
    putimage ( x+pos*dx, y+pos*dy, seta, COPY_PUT );
    som ( 440, 500 );
    som ( 220, 300 );
    putimage ( x+pos*dx, y+pos*dy, fundo_seta, COPY_PUT );
    return ( -2 );
    }
mao = 0;

if ( jogador[quem].nivel != 0 ) //---------------- Inteligencia Artificial
    {
    if ( ponta[0].numero == 0xFF )
	{
	if ( inicio )
	    {
	    for ( i = 0; i < QTD_INICIAL; i++ )
		if ( jogador[quem].hand[i] == 66 )
		    {
		    pos = i;
		    break;
		    }
	    inicio = 0;
	    }
	else
	    {
	    switch ( jogador[quem].nivel )
		{
		case 3:
		case 2:
		    pos = 0;
		    for ( i = 1; i < QTD_INICIAL; i++ )
			if ( quantas_tem[quem][i] >= quantas_tem[quem][pos] )
			    pos = i;
		    break;
		case 1:
		    pos = random(QTD_INICIAL);
		    break;
		default:;
		}
	    }
	num1 = jogador[quem].hand[pos]/10;
	num2 = jogador[quem].hand[pos]%10;
	ponta[0].numero = num1;
	ponta[1].numero = num2;
	ponta[0].estado = ponta[1].estado = 99;
	ponta[0].carreta = ponta[1].carreta = (num1==num2) ? 1 : 0;
	ponta[0].pos = ponta[1].pos = ponta[1].carreta ? 0 : 1;
	ponta[0].pedra = ponta[1].pedra = jogador[quem].hand[pos];
	cpu_lance = tabela[jogador[quem].hand[pos]];
	jogador[quem].hand[pos] = 0xFF;
	jogador[quem].num_pedras--;
	pedra[cpu_lance].dono = 0;
	return ( cpu_lance );
	}
    cpu_poss = 0;
    for ( i = 0; i < NUM_FACES; i++ )
	{
	cpu_dono[i] = 0;
	quantas_poss[i] = 0;
	}
    for ( i = 0; i < NUM_FACES; i++ )
	na_mesa[i] = 0;
    for ( i = 0; i < NUM_PEDRAS; i++ )
	if ( pedra[i].dono == 0 )
	    {
	    num1 = pedra[i].valor/10;
	    num2 = pedra[i].valor%10;
	    na_mesa[num1]++;
	    if ( num1 != num2 )
		na_mesa[num2]++;
	    }

    for ( i = 0; i < QTD_INICIAL; i++ )
	{
	if ( jogador[quem].hand[i] == 0xFF )
	    continue;
	num1 = jogador[quem].hand[i]/10;
	num2 = jogador[quem].hand[i]%10;
	if ( na_mesa[num1]+quantas_tem[quem][num1] >= 7 )
	    cpu_dono[num1] = 1;
	if ( na_mesa[num2]+quantas_tem[quem][num2] >= 7 )
	    cpu_dono[num2] = 1;
	if ( ponta[0].numero == num1 || ponta[0].numero == num2 ||
	     ponta[1].numero == num1 || ponta[1].numero == num2 )
	    {
	    possivel[cpu_poss] = jogador[quem].hand[i];
	    posicao[cpu_poss++] = i;
	    quantas_poss[num1]++;
	    if ( num1 != num2 )
		quantas_poss[num2]++;
	    pos = i;
	    }
	}

    cpu_prox = quem + 1;
    if ( cpu_prox > NUM_JOGADORES )
	cpu_prox = 1;

/*    outtextxy ( 480, 390, "  M J PDF" );
    sprintf ( buffer, "J%d", quem );
    outtextxy ( 610, 400, buffer );
    for ( i = 0; i < NUM_FACES; i++ )
	{
	sprintf ( buffer, "%d %d %d %c%c%c", i, na_mesa[i],
	    quantas_tem[quem][i], (quantas_poss[i]>0)?'*':'-',
	    (cpu_dono[i])?'*':'-', (nao_tem[cpu_prox][i])?'*':'-');
	outtextxy ( 480, 400+i*10, buffer );
	}
    for ( i = 0; i < cpu_poss; i++ )
	{
	num1 = possivel[i]/10;
	num2 = possivel[i]%10;
	sprintf ( buffer, "[%d:%d]", num1, num2 );
	outtextxy ( 560, 400+i*10, buffer );
	}
    if ( cpu_poss > 1 )
	{
	tecla = getch ( );
	if ( tecla == 27 )
	    {
	    closegraph ( );
	    exit ( 1 );
	    }
	if ( tecla == 0 ) getch ( );
	}*/

    cpu_ponta = 9;
    if ( cpu_poss > 1 )
	{
	cpu_parc = quem + 2;
	if ( cpu_parc > NUM_JOGADORES )
	    cpu_parc = quem - 2;
	switch ( jogador[quem].nivel )
	    {
	    case 3:
	    case 2:
		cpu_jogou = 0;
		for ( i = 0; i < cpu_poss; i++ ) //forceja (pontas iguais)
		    {
		    num1 = possivel[i]/10;
		    num2 = possivel[i]%10;
		    if ((num1==ponta[0].numero && num2 == ponta[1].numero)||
			(num1==ponta[1].numero && num2 == ponta[0].numero) )
			{
			if ( quantas_tem[num1]+na_mesa[num1] >=
			    quantas_tem[num2]+na_mesa[num2] )
			    {
			    cpu_ponta = (ponta[0].numero==num1) ? 1 : 0;
			    if ( ponta[cpu_ponta].numero == num2 )
				{
				pos = posicao[i];
				cpu_jogou = 1;
//				outtextxy ( 550, 465, "Iguais(1)" );
				}
			    }
			else
			    {
			    cpu_ponta = (ponta[0].numero==num2) ? 1 : 0;
			    if ( ponta[cpu_ponta].numero == num2 )
				{
				pos = posicao[i];
				cpu_jogou = 1;
//				outtextxy ( 550, 465, "Iguais(2)" );
				}
			    }
			if ( cpu_jogou ) break;
			}
		    }
		if ( cpu_jogou ) break;
		for ( i = 0; i < cpu_poss; i++ ) //--- Joga a carreta
		    {
		    num1 = possivel[i]/10;
		    num2 = possivel[i]%10;
		    if ( num1 == num2 )
			{
			pos = posicao[i];
			cpu_jogou = 1;
//			outtextxy ( 550, 465, "Carreta(7)" );
			}
		    }
		if ( cpu_jogou ) break;
		for ( i = 0; i < cpu_poss; i++ ) //forceja (cor que nao tem)
		    {
		    num1 = possivel[i]/10;
		    num2 = possivel[i]%10;
		    if ( nao_tem[cpu_prox][num1] )
			{
			if ( ponta[0].numero == num2 )
			    {
			    cpu_ponta = 0;
			    pos = posicao[i];
			    cpu_jogou = 1;
//			    outtextxy ( 550, 465, "Forceja(3)" );
			    }
			if ( ponta[1].numero == num2 )
			    {
			    cpu_ponta = 1;
			    pos = posicao[i];
			    cpu_jogou = 1;
//			    outtextxy ( 550, 465, "Forceja(4)" );
			    }
			}
		    if ( nao_tem[cpu_prox][num2] && !cpu_jogou )
			{
			if ( ponta[0].numero == num1 )
			    {
			    cpu_ponta = 0;
			    pos = posicao[i];
			    cpu_jogou = 1;
//			    outtextxy ( 550, 465, "Forceja(5)" );
			    }
			if ( ponta[1].numero == num1 )
			    {
			    cpu_ponta = 1;
			    pos = posicao[i];
			    cpu_jogou = 1;
//			    outtextxy ( 550, 465, "Forceja(6)" );
			    }
			}
		    if ( cpu_jogou ) break;
		    }
		if ( cpu_jogou ) break;
		maior = 0;
		for ( i = 0; i < cpu_poss; i++ ) //--- Joga a que tem mais
		    {
		    num1 = possivel[i]/10;
		    num2 = possivel[i]%10;
		    if ( quantas_tem[quem][num1] > maior )
			{
			cpu_ponta = (ponta[0].numero==num1) ? 1 : 0;
			if ( ponta[cpu_ponta].numero == num2 )
			    {
			    maior = quantas_tem[quem][num1];
			    pos = posicao[i];
			    cpu_jogou = 1;
//			    outtextxy ( 550, 465, "Popular(8)" );
			    }
			}
		    if ( quantas_tem[quem][num2] > maior )
			{
			cpu_ponta = (ponta[0].numero==num2) ? 1 : 0;
			if ( ponta[cpu_ponta].numero == num1 )
			    {
			    maior = quantas_tem[quem][num2];
			    pos = posicao[i];
			    cpu_jogou = 1;
//			    outtextxy ( 550, 465, "Popular(9)" );
			    }
			}
		    }

		if ( cpu_jogou ) break;
	    case 1:
		while ( 1 )
		    {
		    pos = random ( QTD_INICIAL );
		    if ( jogador[quem].hand[pos] == 0xFF )
			continue;
		    num1 = jogador[quem].hand[pos]/10;
		    num2 = jogador[quem].hand[pos]%10;
		    if ( ponta[0].numero == num1 ||
			ponta[0].numero == num2 ||
			ponta[1].numero == num1 || ponta[1].numero == num2 )
			break;
		    }
		break;
	    default:;
	    }
	}
    cpu_lance = tabela[jogador[quem].hand[pos]];
    jogador[quem].hand[pos] = 0xFF;
    jogador[quem].num_pedras--;
    pedra[cpu_lance].dono = 0;
    return ( cpu_lance );
    }

//------------------------------------- comeca a parte humana
setfillstyle ( SOLID_FILL, GREEN );
tecla = 1;
getimage ( x+pos*dx, y+pos*dy, x+pos*dx+15, y+pos*dy+15, fundo_seta );
while ( tecla != 27 )
    {
    ext = 1;
    putimage ( x+pos*dx, y+pos*dy, seta, COPY_PUT );
de_novo:
    tecla = getch ( );
    if ( tecla == 0 ) ext = getch ( );
    if ( tecla == 13 && jogador[quem].hand[pos] != 0xFF )
	    {
	    if ( inicio && jogador[quem].hand[pos] != 66 )
		{
		som ( 220, 300 );
		goto de_novo;
		}
	    num1 = jogador[quem].hand[pos]/10;
	    num2 = jogador[quem].hand[pos]%10;
	    if ( ponta[0].numero != 0xFF &&
		 ponta[0].numero != num1 && ponta[0].numero != num2 &&
		 ponta[1].numero != num1 && ponta[1].numero != num2 )
		{
		som ( 220, 300 );
		goto de_novo;
		}
	    inicio = 0;
	    if ( ponta[0].numero == 0xFF )
		{
		ponta[0].numero = num1;
		ponta[1].numero = num2;
		ponta[0].estado = ponta[1].estado = 99;
		ponta[0].carreta = ponta[1].carreta = (num1==num2) ? 1 : 0;
		ponta[0].pos = ponta[1].pos = ponta[1].carreta ? 0 : 1;
		ponta[0].pedra = ponta[1].pedra = jogador[quem].hand[pos];
		}
	    numero = tabela[jogador[quem].hand[pos]];
	    jogador[quem].hand[pos] = 0xFF;
	    jogador[quem].num_pedras--;
	    pedra[numero].dono = 0;
	    break;
	    }
    if ( ( ext == 75 || ext == 72 ) && pos > menor )
	{
	pos--;
	if ( jogador[quem].hand[pos] == 0xFF )
	    while ( jogador[quem].hand[--pos] == 0xFF );
	moveu = 1;
	}
    if ( ( ext == 77 || ext == 80 ) && pos < maior )
	{
	pos++;
	if ( jogador[quem].hand[pos] == 0xFF )
	    while ( jogador[quem].hand[++pos] == 0xFF );
	moveu = 1;
	}
    if ( moveu )
	{
	putimage ( x+pos_ant*dx, y+pos_ant*dy, fundo_seta, COPY_PUT );
	pos_ant = pos;
	getimage ( x+pos*dx, y+pos*dy, x+pos*dx+15, y+pos*dy+15, fundo_seta );
	moveu = 0;
	}
    }
putimage ( x+pos_ant*dx, y+pos_ant*dy, fundo_seta, COPY_PUT );

return ( numero );
}

/*-------------------------------------------------------------------------*/
int testa_cinco ( void )
{
int i, j, quem, num_carretas;

quem = 0;
for ( i = 1; i <= NUM_JOGADORES; i++ )
    {
    num_carretas = 0;
    for ( j = 0; j < QTD_INICIAL; j ++ )
	{
	if ( jogador[i].hand[j] % 11 == 0 )
	    num_carretas++;
	}
    if ( num_carretas >= 5 )
	{
	quem = i;
	break;
	}
    }
return ( quem );
}

/*-------------------------------------------------------------------------*/
void marca_com_xis ( int x1, int y1, int tipo, int cor )
{
setcolor ( cor );
rectangle ( x1, y1, x1+(tipo?50:25), y1+(tipo?25:50) );
line ( x1, y1, x1+(tipo?50:25), y1+(tipo?25:50) );
line ( x1+(tipo?50:25), y1, x1, y1+(tipo?25:50) );
return;
}

/*-------------------------------------------------------------------------*/
void mostra_resto ( void )
{
int i, j, piece;

for ( i = 1; i <= NUM_JOGADORES; i++ )
    for ( j = 0; j < QTD_INICIAL; j++ )
	{
	if ( jogador[i].hand[j] != 0xFF )
	    {
	    piece = tabela[jogador[i].hand[j]];
	    desenha_pedra ( pedra[piece].valor,
		pedra[piece].x, pedra[piece].y, pedra[piece].pos );
	    }
	}
return;
}

/*-------------------------------------------------------------------------*/
void jogo_normal ( void )
{
int i, j, partida, quem_joga, piece, soma1, soma2,
    qual_ponta, face1, face2, corrigida, passaram, trancado,
    ladoesq, ladodir, tecla, motivo, vencedor;
char buffer[50];

for ( i = 0; i < NUM_EQUIPES; i++ )
    pontos[i] = 0;

inicio = 1;
partida = 1;
while ( partida )
    {
    velocidade = 10-speed;
    distrib_pedras ( em_rede ); //------- 0=sem rede; 1-4=com rede (n§ do jogador)
    velocidade = 50-(speed*5);
    quem_joga = primeiro;

    settextjustify ( RIGHT_TEXT, CENTER_TEXT );
    outtextxy ( 200, 450, jogador[1].nome );
    outtextxy ( 635, 365, jogador[2].nome );
    settextjustify ( LEFT_TEXT, CENTER_TEXT );
    outtextxy ( 420, 30, jogador[3].nome );
    outtextxy ( 5, 365, jogador[4].nome );
    settextjustify ( LEFT_TEXT, TOP_TEXT );

    if ( ( trancado = testa_cinco ( ) ) != 0 )
	{
	mostra_resto ( );
	sprintf ( buffer, "%s saiu com", jogador[trancado].nome );
	mensagem ( buffer, "5 ou mais duplas!!" );
	continue;
	}

    rodada = 0;
    mao = 1;
    passaram = 0;
    motivo = vencedor = 0;
    mostra_placar ( );
    while ( rodada < NUM_PEDRAS && passaram < NUM_JOGADORES && motivo == 0 )
	{
//	setfillstyle ( SOLID_FILL, GREEN ); //-------- cerebro
//	bar ( 450, 390, 638, 478 );
//	setcolor ( YELLOW );

	piece = escolhe_pedra ( quem_joga );
	if ( piece < 0 )
	    {
	    if ( piece == -1 )
		motivo = 99;
	    // senao, passou
	    passaram++;
	    }
	else
	    {
	    passaram = 0;
	    qual_ponta = 0;
	    face1 = pedra[piece].valor/10;
	    face2 = pedra[piece].valor%10;
	    if ( ponta[1].numero == face1 || ponta[1].numero == face2 )
		qual_ponta = 1;

	    if ( rodada > 1 && jogador[quem_joga].nivel > 0 &&
		( ponta[0].numero == face1 || ponta[0].numero == face2 ) &&
		( ponta[1].numero == face1 || ponta[1].numero == face2 ) )
		{
		switch ( jogador[quem_joga].nivel )
		    {
		    case 3:
		    case 2:
			if ( ponta[0].numero == ponta[1].numero )
			    {
			    if ( cpu_ponta != 9 )
				{
				qual_ponta = cpu_ponta;
				break;
				}
			    }
		    case 1:
			qual_ponta = random ( 2 );
			break;
		    default:;
		    }
		}

	    if ( rodada > 1 && jogador[quem_joga].nivel == 0 &&
		( ponta[0].numero == face1 || ponta[0].numero == face2 ) &&
		( ponta[1].numero == face1 || ponta[1].numero == face2 ) )
		{
		tecla = 1;
		qual_ponta = 1;
		marca_com_xis ( pedra[piece].x, pedra[piece].y,
		    pedra[piece].pos, WHITE );
		while ( tecla != 13 )
		    {
		    setcolor ( YELLOW );
		    qual_ponta = 1-qual_ponta;
		    marca_com_xis ( ponta[qual_ponta].x, ponta[qual_ponta].y,
			ponta[qual_ponta].pos, YELLOW );
		    tecla = getch ( );
		    if ( tecla == 0 ) getch ( );
		    desenha_pedra ( ponta[qual_ponta].pedra,
			ponta[qual_ponta].x,ponta[qual_ponta].y,
			ponta[qual_ponta].pos );
		    if ( tecla == 27 )
			break;
		    }
		if ( tecla == 27 ) motivo = 99;
		}
	    if ( jogador[quem_joga].nivel > 0 )
		delay ( velocidade*5 );

	    apaga_pedra ( piece );
	    corrigida = pedra[piece].valor;

	    switch ( ponta[qual_ponta].estado ) //------ maquina de estados
		{
		case 0: //---------- Estado 0
		    if ( ponta[qual_ponta].carreta )
			{
			ponta[qual_ponta].pos = 1;
			ponta[qual_ponta].y += 12;
			ponta[qual_ponta].x -= 52;
			}
		    else
			{
			if ( face1 == face2 )
			    {
			    ponta[qual_ponta].pos = 0;
			    ponta[qual_ponta].y -= 12;
			    ponta[qual_ponta].x -= 27;
			    }
			else
			    {
			    ponta[qual_ponta].pos = 1;
			    ponta[qual_ponta].x -= 52;
			    }
			}
		    if ( face2 != ponta[qual_ponta].numero )
			corrigida = (face2)*10+face1;
		    if ( ponta[qual_ponta].y > 280 )
			ladoesq = 150;
		    else
			ladoesq = 210;
		    if ( ponta[qual_ponta].x < ladoesq )
			ponta[qual_ponta].estado = 1;
		    ponta[qual_ponta].carreta = ( face1 == face2 ) ? 1 : 0;
		    break;
		case 1: //---------- Estado 1
		    ponta[qual_ponta].pos = 0;
		    ponta[qual_ponta].y -= 52;
		    if ( face2 != ponta[qual_ponta].numero )
			corrigida = (face2)*10+face1;
		    ponta[qual_ponta].carreta = 0;
		    ponta[qual_ponta].estado = 2;
		    break;
		case 2: //---------- Estado 2
		    if ( ponta[qual_ponta].carreta )
			{
			ponta[qual_ponta].pos = 0;
			ponta[qual_ponta].x += 12;
			ponta[qual_ponta].y -= 52;
			}
		    else
			{
			if ( face1 == face2 )
			    {
			    ponta[qual_ponta].pos = 1;
			    ponta[qual_ponta].x -= 12;
			    ponta[qual_ponta].y -= 27;
			    }
			else
			    {
			    ponta[qual_ponta].pos = 0;
			    ponta[qual_ponta].y -= 52;
			    }
			}
		    if ( face2 != ponta[qual_ponta].numero )
			corrigida = (face2)*10+face1;
		    if ( ponta[qual_ponta].y < 120 )
		       ponta[qual_ponta].estado = 3;
		    ponta[qual_ponta].carreta = ( face1 == face2 ) ? 1 : 0;
		    break;
		case 3: //---------- Estado 3
		    ponta[qual_ponta].pos = 1;
		    if ( ponta[qual_ponta].carreta )
			ponta[qual_ponta].x += 52;
		    else
			ponta[qual_ponta].x += 27;
		    if ( face1 != ponta[qual_ponta].numero )
			corrigida = (face2)*10+face1;
		    ponta[qual_ponta].carreta = 0;
		    ponta[qual_ponta].estado = 10;
		    break;
		case 10: //---------- Estado 10
		    if ( ponta[qual_ponta].carreta )
			{
			ponta[qual_ponta].pos = 1;
			ponta[qual_ponta].y += 12;
			ponta[qual_ponta].x += 27;
			}
		    else
			{
			if ( face1 == face2 )
			    {
			    ponta[qual_ponta].pos = 0;
			    ponta[qual_ponta].y -= 12;
			    }
			else
			    {
			    ponta[qual_ponta].pos = 1;
			    }
			ponta[qual_ponta].x += 52;
			}
		    if ( face1 != ponta[qual_ponta].numero )
			corrigida = (face2)*10+face1;
		    if ( ponta[qual_ponta].y > 200 )
			ladodir = 370;
		    else
			ladodir = 480;
		    if ( ponta[qual_ponta].x > ladodir )
		       ponta[qual_ponta].estado = 11;
		    ponta[qual_ponta].carreta = ( face1 == face2 ) ? 1 : 0;
		    break;
		case 11: //---------- Estado 11
		    if ( ponta[qual_ponta].carreta )
			{
			ponta[qual_ponta].pos = 0;
			ponta[qual_ponta].y += 52;
			}
		    else
			{
			ponta[qual_ponta].pos = 0;
			ponta[qual_ponta].x += 24;
			ponta[qual_ponta].y += 27;
			}
		    if ( face1 != ponta[qual_ponta].numero )
			corrigida = (face2)*10+face1;
		    ponta[qual_ponta].carreta = 0;
		    ponta[qual_ponta].estado = 12;
		    break;
		case 12: //---------- Estado 12
		    if ( ponta[qual_ponta].carreta )
			{
			ponta[qual_ponta].pos = 0;
			ponta[qual_ponta].x += 12;
			ponta[qual_ponta].y += 27;
			}
		    else
			{
			if ( face1 == face2 )
			    {
			    ponta[qual_ponta].pos = 1;
			    ponta[qual_ponta].x -= 12;
			    }
			else
			    {
			    ponta[qual_ponta].pos = 0;
			    }
			ponta[qual_ponta].y += 52;
			}
		    if ( face1 != ponta[qual_ponta].numero )
			corrigida = (face2)*10+face1;
		    if ( ponta[qual_ponta].y > 300 )
		       ponta[qual_ponta].estado = 13;
		    ponta[qual_ponta].carreta = ( face1 == face2 ) ? 1 : 0;
		    break;
		case 13: //---------- Estado 13
		    ponta[qual_ponta].pos = 1;
		    ponta[qual_ponta].x -= 52;
		    if ( !ponta[qual_ponta].carreta )
			ponta[qual_ponta].y += 24;
		    if ( face2 != ponta[qual_ponta].numero )
			corrigida = (face2)*10+face1;
		    ponta[qual_ponta].carreta = 0;
		    ponta[qual_ponta].estado = 0;
		    break;
		case 99: //---------- Estado 99 (inicial)
		    ponta[0].estado = 0;
		    ponta[1].estado = 10;
		    ponta[qual_ponta].carreta = ( face1 == face2 ) ? 1 : 0;
		default:;
		}

	    move_pedra ( corrigida, ponta[qual_ponta].pos,
		pedra[piece].x, pedra[piece].y,
		ponta[qual_ponta].x , ponta[qual_ponta].y, 1 );
	    quantas_tem[quem_joga][corrigida/10]--;
	    if ( corrigida%11 )
		quantas_tem[quem_joga][corrigida%10]--;
	    last_move[quem_joga] = ponta[qual_ponta].pos;

	    if ( rodada > 0 )
		{
		if ( ponta[qual_ponta].numero == face1 )
		    ponta[qual_ponta].numero = face2;
		else
		    ponta[qual_ponta].numero = face1;
		ponta[qual_ponta].pedra = corrigida;
		}

	    if ( jogador[quem_joga].num_pedras == 0 )
		{
		vencedor = quem_joga;
		motivo = 1;
		}
	    rodada++;
	    }
	quem_joga++;
	if ( quem_joga > NUM_JOGADORES )
	    quem_joga = 1;
	}
    primeiro++;
    if ( primeiro > NUM_JOGADORES )
	primeiro = 1;

    soma1 = soma2 = 0;
    for ( i = 1; i <= NUM_JOGADORES; i++ )
	for ( j = 0; j < QTD_INICIAL; j++ )
	    {
	    if ( i == 1 || i == 3 )
		{
		if ( jogador[i].hand[j] != 0xFF )
		    soma1 += pedra[tabela[jogador[i].hand[j]]].soma;
		}
	    else
		{
		if ( jogador[i].hand[j] != 0xFF )
		    soma2 += pedra[tabela[jogador[i].hand[j]]].soma;
		}
	     }

    switch ( motivo )
	{
	case 0:
	    quem_joga--;
	    if ( quem_joga < 1 )
		quem_joga = NUM_JOGADORES;
	    if ( soma1 > soma2 )
		{
		if ( quem_joga == 1 || quem_joga == 3 )
		    strcpy ( buffer, "fechou pra dentro!!" );
		else
		    strcpy ( buffer, "fechou o jogo!!" );
		pontos[0] += soma1;
		}
	    else
		if ( soma2 > soma1 )
		    {
		    if ( quem_joga == 2 || quem_joga == 4 )
			strcpy ( buffer, "fechou pra dentro!!" );
		    else
			strcpy ( buffer, "fechou o jogo!!" );
		    pontos[1] += soma2;
		    }
		else
		    {
		    strcpy ( buffer, "fechou (para dentro...)" );
		    if ( quem_joga == 1 || quem_joga == 3 )
			pontos[0] += soma1;
		    else
			pontos[1] += soma2;
		    }
	    mensagem ( jogador[quem_joga].nome, buffer  );
	    break;
	case 1:
	    sprintf ( buffer, "%s bateu!!", jogador[vencedor].nome );
	    mensagem ( buffer, "" );
	    if ( vencedor == 1 || vencedor == 3 )
		pontos[1] += soma2;
	    else
		pontos[0] += soma1;
	    break;
	default:;
	}

    mostra_resto ( );

    mostra_placar ( );
    if ( pontos[0] >= LIMITE || pontos[1] >= LIMITE )
	{
	if ( pontos[0] == 0 || pontos[1] == 0 )
	    mensagem ( "P I N O !!", "" );
	else
	    mensagem ( "F I M   D E   J O G O", "" );
	partida = 0;
	}
    else
	if ( mensagem ( "Pressione qualquer tecla",
	    "ou ESC para sair" ) == 27 )
	    partida = 0;
    }

return;
}

/*-------------------------------------------------------------------------*/
void desativa_opcao ( void )
{
char buffer[20];

setcolor ( YELLOW );
bar ( 340, 310, 460, 320 );
strcpy ( buffer, em_rede ? "(em rede)" : "(monousu rio)" );
outtextxy ( 340, 310, buffer );
return;
}

/*-------------------------------------------------------------------------*/
int menu_princ ( void )
{
char  buffer[50];
int menu, tecla, ext, i, nodos;

setfillstyle ( SOLID_FILL, LIGHTGRAY );
setcolor ( BLACK );
bar ( 120, 140, 520, 380);
rectangle ( 122, 142, 518, 378);
settextjustify ( CENTER_TEXT, TOP_TEXT );
settextstyle ( DEFAULT_FONT, HORIZ_DIR, 2 );
outtextxy ( 320, 145, "DOMINO" );
setcolor ( LIGHTRED );
outtextxy ( 322, 147, "DOMINO" );
settextstyle ( DEFAULT_FONT, HORIZ_DIR, 1 );
settextjustify ( LEFT_TEXT, TOP_TEXT );
setcolor ( YELLOW );
for ( i = 2; i <= 10; i++ )
    {
    sprintf ( buffer, "F%d", i );
    outtextxy ( 140, 160+i*15, buffer );
    }

setcolor ( WHITE );
outtextxy ( 175, 175, "Jogador 1.:" );
outtextxy ( 175, 190, "Jogador 2.:" );
outtextxy ( 175, 205, "Jogador 3.:" );
outtextxy ( 175, 220, "Jogador 4.:" );
outtextxy ( 175, 235, "Sons......:" );
outtextxy ( 175, 250, "Velocidade:" );
outtextxy ( 175, 265, "" );
outtextxy ( 175, 280, "" );
outtextxy ( 175, 295, "" );
outtextxy ( 175, 310, "Iniciar uma partida" );
desativa_opcao ( );
settextjustify ( CENTER_TEXT, TOP_TEXT );
setcolor ( BLACK );
outtextxy ( 320, 340, "Escrito por" );
settextstyle ( DEFAULT_FONT, HORIZ_DIR, 2 );
setcolor ( BLACK );
outtextxy ( 320, 355, "Alexsander da Rosa" );
setcolor ( YELLOW );
outtextxy ( 322, 357, "Alexsander da Rosa" );
settextstyle ( DEFAULT_FONT, HORIZ_DIR, 1 );
settextjustify ( LEFT_TEXT, TOP_TEXT );

strcpy ( jogador[0].nome, "Mesa" );
setcolor ( YELLOW );
for ( i = 1; i <= NUM_JOGADORES; i++ )
    outtextxy ( 270, 160+i*15, diff[jogador[i].nivel] );

sprintf ( buffer, "%s", silencio ? "Nenhum" : "PC Speaker" );
outtextxy ( 270, 235, buffer );

sprintf ( buffer, "%d0 %%", speed );
outtextxy ( 270, 250, buffer );

nodos = 1;
menu = tecla = 0;
while ( tecla != 27 )
    {
    ext = 1;
    tecla = getch ( );
    if ( tecla == 0 ) ext = getch ( );
    if ( ext > 59 && ext < 63 )
	{
	i = ext - 58;
	jogador[i].nivel++;
	if ( jogador[i].nivel > 3 )
	    jogador[i].nivel = 0;
	bar ( 270, 160+i*15, 420, 170+i*15 );
	outtextxy ( 270, 160+i*15, diff[jogador[i].nivel] );
	nodos = 0;
	for ( i = 1; i <= NUM_JOGADORES; i++ )
	    if ( jogador[i].nivel == 0 )
		nodos++;
	em_rede = ( nodos > 1 ) ? 1 : 0;
	desativa_opcao ( );
	}
    if ( ext == 63 ) // F5
	{
	silencio = silencio ? 0 : 1;
	bar ( 270, 235, 420, 245 );
	sprintf ( buffer, "%s", silencio ? "Nenhum" : "PC Speaker" );
	outtextxy ( 270, 235, buffer );
	som ( 640, 30 );
	som ( 640, 30 );
	}
    if ( ext == 64 ) // F6
	{
	speed++;
	if ( speed > 10 )
	    speed = 1;
	bar ( 270, 250, 420, 260 );
	sprintf ( buffer, "%d0 %%", speed );
	outtextxy ( 270, 250, buffer );
	}
    if ( ext == 68 ) // F10
	{
	menu = 1;
	net_quem = 1;
	break;
	}
    }
for ( i = 1; i <= NUM_JOGADORES; i++ )
    {
    switch ( jogador[i].nivel )
	{
	case 0:
	    break;
	case 1:
	    sprintf ( jogador[i].nome, "PC-XT #%d", i );
	    break;
	case 2:
	    sprintf ( jogador[i].nome, "i386 #%d", i );
	    break;
	case 3:
	    sprintf ( jogador[i].nome, "Pentium #%d", i );
	    break;
	default:;
	}
    }

return ( menu );
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
void main ( void )
{
int opcao, ingame, i;

em_rede = 0;
silencio = 1;
speed = 5;
if ( !inicializa ( ) )
    {
    puts ( "Erro!" );
    return;
    }

setvisualpage ( 0 );
setfillstyle ( SOLID_FILL, GREEN );
bar ( 0, 0, MAXX, MAXY );

cria_pedras ( );

setactivepage ( 0 );
setfillstyle ( SOLID_FILL, GREEN );
bar ( 0, 0, MAXX, MAXY );
setcolor ( WHITE );
rectangle ( 0, 0, MAXX, MAXY );

ingame = 1;
while ( ingame )
    {
    opcao = menu_princ ( );
    switch ( opcao )
	{
	case 0:
	    ingame = 0;
	    break;
	case 1:
	    net_nodos = 0;
	    for ( i = 1; i <= NUM_JOGADORES; i++ )
		{
		if ( jogador[i].nivel == 0 )
		    {
		    strcpy ( leitura, "Humano" );
		    entra_string ( 350, 160+i*15, 10, 2, 0 );
		    strcpy ( jogador[i].nome, leitura );
		    jogador[i].nome[0] = toupper ( jogador[i].nome[0] );
		    net_nodos++;
		    }
		}
	    if ( em_rede )
		{
		if ( !inicializa_rede ( ) )
		    break;
		procura_jogadores ( );
		}
	    jogo_normal ( );
	    if ( em_rede )
		fecha_rede ( );
	    break;
	default:;
	}
    }

closegraph ( );
return;
}
