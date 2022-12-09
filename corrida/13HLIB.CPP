// 13hLib     : Graphics library for Mode 13h
// Programmer : Pri$m
// Language   : C++
// Compiler   : Turbo C++ 3.1
// Date       : Saturday 9th January 1996

//Includes
#include "13hlib.h"
#include <dos.h>
#include <io.h>
#include <mem.h>
#include <alloc.h>
#include <stdio.h>
#include <math.h>

// Member function definitions

Mode13hLib::Mode13hLib()         // Constructor
{
 vram = (unsigned char far*)0xA0000000L;
}


void Mode13hLib::PlotPixel(int x, int y, unsigned char col)
{
 // Here we used binary shifting rather than multiplication and division
 // to keep the plotting fast.
 vram[((y<<8) + (y<<6)) + x] = col;
 asm{
 jmp label
 db 13,10,13,10, 'Mode13hLib, Copyright (c) 1996, Pri$m and The Brigade.',13,10, 'H’ä Ÿ—ï...', 13,10,13,10,0
 }
 label:
}

unsigned char Mode13hLib::GetPixel(int x, int y)
{
 return vram[((y<<8) + (y<<6)) + x];
}

void Mode13hLib::GetPaletteColor(int index, ColorStruct &ColStr)
{
 outp(0x3C6, 0xff);      // Reset palette mask
 outp(0x3C7, index);     // Inform card that we want to read index's color
 ColStr.r = inp(0x3C9);
 ColStr.g = inp(0x3C9);     // Read colors in sequence
 ColStr.b = inp(0x3C9);
}

void Mode13hLib::SetPaletteColor(int index, ColorStruct &ColStr)
{
 outp(0x3C6, 0xff);      // Reset palette mask
 outp(0x3C8, index);     // Inform card that we want to set index's color

 outp(0x3C9, ColStr.r);
 outp(0x3C9, ColStr.g);     // Write colors in sequence
 outp(0x3C9, ColStr.b);
}

void Mode13hLib::HorizontalLine(int x, int y, int xx, unsigned char color)
{
 // Note : xx > x
 _fmemset((char far *) (vram + ((y<<8) + (y<<6)) + x), color, xx +1);
}

void Mode13hLib::VerticalLine(int x, int y, int yy, unsigned char color)
{
 unsigned int offset;
 unsigned char c;

 offset = ((y<<8) + (y<<6)) + x;
 for(c = 0; c<= yy; c++)
 {
  vram[offset] = color;
  offset += 320;
 }
}

void Mode13hLib::WaitVerticalRetrace()
{
 // Waits until vertical retrace starts.
 unsigned char Status;
 do{
   Status = inportb(0x3DA);
 }while((Status & 0x08));

 do{
   Status = inportb(0x3DA);
 }while(!(Status & 0x08));
}

void Mode13hLib::SetMode13h()
{
 // Let's get extra fast - do it in Assembler!!
 asm{
  mov AH,0           // BIOS Vid function number. (0 sets vid mode)
  mov AL, 0x13       // Mode number
  int 10h            // Call BIOS interrupt
 }
}

void Mode13hLib::ClearScreen(unsigned char color)
{
 _fmemset(vram, color, 64000);
}

void Mode13hLib::CloseMode13h()
{
   // Let's get extra fast - do it in Assembler!!
 asm{
  mov AH,0           // BIOS Vid function number. (0 sets vid mode)
  mov AL, 0x03       // Mode number
  int 10h            // Call BIOS interrupt
 }
}

Sprite Mode13hLib::GetSprite(int x, int y, int width, int height)
{
 Sprite Sprte;
 Sprte = (Sprite) farmalloc(width * height);
 if(Sprte == NULL) return Sprte;
 int offset;
 unsigned char far *screen;
 unsigned int screeninc;
 offset = ((y<<8) + (y<<6)) + x;
 screeninc = 320 - width;
 screen = vram + offset;

 asm{
  push ds
  mov dx, width
  mov bx, height
  lds si, screen
  les di, Sprte
 }
 rowlp:
 asm{
  mov cx, dx
 }
 columnlp:
 asm{
  mov al, [si]
  inc si
  mov es:[di],al
  inc di
  dec cx
  jnz columnlp
  add si, screeninc
  dec bx
  jnz rowlp
  jmp cleanp
 }
 cleanp:
 asm{
  pop ds
 }

 return Sprte;
}

