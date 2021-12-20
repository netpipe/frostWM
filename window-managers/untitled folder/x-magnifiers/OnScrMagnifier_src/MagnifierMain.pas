unit MagnifierMain;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, GR32, Menus, ExtCtrls, pasYCbCr, Math, StdCtrls, AppEvnts, 
  ColorLibrary, ActnList;

Const
  CFormBorder= 3;
  CFormBorder2= CFormBorder*2;

type
  TRegionType = (rtRectangle, rtRoundRect, rtEllipse);
  TfrmMagnifier = class(TForm)
    PopupMenu1: TPopupMenu;
    N01X1: TMenuItem;
    N05X1: TMenuItem;
    N1X1: TMenuItem;
    N2X1: TMenuItem;
    N3X1: TMenuItem;
    N4X1: TMenuItem;
    N8X1: TMenuItem;
    Size1: TMenuItem;
    N1: TMenuItem;
    Exit1: TMenuItem;
    StretchFilter1: TMenuItem;
    sfNearest1: TMenuItem;
    sfDraft1: TMenuItem;
    sfLinear1: TMenuItem;
    sfCosine1: TMenuItem;
    sfSpline1: TMenuItem;
    sfLanczos1: TMenuItem;
    sfMitchell1: TMenuItem;
    Timer1: TTimer;
    GlassSureface1: TMenuItem;
    None1: TMenuItem;
    N101: TMenuItem;
    N201: TMenuItem;
    N301: TMenuItem;
    N401: TMenuItem;
    N501: TMenuItem;
    N601: TMenuItem;
    N701: TMenuItem;
    N801: TMenuItem;
    N901: TMenuItem;
    FPS1: TMenuItem;
    N0fps1: TMenuItem;
    N10fps1: TMenuItem;
    N20fps1: TMenuItem;
    N30fps1: TMenuItem;
    N08X1: TMenuItem;
    Region1: TMenuItem;
    mnRectangle1: TMenuItem;
    mnRoundRect1: TMenuItem;
    mnEllipse1: TMenuItem;
    LockMagnifier1: TMenuItem;
    mnMono1: TMenuItem;
    ColorSpace1: TMenuItem;
    mnBW1: TMenuItem;
    mnColor1: TMenuItem;
    InnerShadow1: TMenuItem;
    ApplicationEvents1: TApplicationEvents;
    mnRotate1801: TMenuItem;
    ActionList1: TActionList;
    actRgnRectangle: TAction;
    actRgnRoundRect: TAction;
    actRgnEllipse: TAction;
    actColorColor: TAction;
    actColorRotate: TAction;
    actColorMono: TAction;
    actColorBW: TAction;
    actSFNearest: TAction;
    actSFDraft: TAction;
    actSFLinear: TAction;
    actSFCosine: TAction;
    actSFSpline: TAction;
    actSFLanczos: TAction;
    actSFMitchell: TAction;
    procedure FormPaint(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure MagnifierRatioClick(Sender: TObject);
    procedure Exit1Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure GlassSurefaceClick(Sender: TObject);
    procedure FPSClick(Sender: TObject);
    procedure FormResize(Sender: TObject);
    procedure LockMagnifier1Click(Sender: TObject);
    procedure InnerShadow1Click(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure FormShow(Sender: TObject);
    procedure ApplicationEvents1Activate(Sender: TObject);
    procedure actColorExecute(Sender: TObject);
    procedure actRegionExecute(Sender: TObject);
    procedure actStretchFilterExecute(Sender: TObject);
    procedure FormMouseMove(Sender: TObject; Shift: TShiftState; X,
      Y: Integer);
    procedure FormMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
  private
    { Private declarations }
    FormRgn, BorderRgn: HRGN;
    Region: TRegionType;
    MagnifierRatio: Double;
    MagnifierStretchFilter: TStretchFilter;
    bmpNoisy, bmpShadow: TBitmap32;
    bmpMagnifier: TBitmap32;
    LastLocation: TRect;
    MagnifierLocation: TRect;
    Dragging: Boolean;
    DragScreenPos, DragMousePos: TPoint;
    DragFormPos: TPoint;
    Function Magnifier(const Force:Boolean; Const MRect:TRect): Boolean;
    Procedure RotateColor(const B:TBitmap32; const ADegree:Integer);
    Procedure Mono(const BMP32:TBitmap32; const BW:Boolean=False);
    Procedure UpdateMagnifier(const Force:Boolean=False);
    procedure InnerShadowMask32(BMP32: TBitmap32; ShadowSize: Word;
      LightColor, DarkColor: TColor32; Degree: Word=45);
    procedure InnerShadowMask32Rect(BMP32: TBitmap32; ShadowSize: Word;
      LightColor, DarkColor: TColor32; Degree: Word=45);
    Procedure PerpareShadow(const aw,ah:Word);
    Procedure HoldDownMouse;
  protected
    procedure WMWindowPosChanging(var Message: TWMWindowPosChanging); message WM_WINDOWPOSCHANGING;
    procedure WMNCHitTest(var Message: TWMNCHitTest); message WM_NCHITTEST;
    procedure WMEraseBkgnd(var Message: TWMEraseBkgnd); message WM_ERASEBKGND;
  public
    { Public declarations }
  end;

var
  frmMagnifier: TfrmMagnifier;

implementation

uses Types;

{$R *.dfm}

Const
  CRadiansToDegree= 180/pi;
  CDegreeToRadians= pi/180;

// VectorExt
Function NormalDegree(const ADegree: Integer): Integer;
Begin
  Result:= ADegree mod 360;
  If Result<0 then
    Result:= 360+Result;
End;

Function FindDegree(const p1, KeyPoint: TPoint): Double;
Begin
  Result:= ArcTan2(p1.Y-KeyPoint.Y, p1.X-KeyPoint.X)* CRadiansToDegree;
End;

Function CreateRgnFromRect(ARect: TRect; AType: TRegionType):HRGN;
Var
  i: Integer;
Begin
  Case AType of
    rtRectangle:
      Begin
        Result:= CreateRectRgn(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);
      End;
    rtRoundRect:
      Begin
        i:= Min(ARect.Right-ARect.Left, ARect.Bottom-ARect.Top) div 4;
        Result:= CreateRoundRectRgn(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom, i, i);
      End;
    rtEllipse:
      Begin
        Result:= CreateEllipticRgn(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);
      End;
  End;//CASE
End;

{ TfrmMagnifier }
procedure TfrmMagnifier.FormPaint(Sender: TObject);
begin
  If Assigned(bmpMagnifier) then
  Begin
    bmpMagnifier.DrawTo(Canvas.Handle, 0, 0);
  End;
end;

procedure TfrmMagnifier.FormCreate(Sender: TObject);
begin
  Dragging:= False;
  MagnifierRatio:= 2;
  MagnifierStretchFilter:= sfNearest;//sfLinear;
  bmpMagnifier:= TBitmap32.Create;
  bmpNoisy:= TBitmap32.Create;
  bmpNoisy.LoadFromFile('water.bmp');  
  bmpNoisy.DrawMode:= dmBlend;
  bmpNoisy.MasterAlpha:= 0; // 30/100*255
  bmpShadow:= TBitmap32.Create;
  bmpShadow.DrawMode:= dmBlend;
  bmpShadow.StretchFilter:= sfDraft;
  LastLocation:= Rect(0,0,0,0);
  Region:= rtRectangle;
  // Default values
  //actRgnRectangle.Execute;
  FormResize(Self);
  MagnifierLocation:= Rect(Left,Top,Left+Width,Top+Height);
end;

procedure TfrmMagnifier.FormDestroy(Sender: TObject);
begin
  bmpShadow.Free;
  bmpMagnifier.Free;
end;

Function TfrmMagnifier.Magnifier(const Force:Boolean; const MRect:TRect):Boolean;
Var
  SDC: HDC;
  B32: TBitmap32;
  ERect: TRect;
  w, h, mw, mh: Integer;
begin
  Result:= False;
  If Visible and (Width>0) and (Height>0) then
  Begin
    //ERect:= Rect(Left, Top, Left+Width, Top+Height);
    If Force or Not EqualRect(LastLocation, MRect) then
    Begin
      ERect:= MRect;
      LastLocation:= ERect;
      w:= ERect.Right-ERect.Left;
      h:= ERect.Bottom-ERect.Top;
      //InflateRect(ERect, -Round((w-(1/MagnifierRatio*w))/2), -Round((h-(1/MagnifierRatio*h))/2));
      mw:= Round(w/MagnifierRatio);
      mh:= Round(h/MagnifierRatio);
      ERect.TopLeft:= Point(ERect.Left+ ((w-mw) div 2), ERect.Top+ ((h-mh) div 2));
      ERect.BottomRight:= Point(ERect.Left+mw, ERect.Top+mh);
      If Not IsRectEmpty(ERect) then
      Begin
        B32:= TBitmap32.Create;
        try
          B32.SetSize(ERect.Right-ERect.Left, ERect.Bottom-ERect.Top);
          If (B32.Width>1) and (B32.Height>1) then
          Begin
            SDC:= GetDC(0);
            BitBlt(B32.Canvas.Handle, 0, 0, B32.Width, B32.Height, SDC, ERect.Left, ERect.Top, SRCCOPY);
            ReleaseDC(0, SDC);
            bmpMagnifier.SetSize(w, h);
            B32.StretchFilter:= MagnifierStretchFilter;
            B32.DrawTo(bmpMagnifier, bmpMagnifier.BoundsRect);
            Result:= True;
          End;
        finally
          B32.Free;
        end;
      End;
    End;
  End;
end;

procedure TfrmMagnifier.WMWindowPosChanging(var Message: TWMWindowPosChanging);
begin
  Inherited;
  If Not (csLoading in ComponentState) then
  Begin
    Timer1.Enabled:= False;
    UpdateMagnifier;
    FormPaint(nil);
    Timer1.Enabled:= True;
    //FormPaint(nil);
  End;
end;

procedure TfrmMagnifier.MagnifierRatioClick(Sender: TObject);
begin
  With Sender as TMenuItem do
  Begin
    MagnifierRatio:= Tag/10;
    {UpdateMagnifier(True);
    FormPaint(nil);}
    Timer1Timer(nil);
    Checked:= True;
  End;
end;

procedure TfrmMagnifier.Exit1Click(Sender: TObject);
begin
  Close;
end;

procedure TfrmMagnifier.Timer1Timer(Sender: TObject);
begin
  UpdateMagnifier(True);
  FormPaint(nil);
end;

procedure TfrmMagnifier.UpdateMagnifier(const Force: Boolean);
Var
  M:Boolean;
  r: TRect;
  TempRegion: HRGN;
begin
  If Not LockMagnifier1.Checked then
  Begin
    MagnifierLocation:= Rect(Left,Top,Left+Width,Top+Height);
    //InflateRect(MagnifierLocation, -MagnifierBorder,-MagnifierBorder);
  End;

  M:= Magnifier(Force, MagnifierLocation);
  If M then
  Begin
    If actColorRotate.Checked then
      RotateColor(bmpMagnifier, 180)
    Else
    If actColorMono.Checked or actColorBW.Checked then
      Mono(bmpMagnifier, actColorBW.Checked);
    If (bmpNoisy.MasterAlpha<>0) then
      bmpNoisy.DrawTo(bmpMagnifier,bmpMagnifier.BoundsRect);
    If InnerShadow1.Checked and Not bmpShadow.Empty then
    Begin
      If Region=rtEllipse then
      Begin
        r:= bmpMagnifier.BoundsRect;
        InflateRect(r, -CFormBorder,-CFormBorder);
        //bmpShadow.DrawTo(bmpMagnifier);
        bmpShadow.DrawTo(bmpMagnifier,R,bmpShadow.BoundsRect);
      End;
    End;
    If Not bmpMagnifier.Empty and (BorderRgn<>0) then
    Begin
      with bmpMagnifier.Canvas.Brush do
      begin
        If Application.Active then
          Color := clActiveCaption
        Else
          Color := clInactiveCaption;
        Style := bsSolid;
      end;
      FillRgn(bmpMagnifier.Canvas.Handle, BorderRgn, bmpMagnifier.Canvas.Brush.handle);
      //FrameRgn(bmpMagnifier.Canvas.Handle, BorderRgn, bmpMagnifier.Canvas.Brush.handle, CFormBorder, CFormBorder);
    End;
  End;
end;

procedure TfrmMagnifier.GlassSurefaceClick(Sender: TObject);
begin
  With Sender as TMenuItem do
  Begin
    bmpNoisy.MasterAlpha:= Round(Tag/100*255);
    Checked:= True;
    Timer1Timer(nil);
  End;
end;

procedure TfrmMagnifier.FPSClick(Sender: TObject);
begin
  With Sender as TMenuItem do
  Begin
    If Tag>0 then
    Begin
      Timer1.Interval:= 1000 div Tag;
      Timer1.Enabled:= True;
    End Else
    Begin
      Timer1.Interval:= 0;
      Timer1.Enabled:= False;
    End;
    Checked:= True;
  End;
end;

procedure TfrmMagnifier.FormResize(Sender: TObject);
Var
  r: TRect;
//  i: Integer;
//  fr: HRGN;
begin
  If FormRgn<>0 then
  Begin
    DeleteObject(FormRgn);
    FormRgn:= 0;
    DeleteObject(BorderRgn);
    BorderRgn:= 0;
  End;
  R:= ClientRect;
  FormRgn:= CreateRgnFromRect(R, Region);
  InflateRect(R, -CFormBorder, -CFormBorder);
  BorderRgn:= CreateRgnFromRect(R, Region);
  CombineRgn(BorderRgn, FormRgn, BorderRgn, RGN_DIFF);
  SetWindowRgn(Handle, FormRgn, True);
  If (Region=rtEllipse) and InnerShadow1.Checked then
    PerpareShadow(Width-CFormBorder2,Height-CFormBorder2);
{  If FormRgn<>0 then
    DeleteObject(FormRgn);
  If BorderRgn<>0 then
    DeleteObject(BorderRgn);

  Case Region of
    rtRectangle:
      Begin
        FormRgn:= CreateRectRgn(0,0,Width,Height);
        BorderRgn:= CreateRectRgn(CFormBorder,CFormBorder,Width-CFormBorder2,Height-CFormBorder2);
        SetWindowRgn(Handle, FormRgn, True);
      End;
    rtRoundRect:
      Begin
        i:= Width;
        If Height<i then i:= Height;
        i:= i div 4;
        FormRgn:= CreateRoundRectRgn(0,0,Width,Height, i, i);
        SetWindowRgn(Handle, fr, True);
        DeleteObject(fr);
      End;
    rtEllipse:
      Begin
        fr:= CreateEllipticRgn(0,0,Width,Height);
        SetWindowRgn(Handle, fr, True);
        DeleteObject(fr);
      End;
  End;//CASE{}
end;

procedure TfrmMagnifier.LockMagnifier1Click(Sender: TObject);
begin
  With (Sender as TMenuItem) do
  Begin
    Checked:= Not Checked;
    SendMessage(Handle, WM_WindowPosChanging, 0, 0); //WMWindowPosChanging(
  End;
end;

procedure TfrmMagnifier.InnerShadowMask32(BMP32: TBitmap32; ShadowSize: Word;
  LightColor, DarkColor: TColor32; Degree: Word=45);
  Function Distance(const p1, p2: TPoint): Double;
  Begin
    Result:= sqrt(sqr(p2.X-p1.X)+sqr(p2.Y-p1.Y));
  End;
Var
  CP, LP, DP, pp:TPoint;
  cr, lr, dr: Integer;
  x,y: Integer;
  cd, ld, dd: Double;
  pa32: PColor32Array;
  LightRGB, DarkRGB: TRGBTriple;
  A:Integer;
begin
  BMP32.Clear($0);
  CP:= Point(BMP32.Width div 2, BMP32.Height div 2);
  If BMP32.Width<BMP32.Height then
    cr:= BMP32.Width div 2
  Else
    cr:= BMP32.Height div 2;
  x:= Round(ShadowSize*Cos(Degree*(CDegreeToRadians)));
  y:= Round(ShadowSize*Sin(Degree*(CDegreeToRadians)));
  LP:= Point(cp.X+x, cp.Y+y);
  DP:= Point(cp.X-x, cp.Y-y);
  lr:= cr; // to be improve
  dr:= cr; // to be improve
  LightRGB.rgbtBlue := BlueComponent(LightColor);
  LightRGB.rgbtGreen:= GreenComponent(LightColor);
  LightRGB.rgbtRed  := RedComponent(LightColor);
  DarkRGB.rgbtBlue := BlueComponent(DarkColor);
  DarkRGB.rgbtGreen:= GreenComponent(DarkColor);
  DarkRGB.rgbtRed  := RedComponent(DarkColor);

  For y:=0 to BMP32.Height-1 do
  Begin
    pa32:= BMP32.ScanLine[y];
    For x:=0 to BMP32.Width-1 do
    Begin
      pp:= Point(x,y);
      cd:= Distance(cp, pp);
      If cd<=cr then
      Begin // x,y is inside the main Circle
        ld:= Distance(lp, pp);
        If ld>lr then
        Begin
          // Lighting area
          A:= Trunc((ld-lr)/ShadowSize*255);
          With LightRGB do
            pa32^[x]:= Color32(rgbtRed, rgbtGreen, rgbtBlue, A);
        End Else
        Begin
          dd:= Distance(dp, pp);
          If dd>dr then
          Begin
            // Darking area
            A:= Trunc((dd-dr)/ShadowSize*255);
            With DarkRGB do
              pa32^[x]:= Color32(rgbtRed, rgbtGreen, rgbtBlue, A);
          End;
        End;
      End;
    End;
  End;
end;

procedure TfrmMagnifier.Mono(const BMP32: TBitmap32; const BW:Boolean=False);
Var
  p: PColor32Array;
  y: Byte;
  i: Integer;
  r, g, b: Byte;
  c32: Cardinal;
begin
  P:= BMP32.Bits;
  If BW then
    For i:=0 to BMP32.Width*BMP32.Height-1 do
    Begin
      c32:= p^[i];
      r:= c32 and $FF;
      g:= c32 shr 8 and $FF;
      b:= c32 shr 16 and $FF;
      //y:= HiByte(r * 77 + g * 151 + b * 28);
      //c:= WinColor(P^[i]);
      y:= RGBtoY(r, g, b);{}
      If y>$7F then y:= $FF Else y:=0;
      p^[i]:= RGB(y,y,y);
    End
  Else
    For i:=0 to BMP32.Width*BMP32.Height-1 do
    Begin
      c32:= p^[i];
      r:= c32 and $FF;
      g:= c32 shr 8 and $FF;
      b:= c32 shr 16 and $FF;
      //y:= HiByte(r * 77 + g * 151 + b * 28);{}
      //c:= WinColor(P^[i]);
      y:= RGBtoY(r, g, b);{}
      p^[i]:= RGB(y,y,y);
    End
  ;
end;

procedure TfrmMagnifier.PerpareShadow(const aw,ah:Word);
Var
  b: TBitmap32;
begin
  If Region=rtEllipse then
  Begin
    //b:= TBitmap32.Create;
    b:= bmpShadow;
    try
      If aw>ah then
        //b.Width:= aw
        b.SetSize(aw,aw)
      Else
        //b.Width:= ah;
        b.SetSize(ah,ah);
      //b.Height:= b.Width;
      b.DrawMode:= dmBlend;
      b.StretchFilter:= sfDraft;
      InnerShadowMask32(b, b.Width div 12, clWhite32, clBlack32);
      {bmpShadow.SetSize(aw, ah);
      b.DrawTo(bmpShadow,bmpShadow.BoundsRect);}
      {bmpShadow.SetSizeFrom(b);
      b.DrawTo(bmpShadow,bmpShadow.BoundsRect);}

      //b.DrawTo(bmpShadow);
    finally
    //  b.Free;
    end;
  End Else
    bmpShadow.SetSize(0,0);
end;

procedure TfrmMagnifier.InnerShadow1Click(Sender: TObject);
begin
  With (Sender as TMenuItem) do
  Begin
    Checked:= Not Checked;
    PerpareShadow(Width-CFormBorder2,Height-CFormBorder2);
    Timer1Timer(Timer1);
  End;
end;

procedure TfrmMagnifier.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
Var
  m: Integer;
begin
  If ssCtrl in Shift then m:=10 else m:=1;
  Case Key of
    VK_Left  : Left:= Left-m;
    VK_Up    : Top := Top-m;
    VK_Right : Left:= Left+m;
    VK_Down  : Top := Top+m;
  End;//CASE
  If Left<=-Width then Left:= -Width+1
  Else If Left>Screen.Width then Left:= Screen.Width-1;
  If Top<=-Height then Top:= -Height+1
  Else If Top>Screen.Height then Top:= Screen.Height-1;
end;

procedure TfrmMagnifier.WMNCHitTest(var Message: TWMNCHitTest);
Var
  cp, mp: TPoint;
  d: Integer;
begin
  If BorderRgn<>0 then
  With Message do
  Begin
    mp:= ScreenToClient(Point(XPos, YPos));
    If PtInRegion(BorderRgn, mp.X,mp.Y) then
    Begin
      cp:= Point(ClientWidth div 2, ClientHeight div 2);
      mp.Y:= Round(mp.Y+(mp.Y-cp.Y)*((cp.X/cp.Y)-1));
      d:= NormalDegree(Round(FindDegree(mp, cp)));
      If (d>330) then
        Result:= HTRIGHT
      Else If (d>300) then
        Result:= HTTOPRIGHT
      Else If (d>240) then
        Result:= HTTOP
      Else If (d>210) then
        Result:= HTTOPLEFT
      Else If (d>150) then
        Result:= HTLEFT
      Else If (d>120) then
        Result:= HTBOTTOMLEFT
      Else If (d>60) then
        Result:= HTBOTTOM
      Else If (d>30) then
        Result:= HTBOTTOMRIGHT
      Else
        Result:= HTRIGHT;
    End Else
      Result:= HTCLIENT;
  End Else
    Inherited;
end;

procedure TfrmMagnifier.FormShow(Sender: TObject);
begin
  SetWindowPos(Handle, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE or SWP_NOSIZE);
end;

procedure TfrmMagnifier.ApplicationEvents1Activate(Sender: TObject);
begin
  UpdateMagnifier(True);
  FormPaint(nil);
end;

procedure TfrmMagnifier.WMEraseBkgnd(var Message: TWMEraseBkgnd);
begin
  Message.Result:= 0;
end;

procedure TfrmMagnifier.RotateColor(const B: TBitmap32;
  const ADegree: Integer);
Var
  p: PColor32Array;
  i: Integer;
  c32: Cardinal;
  rgbt: TRGBTriple;
  h,s,v: Integer;
begin
  P:= B.Bits;
  For i:=0 to B.Width*B.Height-1 do
  Begin
    c32:= p^[i];
    rgbt.rgbtRed:= c32 and $FF;
    rgbt.rgbtGreen:= c32 shr 8 and $FF;
    rgbt.rgbtBlue:= c32 shr 16 and $FF;
    RGBTripleToHSV(rgbt, h,s,v);
    h:= NormalDegree(h+ADegree);
    rgbt:= HSVtoRGBTriple(h,s,v);
    p^[i]:= (rgbt.rgbtBlue shl 16) or (rgbt.rgbtGreen shl 8) or rgbt.rgbtRed;
  End
end;

procedure TfrmMagnifier.InnerShadowMask32Rect(BMP32: TBitmap32;
  ShadowSize: Word; LightColor, DarkColor: TColor32; Degree: Word);
begin

end;

procedure TfrmMagnifier.actColorExecute(Sender: TObject);
begin
  With (Sender as TAction) do
  Begin
    Checked:= True;
    Timer1Timer(nil);
  End;
end;

procedure TfrmMagnifier.actRegionExecute(Sender: TObject);
begin
  With (Sender as TAction) do
  Begin
    Case Tag of
      0: Region:= rtRectangle;
      1: Region:= rtRoundRect;
      2: Region:= rtEllipse;
    End;
    Checked:= True;
    FormResize(nil);
    Timer1Timer(Timer1);
  End;
end;

procedure TfrmMagnifier.actStretchFilterExecute(Sender: TObject);
Const
  SFValues: array[0..6] of TStretchFilter = (
    sfNearest, sfDraft, sfLinear, sfCosine, sfSpline, sfLanczos, sfMitchell);
begin
  With Sender as TAction do
  Begin
    MagnifierStretchFilter:= SFValues[Tag];
    {UpdateMagnifier(True);
    FormPaint(nil);}
    Timer1Timer(nil);
    Checked:= True;
  End;
end;

procedure TfrmMagnifier.FormMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  If Button=mbLeft then
  Begin
    If Not Dragging then
    Begin
      Dragging:= True;
      ReleaseCapture;
      Cursor:= crNone;
      SetCapture(Handle);
      DragScreenPos:= ClientToScreen(Point(X, Y));
    End;
    DragFormPos:= Point(Left,Top);
    DragMousePos:= Point(X, Y);
  End;
end;

procedure TfrmMagnifier.FormMouseMove(Sender: TObject; Shift: TShiftState;
  X, Y: Integer);
Var
  l ,t: Integer;
begin
  If Dragging then
  Begin
    l:= DragFormPos.X+(X-DragMousePos.X);
    t:= DragFormPos.Y+(Y-DragMousePos.Y);
    If (l<>Left) or (t<>Top) then
    Begin
      SetBounds(l, t, Width, Height);
      DragFormPos:= Point(l, t);
    End;
  End;
end;

procedure TfrmMagnifier.FormMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  If Dragging then
  Begin
    With ClientToScreen(Point(X,Y){DragMousePos}) do
    If (Abs(X-DragScreenPos.X)>1) or (Abs(Y-DragScreenPos.Y)>1) then
    Begin
      Dragging:= False;
      Cursor:= crSizeAll;
    End Else
    Begin
      SetCapture(Handle);
      HoldDownMouse;
    End;
  End;
end;

procedure TfrmMagnifier.HoldDownMouse;
Var
  m: tagINPUT;
begin
  m.Itype:= INPUT_MOUSE;
  m.mi.dx:= 0;
  m.mi.dy:= 0;
  m.mi.mouseData:= 0;
  m.mi.dwFlags:= MOUSEEVENTF_LEFTDOWN;
  m.mi.time:= 0;
  //SetCapture(AControl.Handle);
  SendInput(1, m, Sizeof(tagINPUT));
end;

end.
