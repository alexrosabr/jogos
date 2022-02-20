object Form1: TForm1
  Left = 270
  Top = 105
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Rednaxel Tic-Tac-Toe'
  ClientHeight = 179
  ClientWidth = 257
  Color = clGreen
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 8
    Top = 8
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image1Click
  end
  object Image2: TImage
    Left = 48
    Top = 8
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image2Click
  end
  object Image3: TImage
    Left = 88
    Top = 8
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image3Click
  end
  object Image4: TImage
    Left = 8
    Top = 48
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image4Click
  end
  object Image5: TImage
    Left = 48
    Top = 48
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image5Click
  end
  object Image6: TImage
    Left = 88
    Top = 48
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image6Click
  end
  object Image7: TImage
    Left = 8
    Top = 88
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image7Click
  end
  object Image8: TImage
    Left = 48
    Top = 88
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image8Click
  end
  object Image9: TImage
    Left = 88
    Top = 88
    Width = 32
    Height = 32
    ParentShowHint = False
    ShowHint = False
    OnClick = Image9Click
  end
  object Label1: TLabel
    Left = 132
    Top = 5
    Width = 65
    Height = 16
    Caption = 'Computer '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 132
    Top = 25
    Width = 48
    Height = 16
    Caption = 'Human '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 243
    Top = 5
    Width = 7
    Height = 16
    Alignment = taRightJustify
    Caption = '0'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 243
    Top = 25
    Width = 7
    Height = 16
    Alignment = taRightJustify
    Caption = '0'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  object Label5: TLabel
    Left = 132
    Top = 45
    Width = 35
    Height = 16
    Caption = 'Draw '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 243
    Top = 45
    Width = 7
    Height = 16
    Alignment = taRightJustify
    Caption = '0'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  object Gauge1: TGauge
    Left = 136
    Top = 100
    Width = 105
    Height = 16
    Color = clBlack
    ForeColor = clNavy
    ParentColor = False
    Progress = 0
    Visible = False
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 160
    Width = 257
    Height = 19
    Panels = <
      item
        Alignment = taRightJustify
        Text = '0/0'
        Width = 50
      end
      item
        Alignment = taCenter
        Text = '0%'
        Width = 50
      end
      item
        Text = 'Level'
        Width = 50
      end>
    ParentShowHint = False
    ShowHint = False
    SimplePanel = False
  end
  object Button1: TButton
    Left = 152
    Top = 68
    Width = 75
    Height = 25
    Caption = '&New Game'
    ParentShowHint = False
    ShowHint = False
    TabOrder = 1
    OnClick = Novojogo1Click
  end
  object MainMenu1: TMainMenu
    Left = 216
    Top = 123
    object Arquivo1: TMenuItem
      Caption = '&Game'
      object Novojogo1: TMenuItem
        Caption = 'New &Game'
        ShortCut = 113
        OnClick = Novojogo1Click
      end
      object QuitGame: TMenuItem
        Caption = '&Resign'
        ShortCut = 16465
        OnClick = QuitGameClick
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object Sair1: TMenuItem
        Caption = 'E&xit'
        OnClick = Sair1Click
      end
    end
    object Options1: TMenuItem
      Caption = '&Options'
      object Alternar: TMenuItem
        Caption = 'Alternate &start'
        Checked = True
        ShortCut = 114
        OnClick = AlternarClick
      end
      object XisHumano: TMenuItem
        Caption = 'Human plays with &X'
        Checked = True
        ShortCut = 115
        OnClick = XisHumanoClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object Menu1: TMenuItem
        Caption = '&Artificial Intelligence'
        Hint = 'Level'
        object Princip: TMenuItem
          Caption = '&Novice'
          RadioItem = True
          ShortCut = 16462
          OnClick = PrincipClick
        end
        object Intermed: TMenuItem
          Caption = '&Intermediate'
          Checked = True
          RadioItem = True
          ShortCut = 16457
          OnClick = IntermedClick
        end
        object Expert: TMenuItem
          Caption = '&Expert'
          RadioItem = True
          ShortCut = 16453
          OnClick = ExpertClick
        end
      end
      object Language1: TMenuItem
        Caption = '&Language'
        object English1: TMenuItem
          Caption = '&English'
          Checked = True
          RadioItem = True
          OnClick = English1Click
        end
        object Portuguese1: TMenuItem
          Caption = '&Português'
          RadioItem = True
          OnClick = Portuguese1Click
        end
      end
    end
    object Ajuda1: TMenuItem
      Caption = '&Help'
      object HelpTopics: TMenuItem
        Caption = '&Help Topics'
        Enabled = False
        ShortCut = 112
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object SobreoJogodaVelha1: TMenuItem
        Caption = '&About Jogo da Velha...'
        OnClick = SobreoJogodaVelha1Click
      end
    end
  end
end