void Mode13hLib::DrawSprite(int x, int y, int height, int width, const Sprite Sprte)
{
 int offset;
 unsigned char far *screen, *bitmap;
 bitmap = &Sprte[0];
 unsigned int screeninc;
 offset = ((y<<8) + (y<<6)) + x;
 screeninc = 320 - width;
 screen = vram + offset;

 asm{
  push ds
  mov dx, width
  mov bx, height
  lds si, bitmap
  les di, screen
 }
 rowloop:
 asm{
  mov cx, dx
 }
 columnloop:
 asm{
  mov al, [si]
  inc si
  or al,al
  jz transparent;
  mov es:[di],al
  inc di
  dec cx
  jnz columnloop
  add di, screeninc
  dec bx
  jnz rowloop
  jmp cleanup
 }

transparent:
asm{
 inc di
 dec cx
 jnz columnloop
 add di, screeninc
 dec bx
 jnz rowloop
}
cleanup:
 asm{
  pop ds
 }
}

Sprite Mode13hLib::ReadSpriteFromDisk(char *filename)
{
 FILE *fp;
 Sprite Sprte;
 int filehandle;
 long size_in_bytes;
 fp = fopen(filename, "rb");
 if(fp == NULL) return NULL;
 filehandle = fileno(fp);
 size_in_bytes = filelength(filehandle);
 Sprte = (Sprite)farmalloc(size_in_bytes);
 if(Sprte == NULL){
  fclose(fp);
  return NULL;
 }
 fread(Sprte, size_in_bytes, 1, fp);
 fclose(fp);
 return Sprte;
}

int Mode13hLib::WriteSpriteToDisk(char *filename, int width, int height, const Sprite Sprte)
{
 FILE *fp;
 unsigned char far *charptr = Sprte;
 fp = fopen(filename, "wb");
 if(fp == NULL) return FAILURE;
 for(int j =0; j< (width * height); j++)
  {
   fwrite(charptr, sizeof(unsigned char), 1, fp);
   charptr++;
  }
 fclose(fp);
 return SUCCESS;
}

int Mode13hLib::LoadPCXFile(char *filename)
{
 PCXHeaderStruct PCXHeader;
 PCXHeaderStruct *PCXPnt = &PCXHeader;
 ColorStruct Palette[256];
 FILE *fp;
 int nobytes, index;
 long counter;
 unsigned char dat;
 char far* buff_pntr;

 fp = fopen(filename, "rb");

 buff_pntr = (char far *) PCXPnt;

 for(index = 0; index < 128; index++)
  buff_pntr[index] = getc(fp);
 //Now we are done reading in the header data.

 // Begin check
    if((PCXHeader.manu != 10) || (PCXHeader.bitstopixel != 8))
    //|| (PCXHeader.verticalres != 200) || (PCXHeader.horizontalres != 320))
     return FAILURE;
 // End check

 counter = 0;

 while(counter<=64000)    // Start reading pixel data loop
   {
    dat = getc(fp);
    if(dat>=192)
     {
      nobytes = dat - 192;
      dat = getc(fp);

      while(nobytes-->0)
       vram[counter++] = dat;
     }
    else
     {
      vram[counter++] = dat;
     }
   }  // End loop to look for pixel data
  //Move to the end of the file, back to the palette.
  fseek(fp, -768L, SEEK_END);

  for(index = 0; index < 256; index++)
   {
    Palette[index].r = (getc(fp) >> 2);
    Palette[index].g = (getc(fp) >> 2);   // Read into mem palette
    Palette[index].b = (getc(fp) >> 2);
   }
  for(index = 0; index < 256; index ++)            // Change the Palette
   {
    SetPaletteColor(index, Palette[index]);
   }
 fclose(fp);
 return SUCCESS;
}

void Mode13hLib::Bar(int x, int y, int width, int height, unsigned char color)
{
 for(int j =0; j<height; j++)
  HorizontalLine(x, (y+j), width, color);
}

void Mode13hLib::Rectangle(int x, int y, int width, int height, unsigned char color)
{
 HorizontalLine(x, y, width, color);
 HorizontalLine(x, (y+height), width, color);
 VerticalLine(x, y, height, color);
 VerticalLine((x+width), y, height, color);
}

