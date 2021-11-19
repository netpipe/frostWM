

import os, sys
import wx

import time         # Date-Time message text formatting
import random       # Text colors

import Image        # Pil
import ImageDraw    # Pil floodfill
import ImageChops   # Pil image inversion (black <==> white)

import ImgConv      # wxImage <==> PilImage
import BitmapManip  # mask wxBmap  <==> PilImage <== file

#------------------------------------------------------------------------------

"""
On MSW the actual available client area in frameless windows seems to be 
   [(0, 0) to (w-2, h-2)] in relation to the requesred frame size. 
There is an unseen surrounding 1-pixel border.
The origin of the client area is at (0, 0).

A wxFrame without a frame is just a wxWwindow ! But, "wx.Window" can not
be specified with "wx.FRAME_SHAPED" and there is no setting "wx.WINDOW_SHAPED".
"""
CLIENT_SIZE_ADJUST = 0      # Non-MSW platforms may need their own fudge factors.
if wx.Platform == '__WXMSW__' :     # MSW "fudge factor", unfortunately.
    CLIENT_SIZE_ADJUST = 2

#------------------------------------------------------------------------------

class ShapedWindow( wx.Frame ) :
    """
    Shaped window from disk image files and optional disk transparency mask files. 

    The user cannot resize the window because there are no resizing decorations !
    The entire popup is just a control-less bitmap image.
    However, all that is visible (opaque) can be repositioned by dragging.
    """
    def __init__( self, image_filename, mask_filename=None, 
                        outer_or_inner_window=1,            # default to a shaped frame window
                        posn=(0, 0), bgTransparency=100 ) :
    
        style = ( wx.FRAME_SHAPED | wx.DEFAULT_FRAME_STYLE )
        wx.Frame.__init__( self, None, style=style )
        
        self.SetPosition( posn )
        
        self.is_border_window =    outer_or_inner_window
        self.is_inner_window = not outer_or_inner_window
        
        #------------------------------
        
        # Set the client size to the fileImage size.
        image_PilImage = Image.open( image_filename )
        if image_PilImage.mode != 'RGB' :
            image_PilImage = image_PilImage.convert( 'RGB' )
        
        clientSizeX = image_PilImage.size[0] + CLIENT_SIZE_ADJUST
        clientSizeY = image_PilImage.size[1] + CLIENT_SIZE_ADJUST
        self.SetClientSize( (clientSizeX, clientSizeY) )
        
        #------------------------------
        
        # Set overall window transparency level.
        transparencyValue = int( bgTransparency/100.0 * 255 )
        self.SetTransparent( transparencyValue )
        
        if mask_filename :
            
            mask_wxBmap = BitmapManip.CreateMaskBitmapFromFile( mask_filename )
        
        else :
            
            # Set the window's shape (the transparent pixels) according to the mask.
            mask_wxBmap = wx.Bitmap( image_filename )
            
        #end if
        
        # Indicate the color of the pixels that are to become transparent.
        mask_wxBmap.SetMaskColour( wx.Color( 0, 0, 0 ) )
        # Set window's transparent pixels according to the mask values.
        self.SetShape( wx.RegionFromBitmap( mask_wxBmap ) )
        
        
        #------------------------------
        
        # Convert it to a wxBitmap for use in window drawing using a DC.
        #self.image_wxBitmap = ImgConv.WxBitmapFromPilImage( image_PilImage )
        self.image_wxBitmap = wx.Bitmap( image_filename, wx.BITMAP_TYPE_ANY )
        
        if wx.Platform == '__WXGTK__' :     # GTK-only, use as an event handler.
            self.Bind( wx.EVT_WINDOW_CREATE, self.DrawWindow )
        #end if
        
        #------------------------------
        
        # This handler is always required.
        self.Bind( wx.EVT_PAINT, self.OnPaint )
        
        # Enable the user to quit the app by pressing <ESC>.
        self.Bind( wx.EVT_KEY_UP, self.OnKeyDown )  # Comment this to disable.
        self.Bind( wx.EVT_RIGHT_UP, self.OnQuit )
        
        # Enable window dragging.
        self.Bind( wx.EVT_MOTION, self.OnMotion )    # Comment this to disable.
        
        #------------------------------
        
        if self.is_border_window :
            self.textPosns = []                   # A place-holder
            self.textFgColor, self.textBgColor = GetRamdomWxColorAndInverse()
            
            self.updateTodTimer = wx.Timer( self, id=wx.NewId() )
            self.Bind( wx.EVT_TIMER, self.OnTimer )
            self.timerPeriod = 1000
            self.updateTodTimer.Start( self.timerPeriod, oneShot=False )  # Always astable.
        #end if
        
    #end def __init__
    
    #--------------------------------------------
    
    def SetOtherWindow( self, otherWindow ) :
        """ Allow the other ShapedWindow to be referenced in this instantiation. """
        self.otherWindow = otherWindow
    #end def
    
    def SetMyPosition( self, posn ) :
        """ This is for "OtherWindow" to call, never "self"."""
        self.SetPosition( posn )
    #end def
    
    #--------------------------------------------
    
    def OnPaint( self, event ) :
        self.DrawWindow()       # A wxPaintDC is not needed or wanted.
        event.Skip()    # Very important to let all higher level handlers be called.
    #end def
    
    def DrawWindow( self ) :
        """Implement window drawing at any time."""
        
        ShapedWindowDC = wx.ClientDC( self )      # dc.Clear() erases to black.
        
        # Paste the background image bitmap onto the DC.
        ShapedWindowDC.DrawBitmap( self.image_wxBitmap, 0, 0 )
        
        if self.is_border_window :
            DrawTextMessages( ShapedWindowDC, self.textPosns,
                              self.textFgColor, self.textBgColor )
    #end def DrawWindow
    
    #--------------------------------------------
    
    def OnQuit( self, event ) :
        wx.Exit()
    #end def
    
    def OnKeyDown( self, event ) :
        """Quit the app if the user presses Q, q or Esc"""
        
        keyCode = event.GetKeyCode()
        quitCodes = [27, ord('Q'), ord('q')]
        
        event.Skip()        # Allow any following event processing.
        if (keyCode in quitCodes) :
            #self.Close( force=True )
            wx.Exit()
        #end if
        
    #end def
    
    #--------------------------------------------
    
    def OnMotion( self, event ) :
        """Implement window client area dragging since this window has no frame to grab."""
        
        if not event.Dragging() :    # Mouse is moving but no button is down.
            self.dragPosn = None
            return
        #end if
            
        self.CaptureMouse()
        
        if self.dragPosn == None :      # Previous non-dragging mouse position
            # Capture the first mouse coord after pressing any button
            self.dragPosn = event.GetPosition()
        else:
            currPosn = event.GetPosition()
            displacement = self.dragPosn - currPosn     # always nonzero
            newPosn = self.GetPosition() - displacement
            self.SetPosition( newPosn )
            
            self.otherWindow.SetPosition( newPosn )
        #end if
        
        event.Skip()
        
    #end def
    
    #--------------------------------------------
    
    def OnTimer( self, event ) :
        """Update the pop-up message with the current time of day."""
        
        self.textFgColor, self.textBgColor = GetRamdomWxColorAndInverse()
        self.DrawWindow()
        
    #end def
    
