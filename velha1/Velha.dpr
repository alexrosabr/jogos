program Velha;



uses
  Forms,
  UnPrinc in 'UnPrinc.pas' {Form1};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'Jogo da Velha';
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