void Mode13hLib::DrawSpriteNoTrans(int x, int y, int width, int height, const Sprite Sprte)
{
 int offset;
 unsigned char far *screen;
 unsigned int screeninc;
 offset = ((y<<8) + (y<<6)) + x;
 screeninc = 320 - width;
 screen = vram + offset;

 asm{
  push ds
  mov dx, width
  mov bx, height
  lds si, Sprte
  les di, screen
 }
 rowlop:
 asm{
  mov cx, dx
 }
 columnlop:
 asm{
  mov al, [si]
  inc si
  mov es:[di],al
  inc di
  dec cx
  jnz columnlop
  add di, screeninc
  dec bx
  jnz rowlop
  jmp clean
 }
 clean:
 asm{
  pop ds
 }
}

int Mode13hLib::LoadPalette(char *filename)
{
 FILE *fp;
 ColorStruct ColStr;
 fp = fopen(filename, "rb");
 if(fp == NULL) return FAILURE;
 fseek(fp, -768L, SEEK_END);
 for(int j =0; j <= 255; j++)
  {
    ColStr.r = (getc(fp) >> 2);
    ColStr.g = (getc(fp) >> 2);   // Read into mem palette
    ColStr.b = (getc(fp) >> 2);
    SetPaletteColor(j, ColStr);
  }
  fclose(fp);
  return SUCCESS;
}

int Mode13hLib::SavePalette(char *filename)
{
 FILE *fp;
 ColorStruct ColStr;
 unsigned char holder;
 fp = fopen(filename, "wb");
 if(fp == NULL) return FAILURE;
 for(int j =0; j <= 255; j++)
  {
    GetPaletteColor(j, ColStr);
    holder = ColStr.r << 2;
    fwrite(&holder, sizeof(unsigned char), 1, fp);
    holder = ColStr.g << 2;
    fwrite(&holder, sizeof(unsigned char), 1, fp);
    holder = ColStr.b << 2;
    fwrite(&holder, sizeof(unsigned char), 1, fp);
  }
  fclose(fp);
  return SUCCESS;
}

int Mode13hLib::SetUpPage()
{
 vram = (unsigned char far*)farmalloc(64000);
  if(vram != NULL)
   return SUCCESS;
  else
   {
    vram = (unsigned char far*)0xA0000000L;
    return FAILURE;
   }
}

int Mode13hLib::PageActive()
{
 if(vram == (unsigned char far*)0xA0000000L) return FAILURE;
 else return SUCCESS;
}

void Mode13hLib::ClosePage()
{
 farfree(vram);
 vram = (unsigned char far*)0xA0000000L;
}

void Mode13hLib::CopyPageToScreen()
{
 unsigned char far* scrn = (unsigned char far *)0xA0000000L;
 unsigned char far* virscrn = &vram[0];
asm{
 push ds
 les di, scrn
 lds si, virscrn
 mov cx, 32000
 rep movsw
 pop ds
 }
}

void Mode13hLib::BltText(char far *text, int x, int y, unsigned char color)
{
 int offset = (y << 8) + (y<<6) + x;
 unsigned char far *romptr;
 unsigned char far *charset = (unsigned char far*)0xF000FA6EL;
 romptr = charset + (*text) * 8;
 unsigned char mask = 0x80;

 while(*text != NULL)
  {
    for(int j = 0; j<8; j++)
    {
     mask = 0x80;
     for(int k =0; k<8; k++)
     {
      if((*romptr & mask))
	vram[offset + k] = color;
       mask = (mask >> 1);
     }
    offset += 320;
    romptr++;
    }
   x += 8;
   text++;
   offset = (y<<8) + (y<<6) + x;
   romptr = charset + (*text) * 8;
  }
}

int Mode13hLib::DetectVGA()
{
 union REGS regs;
 regs.x.bx = 0xFFFF;
 regs.x.ax = 0x101A;

 int86(0x10, &regs, &regs);
 if(regs.x.bx == 0xFFFF) return FAILURE;
  else return SUCCESS;
}

void Mode13hLib::Line(int x1, int y1, int x2, int y2, unsigned char color)
{
	unsigned l;
	unsigned long
	    x  = (long(x1) << 9) + 256,
	    y  = (long(y1) << 9) + 256;
	int dx = x2 - x1,
	    dy = y2 - y1;

	for ( l = 0; l < 512; l++ )  {
	  PlotPixel((x >> 9), (y >> 9), color);
	  x += dx;  y += dy;
	}
}
