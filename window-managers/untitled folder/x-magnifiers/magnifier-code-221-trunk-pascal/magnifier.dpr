{
magnifier.dpr

Project file and main source file

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
program lente;

{*******************************************************************
*  Compatibility preprocessor code to allow the project to be compiled
* both on the Free Pascal Compiler and on Borland Delphi
*******************************************************************}
{$IFDEF FPC}
  {$MODE DELPHI}{$H+}
{$ENDIF}

{$IFDEF Win32}
  {$DEFINE Windows}
{$ENDIF}

{*******************************************************************
*  RC and RES Resource files are Windows only
*  Separate files for Delphi and FPC avoids the need to rebuild it
*******************************************************************}
{$IFDEF Windows}
  {$IFDEF FPC}
    {$R fpcmagnifier.rc}
  {$ELSE}
    {$R magnifier.res}
  {$ENDIF}
{$ENDIF}

uses
{$IFDEF FPC}
  Interfaces,
{$ENDIF}
{$IFDEF Windows}
  Windows,
{$ENDIF}
  Forms,
  glass in 'glass.pas',
  constants in 'constants.pas',
  appsettings in 'appsettings.pas',
  app in 'app.pas',
  about in 'about.pas',
  plugins in 'plugins.pas',
  translationsvmg in 'translationsvmg.pas',
  configdlg in 'configdlg.pas',
//  lazfmlviewer in 'lazfmlviewer.pas',
//  fmlscan in 'fmldriver.pas',
  plugininfo, startform;

{@@
  Application Main Procedure

  This procedure is executed as soon as the program is executed
}
var
  AHandle, Semaphore: Cardinal;
  lResult, lStartForm: Boolean;
begin
  lStartForm := False;

  {*******************************************************************
  *  Verifies if another instance is already running by using semaphores
  *******************************************************************}
  {$IFDEF Windows}
    Semaphore := OpenSemaphore(EVENT_ALL_ACCESS, False, szAppTitle);

    // No one is holding the semaphore, so let's create one
    if Semaphore = 0 then
     Semaphore := CreateSemaphore(nil, 3, 3, szAppTitle)
    else
    begin
      AHandle := FindWindow(nil, szAppTitle);
      SendMessage(AHandle, MYWM_SHOWGLASS, 0, 0);
      Exit;
    end;
  {$ENDIF}

  {*******************************************************************
  *  Program initialization
  *
  *  The order in which these methods are called is very important
  *******************************************************************}
  Application.Initialize;
  Application.CreateForm(TMainWindow, vMainWindow);
  Application.CreateForm(TAboutWindow, vAboutWindow);
  Application.CreateForm(TConfigDialog, vConfigDialog);
//  Application.CreateForm(TFMLViewerDialog, vFMLViewerDialog);
  {$IFDEF Windows} // Start up screen on the first run in Windows
  lStartForm := vConfigurations.firstRun;
  if lStartForm then
  begin
    Application.CreateForm(TvStartWindow, vStartWindow);
    vStartWindow.Show();
  end;
  {$ENDIF}

  {*******************************************************************
  *  Features that need to be initialized at software startup are
  * processed here
  *******************************************************************}

  // The Dynamic mode only works on Windows, so force that here
  {$IFNDEF Win32}
  vConfigurations.UsePlugins := False;
  {$ENDIF}

  Application.ShowMainForm := False;

  // Initializes HotKeys
  vMainWindow.UpdateHotKey(nil);

  // If the user selected to auto-start, handle this case here
  if vConfigurations.showWhenExecuted and (not lStartForm) then
    vMainWindow.ExecuteLens(nil);
  { Enters the Message Loop }
  Application.Run;

  { Finalization - Release the semaphore for future instances }
  {$IFDEF Windows}
    if Semaphore <> 0 then CloseHandle(Semaphore);
  {$ENDIF}
end.