#end class ShapedWindow

#------------------------------------------------------------------------------

def DrawTextMessages( dc, textPosns, fgColor=wx.WHITE, bgColor=wx.BLACK ) :
    
    textPadding = ' '      # Make sure that the entire opaque text "island" is drawn upon.
    
    # Draw the text message.
    dc.SetBackgroundMode( wx.SOLID )  # wx.SOLID or wx.TRANSPARENT
    
    dc.SetTextForeground( fgColor )
    dc.SetTextBackground( bgColor )  # has no effect if the bg is wx.TRANSPARENT
        
    # Draw some text.
    xOffset, yOffset = textPosns[0]
    dayDateStr =  time.strftime( '%A, %B %d, %Y', time.localtime())
    dc.DrawText( textPadding+dayDateStr+textPadding, xOffset, yOffset )
    
    xOffset, yOffset = textPosns[1]
    dayDateStrList = dayDateStr.split( ',' )
    dayName = dayDateStrList[0]
    monthName, day = dayDateStrList[1].split()
    year = dayDateStrList[2].strip()
    numericalDateTimeStr = year + '-' + monthName + '-' + day + ' ' + dayName
    dc.DrawText( textPadding+numericalDateTimeStr+textPadding, xOffset, yOffset )
    
    xOffset, yOffset = textPosns[2]
    todStr = time.strftime( '%I:%M:%S %p', time.localtime() )
    dc.DrawText( textPadding+todStr+textPadding, xOffset, yOffset )
    
    xOffset, yOffset = textPosns[3]
    quitMsg = 'Press <ESC>, Q or q to Quit this Demo'
    dc.DrawText( textPadding+quitMsg+textPadding, xOffset, yOffset )
        
