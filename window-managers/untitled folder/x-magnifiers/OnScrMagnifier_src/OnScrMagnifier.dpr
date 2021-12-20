program OnScrMagnifier;

uses
  Forms,
  MagnifierMain in 'MagnifierMain.pas' {frmMagnifier};

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'On Screen Magnifier';
  Application.CreateForm(TfrmMagnifier, frmMagnifier);
  Application.Run;
end.
