object Form1: TForm1
  Left = 449
  Top = 144
  BorderStyle = bsSingle
  Caption = #1054#1073#1098#1077#1082#1090' "'#1056#1072#1094#1080#1086#1085#1072#1083#1100#1085#1072#1103' '#1076#1088#1086#1073#1100'"'
  ClientHeight = 300
  ClientWidth = 326
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesktopCenter
  Visible = True
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 216
    Top = 88
    Width = 16
    Height = 33
    AutoSize = False
    Caption = '='
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object OperatorSymbol: TLabel
    Left = 80
    Top = 88
    Width = 16
    Height = 32
    Caption = '+'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Functions1: TRadioGroup
    Left = 8
    Top = 152
    Width = 313
    Height = 81
    Caption = #1060#1091#1085#1082#1094#1080#1080
    Columns = 5
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ItemIndex = 0
    Items.Strings = (
      'F1(X)'
      'F2(X)'
      'F3(X)'
      'F4(X)'
      'F5(X)')
    ParentFont = False
    TabOrder = 11
    Visible = False
    OnClick = OperatorSelectClick
  end
  object F5: TGroupBox
    Left = 80
    Top = 56
    Width = 121
    Height = 89
    Caption = 'F(X)'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 7
    Visible = False
    object Label17: TLabel
      Left = 48
      Top = 40
      Width = 6
      Height = 14
      Caption = '+'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel7: TBevel
      Left = 64
      Top = 48
      Width = 33
      Height = 1
      Shape = bsTopLine
    end
    object Label18: TLabel
      Left = 64
      Top = 32
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '5X'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label19: TLabel
      Left = 64
      Top = 56
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '11'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label20: TLabel
      Left = 8
      Top = 32
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '3'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel8: TBevel
      Left = 8
      Top = 48
      Width = 33
      Height = 1
      Shape = bsTopLine
    end
    object Label21: TLabel
      Left = 8
      Top = 56
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'X'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object F1: TGroupBox
    Left = 80
    Top = 56
    Width = 121
    Height = 89
    Caption = 'F(X)'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    Visible = False
    object Label2: TLabel
      Left = 32
      Top = 40
      Width = 25
      Height = 14
      Caption = '2X + '
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel2: TBevel
      Left = 64
      Top = 48
      Width = 33
      Height = 1
      Shape = bsTopLine
    end
    object Label3: TLabel
      Left = 64
      Top = 32
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '1'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label4: TLabel
      Left = 64
      Top = 56
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'X'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object F2: TGroupBox
    Left = 80
    Top = 56
    Width = 121
    Height = 89
    Caption = 'F(X)'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    Visible = False
    object Bevel3: TBevel
      Left = 32
      Top = 48
      Width = 65
      Height = 1
      Shape = bsTopLine
    end
    object Label6: TLabel
      Left = 32
      Top = 32
      Width = 65
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'X - 1'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label7: TLabel
      Left = 48
      Top = 56
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'X + 2'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object F3: TGroupBox
    Left = 80
    Top = 56
    Width = 121
    Height = 89
    Caption = 'F(X)'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 5
    Visible = False
    object Label5: TLabel
      Left = 32
      Top = 40
      Width = 10
      Height = 14
      Caption = 'X '
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel4: TBevel
      Left = 64
      Top = 48
      Width = 33
      Height = 1
      Shape = bsTopLine
    end
    object Label8: TLabel
      Left = 64
      Top = 32
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '1'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label9: TLabel
      Left = 64
      Top = 56
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'X'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label10: TLabel
      Left = 40
      Top = 32
      Width = 6
      Height = 14
      Caption = '2'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label11: TLabel
      Left = 54
      Top = 40
      Width = 7
      Height = 14
      Caption = '- '
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object F4: TGroupBox
    Left = 80
    Top = 56
    Width = 121
    Height = 89
    Caption = 'F(X)'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 6
    Visible = False
    object Label12: TLabel
      Left = 48
      Top = 40
      Width = 4
      Height = 14
      Caption = '-'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel5: TBevel
      Left = 64
      Top = 48
      Width = 33
      Height = 1
      Shape = bsTopLine
    end
    object Label13: TLabel
      Left = 64
      Top = 32
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '1'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label14: TLabel
      Left = 64
      Top = 56
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '1 + X'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label15: TLabel
      Left = 8
      Top = 32
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'X'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel6: TBevel
      Left = 8
      Top = 48
      Width = 33
      Height = 1
      Shape = bsTopLine
    end
    object Label16: TLabel
      Left = 8
      Top = 56
      Width = 33
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '3'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 273
    Width = 326
    Height = 27
    Panels = <
      item
        Width = 60
      end
      item
        Width = 130
      end
      item
        Width = 100
      end>
    SimplePanel = False
  end
  object Operand1: TGroupBox
    Left = 8
    Top = 56
    Width = 65
    Height = 89
    Caption = 'X'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    object Bevel1: TBevel
      Left = 8
      Top = 48
      Width = 49
      Height = 33
      Shape = bsTopLine
    end
    object EnterNum: TEdit
      Left = 8
      Top = 24
      Width = 49
      Height = 23
      Hint = #1063#1080#1089#1083#1080#1090#1077#1083#1100
      TabOrder = 0
      Text = '1'
      OnChange = OperatorSelectClick
    end
    object EnterDenom: TEdit
      Left = 8
      Top = 56
      Width = 49
      Height = 23
      Hint = #1047#1085#1072#1084#1077#1085#1072#1090#1077#1083#1100
      TabOrder = 1
      Text = '1'
      OnChange = OperatorSelectClick
    end
  end
  object Operand2: TGroupBox
    Left = 128
    Top = 56
    Width = 65
    Height = 89
    Caption = 'X1'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    object Bevel26: TBevel
      Left = 8
      Top = 48
      Width = 49
      Height = 33
      Shape = bsTopLine
    end
    object EnterNum2: TEdit
      Left = 8
      Top = 24
      Width = 49
      Height = 23
      Hint = #1063#1080#1089#1083#1080#1090#1077#1083#1100
      TabOrder = 0
      Text = '1'
      OnChange = OperatorSelectClick
    end
    object EnterDenom2: TEdit
      Left = 8
      Top = 56
      Width = 49
      Height = 23
      Hint = #1047#1085#1072#1084#1077#1085#1072#1090#1077#1083#1100
      TabOrder = 1
      Text = '1'
      OnChange = OperatorSelectClick
    end
  end
  object ResultGroup: TGroupBox
    Left = 240
    Top = 56
    Width = 81
    Height = 89
    Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    object ResNum: TLabel
      Left = 8
      Top = 32
      Width = 49
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '1'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object ResDenom: TLabel
      Left = 8
      Top = 56
      Width = 49
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '1'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object ResDivider: TBevel
      Left = 16
      Top = 48
      Width = 33
      Height = 1
      Shape = bsTopLine
    end
    object CmpRes1: TLabel
      Left = 8
      Top = 40
      Width = 6
      Height = 14
      Alignment = taCenter
      Caption = '1'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
  end
  object BitBtn1: TBitBtn
    Left = 8
    Top = 240
    Width = 313
    Height = 25
    Caption = #1042#1099#1095#1080#1089#1083#1080#1090#1100
    TabOrder = 8
    OnClick = Calculate
    Kind = bkIgnore
  end
  object Operators1: TRadioGroup
    Left = 8
    Top = 152
    Width = 313
    Height = 81
    Caption = #1054#1087#1077#1088#1072#1094#1080#1080
    Columns = 5
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ItemIndex = 0
    Items.Strings = (
      '+'
      '-'
      '*'
      '/'
      '='
      '>'
      '<'
      '<>'
      '>='
      '<=')
    ParentFont = False
    TabOrder = 9
    OnClick = OperatorSelectClick
  end
  object ModeSelect: TRadioGroup
    Left = 8
    Top = 0
    Width = 313
    Height = 49
    Caption = #1056#1077#1078#1080#1084' '#1088#1072#1073#1086#1090#1099
    Columns = 2
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ItemIndex = 0
    Items.Strings = (
      #1041#1080#1085#1072#1088#1085#1099#1077' '#1086#1087#1077#1088#1072#1094#1080#1080
      #1042#1099#1095#1080#1089#1083#1077#1085#1080#1077' '#1092#1086#1088#1084#1091#1083)
    ParentFont = False
    TabOrder = 12
    OnClick = ModeSelectClick
  end
  object MainMenu1: TMainMenu
    OwnerDraw = True
    Left = 336
    object N1: TMenuItem
      Caption = #1060#1072#1081#1083
      object N3: TMenuItem
        Caption = #1054#1090#1082#1088#1099#1090#1100' '#1090#1077#1089#1090#1086#1074#1099#1081' '#1092#1072#1081#1083
        OnClick = N3Click
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object N5: TMenuItem
        Caption = #1047#1072#1082#1088#1099#1090#1100' '#1087#1088#1086#1075#1088#1072#1084#1084#1091
        OnClick = N5Click
      end
    end
    object N6: TMenuItem
      Caption = #1057#1087#1088#1072#1074#1082#1072
      object N2: TMenuItem
        Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077
        OnClick = N2Click
      end
    end
  end
end