#end def

#------------------------------------------------------------------------------

def CreateInnerMaskBmapFromOuterMask( srcBmap ) :
    """ 
    Derive the inner mask wxBitmap from the Outer mask wxBitmap.
    
    The srcBmap must be "well behaved" in that a continuous border 
    must present so that a floodfill to the perimeter area will not reach 
    into the inner area. The border color must be >=128. So, 
    the srcBmap consists of a transparent/BLACK perimeter, an white/opaque
    frame border and a transparent/BLACK inner area.
    
    When completed, the outer_area+border will be transparent/BLACK, 
    the parent's frame border will be transparent/BLACK and the inner area 
    will be opaque/WHITE.
    
    1. outer perimeter (black) --> Floodfill to white/255
                                   Now both perimeter and border are white).
    2. Invert the image and return as a wxBitmap..
    
    """
    # Start with an 'L' Pil copy of the RGB input wxBitmap.
    dstPilImage = ImgConv.PilImageFromWxBitmap( srcBmap ).convert( 'L' )
    
    # Make sure the image is quantized to binary.
    dstPilImage = dstPilImage.point(lambda i: (i / 128) * 255)
    size = dstPilImage.size
    ImageDraw.floodfill( dstPilImage, (0, 0), (255) )
    
    return ImgConv.WxBitmapFromPilImage( ImageChops.invert( dstPilImage ) )
    
#end def

#------------------------------------------------------------------------------

def GetRamdomWxColorAndInverse() :
    
    r, g, b = (random.randint(0, 127), random.randint(0, 127), random.randint(0, 127))
    
    if random.randint(0, 1) :       # Gaurantee a large contrast
        r, g, b = (255-r, 255-g, 255-b)
    #end if
    
    R, G, B, = (255-r, 255-g, 255-b)        # The inverse
    
    return (wx.Colour(r, g, b), wx.Colour(R, G, B))
    
#end def

#==============================================================================

