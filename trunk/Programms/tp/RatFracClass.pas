unit RatFracClass;

interface
uses SysUtils;
type
    TRationalFraction = class
    private
        FNumerator : integer;   // числитель
        FDenominator : integer; // знаменатель
        FErrorFlag : boolean;   // флаг ошибки
        // функции получения значений свойств
        function GetNumStr : string;
        function GetDenomStr : string;
        function GetErrorStr : string;
        // служебные функции
        function Nod( a, b : integer) : integer;
        procedure Normalize;
    public        
        // свойства
        property Numerator      : integer read FNumerator;
        property Denominator    : integer read FDenominator;
        property StrNumerator   : string read GetNumStr;
        property StrDenominator : string read GetDenomStr;
        property ErrorFlag      : boolean read FErrorFlag;
        property ErrorStr       : string read GetErrorStr;
        // Конструкторы
        constructor Create(NumValue, DenumValue : integer);  Overload;
        constructor Create( Value : TRationalFraction); Overload;
        constructor Create; Overload;
        // методы задания значений
        procedure SetValue(NumValue, DenumValue : integer);  Overload;
        procedure SetValue( Value : TRationalFraction); Overload;
        // методы операций
        procedure Multiply( Factor : TRationalFraction); Overload;
        procedure Divide( Factor : TRationalFraction); Overload;
        procedure Add( Factor : TRationalFraction); Overload;
        procedure Sub( Factor : TRationalFraction); Overload;
        // методы сравнения
        function Eq( Factor : TRationalFraction ):boolean; Overload; //=
        function Gt( Factor : TRationalFraction ):boolean; Overload; // >
        function Lt( Factor : TRationalFraction ):boolean; Overload; // <
        function Ne( Factor : TRationalFraction ):boolean; Overload; //<>
        function Ge( Factor : TRationalFraction ):boolean; Overload; //>=
        function Le( Factor : TRationalFraction ):boolean; Overload; //<=   
    end;
implementation
{ TRationalFraction }
//******************************************************************************
// Конструкторы
//******************************************************************************
constructor TRationalFraction.Create(NumValue, DenumValue: integer);
begin
    FErrorFlag := false;
    SetValue( NumValue, DenumValue );
end;

constructor TRationalFraction.Create(Value: TRationalFraction);
begin
    FErrorFlag := false;
    SetValue( Value );
end;

constructor TRationalFraction.Create;
begin
    FErrorFlag := false;
    FNumerator := 0;
    FDenominator  := 1;
end;
//******************************************************************************
// Установка значений
//******************************************************************************
procedure TRationalFraction.SetValue(NumValue, DenumValue: integer);
begin
    FNumerator := NumValue;
    FDenominator  := DenumValue;
    Normalize;
end;

procedure TRationalFraction.SetValue(Value: TRationalFraction);
begin
    FNumerator := Value.FNumerator;
    FDenominator  := Value.FDenominator ;
    Normalize;
end;
//******************************************************************************
// умножение
//******************************************************************************
procedure TRationalFraction.Multiply(Factor: TRationalFraction);
begin
    FNumerator := FNumerator * Factor.FNumerator;
    FDenominator  := FDenominator  * Factor.FDenominator ;
    Normalize;
end;
//******************************************************************************
// деление двух дробей
//******************************************************************************
procedure TRationalFraction.Divide(Factor: TRationalFraction);
begin
    FNumerator := FNumerator * Factor.FDenominator ;
    FDenominator  := FDenominator  * Factor.FNumerator;
    Normalize;
end;
//******************************************************************************
// Сложение
//******************************************************************************
procedure TRationalFraction.Add( Factor : TRationalFraction);
begin
    if(Factor.FErrorFlag) then FErrorFlag := true else
    begin
        FNumerator := FNumerator * Factor.FDenominator  + Factor.FNumerator * FDenominator ;
        FDenominator  := FDenominator  * Factor.FDenominator ;
        Normalize;
    end;
end;
//******************************************************************************
// Вычитание
//******************************************************************************
procedure TRationalFraction.Sub( Factor : TRationalFraction);
begin
    if(Factor.FErrorFlag) then FErrorFlag := true else
    begin
        FNumerator := FNumerator * Factor.FDenominator  - Factor.FNumerator * FDenominator ;
        FDenominator  := FDenominator  * Factor.FDenominator ;
        Normalize;
    end;
end;
//******************************************************************************
// Нормализация
//******************************************************************************
procedure TRationalFraction.Normalize;
var
    a : integer;
