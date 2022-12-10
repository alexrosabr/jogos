#include <dos.h>
#include <mem.h>
#include <bios.h>
#include <stdio.h>
#include <conio.h>
#include <values.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>

#define NUMPACKETS  10		// max outstanding packets before loss
#define	MAXNETNODES 8		// max computers in a game
#define	MAXPLAYERS  4		// 4 players max + drones

extern net_nodos, net_quem;
extern char mensagem ( char *linha1, char *linha2 );
//extern struct player *jogador;

//===========================================================================

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long LONG;

typedef struct
    {
    char private[512];
    } doomdata_t;

typedef struct
    {
    short gameid;		 // so multiple games can setup at once
    short drone;
    short nodesfound;
    short nodeswanted;
    } setupdata_t;

typedef struct
    {
    BYTE network[4];             /* high-low */
    BYTE node[6];                /* high-low */
    } localadr_t;

typedef struct
    {
    BYTE    node[6];                /* high-low */
    } nodeadr_t;

typedef struct IPXPacketStructure
    {
    WORD PacketCheckSum;         /* high-low */
    WORD PacketLength;           /* high-low */
    BYTE PacketTransportControl;
    BYTE PacketType;

    BYTE dNetwork[4];            /* high-low */
    BYTE dNode[6];               /* high-low */
    BYTE dSocket[2];             /* high-low */

    BYTE sNetwork[4];            /* high-low */
    BYTE sNode[6];               /* high-low */
    BYTE sSocket[2];             /* high-low */
    } IPXPacket;

typedef struct ECBStructure
    {
    WORD Link[2];                /* offset-segment */
    WORD ESRAddress[2];          /* offset-segment */
    BYTE InUseFlag;
    BYTE CompletionCode;
    WORD ECBSocket;              /* high-low */
    BYTE IPXWorkspace[4];        /* N/A */
    BYTE DriverWorkspace[12];    /* N/A */
    BYTE ImmediateAddress[6];    /* high-low */
    WORD FragmentCount;          /* low-high */

    WORD fAddress[2];            /* offset-segment */
    WORD fSize;                  /* low-high */
    } ECB;

typedef struct
    {
    ECB ecb;
    IPXPacket ipx;

    long time;
    doomdata_t data;
    } packet_t;

typedef struct
    {
    short remotenode;		// dest for send, set by get (-1 = no packet)
    short datalength;		// bytes in doomdata to be sent / bytes read

// info common to all nodes
    short numnodes;		// console is allways node 0

// info specific to this node
    short consoleplayer;	// 0-3 = player number
    short numplayers;		// 1-4
    short drone;		// 1 = drone

// packet data to be sent
    char data[512];
    } doomcom_t;

doomcom_t doomcom;
setupdata_t nodesetup[MAXNETNODES];
packet_t packets[NUMPACKETS];
nodeadr_t nodeadr[MAXNETNODES+1];	// first is local, last is broadcast
nodeadr_t remoteadr;      		// set by each GetPacket
localadr_t localadr;			// set at startup

//===========================================================================
int     socketid = 0x869c;    	// 0x869c is the official DOOM socket

union   REGS    regs;           // scratch for int86 calls
struct  SREGS   sregs;

unsigned short	enteripx[2];

long	   		localtime;		// for time stamp in packets
long			remotetime;

char *hex = "0123456789abcdef", buffer[30];

/*-------------------------------------------------------------------------*/
int OpenSocket ( short socketNumber )
{
regs.x.bx = 0;
regs.h.al = 0;                  // longevity
regs.x.dx = socketNumber;
int86 ( 0x7A, &regs, &regs);
if ( regs.h.al )
    {
    sprintf ( buffer, "em OpenSocket: %02Xh", regs.h.al );
    mensagem ( "Erro E02: Falha", buffer );
    closegraph ( );
    exit ( 0 );
    }
return ( regs.x.dx );
}

/*-------------------------------------------------------------------------*/
void CloseSocket ( short socketNumber )
{
regs.x.bx = 1;
regs.x.dx = socketNumber;
int86 ( 0x7A, &regs, &regs);
return;
}

/*-------------------------------------------------------------------------*/
void GetLocalAddress ( void )
{
regs.x.si = FP_OFF ( &localadr );
sregs.es = FP_SEG ( &localadr );
regs.x.bx = 9;

int86x ( 0x7A, &regs, &regs, &sregs);
if ( regs.h.al )
    {
    sprintf ( buffer, "em GetLocalAddress: %02Xh", regs.h.al );
    mensagem ( "Erro E03: Falha", buffer );
    closegraph ( );
    exit ( 0 );
    }
return;
}

/*-------------------------------------------------------------------------*/
void ListenForPacket ( ECB *ecb )
{
regs.x.si = FP_OFF(ecb);
sregs.es = FP_SEG(ecb);
regs.x.bx = 4;

int86x (0x7a,&regs,&regs,&sregs);
if (regs.h.al)
    {
    sprintf ( buffer, "em ListenForPacket: %02Xh", regs.h.al );
    mensagem ( "Erro E04: Falha", buffer );
    closegraph ( );
    exit ( 0 );
    }
return;
}