def demo() :
    
    CREATE_INNER_WINDOW = 0     # for inner window instantiation of Shaped Window()
    CREATE_OUTER_WINDOW = 1     # for outer window instantiation of Shaped Window()
    
    # Define where all the text messages (static and dynamic) are to be placed
    textPosns = []
    textPosns.append( (115, 100) )      # Date
    textPosns.append( (120, 125) )      # Numerical date
    textPosns.append( (150, 150) )      # Tod
    textPosns.append( ( 65, 275) )      # Quit
    
    ####  COMPOSE THE OUTER WINDOW  ---------------------------------
    
    # Image1 is the fully opaque custom-drawn frame border.
    # Image2 is another fully opaque image, such as a logo. frame handle, etc.
    # Existing files
    frameBorder_filename = 'IMAGES/OuterFrameBorder.PNG'
    logo_filename        = 'IMAGES/OuterLogo.PNG'
    
    # Files to be created
    outerImage_filename = 'IMAGES/OUTER_IMAGE.PNG'
    outerMask_filename  = 'IMAGES/OUTER_MASK.PNG'
    innerMask_filename  = 'IMAGES/INNER_MASK.PNG'
    
    print '\n----  demo():  CREATING Combo Image and Combo Mask From Source Image Files.'
    maskBmap1 = BitmapManip.CreateMaskBitmapFromFile( frameBorder_filename )
    maskBmap2 = BitmapManip.CreateMaskBitmapFromFile( logo_filename )

    #----------

    # Create opaque "islands" where the text messages will be written.

    # DateTime
    dateTimeStr = time.strftime( '%A, %B %d, %Y', time.localtime())
    textExtent, textOffset = BitmapManip.GetTextExtent( dateTimeStr )
    textMask_bmap = ImgConv.WxBitmapFromPilImage( Image.new( 'L', (textExtent[0]-10, textExtent[1]-2), (255) ) )
    maskBmap1 = BitmapManip.CombineMasks( maskBmap1, textMask_bmap, textPosns[0], extend=False )

    # Numerical dateTime
    dayDateStrList = dateTimeStr.split( ',' )
    dayName = dayDateStrList[0]
    monthName, day = dayDateStrList[1].split()
    year = dayDateStrList[2].strip()
    numericalDateTimeStr = year + '-' + monthName + '-' + day + ' ' + dayName

    textExtent, textOffset = BitmapManip.GetTextExtent( numericalDateTimeStr )
    textMask_bmap = ImgConv.WxBitmapFromPilImage( Image.new( 'L', (textExtent[0]-10, textExtent[1]-2), (255) ) )
    maskBmap1 = BitmapManip.CombineMasks( maskBmap1, textMask_bmap, textPosns[1], extend=False )

    # Time-of-Day String
    todStr = '88:88:88 PM'     # Just a template of the correct size
    textExtent, textOffsetDUMMY = BitmapManip.GetTextExtent( todStr )
    textMask_bmap = ImgConv.WxBitmapFromPilImage( Image.new( 'L', (textExtent[0]-10, textExtent[1]-2), (255) ) )
    maskBmap1 = BitmapManip.CombineMasks( maskBmap1, textMask_bmap, textPosns[2], extend=False )

    # Quit message
    quitMsg = 'Press <ESC>, Q or q to Quit this Demo'
    textExtent, textOffsetDUMMY = BitmapManip.GetTextExtent( quitMsg )
    textMask_bmap = ImgConv.WxBitmapFromPilImage( Image.new( 'L', (textExtent[0]-30, textExtent[1]-2), (255) ) )
    maskBmap1 = BitmapManip.CombineMasks( maskBmap1, textMask_bmap, textPosns[3], extend=False )

    #----------

    # "Paste", using transparency, the logo image onto the border image.
    logo_offset = (300, 300)      # Image2's paste position relative to Image1.
    outer_maskBmap = BitmapManip.CombineMasks( maskBmap1, maskBmap2, logo_offset, extend=True )
    # Save file only to show the results.
    outer_maskBmap.SaveFile( outerMask_filename, wx.BITMAP_TYPE_PNG )

    outer_pilImage = BitmapManip.CombineFileImagesUsingFileMasks(  \
                            frameBorder_filename, None, 
                            logo_filename, None, 
                            logo_offset )
    
    # Save file only to show the results.
    outer_pilImage.save( outerImage_filename )     # A given input to Shaped Window()

    outer_pilImage = BitmapManip.CombineFileImagesUsingFileMasks(  \
                            outerMask_filename,  None, 
                            outerImage_filename, None, 
                            (0, 0) )
    
    outer_pilImage.save( outerImage_filename )     # A given input to Shaped Window()

    #---------
    
    # Exactly center the completed popup window in the screen.
    screenSize = wx.DisplaySize()
    outerSize = outer_pilImage.size
    
    bothPosn = ( (screenSize[0] - outerSize[0])/2, (screenSize[1] - outerSize[1])/2 )
    
    myOuterWindow = ShapedWindow( outerImage_filename, outerMask_filename, 
                                  CREATE_OUTER_WINDOW, 
                                  bothPosn, bgTransparency=100 )
    
    myOuterWindow.textPosns = textPosns
    
    ####  COMPOSE THE INNER WINDOW  ---------------------------------
    
    """
    Use some chicanery to create innerMask from OuterMask.
    By doing this, only image files (and no mask files) are needed
    to create mask bmaps for transparency. The image files must have been
    created or modified to have transparency.
    """
    
    if not os.path.exists( innerMask_filename ) :
        innerMaskBmap = CreateInnerMaskBmapFromOuterMask( outer_maskBmap )
        innerMaskBmap.SaveFile( innerMask_filename, wx.BITMAP_TYPE_PNG )
    #end if
    
    # Note:  innerImage should be at least as big as outerImage's border.
    innerImage_filename = 'IMAGES/PERCOLATION.PNG'
    
    myInnerWindow = ShapedWindow( innerImage_filename, innerMask_filename, 
                                  CREATE_INNER_WINDOW, 
                                  bothPosn, bgTransparency=30 )
    #------------------------
    
    myOuterWindow.SetOtherWindow( myInnerWindow )
    myInnerWindow.SetOtherWindow( myOuterWindow )
    
    # The windows are displayed now to avoid an annoying lag between seeing the
    #   parent window and seeing the inner's.
    myInnerWindow.Show()
    myOuterWindow.Show()
    
#end def demo

#==============================================================================

if __name__ == '__main__' :
    
    # There must be an app in order to use wx methods.
    demoApp = wx.App( redirect=False )
    demo()
    demoApp.MainLoop()
    
    #sys.exit(1)
    
#end if
