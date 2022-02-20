//#################################################################
//# VELHA - Rednaxel
//#################################################################
// 1.02 - separação do .RES
// 1.03 - onPaint
// 1.04 - minor Bugs
// 1.05 - GetUserLanguageID

unit UnPrinc;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, Menus, ImgList, ComCtrls, StdCtrls, ToolWin, Gauges;

type
  TForm1 = class(TForm)
    Image1: TImage;
    Image2: TImage;
    Image3: TImage;
    Image4: TImage;
    Image5: TImage;
    Image6: TImage;
    Image7: TImage;
    Image8: TImage;
    Image9: TImage;
    MainMenu1: TMainMenu;
    Arquivo1: TMenuItem;
    Novojogo1: TMenuItem;
    Princip: TMenuItem;
    Intermed: TMenuItem;
    Expert: TMenuItem;
    N2: TMenuItem;
    Sair1: TMenuItem;
    Ajuda1: TMenuItem;
    HelpTopics: TMenuItem;
    N3: TMenuItem;
    SobreoJogodaVelha1: TMenuItem;
    Alternar: TMenuItem;
    StatusBar1: TStatusBar;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Button1: TButton;
    Gauge1: TGauge;
    Options1: TMenuItem;
    XisHumano: TMenuItem;
    Menu1: TMenuItem;
    N1: TMenuItem;
    QuitGame: TMenuItem;
    Language1: TMenuItem;
    English1: TMenuItem;
    Portuguese1: TMenuItem;
    procedure Sair1Click(Sender: TObject);
    procedure PrincipClick(Sender: TObject);
    procedure IntermedClick(Sender: TObject);
    procedure ExpertClick(Sender: TObject);
    procedure Novojogo1Click(Sender: TObject);
    procedure Image1Click(Sender: TObject);
    procedure Image2Click(Sender: TObject);
    procedure Image3Click(Sender: TObject);
    procedure Image4Click(Sender: TObject);
    procedure Image5Click(Sender: TObject);
    procedure Image6Click(Sender: TObject);
    procedure Image7Click(Sender: TObject);
    procedure Image8Click(Sender: TObject);
    procedure Image9Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure SobreoJogodaVelha1Click(Sender: TObject);
    procedure AlternarClick(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure FormPaint(Sender: TObject);
    procedure XisHumanoClick(Sender: TObject);
    procedure QuitGameClick(Sender: TObject);
    procedure English1Click(Sender: TObject);
    procedure Portuguese1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

const
  VERSAO = ' 1.05';
  HUMANO = 1;
  CPU = 10;
  VELHA = 100;
  BRANCO = 1;
  XIS = 2;
  BOLA = 3;
  DFLT_QUIT = 'Really resign the current game?';
  DFLT_LEVEL = 'Level';

var
  Form1: TForm1;
  nivel, bmpCPU, bmpHUMANO: integer;
  partida, PerguntaDesiste, TextoSobre: string;
  pensando, emjogo, cpufirst: boolean;
  vitHumano, vitCPU, vitVelha: array [1..3] of integer;

implementation

{$R *.DFM}
{$R FIGURAS.RES}
//#################################################################
//#################################################################
//#################################################################

//-----------------------------------------------------------------
function GameOver ( nova, quem: integer;
  historico:string; valendo:boolean ): integer;
var x,y,c,i,j,rev,winner,linha:integer;
  matriz: array[1..4,1..4] of integer;
  x1,y1,x2,y2:integer;
begin
//--- inicializa as posicoes
x1 := 0;
y1 := 0;
x2 := 0;
y2 := 0;
//--- limpa a matriz
rev := 0;
for i:=1 to 4 do
  for j:=1 to 4 do
    matriz[i,j] := 0;
//--- verifica quem saiu jogando
if cpufirst then
  y := CPU
else
  y := HUMANO;
//--- preenche a matriz
for c := 1 to Length(historico) do begin
  x := StrToInt(Copy(historico,c,1));
  i := ((x-1) div 3)+1;
  j := ((x-1) mod 3)+1;
  if matriz[i,j] = 0 then matriz[i,j] := y;
  if y = CPU then y := HUMANO else y := CPU;
  end;
//--- insere uma jogada para teste
if nova <> 0 then begin
  i := ((nova-1) div 3)+1;
  j := ((nova-1) mod 3)+1;
  if matriz[i,j] = 0 then matriz[i,j] := quem;
  end;
//--- forma o quadrado magico
for c := 1 to 3 do begin
  matriz[c,4] := matriz[c,1]+matriz[c,2]+matriz[c,3];
  matriz[4,c] := matriz[1,c]+matriz[2,c]+matriz[3,c];
  matriz[4,4] := matriz[4,4]+matriz[c,c];
  rev := rev + matriz[c,4-c];
  end;
//--- testa tudo
winner := 0;
linha := 0;
for x := 1 to 2 do begin
  if x = 1 then y := HUMANO else y := CPU;
  for c := 1 to 3 do begin
    if matriz[c,4]=y*3 then begin
      linha := c;
      winner := y;
      Break;
      end;
    if matriz[4,c]=y*3 then begin
      linha := c+3;
      winner := y;
      Break;
      end;
    end;
  if matriz[4,4]=y*3 then begin
    linha := 7;
    winner := y;
    end;
  if rev=y*3 then begin
    linha := 8;
    winner := y;
    end;
  if winner <> 0 then Break;
  end;
//--- desenha se for pra valer
if (nova <> 0) or not valendo then linha := 0;
if linha > 0 then begin
  Form1.Canvas.Pen.Width := 3;
  Form1.Canvas.Pen.Color := $00000000;
  case linha of
    1: begin
      x1 := Form1.Image1.Left + 8;
      y1 := Form1.Image1.Top + 16;
      x2 := Form1.Image3.Left + 24;
      y2 := Form1.Image3.Top + 16;
      end;
    2: begin
      x1 := Form1.Image4.Left + 8;
      y1 := Form1.Image4.Top + 16;
      x2 := Form1.Image6.Left + 24;
      y2 := Form1.Image6.Top + 16;
      end;
    3: begin
      x1 := Form1.Image7.Left + 8;
      y1 := Form1.Image7.Top + 16;
      x2 := Form1.Image9.Left + 24;
      y2 := Form1.Image9.Top + 16;
      end;
    4: begin
      x1 := Form1.Image1.Left + 16;
      y1 := Form1.Image1.Top + 8;
      x2 := Form1.Image7.Left + 16;
      y2 := Form1.Image7.Top + 24;
      end;
    5: begin
      x1 := Form1.Image2.Left + 16;
      y1 := Form1.Image2.Top + 8;
      x2 := Form1.Image8.Left + 16;
      y2 := Form1.Image8.Top + 24;
      end;
    6: begin
      x1 := Form1.Image3.Left + 16;
      y1 := Form1.Image3.Top + 8;
      x2 := Form1.Image9.Left + 16;
      y2 := Form1.Image9.Top + 24;
      end;
    7: begin
      x1 := Form1.Image1.Left + 8;
      y1 := Form1.Image1.Top + 8;
      x2 := Form1.Image9.Left + 24;
      y2 := Form1.Image9.Top + 24;
      end;
    8: begin
      x1 := Form1.Image7.Left + 7;
      y1 := Form1.Image7.Top + 23;
      x2 := Form1.Image3.Left + 23;
      y2 := Form1.Image3.Top + 7;
    end
  else
  end;
  Form1.Canvas.MoveTo ( x1, y1 );
  Form1.Canvas.LineTo ( x2, y2 );
  end;
GameOver := winner;
end;

//-----------------------------------------------------------------
function MiniMaxValue(game:string):integer;
var cpumove : boolean;
  utility, value : integer;
  lowest, highest, i : integer;
begin
Application.ProcessMessages;
if cpufirst then begin
  if Length(game) mod 2 = 0 then
    cpumove := true else cpumove := false;
  end
else begin
  if Length(game) mod 2 = 0 then
    cpumove := false else cpumove := true;
  end;
utility := GameOver(0,0,game,false);
if (Length(game)=9) or (utility<>0) then begin
  if utility = CPU then value := 1
    else if utility = HUMANO then value := -1
      else value := 0;
  MiniMaxValue := value;
  Exit;
  end
else
  if cpumove then begin
    highest := -99;
    for i := 1 to 9 do begin
      if Pos(IntToStr(i),game) <> 0 then continue;
      value := MiniMaxValue (game+IntToStr(i));
      if value > highest then
        highest := value;
      end;
    MiniMaxValue := highest;
    Exit;
    end
  else begin
    lowest := 99;
    for i := 1 to 9 do begin
      if Pos(IntToStr(i),game) <> 0 then continue;
      value := MiniMaxValue (game+IntToStr(i));
      if value < lowest then
        lowest := value;
      end;
    MiniMaxValue := lowest;
    Exit;
    end;
end;

//-----------------------------------------------------------------
function MiniMaxDecision(sit:string):integer;
var i, bestmove, highestvalue, value, cont, num : integer;
  old_cursor : TCursor;
  vetor : array[1..9] of integer;
begin
highestvalue := -99;
bestmove := -1;
old_cursor := Screen.Cursor;
Screen.Cursor := crHourglass;
Form1.Gauge1.Visible := true;
Form1.Gauge1.Progress := 0;
try
  for i := 1 to 9 do begin
    vetor[i] := -99;
    if Pos(IntToStr(i),sit) <> 0 then continue;
    value := MiniMaxValue (sit+IntToStr(i));
    if value >= highestvalue then begin
      highestvalue := value;
      bestmove := i;
      end;
    Form1.Gauge1.Progress := i*11;
    vetor[i] := value;
    end;
  finally
  Screen.Cursor := old_cursor;
  end;
Form1.Gauge1.Progress := 100;
Form1.Gauge1.Visible := false;
cont := 0;
for i := 1 to 9 do
  if vetor[i] = highestvalue then Inc(cont);
if cont > 1 then begin
  num := random(cont);
  cont := 0;
  for i := 1 to 9 do
    if vetor[i] = highestvalue then begin
      Inc(cont);
      if cont = num then begin
        bestmove := i;
        break;
        end;
      end;
  end;
MiniMaxDecision := bestmove;
end;

//-----------------------------------------------------------------
procedure MostraPlacar(level:integer);
var pg, pj:integer;
begin
with Form1 do
  begin
  Label3.Caption := IntToStr(VitCPU[level]);
  Label4.Caption := IntToStr(VitHumano[level]);
  Label6.Caption := IntToStr(VitVelha[level]);
  pj := (VitHumano[level]+VitCpu[level]+VitVelha[level])*2;
  pg := VitHumano[level]*2+VitVelha[level];
  if pj > 0 then begin
    StatusBar1.Panels[0].Text := IntToStr(pg)+'/'+IntToStr(pj);
    StatusBar1.Panels[1].Text := IntToStr(100*pg div pj)+'%';
    end
  else begin
    StatusBar1.Panels[0].Text := '0/0';
    StatusBar1.Panels[1].Text := '0%';
    end;
  StatusBar1.Panels[2].Text := Menu1.Hint+': '+IntToStr(nivel);
  end;
end;

//-----------------------------------------------------------------
function EstaLivre ( posic:integer ):boolean;
begin
if Pos(IntToStr(posic),partida) <> 0 then
  EstaLivre := false
else
  EstaLivre := true;
end;

//-----------------------------------------------------------------
procedure SetaCelula ( var img: TImage; pedra, pos: integer );
var figura : string;
  x, y : integer;
  TmpBmp : TBitMap;
begin
figura := 'FIGURA'+IntToStr(pedra);
TmpBmp := TBitMap.Create;
try
  x := img.Left;
  y := img.Top;
  TmpBmp.LoadFromResourceName(hInstance,figura);
  Form1.Canvas.Draw ( x, y, TmpBmp );
except
  emjogo := false;
  Form1.XisHumano.Enabled := true;
  Form1.QuitGame.Enabled := false;
  Form1.Close;
end;
TmpBmp.Free;
if pos < 0 then Exit;
if pos <> 0 then img.Tag := pos * 10;
img.Tag := (img.Tag div 10) * 10 + pedra;
//img.Hint := IntToStr(img.Tag);
end;

//-----------------------------------------------------------------
procedure Vitoria ( w: integer );
var pg, pj:integer;
begin
case w of
  CPU: begin
    Inc(vitCPU[nivel]);
    Form1.Label3.Caption := IntToStr(vitCPU[nivel]);
    end;
  HUMANO: begin
    Inc(vitHumano[nivel]);
    Form1.Label4.Caption := IntToStr(vitHumano[nivel]);
    end;
  VELHA: begin
    Inc(vitVelha[nivel]);
    Form1.Label6.Caption := IntToStr(vitVelha[nivel]);
    end;
  end;
with Form1 do begin
  Button1.Enabled := true;
  Button1.SetFocus;
  Novojogo1.Enabled := true;
{  Princip.Enabled := true;
  Intermed.Enabled := true;
  Expert.Enabled := true;}
  end;
emjogo := false;
Form1.QuitGame.Enabled := false;
Form1.XisHumano.Enabled := true;
pj := (VitHumano[nivel]+VitCpu[nivel]+VitVelha[nivel])*2;
pg := VitHumano[nivel]*2+VitVelha[nivel];
Form1.StatusBar1.Panels[0].Text := IntToStr(pg)+'/'+IntToStr(pj);
Form1.StatusBar1.Panels[1].Text := IntToStr(100*pg div pj)+'%';
end;

//-----------------------------------------------------------------
procedure ComputaJogada ( var jogada: TImage; quemjoga:integer );
var lance, i:integer;
  temp: TImage;
begin
if pensando then begin
  Beep;
  Exit;
  end;
if quemjoga = HUMANO then begin
  if not emjogo or ((jogada.Tag mod 10) <> 1) then begin
    Beep;
    Exit;
    end;
  SetaCelula(jogada,bmpHUMANO,0);
  partida := partida + IntToStr(jogada.Tag div 10);
  if GameOver(0,0,partida,true) = HUMANO then begin
    Vitoria ( HUMANO );
    Exit;
    end;
  end;
if Length(partida) >= 9 then begin
  Vitoria ( VELHA );
  Exit;
  end;
//----- Inicio IA -----
pensando := true;
lance := 1;
case nivel of
  1: begin //--- INICIANTE
    repeat
      lance := random(9)+1;
    until EstaLivre ( lance );
    end;
  2: begin //--- INTERMEDIARIO
    lance := 0;
    for i:= 1 to 9 do
      if GameOver(i,CPU,partida,false) = CPU then lance := i;
    if lance = 0 then begin
      for i:= 1 to 9 do
        if GameOver(i,HUMANO,partida,false) = HUMANO then lance := i;
      if lance = 0 then
        if nivel = 3 then
          lance := MiniMaxDecision (partida)
        else
          repeat
            lance := random(9)+1;
          until EstaLivre ( lance );
      end;
    end;
  3: begin //--- EXPERT
    if partida = '' then lance := random(5)*2+1
      else
    lance := MiniMaxDecision (partida);
    end;
  end; //--- case
//----- Fim IA -----
if (lance < 1) or (lance > 9 ) then begin
  ShowMessage('Fatal Error EF01: Invalid value.');
  Exit;
  end;
case lance of
  1: temp := Form1.Image1;
  2: temp := Form1.Image2;
  3: temp := Form1.Image3;
  4: temp := Form1.Image4;
  5: temp := Form1.Image5;
  6: temp := Form1.Image6;
  7: temp := Form1.Image7;
  8: temp := Form1.Image8;
  9: temp := Form1.Image9;
  end;
SetaCelula(temp,bmpCPU,0);
pensando := false;
partida := partida + IntToStr(temp.Tag div 10);
if GameOver(0,0,partida,true) = CPU then begin
  Vitoria ( CPU );
  Exit;
  end;
if Length(partida) >= 9 then begin
  Vitoria ( VELHA );
  Exit;
  end;
end;

//#################################################################
//#################################################################
//#################################################################

procedure TForm1.Sair1Click(Sender: TObject);
begin
Form1.Close;
end;

procedure TForm1.PrincipClick(Sender: TObject);
begin
if nivel = 1 then Exit;
Princip.Checked := true;
nivel := 1;
MostraPlacar(nivel);
cpufirst := false;
NovoJogo1Click(self);
end;

procedure TForm1.IntermedClick(Sender: TObject);
begin
if nivel = 2 then Exit;
Intermed.Checked := true;
nivel := 2;
MostraPlacar(nivel);
cpufirst := false;
NovoJogo1Click(self);
end;

procedure TForm1.ExpertClick(Sender: TObject);
begin
if nivel = 3 then Exit;
Expert.Checked := true;
nivel := 3;
MostraPlacar(nivel);
cpufirst := false;
NovoJogo1Click(self);
end;

procedure TForm1.Novojogo1Click(Sender: TObject);
begin
with Canvas do begin
  Pen.Width := 1;
  Pen.Color := clBlack;
  Brush.Color := Pen.Color;
  Rectangle(8,8,120,120);
  end;
SetaCelula(Image1,BRANCO,1);
SetaCelula(Image2,BRANCO,2);
SetaCelula(Image3,BRANCO,3);
SetaCelula(Image4,BRANCO,4);
SetaCelula(Image5,BRANCO,5);
SetaCelula(Image6,BRANCO,6);
SetaCelula(Image7,BRANCO,7);
SetaCelula(Image8,BRANCO,8);
SetaCelula(Image9,BRANCO,9);
partida := '';
if Alternar.Checked then cpufirst := not cpufirst;
if cpufirst then ComputaJogada(Image1,CPU);
emjogo := true;
Form1.QuitGame.Enabled := true;
Form1.XisHumano.Enabled := false;
Button1.Enabled := false;
Novojogo1.Enabled := false;
end;

procedure TForm1.Image1Click(Sender: TObject);
begin
ComputaJogada(Image1,HUMANO);
end;

procedure TForm1.Image2Click(Sender: TObject);
begin
ComputaJogada(Image2,HUMANO);
end;

procedure TForm1.Image3Click(Sender: TObject);
begin
ComputaJogada(Image3,HUMANO);
end;

procedure TForm1.Image4Click(Sender: TObject);
begin
ComputaJogada(Image4,HUMANO);
end;

procedure TForm1.Image5Click(Sender: TObject);
begin
ComputaJogada(Image5,HUMANO);
end;

procedure TForm1.Image6Click(Sender: TObject);
begin
ComputaJogada(Image6,HUMANO);
end;

procedure TForm1.Image7Click(Sender: TObject);
begin
ComputaJogada(Image7,HUMANO);
end;

procedure TForm1.Image8Click(Sender: TObject);
begin
ComputaJogada(Image8,HUMANO);
end;

procedure TForm1.Image9Click(Sender: TObject);
begin
ComputaJogada(Image9,HUMANO);
end;

procedure TForm1.FormCreate(Sender: TObject);
var i, prim_lang:integer;
  lang : LANGID;
begin
Caption := Caption + VERSAO;
ClientHeight := 128 + StatusBar1.Height;
cpufirst := false;
Randomize;
bmpCPU := BOLA;
bmpHUMANO := XIS;
//--- nivel medio
for i:=1 to 3 do begin
  vitCPU[i]:=0;
  vitHumano[i]:=0;
  vitVelha[i]:=0;
  end;
nivel := 1;
IntermedClick(self);
pensando := false;
PerguntaDesiste := DFLT_QUIT;
lang := GetUserDefaultLangID();
prim_lang := lang and $03ff;
if prim_lang = $16 then Portuguese1Click(self);
end;

procedure TForm1.SobreoJogodaVelha1Click(Sender: TObject);
begin
Application.MessageBox('Escrito por Alexsander da Rosa'+chr(13)+
  'Copyright © 2002 Rednaxel Informática'+chr(13)+
  'http://www.rednaxel.com.br',
  'About Velha'+VERSAO, MB_OK + MB_ICONINFORMATION );
end;

procedure TForm1.AlternarClick(Sender: TObject);
begin
Alternar.Checked := not Alternar.Checked;
end;

procedure TForm1.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
begin
if emjogo then
  if MessageDlg( PerguntaDesiste, mtConfirmation,
    [mbOk, mbCancel], 0) = mrCancel then
     CanClose := False;
end;

procedure TForm1.FormPaint(Sender: TObject);
begin
with Canvas do begin
  Pen.Width := 1;
  Pen.Color := clBlack;
  Brush.Color := Pen.Color;
  Rectangle(8,8,120,120);
  end;
SetaCelula(Image1,Image1.Tag mod 10,-1);
SetaCelula(Image2,Image2.Tag mod 10,-1);
SetaCelula(Image3,Image3.Tag mod 10,-1);
SetaCelula(Image4,Image4.Tag mod 10,-1);
SetaCelula(Image5,Image5.Tag mod 10,-1);
SetaCelula(Image6,Image6.Tag mod 10,-1);
SetaCelula(Image7,Image7.Tag mod 10,-1);
SetaCelula(Image8,Image8.Tag mod 10,-1);
SetaCelula(Image9,Image9.Tag mod 10,-1);
end;

procedure TForm1.XisHumanoClick(Sender: TObject);
begin
XisHumano.Checked := not XisHumano.Checked;
if XisHumano.Checked then begin
  bmpCPU := BOLA;
  bmpHUMANO := XIS;
  end
  else begin
  bmpCPU := XIS;
  bmpHUMANO := BOLA;
  end;
end;

procedure TForm1.QuitGameClick(Sender: TObject);
begin
if emjogo then
  Vitoria(CPU);
end;

procedure TForm1.English1Click(Sender: TObject);
begin
English1.Checked := true;
Caption := 'Rednaxel Tic-Tac-Toe'+VERSAO;
Label1.Caption := 'Computer ';
Label2.Caption := 'Human ';
Label5.Caption := 'Draw ';
Button1.Caption := '&New Game';
Arquivo1.Caption := '&Game';
Options1.Caption := '&Options';
Ajuda1.Caption := '&Help';
Novojogo1.Caption := Button1.Caption;
QuitGame.Caption := '&Resign';
Sair1.Caption := 'E&xit';
Alternar.Caption := 'Alternate &start';
XisHumano.Caption := 'Human plays with &X';
Menu1.Caption := '&Artificial Intelligence';
Menu1.Hint := DFLT_LEVEL;
Language1.Caption := '&Language';
Princip.Caption := '&Novice';
Intermed.Caption := '&Intermediate';
Expert.Caption := '&Expert';
StatusBar1.Panels[2].Text := Menu1.Hint+': '+IntToStr(nivel);
SobreoJogodaVelha1.Caption := 'About Jogo da Velha...';
HelpTopics.Caption := '&Help Topics';
PerguntaDesiste := DFLT_QUIT;
end;

procedure TForm1.Portuguese1Click(Sender: TObject);
begin
Portuguese1.Checked := true;
Caption := 'Rednaxel Velha'+VERSAO;
Label1.Caption := 'Computador ';
Label2.Caption := 'Humano';
Label5.Caption := 'Empate';
Button1.Caption := '&Novo Jogo';
Arquivo1.Caption := '&Jogo';
Options1.Caption := '&Opções';
Ajuda1.Caption := '&Ajuda';
Novojogo1.Caption := Button1.Caption;
QuitGame.Caption := '&Desistir';
Sair1.Caption := 'Sai&r';
Alternar.Caption := 'Alternar &saída';
XisHumano.Caption := 'Humano joga com &X';
Menu1.Caption := 'Inteligência &Artificial';
Menu1.Hint := 'Nível';
Language1.Caption := '&Idioma';
Princip.Caption := '&Novato';
Intermed.Caption := '&Intermediário';
Expert.Caption := '&Especialista';
StatusBar1.Panels[2].Text := Menu1.Hint+': '+IntToStr(nivel);
SobreoJogodaVelha1.Caption := 'Sobre o Jogo da Velha...';
HelpTopics.Caption := '&Tópicos da Ajuda';
PerguntaDesiste := 'Desiste mesmo da partida?';
end;

end.

//#################################################################
//#################################################################
//#################################################################

