{
configdlg.pas

Configurations Dialog

Copyright (C) 1998 - 2010 Harri Pyy, Chris O'Donnell, Felipe Monteiro de Carvalho

This file is part of Virtual Magnifying Glass.

Virtual Magnifying Glass is free software;
you can redistribute it and/or modify it under the
terms of the GNU General Public License version 2
as published by the Free Software Foundation.

Virtual Magnifying Glass is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

Please note that the General Public License version 2 does not permit
incorporating Virtual Magnifying Glass into proprietary
programs.

AUTHORS: Chris O'Donnell, Felipe Monteiro de Carvalho and Harri Pyy
}
unit configdlg;

{$IFDEF FPC}
  {$MODE DELPHI}{$H+}
{$ENDIF}

interface

uses SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
 ComCtrls, ExtCtrls, StdCtrls, Buttons, appsettings, constants,
 plugininfo;

type

  { TConfigDialog }

  TConfigDialog = class(TForm)
  private
    { Private declarations }
    pgsCategories: TPageControl;
    lblDisabledHint: TLabel;
    { General tab }
    tabGeneral: TTabSheet;
    chkAutoRun: TCheckBox;
    chkShowWhenExecuted: TCheckBox;
    { Plugins tab }
    tabPlugins: TTabSheet;
    stPluginsList: TStaticText;
    lstPluginsList: TListBox;
    { HotKey tab }
    tabHotKey: TTabSheet;
    rgrpHotKeyInvoke: TRadioGroup;
    lblHotKeyKey: TLabel;
    txtHotKeyKey: TEdit;
    { Others }
    btnClose: TButton;
    procedure FormShow(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
  public
    { Public declarations }
    procedure UpdateInterface;
    procedure SaveConfigurations;
    constructor Create(AOwner: TComponent); override;
  end;

var
  vConfigDialog: TConfigDialog;

implementation

uses
  glass, translationsvmg, app
{$ifdef fpc}
  , LResources
{$endif}
 ;

{$ifndef fpc}
  {$R *.DFM}
{$endif}

{ TConfigDialog }

{*******************************************************************}
{@@
  Allocates and initializes the configurations dialog.

  Obs: The captions are set here too because of a
  possible Lazarus bug:

  The labels of the PageControl don't show on the first
  time the form is shown if set only on OnShow, using
  Lazarus on win32.
}
{*******************************************************************}
constructor TConfigDialog.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);

  Position := poScreenCenter;
  Height := 400;
  Width := 500;

  OnShow := FormShow;
  OnClose := FormClose;

  Self.VertScrollBar.Visible := False;
  Self.HorzScrollBar.Visible := False;

  pgsCategories := TPageControl.Create(Self);
  with pgsCategories do
  begin
    Parent := Self;
    Height := 275;
    Left := 25;
    Top := 25;
    Width := 450;
    TabOrder := 0;
    Anchors := [akLeft, akRight, akTop];
  end;

  { General tab }

  tabGeneral := TTabSheet.Create(Self);
  tabGeneral.PageControl := pgsCategories;
  tabGeneral.Caption := vTranslations.lpTabGeneral;

  chkAutoRun := TCheckBox.Create(Self);
  with chkAutoRun do
  begin
    Parent := tabGeneral;
    Top := 10;
    Left := 10;
    Width := 400;
  end;

  chkShowWhenExecuted := TCheckBox.Create(Self);
  with chkShowWhenExecuted do
  begin
    Parent := tabGeneral;
    Top := 35;
    Left := 10;
    Width := 400;
  end;

  { Plugins tab }

  tabPlugins := TTabSheet.Create(Self);
  {$ifdef USE_PLUGINS}
  tabPlugins.PageControl := pgsCategories;
  {$endif}
  tabPlugins.Caption := vTranslations.lpTabPlugins;

  stPluginsList := TStaticText.Create(Self);
  with stPluginsList do
  begin
    Parent := tabPlugins;
    Top := 10;
    Left := 10;
    Width := 400;
  end;

  lstPluginsList := TListBox.Create(Self);
  with lstPluginsList do
  begin
    Parent := tabPlugins;
    Top := 35;
    Left := 10;
    Height := 200;
    Width := 400;
    MultiSelect := False;
  end;

  { HotKey tab }

  tabHotKey := TTabSheet.Create(Self);
  tabHotKey.PageControl := pgsCategories;
  tabHotKey.Caption := vTranslations.lpTabHotKey;

  lblHotKeyKey := TLabel.Create(Self);
  with lblHotKeyKey do
  begin
    Parent := tabHotKey;
    Top := 10;
    Left := 10;
    AutoSize := True;
    Caption := vTranslations.lpHotKeyKey;
  end;

  txtHotKeyKey := TEdit.Create(Self);
  with txtHotKeyKey do
  begin
    Parent := tabHotKey;
    Top := 30;
    Left := 10;
    Width := 50;
    MaxLength := 1;
  end;

  rgrpHotKeyInvoke := TRadioGroup.Create(Self);
  with rgrpHotKeyInvoke do
  begin
    Parent := tabHotKey;
    Top := 60;
    Left := 10;
    Height := 100;
    Width := 400;
    Caption := vTranslations.lpHotKeyInvoke;
    Items.Add(vTranslations.lpHotKeyDisableInvoke);
    Items.Add(vTranslations.lpHotKeyCtrlAltKey);
    Items.Add(vTranslations.lpHotKeyDisableAll);
    ItemIndex := 0;
  end;

  { Others }

  lblDisabledHint := TLabel.Create(Self);
  with lblDisabledHint do
  begin
    Parent := Self;
    Left := 25;
    Top := 305;
    AutoSize := True;
    Caption := vTranslations.lpDisabledHint;
  end;

  btnClose := TButton.Create(Self);
  with btnClose do
  begin
    Parent := Self;
    Height := 33;
    Left := 150;
    Top := 333;
    Width := 200;
    ModalResult := mrOK;
    TabOrder := 0;
    Anchors := [akLeft, akRight, akTop];
  end;

  ActiveControl := btnClose;
