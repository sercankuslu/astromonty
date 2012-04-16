program RatFrac;

uses
  Forms,
  RatFracForm in 'RatFracForm.pas' {Form1},
  RatFracClass in 'RatFracClass.pas',
  AboutForm in 'AboutForm.pas' {Form2};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TForm2, Form2);
  Application.Run;
end.