/*-------------------------------------------------------------------------*/
void SendPacket ( int destination )
{
int j;

// find a free packet buffer to use
while ( packets[0].ecb.InUseFlag )
    {
    }

// set the time
packets[0].time = localtime;

// set the address
for (j=0 ; j<6 ; j++)
    packets[0].ipx.dNode[j] = packets[0].ecb.ImmediateAddress[j] =
    nodeadr[destination].node[j];

// set the length (ipx + time + datalength)
packets[0].ecb.fSize = sizeof(IPXPacket) + 4 + doomcom.datalength + 4;

// put the data into an ipx packet
memcpy (&packets[0].data, &doomcom.data, doomcom.datalength);

// send the packet
regs.x.si = FP_OFF(&packets[0]);
sregs.es = FP_SEG(&packets[0]);
regs.x.bx = 3;
int86x (0x7a, &regs, &regs, &sregs);

if (regs.h.al)
    {
    sprintf ( buffer, "em SendPacket: %02Xh", regs.h.al );
    mensagem ( "Erro E05: Falha", buffer );
    closegraph ( );
    exit ( 0 );
    }
return;
}

/*-------------------------------------------------------------------------*/
unsigned short ShortSwap (unsigned short i)
{
return ((i&255)<<8) + ((i>>8)&255);
}

/*-------------------------------------------------------------------------*/
int GetPacket (void)
{
int packetnum;
int i;
long besttic;
packet_t *packet;

// if multiple packets are waiting, return them in order by time

besttic = MAXLONG;
packetnum = -1;
doomcom.remotenode = -1;

for ( i = 1 ; i < NUMPACKETS ; i++)
    {
    if (packets[i].ecb.InUseFlag)
	continue;

    if (packets[i].time < besttic)
	{
	besttic = packets[i].time;
	packetnum = i;
	}
    }

if ( besttic == MAXLONG )
    return ( 0 );                    // no packets

packet = &packets[packetnum];

if (besttic == -1 && localtime != -1)
    {
    ListenForPacket (&packet->ecb);
    return ( 0 );		     // setup broadcast from other game
    }

remotetime = besttic;

//
// got a good packet
//
if (packet->ecb.CompletionCode)
    {
    sprintf ( buffer, "em GetPacket: %02Xh", packet->ecb.CompletionCode );
    mensagem ( "Erro E06: Falha", buffer );
    }

// set remoteadr to the sender of the packet
memcpy (&remoteadr, packet->ipx.sNode, sizeof(remoteadr));
for (i=0 ; i<doomcom.numnodes ; i++)
    if (!memcmp(&remoteadr, &nodeadr[i], sizeof(remoteadr)))
	break;

if (i < doomcom.numnodes)
    doomcom.remotenode = i;
else
    {
    if (localtime != -1)
	{	// this really shouldn't happen
	ListenForPacket (&packet->ecb);
	return ( 0 );
	}
    }

// copy out the data
doomcom.datalength = ShortSwap(packet->ipx.PacketLength) - 38;
memcpy (&doomcom.data, &packet->data, doomcom.datalength);

// repost the ECB
ListenForPacket (&packet->ecb);

return ( 1 );
}

/*-------------------------------------------------------------------------*/
int inicializa_rede ( void )
{
int i, j;

setfillstyle ( SOLID_FILL, LIGHTGRAY );
setcolor ( BLACK );
bar ( 120, 140, 520, 380);
rectangle ( 122, 142, 518, 378);

regs.x.ax = 0x7A00;
int86x (0x2F,&regs,&regs,&sregs);
if ( regs.h.al != 0xFF )
    {
    setcolor ( YELLOW );
    mensagem ( "Erro E01: Driver", "IPX nao detectado!" );
    return ( 0 );
    }
enteripx[0] = regs.x.di;
enteripx[1] = sregs.es;

socketid = OpenSocket ( (socketid>>8) + ((socketid&255)<<8) );

GetLocalAddress ( );

sprintf ( buffer, "Local: %02X.%02X.%02X.%02X.%02X.%02X", localadr.node[0],
    localadr.node[1], localadr.node[2], localadr.node[3],
    localadr.node[4], localadr.node[5] );

outtextxy ( 130, 150, buffer );
//
// set up several receiving ECBs
//
memset ( packets, 0, NUMPACKETS*sizeof(packet_t) );

for ( i = 1; i < NUMPACKETS; i++)
    {
    packets[i].ecb.InUseFlag = 0x1D;
    packets[i].ecb.ECBSocket = socketid;
    packets[i].ecb.FragmentCount = 1;
    packets[i].ecb.fAddress[0] = FP_OFF(&packets[i].ipx);
    packets[i].ecb.fAddress[1] = FP_SEG(&packets[i].ipx);
    packets[i].ecb.fSize = sizeof(packet_t)-sizeof(ECB);

    ListenForPacket ( &packets[i].ecb );
    }

//
// set up a sending ECB
//
memset ( &packets[0],0,sizeof(packets[0]));

packets[0].ecb.ECBSocket = socketid;
packets[0].ecb.FragmentCount = 1;
packets[0].ecb.fAddress[0] = FP_OFF(&packets[0].ipx);
packets[0].ecb.fAddress[1] = FP_SEG(&packets[0].ipx);
for ( j = 0; j < 4 ;j++ )
    packets[0].ipx.dNetwork[j] = localadr.network[j];
packets[0].ipx.dSocket[0] = socketid&255;
packets[0].ipx.dSocket[1] = socketid>>8;

// known local node at 0
for ( i = 0 ;i < 6 ;i++ )
    nodeadr[0].node[i] = localadr.node[i];

// broadcast node at MAXNETNODES
for ( j = 0 ;j < 6 ;j++ )
    nodeadr[MAXNETNODES].node[j] = 0xff;

return ( 1 );
}

