program RatFrac;

uses
  Forms,
  RatFracForm in 'RatFracForm.pas' {Form1},
  RatFracClass in 'RatFracClass.pas',
  AboutForm in 'AboutForm.pas' {Form2},
  LogForm in 'LogForm.pas' {Form3};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TForm2, Form2);
  Application.CreateForm(TForm3, Form3);
  Application.Run;
end.
