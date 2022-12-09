// 13hLib     : Graphics library for Mode 13h
// Programmer : Pri$m
// Language   : C++
// Compiler   : Turbo C++ 3.1

#ifndef __13hlib_h__

//Includes
#include <alloc.h>
//Defines
#define SUCCESS 1
#define FAILURE 0
#define KillSprite(x) farfree(x)
// These macros are for use with the int type functions below.

//Type definitions
typedef unsigned char far* Sprite;

struct ColorStruct{   // Contains RGB data for a color
 unsigned char r;
 unsigned char g;
 unsigned char b;
};                    // Use this when changing the palette.

struct PCXHeaderStruct{
 char manu;
 char version;
 char encoding;
 char bitstopixel;
 int x, y;
 int w, h;
 int horizontalres;
 int verticalres;
 char ega_palette[48];
 char reserved;
 char colplanes;
 int bytetoline;
 int palette;
 char buff[58];
};

class Mode13hLib{
  public:
   Mode13hLib();
   void PlotPixel(int x, int y, unsigned char col);
   void GetPaletteColor(int index, ColorStruct& ColStr);
   void SetPaletteColor(int index, ColorStruct& ColStr);
   void HorizontalLine(int x, int y, int xx, unsigned char color);
   void VerticalLine(int x, int y, int yy, unsigned char color);
   void WaitVerticalRetrace();
   void DrawSprite(int x, int y, int width, int height, const Sprite Sprte);
   void DrawSpriteNoTrans(int x, int y, int width, int height, const Sprite Sprte);
   void ClearScreen(unsigned char color);
   void Line(int x1, int y1, int x2, int y2, unsigned char color);
   void Bar(int x, int y, int width, int height, unsigned char color);
   void Rectangle(int x, int y, int width, int heigth, unsigned char color);
   void SetMode13h();
   void CloseMode13h();
   void CopyPageToScreen();
   void ClosePage();
   void BltText(char far *text, int x, int y, unsigned char color);
   unsigned char GetPixel(int x, int y);
   int DetectVGA();
   int WriteSpriteToDisk(char *filename, int width, int height, const Sprite Sprte);
   int LoadPCXFile(char *filename);
   int LoadPalette(char *filename);
   int SavePalette(char *filename);
   int SetUpPage();
   int PageActive();
   Sprite  ReadSpriteFromDisk(char *filename);
   Sprite  GetSprite(int x, int y, int width, int height);

    protected:
   unsigned char far *vram;                      // Pointer to vram.
};


#define __13hlib_h__
#endif
