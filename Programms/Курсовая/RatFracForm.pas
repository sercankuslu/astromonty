unit RatFracForm;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, ComCtrls, Menus,
  ToolWin, RatFracClass, AboutForm, LogForm;

type
  TForm1 = class(TForm)
    StatusBar1: TStatusBar;
    Operand1: TGroupBox;
    Bevel1: TBevel;
    EnterNum: TEdit;
    EnterDenom: TEdit;
    MainMenu1: TMainMenu;
    N2: TMenuItem;
    GroupBox7: TGroupBox;
    Operand2: TGroupBox;
    Bevel26: TBevel;
    EnterNum2: TEdit;
    EnterDenom2: TEdit;
    SimpleBtn: TRadioButton;
    FormulsBtn: TRadioButton;
    ResultGroup: TGroupBox;
    ResNum: TLabel;
    ResDenom: TLabel;
    ResDivider: TBevel;
    Operators: TGroupBox;
    OperatorGt: TRadioButton;
    OperatorLt: TRadioButton;
    OperatorEq: TRadioButton;
    OperatorNe: TRadioButton;
    OperatorGe: TRadioButton;
    OperatorLe: TRadioButton;
    CmpRes1: TLabel;
    Label1: TLabel;
    OperatorSymbol: TLabel;
    OperatorPlus: TRadioButton;
    OperatorDiv: TRadioButton;
    OperatorMinus: TRadioButton;
    OperatorMul: TRadioButton;
    F1: TGroupBox;
    Label2: TLabel;
    Bevel2: TBevel;
    Label3: TLabel;
    Label4: TLabel;
    F2: TGroupBox;
    Bevel3: TBevel;
    Label6: TLabel;
    Label7: TLabel;
    F3: TGroupBox;
    Label5: TLabel;
    Bevel4: TBevel;
    Label8: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    F4: TGroupBox;
    Label12: TLabel;
    Bevel5: TBevel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Bevel6: TBevel;
    Label16: TLabel;
    F5: TGroupBox;
    Label17: TLabel;
    Bevel7: TBevel;
    Label18: TLabel;
    Label19: TLabel;
    Label20: TLabel;
    Bevel8: TBevel;
    Label21: TLabel;
    Functions: TGroupBox;
    Funct5: TRadioButton;
    Funct1: TRadioButton;
    Funct4: TRadioButton;
    Funct2: TRadioButton;
    Funct3: TRadioButton;
    BitBtn1: TBitBtn;
    N1: TMenuItem;
    N3: TMenuItem;
    procedure FormShow(Sender: TObject);
    procedure FormCreate(Sender: TObject);

    procedure FormDestroy(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure OperatorSelectClick(Sender: TObject);
    procedure SimpleBtnClick(Sender: TObject);
    procedure FormulsBtnClick(Sender: TObject);
    procedure FunctSelectClick(Sender: TObject);
    procedure N2Click(Sender: TObject);
    procedure N3Click(Sender: TObject);
    function CheckStringOper(InStr : string; var LogStr:string) : boolean;
  private
    X, X1, Res : TRationalFraction;
    Operation : short;
    { Private declarations }
  public
    { Public declarations }
  end;
var
  Form1: TForm1;

  Number : integer;
implementation

{$R *.dfm}
procedure TForm1.FormShow(Sender: TObject);
begin
    EnterNum.Text := '1';
    EnterDenom.Text := '1';
end;
procedure TForm1.Button1Click(Sender: TObject);
var
    a, b : integer;
    CompResult : boolean;
    N, Res1, Res2 : TRationalFraction;
begin
    // вводим данные с обработкой ошибок
    try
        a := StrToInt(EnterNum.Text);
        b := StrToInt(EnterDenom.Text);
        X.SetValue(a, b);
        if(Operand2.Visible) then begin
            a := StrToInt(EnterNum2.Text);
            b := StrToInt(EnterDenom2.Text);
            X1.SetValue(a, b);
        end else X1.SetValue(1,1);
    except
        on Exception : EConvertError do
        begin
            StatusBar1.Panels[0].Text := 'Ошибка:';
            StatusBar1.Panels[1].Text := 'при вводе числа';
            StatusBar1.Panels[2].Text := Exception.Message;
            exit;
        end
        //ShowMessage(Exception.Message);
    end;
    if(X.ErrorFlag) then
    begin
        StatusBar1.Panels[0].Text := 'Ошибка';
        StatusBar1.Panels[1].Text := 'в дроби X';
        StatusBar1.Panels[2].Text := X.ErrorStr;
        ResNum.Caption := 'Ошибка';
        ResDenom.Caption := 'Ошибка';
        CmpRes1.Caption := 'Ошибка';
        exit;
    end;
    if(X1.ErrorFlag) then
    begin
        StatusBar1.Panels[0].Text := 'Ошибка';
        StatusBar1.Panels[1].Text := 'в дроби X1';
        StatusBar1.Panels[2].Text := X1.ErrorStr;
        ResNum.Caption := 'Ошибка';
        ResDenom.Caption := 'Ошибка';
        CmpRes1.Caption := 'Ошибка';
        exit;
    end;
    case Operation of
        0 : begin // +
            Res.SetValue(X);
            Res.Add(X1);
        end;
        1 : begin // -
            Res.SetValue(X);
            Res.Sub(X1);
        end;
        2 : begin // *
            Res.SetValue(X);
            Res.Multiply(X1);
        end;
        3 : begin // /
            Res.SetValue(X);
            Res.Divide(X1);
        end;
        4 : begin // =
            CompResult := X.Eq(X1);
        end;
        5 : begin // >
            CompResult := X.Gt(X1);
        end;
        6 : begin // <
            CompResult := X.Lt(X1);
        end;
        7 : begin // <>
            CompResult := X.Ne(X1);
        end;
        8 : begin // >=
            CompResult := X.Ge(X1);
        end;
        9 : begin // <=
            CompResult := X.Le(X1);
        end;
        10 : begin  // F(x) = 2x+1/x
            Res.SetValue(X);
            N := TRationalFraction.Create(2,1);
            Res.Multiply(N);
            Res1 := TRationalFraction.Create(1,1);
            Res1.Divide(X);
            Res.Add(Res1);
            N.Free;
            Res1.Free;
        end;
        11 : begin   // F(x) = (x-1)/(x-2)
            Res1 := TRationalFraction.Create(X);
            N := TRationalFraction.Create(2,1);
            Res1.Sub(N);
            Res.SetValue(X);
            N.SetValue(1,1);
            Res.Sub(N);
            Res.Divide(Res1);
            N.Free;
            Res1.Free;
        end;
        12 : begin    // F(x) = x*x - 1/x
            Res1 := TRationalFraction.Create(1,1);
            Res.SetValue(X);
            Res.Multiply(X);
            Res1.Divide(X);
            Res.Sub(Res1);
            Res1.Free;
        end;
        13 : begin   // F(x) = x/3 - 1/(1+x)
            Res1 := TRationalFraction.Create(X);
            Res2 := TRationalFraction.Create(1,1);
            Res.SetValue(X);
            N := TRationalFraction.Create(3,1);
            Res.Divide(N);
            N.SetValue(1,1);
            Res1.Add(N);
            Res2.Divide(Res1);
            Res.Sub(Res2);
            Res1.Free;
            Res2.Free;
            N.Free;
        end;      //F(x) = 3/x + 5 * x/11
        14 : begin
            Res1 := TRationalFraction.Create(X);
            Res.SetValue(3,1);
            Res.Divide(X);
            N := TRationalFraction.Create(11,1);
            Res1.Divide(N);
            N.SetValue(5,1);
            Res1.Multiply(N);
            Res.Add(Res1);
            Res1.Free;
            N.Free;
        end;
    end;

    if(Operation <= 3) or (Operation >= 10 ) then // операторы вычисления
    begin
        ResNum.Caption := Res.StrNumerator;
        ResDenom.Caption := Res.StrDenominator;
        if(Res.ErrorFlag) then
        begin
            StatusBar1.Panels[0].Text := 'Ошибка';
            StatusBar1.Panels[1].Text := 'Результат операции: ';
            StatusBar1.Panels[2].Text := Res.ErrorStr;
            exit;
        end else
        begin
            StatusBar1.Panels[0].Text := 'Успешно';
            StatusBar1.Panels[1].Text := 'Результат операции: ';
            StatusBar1.Panels[2].Text := Res.ErrorStr;
        end;
    end else
    if(Operation < 10) then
    begin
        if(CompResult) then
            CmpRes1.Caption := 'Истина'
        else
            CmpRes1.Caption := 'Ложь';
    end;

end;

procedure TForm1.FormCreate(Sender: TObject);
begin
    X  := TRationalFraction.Create(1,1);
    X1 := TRationalFraction.Create(1,1);
    Res := TRationalFraction.Create(1,1);
    Operation := 0; // +
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
    Res.Free;
    X.Free;
    X1.Free;
end;

procedure TForm1.OperatorSelectClick(Sender: TObject);
var
    isCompare : bool;
begin
    if(SimpleBtn.Checked) then
    begin

    if( OperatorPlus.Checked) then
    begin
        OperatorSymbol.Caption := '+';
        isCompare := false;
        Operation := 0;
    end;
    if( OperatorMinus.Checked) then
    begin
        OperatorSymbol.Caption := '-';
        isCompare := false;
        Operation := 1;
    end;
    if( OperatorMul.Checked) then
    begin
        OperatorSymbol.Caption := '*';
        isCompare := false;
        Operation := 2;
    end;
    if( OperatorDiv.Checked) then
    begin
        OperatorSymbol.Caption := '/';
        isCompare := false;
        Operation := 3;
    end;
    if( OperatorEq.Checked) then
    begin
        OperatorSymbol.Caption := '=';
        isCompare := true;
        Operation := 4;
    end;
    if( OperatorGt.Checked) then
    begin
        OperatorSymbol.Caption := '>';
        isCompare := true;
        Operation := 5;
    end;
    if( OperatorLt.Checked) then
    begin
        OperatorSymbol.Caption := '<';
        isCompare := true;
        Operation := 6;
    end;
    if( OperatorNe.Checked) then
    begin
        OperatorSymbol.Caption := '<>';
        isCompare := true;
        Operation := 7;
    end;
    if( OperatorGe.Checked) then
    begin
        OperatorSymbol.Caption := '>=';
        isCompare := true;
        Operation := 8;
    end;
    if( OperatorLe.Checked) then
    begin
        OperatorSymbol.Caption := '<=';
        isCompare := true;
        Operation := 9;
    end;
    if(isCompare) then
    begin
        CmpRes1.Visible := true;
        ResDenom.Visible := false;
        ResNum.Visible := false;
        ResDivider.Visible := false;
    end else
    begin
        CmpRes1.Visible := false;
        ResDenom.Visible := true;
        ResNum.Visible := true;
        ResDivider.Visible := true;
    end;
    end;
    Button1Click(Sender);
 end;

procedure TForm1.SimpleBtnClick(Sender: TObject);
begin
    Operand2.Visible := true;
    Operators.Visible := true;
    Functions.Visible := false;    
    OperatorSymbol.Visible := true;
    Operation := 0;             // возвращаем на операцию '+'
    OperatorPlus.Checked := true;
    F1.Visible := false;
    F2.Visible := false;
    F3.Visible := false;
    F4.Visible := false;
    F5.Visible := false;
    OperatorSelectClick(Sender);
end;

procedure TForm1.FormulsBtnClick(Sender: TObject);
begin
    Operand2.Visible := false;
    Operators.Visible := false;
    Functions.Visible := true;
    OperatorSymbol.Visible := false;
    Operation := 10;
    Funct1.Checked := true;
    ResNum.Visible := true;
    Resdenom.Visible := true;
    ResDivider.Visible := true;
    CmpRes1.Visible := false;
    OperatorSelectClick(Sender);
    FunctSelectClick(Sender);
end;

procedure TForm1.FunctSelectClick(Sender: TObject);

begin

    if(Funct1.Checked) then
    begin
        Operation:=10;
        F1.Visible := true;
    end else F1.Visible := false;
    if(Funct2.Checked) then
    begin
        Operation:=11;
        F2.Visible := true;
     end else F2.Visible := false;
    if(Funct3.Checked) then
    begin
        Operation:=12;
        F3.Visible := true;
    end else F3.Visible := false;
    if(Funct4.Checked) then
    begin
        Operation:=13;
        F4.Visible := true;
    end else F4.Visible := false;
    if(Funct5.Checked) then
    begin
        Operation:=14;
        F5.Visible := true;
    end else F5.Visible := false;
    Button1Click(Sender);
end;

procedure TForm1.N2Click(Sender: TObject);
begin
    Form2.ShowModal;
end;
// функция читает из выбранного файла данные, обрабатывает их и выводит на форму Form3
procedure TForm1.N3Click(Sender: TObject);
var
    T1, T2, T3 : TRationalFraction;
    i,m, k, l : integer;
    s, log : string;
    c,c1 : char;
    b : boolean;
    f: Textfile;
begin
    if(Form3.OpenDialog1.Execute) then
    begin
        Form3.Memo1.Clear;
        Form3.Show;
        AssignFile(f, Form3.OpenDialog1.FileName ); {Assigns the Filename}
        Reset(f); {Opens the file for reading}
        Form3.Memo1.Lines.Add('Открыт файл : ' + Form3.OpenDialog1.FileName);
        repeat
            Readln(f, s);
            CheckStringOper(s, log);
            Form3.Memo1.Lines.Add(log);
        until(Eof(f));
        CloseFile(f);
    end;
end;
//******************************************************************************
// функция возвращает результат проверки утверждения из строки s
// формат строки
// проверка арифметических операций
// X1/Y1 S X2/Y2 = X3/Y3 , где S может быть '+', '-', '*', '/'
// проверка операций сравнения
// X1/Y1 S X2/Y2 = R , где R может быть 't', 'f',
// X1,Y1, X2, Y2 операнды
// X3, Y3  ожидаемый результат опперации
function TForm1.CheckStringOper(InStr : string; var LogStr :string) : boolean;
var
    T1, T2, T3, Res1 : TRationalFraction;
    i,m, k, o, r : integer;
    c,c1,c3 : char;
    b, eq, b1, t, br : boolean;
    Sym, s, Comment : string;
begin
    k:=0;
    b:=false;
    b1 := false;

    c := ' ';
    c1 := ' ';
    t:=true;


    T1 := TRationalFraction.Create;
    T2 := TRationalFraction.Create;
    T3 := TRationalFraction.Create;
    Res1 := TRationalFraction.Create;

    br := false;
    r := 0;
    eq := false;
    o := 0;
    k := 1;
    i := 0;
    while (i <= Length(InStr) and k < 7) do
    begin
        i := i + 1;
        if(InStr[i] = ' ') then continue else
        if(InStr[i] = '#') then break;   // дальше идет коментарий
        case k of
            1: begin    // первый аргумент
                T1.SetFromString(InStr, i);
                k := 2;
            end;
            2: begin    // символ операции
                case InStr[i] of
                    '+': o := 1;
                    '-': o := 2;
                    '*': o := 3;
                    '/': o := 4;
                    '<': begin
                        if(InStr[i+1]=' ') then o := 5 else     // <
                        begin
                            if(InStr[i+1]='=') then o := 9 else // <=
                            if(InStr[i+1]='>') then o := 7 else // <>
                            i:= i + 1;
                        end;
                        eq := true;
                    end;
                    '>': begin
                        if(InStr[i+1]=' ') then o := 6 else     // >
                        begin
                            if(InStr[i+1]='=') then o := 10 else // >=
                            i:= i + 1;
                        end;
                        eq := true;
                    end;
                    '=': begin o := 8; eq:= true end;
                    else break; // неизвестный символ
                end;
                k := 3;
            end;
            3: begin    // второй аргумент
                T2.SetFromString(InStr, i);
                k := 4;
            end;
            4: begin    // символ равенства
                if InStr[i] <> '=' then break; // если не равно - ошибка
                if(not eq) then k := 5 else k := 6;
            end;
            5: begin    // ожидаемый результат - дробь
                if InStr[i] = 'e' then begin r := 1; break; end; // ожидаемый результат - ошибка
                T3.SetFromString(InStr, i);
                k:=7;  // ввод данных завершен
            end;
            6: begin    // ожидаемый результат символ
                if InStr[i] = 't' then br := true else
                if InStr[i] = 'f' then br := false;
                k:=7;  // ввод данных завершен
            end;
            else break;
        end;
    end;
    if(o = 0) then exit;
    {
    for i:= 1 to Length(InStr) do
    begin
        if(InStr[i] = '#') then
        begin
            s := Copy(InStr,1, i - 1);
            t := false;
            break;
        end;

    end;

    if(t) then s := InStr;




    T1.SetFromString(s, k);
    for i:=k to Length(s) do
        if(s[i]<> ' ') then
            case s[i] of
                '*', '/',
                '+', '-' : begin
                    c := s[i];
                    k := i+1;
                    break;
                end;
                '=', '>',
                '<', '!',
                ',', '.' : begin
                    c := s[i];
                    k := i + 1;
                    eq := true;
                    break;
                end;
                else break;
            end;

    T2.SetFromString(s, k);
    for i:=k to Length(s) do
        if(s[i]<> ' ') then
            case s[i] of
                '=': begin
                    c1 := s[i];
                    k := i + 1;
                    break;
                end;
                else break;
            end;
    if(not eq) then
        T3.SetFromString(s, k)
    else
    begin
        for i:=k to Length(s) do
            if((s[i] = 't') or (s[i] = 'f')) then
            begin
                c3 :=s[i];
                k := i + 1;
                break;
            end;
    end;
    // вычисляем
    Res1.SetValue(T1);
    Sym:= c;
    case c of
        '*': Res1.Multiply(T2);
        '/': Res1.Divide(T2);
        '+': Res1.Add(T2);
        '-': Res1.Sub(T2);
        '=': b:=T1.Eq(T2);
        '!': begin b:=T1.Ne(T2); Sym:='<>'; end;
        '<': b:=T1.Lt(T2);
        '>': b:=T1.Gt(T2); 
        ',': begin b:=T1.Le(T2); Sym:='<='; end;
        '.': begin b:=T1.Ge(T2); Sym:='>='; end;
    end;

    LogStr := T1.StrNumerator + '/' + T1.StrDenominator + ' ' + Sym + ' ' + T2.StrNumerator + '/' + T2.StrDenominator  + ' ' + c1 + ' ';
    if( not eq) then
    begin
        b := Res1.Eq(T3);
        LogStr := LogStr + Res1.StrNumerator + '/' + Res1.StrDenominator + ' (Ожидаем : ' + T3.StrNumerator + '/' + T3.StrDenominator + ')';
        if(b) then
            LogStr:=LogStr + ' - (Верно) ' + Comment
        else
            LogStr:=LogStr + ' - (Ошибка) ' + Comment;
        Result := b;
    end else
    begin
        if(b) then
            LogStr := LogStr + ' Истина '
        else
            LogStr := LogStr + ' Ложь ';
        LogStr := LogStr + ' (Ожидаем : ';
        if(c3 = 't') then
        begin
            LogStr := LogStr + ' Истина) ';
            b1 := true;
        end else
        begin
            LogStr := LogStr + ' Ложь) ';
            b1:=false;
        end;
        if(b = b1) then
            LogStr:=LogStr + ' - (Верно) ' + Comment
        else
            LogStr:=LogStr + ' - (Ошибка) ' + Comment;
        Result := (b = b1);
    end;
    }
    T1.Free;
    T2.Free;
    T3.Free;
    Res1.Free;
end;
end.