end;

{*******************************************************************}
{@@
  Updates the dialog every time it is shown
}
{*******************************************************************}
procedure TConfigDialog.FormShow(Sender: TObject);
begin
  UpdateInterface;
end;

{*******************************************************************}
{@@
  Updates the configurations every time the dialog closes
}
{*******************************************************************}
procedure TConfigDialog.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  SaveConfigurations;
end;

{*******************************************************************}
{@@
  Call this method to update the interface according
  to the current vConfigurations state and the
  current translation
}
{*******************************************************************}
procedure TConfigDialog.UpdateInterface;
var
  SR: TSearchRec;
  i: Integer;
begin
  {*******************************************************************
  *  Updates the translations
  *******************************************************************}

  Caption := vTranslations.lpConfigDialogTitle;

  { General tab }

  tabGeneral.Caption := vTranslations.lpTabGeneral;
  chkAutoRun.Caption := vTranslations.lpAutoRun;
  chkShowWhenExecuted.Caption := vTranslations.lpShowWhenExecuted;

  { Plugins tab }

  tabPlugins.Caption := vTranslations.lpTabPlugins;
  stPluginsList.Caption := vTranslations.lpChoosePlugin;

  { HotKey tab }

  tabHotKey.Caption := vTranslations.lpTabHotKey;
  lblHotKeyKey.Caption := vTranslations.lpHotKeyKey;

  rgrpHotKeyInvoke.Caption := vTranslations.lpHotKeyInvoke;
  rgrpHotKeyInvoke.Items.Clear;
  rgrpHotKeyInvoke.Items.Add(vTranslations.lpHotKeyDisableInvoke);
  rgrpHotKeyInvoke.Items.Add(vTranslations.lpHotKeyCtrlAltKey);
  rgrpHotKeyInvoke.Items.Add(vTranslations.lpHotKeyDisableAll);

  { Others }

  lblDisabledHint.Caption := vTranslations.lpDisabledHint;

  btnClose.Caption := vTranslations.lpClose;

  {*******************************************************************
  *  Updates the configurations
  *******************************************************************}

  { General tab }

  chkAutoRun.Checked := vConfigurations.autoRun;

  chkShowWhenExecuted.Checked := vConfigurations.showWhenExecuted;

{$IFDEF Unix}
  chkAutoRun.Enabled := False;
{$ENDIF}

  { Plugins tab }

  lstPluginsList.Items.Clear;
  lstPluginsList.ItemIndex := -1;

  if FindFirst(vConfigurations.MyDirectory + ID_PLUGIN_WILDCARD, faAnyFile, SR) = 0 then
  begin
    repeat
      if (SR.Attr <> faDirectory) then
       lstPluginsList.Items.Add(SR.Name);
    until FindNext(SR) <> 0;
    
    // Must free up resources used by these successful finds
    FindClose(SR);
  end;
  
  { Tryes to select the plugin currently in use }
  for i := 0 to lstPluginsList.Count - 1 do
  begin
    if lstPluginsList.Items.Strings[i] = vConfigurations.PluginName then
     lstPluginsList.ItemIndex := i;
  end;

  { HotKey tab }

  rgrpHotKeyInvoke.ItemIndex := vConfigurations.HotKeyMode;

  txtHotKeyKey.Text := vConfigurations.HotKeyKey;

{$ifdef Unix}
  txtHotKeyKey.Enabled := False;
  rgrpHotKeyInvoke.Enabled := False;
{$endif}
end;

{*******************************************************************}
{@@
  Saves the dialog information to vConfigurations
}
{*******************************************************************}
procedure TConfigDialog.SaveConfigurations;
begin
  { General tab }

  vConfigurations.autoRun := chkAutoRun.Checked;
  
  vConfigurations.showWhenExecuted := chkShowWhenExecuted.Checked;

  { Plugins tab }

  if lstPluginsList.ItemIndex <> -1 then
  begin
    vConfigurations.PluginName := lstPluginsList.Items.Strings[lstPluginsList.ItemIndex];
  end;
  
  { HotKey tab }

  vConfigurations.HotKeyMode := rgrpHotKeyInvoke.ItemIndex;
  vConfigurations.HotKeyKey  := txtHotKeyKey.Text[1];

  vMainWindow.UpdateHotKey(Self);
end;

end.

