object Form3: TForm3
  Left = 272
  Top = 445
  Width = 632
  Height = 325
  Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090#1099' '#1090#1077#1089#1090#1080#1088#1086#1074#1072#1085#1080#1103' '#1076#1072#1085#1085#1099#1084#1080' '#1080#1079' '#1092#1072#1081#1083#1072':'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDefaultPosOnly
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 0
    Top = 0
    Width = 616
    Height = 267
    Align = alClient
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 0
    WantReturns = False
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = '*.log'
    Filter = #1058#1077#1082#1089#1090#1086#1074#1099#1077' '#1092#1072#1081#1083#1099'|*.txt|'#1060#1072#1081#1083#1099' '#1083#1086#1075#1086#1074'|*.log|'#1042#1089#1077' '#1092#1072#1081#1083#1099'|*.*'
    Left = 376
  end
  object MainMenu1: TMainMenu
    Left = 344
    object N1: TMenuItem
      Caption = #1060#1072#1081#1083
      object N2: TMenuItem
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100
        OnClick = N2Click
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object N4: TMenuItem
        Caption = #1047#1072#1082#1088#1099#1090#1100
        OnClick = N4Click
      end
    end
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = '*.txt'
    Filter = #1058#1077#1082#1089#1090#1086#1074#1099#1077' '#1092#1072#1081#1083#1099'|*.txt|'#1060#1072#1081#1083#1099' '#1083#1086#1075#1086#1074'|*.log|'#1042#1089#1077' '#1092#1072#1081#1083#1099'|*.*'
    Left = 408
  end
end
