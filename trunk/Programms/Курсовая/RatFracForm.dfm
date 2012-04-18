object Form1: TForm1
  Left = 642
  Top = 193
  BorderStyle = bsSingle
  Caption = 'Form1'
  ClientHeight = 296
  ClientWidth = 330
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Visible = True
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 216
    Top = 80
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
    Top = 80
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
  object StatusBar1: TStatusBar
    Left = 0
    Top = 269
    Width = 330
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
    Top = 48
    Width = 65
    Height = 89
    Caption = 'X'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial Black'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    object Bevel1: TBevel
      Left = 8
      Top = 48
      Width = 49
      Height = 33
      Shape = bsTopLine
    end
    object EnterNum: TEdit
      Left = 16
      Top = 24
      Width = 33
      Height = 25
      Hint = #1063#1080#1089#1083#1080#1090#1077#1083#1100
      TabOrder = 0
      Text = '1'
      OnChange = OperatorSelectClick
    end
    object EnterDenom: TEdit
      Left = 16
      Top = 56
      Width = 33
      Height = 25
      Hint = #1047#1085#1072#1084#1077#1085#1072#1090#1077#1083#1100
      TabOrder = 1
      Text = '1'
      OnChange = OperatorSelectClick
    end
  end
  object GroupBox7: TGroupBox
    Left = 8
    Top = 0
    Width = 313
    Height = 41
    Caption = #1056#1077#1078#1080#1084' '#1088#1072#1073#1086#1090#1099
    TabOrder = 2
    object SimpleBtn: TRadioButton
      Left = 8
      Top = 16
      Width = 121
      Height = 17
      Caption = #1055#1088#1086#1089#1090#1099#1077' '#1086#1087#1077#1088#1072#1094#1080#1080
      Checked = True
      TabOrder = 0
      TabStop = True
      OnClick = SimpleBtnClick
    end
    object FormulsBtn: TRadioButton
      Left = 168
      Top = 16
      Width = 137
      Height = 17
      Caption = #1042#1099#1095#1080#1089#1083#1077#1085#1080#1077' '#1092#1086#1088#1084#1091#1083
      TabOrder = 1
      OnClick = FormulsBtnClick
    end
  end
  object Operand2: TGroupBox
    Left = 128
    Top = 48
    Width = 65
    Height = 89
    Caption = 'X1'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial Black'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    object Bevel26: TBevel
      Left = 8
      Top = 48
      Width = 49
      Height = 33
      Shape = bsTopLine
    end
    object EnterNum2: TEdit
      Left = 16
      Top = 24
      Width = 33
      Height = 25
      Hint = #1063#1080#1089#1083#1080#1090#1077#1083#1100
      TabOrder = 0
      Text = '1'
      OnChange = OperatorSelectClick
    end
    object EnterDenom2: TEdit
      Left = 16
      Top = 56
      Width = 33
      Height = 25
      Hint = #1047#1085#1072#1084#1077#1085#1072#1090#1077#1083#1100
      TabOrder = 1
      Text = '1'
      OnChange = OperatorSelectClick
    end
  end
  object ResultGroup: TGroupBox
    Left = 240
    Top = 48
    Width = 81
    Height = 89
    Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Width = 7
      Height = 15
      Alignment = taCenter
      Caption = '1'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
  end
  object Operators: TGroupBox
    Left = 8
    Top = 144
    Width = 313
    Height = 81
    Caption = #1054#1087#1077#1088#1072#1094#1080#1080
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial Black'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 5
    object OperatorGt: TRadioButton
      Left = 16
      Top = 48
      Width = 33
      Height = 17
      Caption = '>'
      TabOrder = 0
      OnClick = OperatorSelectClick
    end
    object OperatorLt: TRadioButton
      Left = 72
      Top = 48
      Width = 33
      Height = 17
      Caption = '<'
      TabOrder = 1
      OnClick = OperatorSelectClick
    end
    object OperatorEq: TRadioButton
      Left = 240
      Top = 24
      Width = 33
      Height = 17
      Caption = '='
      TabOrder = 2
      OnClick = OperatorSelectClick
    end
    object OperatorNe: TRadioButton
      Left = 240
      Top = 48
      Width = 41
      Height = 17
      Caption = '<>'
      TabOrder = 3
      OnClick = OperatorSelectClick
    end
    object OperatorGe: TRadioButton
      Left = 128
      Top = 48
      Width = 33
      Height = 17
      Caption = '>='
      TabOrder = 4
      OnClick = OperatorSelectClick
    end
    object OperatorLe: TRadioButton
      Left = 184
      Top = 48
      Width = 33
      Height = 17
      Caption = '<='
      TabOrder = 5
      OnClick = OperatorSelectClick
    end
    object OperatorPlus: TRadioButton
      Left = 16
      Top = 24
      Width = 33
      Height = 17
      Caption = '+'
      Checked = True
      TabOrder = 6
      TabStop = True
      OnClick = OperatorSelectClick
    end
    object OperatorDiv: TRadioButton
      Left = 184
      Top = 24
      Width = 33
      Height = 17
      Caption = '/'
      TabOrder = 7
      OnClick = OperatorSelectClick
    end
    object OperatorMinus: TRadioButton
      Left = 72
      Top = 24
      Width = 33
      Height = 17
      Caption = '-'
      TabOrder = 8
      OnClick = OperatorSelectClick
    end
    object OperatorMul: TRadioButton
      Left = 128
      Top = 24
      Width = 33
      Height = 17
      Caption = '*'
      TabOrder = 9
      OnClick = OperatorSelectClick
    end
  end
  object F1: TGroupBox
    Left = 80
    Top = 48
    Width = 121
    Height = 89
    Caption = 'F(X)'
    TabOrder = 6
    Visible = False
    object Label2: TLabel
      Left = 32
      Top = 40
      Width = 31
      Height = 15
      Caption = '2X + '
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object F2: TGroupBox
    Left = 80
    Top = 48
    Width = 121
    Height = 89
    Caption = 'F(X)'
    TabOrder = 7
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
      Font.Name = 'Arial Black'
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
      Caption = 'X - 2'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object F3: TGroupBox
    Left = 80
    Top = 48
    Width = 121
    Height = 89
    Caption = 'F(X)'
    TabOrder = 8
    Visible = False
    object Label5: TLabel
      Left = 32
      Top = 40
      Width = 13
      Height = 15
      Caption = 'X '
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label10: TLabel
      Left = 40
      Top = 32
      Width = 7
      Height = 15
      Caption = '2'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label11: TLabel
      Left = 54
      Top = 40
      Width = 8
      Height = 15
      Caption = '- '
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object F4: TGroupBox
    Left = 80
    Top = 48
    Width = 121
    Height = 89
    Caption = 'F(X)'
    TabOrder = 9
    Visible = False
    object Label12: TLabel
      Left = 48
      Top = 40
      Width = 4
      Height = 15
      Caption = '-'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object F5: TGroupBox
    Left = 80
    Top = 48
    Width = 121
    Height = 89
    Caption = 'F(X)'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial Black'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 10
    Visible = False
    object Label17: TLabel
      Left = 48
      Top = 40
      Width = 7
      Height = 15
      Caption = '+'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
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
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object Functions: TGroupBox
    Left = 8
    Top = 144
    Width = 313
    Height = 81
    Caption = #1060#1091#1085#1082#1094#1080#1080
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial Black'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 11
    Visible = False
    object Funct5: TRadioButton
      Left = 240
      Top = 24
      Width = 65
      Height = 17
      Caption = 'F5(X)'
      TabOrder = 0
      OnClick = FunctSelectClick
    end
    object Funct1: TRadioButton
      Left = 16
      Top = 24
      Width = 65
      Height = 17
      Caption = 'F1(X)'
      Checked = True
      TabOrder = 1
      TabStop = True
      OnClick = FunctSelectClick
    end
    object Funct4: TRadioButton
      Left = 128
      Top = 48
      Width = 65
      Height = 17
      Caption = 'F4(X)'
      TabOrder = 2
      OnClick = FunctSelectClick
    end
    object Funct2: TRadioButton
      Left = 16
      Top = 48
      Width = 65
      Height = 17
      Caption = 'F2(X)'
      TabOrder = 3
      OnClick = FunctSelectClick
    end
    object Funct3: TRadioButton
      Left = 128
      Top = 24
      Width = 65
      Height = 17
      Caption = 'F3(X)'
      TabOrder = 4
      OnClick = FunctSelectClick
    end
  end
  object BitBtn1: TBitBtn
    Left = 8
    Top = 232
    Width = 313
    Height = 25
    Caption = #1042#1099#1095#1080#1089#1083#1080#1090#1100
    TabOrder = 12
    Kind = bkIgnore
  end
  object MainMenu1: TMainMenu
    OwnerDraw = True
    Left = 336
    object N1: TMenuItem
      Caption = #1060#1072#1081#1083
      object N3: TMenuItem
        Caption = #1058#1077#1089#1090
        OnClick = N3Click
      end
    end
    object N2: TMenuItem
      Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077
      OnClick = N2Click
    end
  end
end
