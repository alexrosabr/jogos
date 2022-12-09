#define superkey teclando[inportb(0x60)&0x7f]=1-((inportb(0x60)&0x80)>>7)

#define _ESC	1
#define _LEFT	LEFT
#define _RIGHT	RIGHT
#define _UP	UP
#define _DOWN	DOWN
#define _CENTER LEFT+1
#define _HOME	UP-1
#define _END	DOWN-1

unsigned char teclando[128];

void inicializa_superkey ( void );

void inicializa_superkey ( void )
{
int i;

for ( i = 0; i < 100; i++ )
    teclando[i] = 0;
}