/*-------------------------------------------------------------------------*/
void fecha_rede ( void )
{
CloseSocket ( socketid );
return;
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
void procura_jogadores ( void )
{
int i, linha;
struct time time;
int oldsec;
setupdata_t *setup, *dest;
int total, console;

//
// wait until we get [net_nodos] packets, then start playing
// the playernumbers are assigned by netid
//
if ( net_nodos > 2 )
    sprintf ( buffer, "Procurando pelos outros %d jogadores...", net_nodos-1 );
else
    strcpy ( buffer, "Procurando pelo outro jogador..." );
outtextxy ( 130, 160, buffer );
outtextxy ( 130, 170, "Pressione ESC para abortar" );
linha = 180;
moveto ( 130, linha );

oldsec = -1;
setup = (setupdata_t *)&doomcom.data;
localtime = -1;		// in setup time, not game time

//
// build local setup info
//
nodesetup[0].nodesfound = 1;
nodesetup[0].nodeswanted = net_nodos;
doomcom.numnodes = 1;

do
    {
    //
    // check for aborting
    //
    while ( bioskey (1) )
	{
	if ( ( bioskey (0) & 0xFF ) == 27 ) // ESC
	    {
	    mensagem ( "Sincroniza‡ao de jogo", "em rede abortada!" );
	    closegraph ( );
	    exit ( 0 );
	    }
	}

    //
    // listen to the network
    //
    while ( GetPacket ( ) )
	{
	if ( doomcom.remotenode == -1 )
	    dest = &nodesetup[doomcom.numnodes];
	else
	    dest = &nodesetup[doomcom.remotenode];

	if ( remotetime != -1)
	    {	// an early game packet, not a setup packet
	    if ( doomcom.remotenode == -1)
		// Error ("Got an unknown game packet during setup");
		// if it allready started, it must have found all nodes
	    dest->nodesfound = dest->nodeswanted;
	    continue;
	    }

	// update setup ingo
	memcpy ( dest, setup, sizeof(*dest) );

	if ( doomcom.remotenode != -1)
	    continue;			// allready know that node address

	//
	// this is a new node
	//
	memcpy (&nodeadr[doomcom.numnodes], &remoteadr,
	    sizeof(nodeadr[doomcom.numnodes]) );

	//
	// if this node has a lower address, take all startup info
	//
	if ( memcmp (&remoteadr, &nodeadr[0], sizeof(&remoteadr) ) < 0 )
	    {
	    }

	doomcom.numnodes++;

	outtext ("Achei um!");
	    {
	    linha += 10;
	    moveto ( 180, linha );
	    }

	if ( doomcom.numnodes < net_nodos )
	    outtext ( "Procurando o resto..." );
	}

    //
    // we are done if all nodes have found all other nodes
    //
    for ( i=0 ; i<doomcom.numnodes ; i++ )
	if (nodesetup[i].nodesfound != nodesetup[i].nodeswanted)
	    break;

    if ( i == nodesetup[0].nodeswanted )
	break;		// got them all

    //
    // send out a broadcast packet every second
    //
    gettime (&time);
    if ( time.ti_sec == oldsec )
	continue;
    oldsec = time.ti_sec;

    outtext ("*");
    if ( getx() > 500 )
	{
	linha += 10;
	moveto ( 130, linha );
	}
    doomcom.datalength = sizeof(*setup);

    nodesetup[0].nodesfound = doomcom.numnodes;

    memcpy ( &doomcom.data, &nodesetup[0], sizeof(*setup) );

    SendPacket ( MAXNETNODES );	// send to all

    } while (1);

//
// count players
//
console = 0;
total = 0;
for ( i = 0 ; i < net_nodos ; i++ )
    {
    if ( nodesetup[i].drone )
	continue;
    total++;
    if ( total > MAXPLAYERS )
	{
	sprintf ( buffer, "Mais de %i jogadores", MAXPLAYERS );
	mensagem ( buffer, "especificados!" );
	}
    if ( memcmp (&nodeadr[i], &nodeadr[0], sizeof(nodeadr[0])) < 0 )
	console++;
    }


if ( !total )
    {
    mensagem ( "Nenhum jogador especificado!", "" );
    return;
    }

doomcom.consoleplayer = console;
doomcom.numplayers = total;

//outtext ("Console is player %i of %i\n", console+1, total);

return;
}