begin
    a := Nod( abs(FNumerator), abs(FDenominator ));
    if FDenominator = 0 then FErrorFlag:=true else FErrorFlag := false;
    if( a = 0 ) then begin
        exit;
    end;
    FNumerator := FNumerator div a;
    FDenominator  := FDenominator  div a;
    if (FNumerator < 0) and (FDenominator  < 0) then
    begin
        FNumerator := abs(FNumerator);
        FDenominator  := abs(FDenominator );
    end;
    if (FDenominator  < 0) and (FNumerator > 0) then
    begin
        FDenominator  := abs(FDenominator );
        FNumerator := -FNumerator;
    end;
    if(FNumerator = 0) then FDenominator  := 1;
end;
//******************************************************************************
// Нахождение Наибольшего общего делителя
//******************************************************************************
function TRationalFraction.Nod( a, b : integer) : integer;
var
    m, n, r, res, k: integer;
begin
    m := a;
    n := b;
    r := 0;
    res := 0;
    repeat
    if m = 0 then begin Nod := n Shl r; break; end; // NOD(0,n) = n
    if (n = 0) or (m = n) then begin Nod := m Shl r; break; end;// NOD(0,n) = n
    if (m = 1) or (n = 1) then begin Nod := 1 Shl r; break; end;
    if (m and 1 = 0) then
    begin
        if ( n and 1 = 0) then
        begin
            r := r + 1;
            m := m Shr 1;
            n := n Shr 1;
            Continue;
        end else
        begin
            m := m Shr 1;
            Continue;
        end;
    end else
    begin
        if ( n and 1 = 0) then
        begin
            n := n Shr 1;
            Continue;
        end else
        begin
            if (n > m) then
            begin
                k := m;
                m := (n - m) Shr 1;
                n := k;
                Continue;
            end else
            begin
                m := (m - n) Shr 1;
                Continue;
            end;
        end;
    end;
    until false;
end;
function TRationalFraction.GetNumStr : string;
begin
    if(FDenominator  = 0) then GetNumStr := 'Ошибка' else
        GetNumStr := IntToStr(FNumerator);
end;
function TRationalFraction.GetDenomStr : string;
begin
    if(FDenominator  = 0) then GetDenomStr := 'Ошибка' else
        GetDenomStr := IntToStr(FDenominator );
end;
function TRationalFraction.GetErrorStr : string;
begin
    if(FErrorFlag) then GetErrorStr := 'Деление на "0"' else
        GetErrorStr := 'Дробь верна';
end;
//******************************************************************************
// Методы сравнения
//******************************************************************************

function TRationalFraction.Eq(Factor: TRationalFraction): boolean;
begin
    if (FErrorFlag or Factor.FErrorFlag) then Eq:=false else
        if FNumerator*Factor.FDenominator  = Factor.FNumerator*FDenominator  then Eq := true
        else Eq := false;
end;

function TRationalFraction.Ge(Factor: TRationalFraction): boolean;
begin
    if (FErrorFlag or Factor.FErrorFlag) then Ge := false else
        if FNumerator*Factor.FDenominator  >= Factor.FNumerator*FDenominator  then Ge := true
        else Ge := false;
end;

function TRationalFraction.Gt(Factor: TRationalFraction): boolean;
begin
    if (FErrorFlag or Factor.FErrorFlag) then Gt:=false else
        if FNumerator*Factor.FDenominator  > Factor.FNumerator*FDenominator  then Gt := true
        else Gt := false;
end;

function TRationalFraction.Le(Factor: TRationalFraction): boolean;
begin
    if (FErrorFlag or Factor.FErrorFlag) then Le:=false else
        if FNumerator*Factor.FDenominator  <= Factor.FNumerator*FDenominator  then Le := true
        else Le := false;
end;

function TRationalFraction.Lt(Factor: TRationalFraction): boolean;
begin
    if (FErrorFlag or Factor.FErrorFlag) then Lt:=false else
        if FNumerator*Factor.FDenominator  < Factor.FNumerator*FDenominator  then Lt := true
        else Lt := false;
end;

function TRationalFraction.Ne(Factor: TRationalFraction): boolean;
begin
    if (FErrorFlag or Factor.FErrorFlag) then Ne:=false else
    if FNumerator*Factor.FDenominator  <> Factor.FNumerator*FDenominator  then Ne := true
    else Ne := false;
end;



END.
