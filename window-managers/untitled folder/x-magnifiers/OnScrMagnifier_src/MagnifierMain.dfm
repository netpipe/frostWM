object frmMagnifier: TfrmMagnifier
  Left = 200
  Top = 476
  Cursor = crSizeAll
  AlphaBlend = True
  BorderStyle = bsNone
  Caption = 'On Screen Magnifier'
  ClientHeight = 166
  ClientWidth = 192
  Color = clBtnFace
  Constraints.MinHeight = 8
  Constraints.MinWidth = 8
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Shell Dlg 2'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnMouseDown = FormMouseDown
  OnMouseMove = FormMouseMove
  OnMouseUp = FormMouseUp
  OnPaint = FormPaint
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PopupMenu1: TPopupMenu
    Left = 8
    Top = 8
    object Size1: TMenuItem
      Caption = 'Magnifier'
      object N01X1: TMenuItem
        Tag = 1
        Caption = '0.3 X'
        GroupIndex = 1
        RadioItem = True
        OnClick = MagnifierRatioClick
      end
      object N05X1: TMenuItem
        Tag = 5
        Caption = '0.5 X'
        GroupIndex = 1
        RadioItem = True
        OnClick = MagnifierRatioClick
      end
      object N08X1: TMenuItem
        Tag = 8
        Caption = '0.8 X'
        GroupIndex = 1
        RadioItem = True
        OnClick = MagnifierRatioClick
      end
      object N1X1: TMenuItem
        Tag = 10
        Caption = '1 X'
        GroupIndex = 1
        RadioItem = True
        OnClick = MagnifierRatioClick
      end
      object N2X1: TMenuItem
        Tag = 20
        Caption = '2 X'
        Checked = True
        GroupIndex = 1
        RadioItem = True
        OnClick = MagnifierRatioClick
      end
      object N3X1: TMenuItem
        Tag = 30
        Caption = '3 X'
        GroupIndex = 1
        RadioItem = True
        OnClick = MagnifierRatioClick
      end
      object N4X1: TMenuItem
        Tag = 40
        Caption = '4 X'
        GroupIndex = 1
        RadioItem = True
        OnClick = MagnifierRatioClick
      end
      object N8X1: TMenuItem
        Tag = 80
        Caption = '8 X'
        GroupIndex = 1
        RadioItem = True
        OnClick = MagnifierRatioClick
      end
    end
    object Region1: TMenuItem
      Caption = 'Region'
      object mnRectangle1: TMenuItem
        Action = actRgnRectangle
        GroupIndex = 6
        RadioItem = True
      end
      object mnRoundRect1: TMenuItem
        Tag = 1
        Action = actRgnRoundRect
        GroupIndex = 6
        RadioItem = True
      end
      object mnEllipse1: TMenuItem
        Tag = 2
        Action = actRgnEllipse
        GroupIndex = 6
        RadioItem = True
      end
    end
    object StretchFilter1: TMenuItem
      Caption = 'Stretch Filter'
      object sfNearest1: TMenuItem
        Action = actSFNearest
        GroupIndex = 2
        RadioItem = True
      end
      object sfDraft1: TMenuItem
        Tag = 1
        Action = actSFDraft
        GroupIndex = 2
        RadioItem = True
      end
      object sfLinear1: TMenuItem
        Tag = 2
        Action = actSFLinear
        GroupIndex = 2
        RadioItem = True
      end
      object sfCosine1: TMenuItem
        Tag = 3
        Action = actSFCosine
        GroupIndex = 2
        RadioItem = True
      end
      object sfSpline1: TMenuItem
        Tag = 4
        Action = actSFSpline
        GroupIndex = 2
        RadioItem = True
      end
      object sfLanczos1: TMenuItem
        Tag = 5
        Action = actSFLanczos
        GroupIndex = 2
        RadioItem = True
      end
      object sfMitchell1: TMenuItem
        Tag = 6
        Action = actSFMitchell
        GroupIndex = 2
        RadioItem = True
      end
    end
    object GlassSureface1: TMenuItem
      Caption = 'Glass Sureface'
      object None1: TMenuItem
        Caption = 'None'
        Checked = True
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N101: TMenuItem
        Tag = 10
        Caption = '10 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N201: TMenuItem
        Tag = 20
        Caption = '20 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N301: TMenuItem
        Tag = 30
        Caption = '30 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N401: TMenuItem
        Tag = 40
        Caption = '40 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N501: TMenuItem
        Tag = 50
        Caption = '50 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N601: TMenuItem
        Tag = 60
        Caption = '60 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N701: TMenuItem
        Tag = 70
        Caption = '70 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N801: TMenuItem
        Tag = 80
        Caption = '80 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
      object N901: TMenuItem
        Tag = 90
        Caption = '90 %'
        GroupIndex = 3
        RadioItem = True
        OnClick = GlassSurefaceClick
      end
    end
    object FPS1: TMenuItem
      Caption = 'Frame Per Second'
      object N0fps1: TMenuItem
        Caption = '0 fps'
        GroupIndex = 4
        RadioItem = True
        OnClick = FPSClick
      end
      object N10fps1: TMenuItem
        Tag = 10
        Caption = '10 fps'
        Checked = True
        GroupIndex = 4
        RadioItem = True
        OnClick = FPSClick
      end
      object N20fps1: TMenuItem
        Tag = 20
        Caption = '20 fps'
        GroupIndex = 4
        RadioItem = True
        OnClick = FPSClick
      end
      object N30fps1: TMenuItem
        Tag = 30
        Caption = '30 fps'
        GroupIndex = 4
        RadioItem = True
        OnClick = FPSClick
      end
    end
    object ColorSpace1: TMenuItem
      Caption = 'Color Space'
      object mnColor1: TMenuItem
        Action = actColorColor
        GroupIndex = 8
        RadioItem = True
      end
      object mnRotate1801: TMenuItem
        Action = actColorRotate
        GroupIndex = 8
        RadioItem = True
      end
      object mnMono1: TMenuItem
        Action = actColorMono
        GroupIndex = 8
        RadioItem = True
      end
      object mnBW1: TMenuItem
        Action = actColorBW
        GroupIndex = 8
        RadioItem = True
      end
    end
    object InnerShadow1: TMenuItem
      Caption = 'Inner Shadow (Ellipse)'
      OnClick = InnerShadow1Click
    end
    object LockMagnifier1: TMenuItem
      Caption = 'Lock Magnifier'
      OnClick = LockMagnifier1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Exit1: TMenuItem
      Caption = 'E&xit'
      OnClick = Exit1Click
    end
  end
  object Timer1: TTimer
    Interval = 100
    OnTimer = Timer1Timer
    Left = 40
    Top = 8
  end
  object ApplicationEvents1: TApplicationEvents
    OnActivate = ApplicationEvents1Activate
    OnDeactivate = ApplicationEvents1Activate
    Left = 8
    Top = 40
  end
  object ActionList1: TActionList
    Left = 40
    Top = 40
    object actRgnRectangle: TAction
      Category = 'Region'
      Caption = 'Rectangle'
      Checked = True
      GroupIndex = 6
      OnExecute = actRegionExecute
    end
    object actRgnRoundRect: TAction
      Tag = 1
      Category = 'Region'
      Caption = 'Round Rect'
      GroupIndex = 6
      OnExecute = actRegionExecute
    end
    object actRgnEllipse: TAction
      Tag = 2
      Category = 'Region'
      Caption = 'Ellipse'
      GroupIndex = 6
      OnExecute = actRegionExecute
    end
    object actColorColor: TAction
      Category = 'ColorSpace'
      Caption = 'Color'
      Checked = True
      GroupIndex = 8
      OnExecute = actColorExecute
    end
    object actColorRotate: TAction
      Category = 'ColorSpace'
      Caption = 'Rotate 180'
      GroupIndex = 8
      OnExecute = actColorExecute
    end
    object actColorMono: TAction
      Category = 'ColorSpace'
      Caption = 'Mono'
      GroupIndex = 8
      OnExecute = actColorExecute
    end
    object actColorBW: TAction
      Category = 'ColorSpace'
      Caption = 'B && W'
      GroupIndex = 8
      OnExecute = actColorExecute
    end
    object actSFNearest: TAction
      Category = 'StretchFilter'
      Caption = 'Nearest'
      Checked = True
      GroupIndex = 2
      OnExecute = actStretchFilterExecute
    end
    object actSFDraft: TAction
      Tag = 1
      Category = 'StretchFilter'
      Caption = 'Draft'
      GroupIndex = 2
      OnExecute = actStretchFilterExecute
    end
    object actSFLinear: TAction
      Tag = 2
      Category = 'StretchFilter'
      Caption = 'Linear'
      GroupIndex = 2
      OnExecute = actStretchFilterExecute
    end
    object actSFCosine: TAction
      Tag = 3
      Category = 'StretchFilter'
      Caption = 'Cosine'
      GroupIndex = 2
      OnExecute = actStretchFilterExecute
    end
    object actSFSpline: TAction
      Tag = 4
      Category = 'StretchFilter'
      Caption = 'Spline'
      GroupIndex = 2
      OnExecute = actStretchFilterExecute
    end
    object actSFLanczos: TAction
      Tag = 5
      Category = 'StretchFilter'
      Caption = 'Lanczos'
      GroupIndex = 2
      OnExecute = actStretchFilterExecute
    end
    object actSFMitchell: TAction
      Tag = 6
      Category = 'StretchFilter'
      Caption = 'Mitchell'
      GroupIndex = 2
      OnExecute = actStretchFilterExecute
    end
  end
end
