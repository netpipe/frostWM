
import wx

class Frame(wx.Frame):
 def __init__(self):
  wx.Frame.__init__(self, None, -1, "Shaped Window",
  style =
   wx.FRAME_SHAPED
   wx.SIMPLE_BORDER
   wx.FRAME_NO_TASKBAR
   wx.STAY_ON_TOP
)
 self.hasShape = False
 self.delta = (0,0)

 self.Bind(wx.EVT_PAINT, self.OnPaint)
 self.Bind(wx.EVT_MOTION, self.OnMouseMove)

 self.bkground = wx.Bitmap("/home/adam/bkgrnd.gif", wx.BITMAP_TYPE_GIF)
 w, h = self.bkground.GetWidth(), self.bkground.GetHeight()
 self.SetClientSize((w, h))
 self.Bind(wx.EVT_WINDOW_CREATE, self.SetWindowShape)
 panel = Panel(self, w, h)
 panel.Show()
 dc = wx.ClientDC(self)
 dc.DrawBitmap(self.bkground, 0,0, True)

 def SetWindowShape(self, evt):
# Use the bitmap's mask to determine the region
 r = wx.RegionFromBitmap(self.bkground)
 self.hasShape = self.SetShape(r)

 def OnPaint(self, event):
 dc = wx.PaintDC(self)
 dc.DrawBitmap(self.bkground, 0,0, True)

 def OnMouseMove(self, evt):
  if evt.Dragging() and evt.LeftIsDown():
   x, y = self.ClientToScreen(evt.GetPosition())
   fp = (x - self.delta[0], y - self.delta[1])
   self.Move(fp)

class Panel(wx.Panel):
 def __init__(self, parent, width, height):
 wx.Panel.__init__(self, parent, -1, size=(width, height))

 button = wx.Button(self, -1, "hello")
 button.SetSize(button.GetBestSize())
 self.SetSize((width, height))
 #print dir(self)

if __name__ == "__main__":
 app = wx.PySimpleApp()
 frame = Frame()
 frame.Show()
 app.MainLoop()
