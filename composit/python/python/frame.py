
import wx

#------------------------------------------------------------------------------

def GetRoundedRectileBitmap( w, h, r  ):

    maskColor = wx.Color( 0, 0, 0 )
    shownColor = wx.Color( 5, 5, 5 )
    
    bmap = wx.EmptyBitmap( w, h )
    dc = wx.MemoryDC( bmap )
    
    dc.SetBrush( wx.Brush( maskColor ) )
    dc.DrawRectangle( 0, 0, w, h )
    
    dc.SetBrush( wx.Brush( shownColor ) )
    dc.SetPen( wx.Pen( shownColor ) )
    dc.DrawRoundedRectangle( 0, 0, w, h, r )
    
    dc.SelectObject( wx.NullBitmap )    # release the associated bitmap from the DC tool
    bmap.SetMaskColour( maskColor )
    
    return bmap

#------------------------------------------------------------------------------

def GetRoundShape( w, h, r  ):
    return wx.RegionFromBitmap( GetRoundedRectileBitmap( w, h, r) )

#------------------------------------------------------------------------------

class FancyFrame( wx.Frame ):

    def __init__( self ):
    
        style = ( wx.STAY_ON_TOP | wx.FRAME_NO_TASKBAR | wx.FRAME_SHAPED )
                  
        wx.Frame.__init__( self, None, title='Fancy', style=style )
        
        self.SetSize( (300, 120) )
        self.SetPosition( (400,300) )
        self.SetTransparent( 245  )     # 245/255'ths opaque

        self.Bind( wx.EVT_KEY_UP, self.OnKeyUp )
        self.Bind( wx.EVT_MOTION, self.OnMouse )
        self.Bind( wx.EVT_PAINT, self.OnPaint )
        self.move = True
        
        if wx.Platform == '__WXGTK__':
            self.Bind( wx.EVT_WINDOW_CREATE, self.SetRoundShape )
        else:
            self.SetRoundShape()
        #end if

        self.Show( True )
        
    #end def

    def SetRoundShape( self, event=None ):
        w, h = self.GetSizeTuple()
        self.SetShape( GetRoundShape( w, h, 10 ) )

    def OnPaint( self, event ):
        dc = wx.PaintDC( self )
        dc = wx.GCDC( dc )
        w, h = self.GetSizeTuple()
        r = 10
        dc.SetPen( wx.Pen( "#806666", width = 2 ) )
        dc.SetBrush( wx.Brush( "#80A0B0") )
        dc.DrawRoundedRectangle( 0, 0, w, h, r  )

    def OnKeyUp( self, event ):
        """quit if user presses Q, q or <Esc>"""
        
        if ( event.GetKeyCode() in [ 27, ord( 'Q' ), ord( 'q' ) ] ):
            self.Close( force=True )
        #end if
        
        if ( event.GetKeyCode() in [ ord( 'M' ), ord( 'm' ) ] ):
            if self.move :
                self.Unbind( wx.EVT_MOTION )
            else :
                self.Bind( wx.EVT_MOTION, self.OnMouse )
            #end if
            
            self.move = not self.move
        #end if
        
        event.Skip()
        
    #end OnKeyUp def
    
    def OnMouse( self, event ):
        """implement dragging"""
        
        if not event.Dragging():
            self._dragPos = None
            return
            
        self.CaptureMouse()
        
        if not self._dragPos:
            self._dragPos = event.GetPosition()
        else:
            pos = event.GetPosition()
            displacement = self._dragPos - pos
            self.SetPosition( self.GetPosition() - displacement  )
        #end if
        
    #end def
    
#end class FancyFrame

#------------------------------------------------------------------------------

print '\n>>>>  Press <Esc>, "Q" or "q" to End this Program'
print '\n>>>>  Press "M" or "m" to Toggle Window Dragging Ability'

app = wx.App( redirect=False )
f = FancyFrame()
app.MainLoop()
