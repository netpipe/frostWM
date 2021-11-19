

#You must give the window a position on the screen. By default, a shape window is set with a position off-screen (-1000, -1000) by SDL2. It also must be made resizable via the flags.

#window = ShapeWindow("Hello World!", (800, 500), (200, 200), sdl2.SDL_WINDOW_RESIZABLE)

#Once done, after setting the window shape, you should set the size of the window to the size of the surface to render:

#sdl2.SDL_SetWindowShape(window.window, circle_sf, shape_mode)
#sdl2.SDL_SetWindowSize(window.window, circle.size[0], circle.size[1])




import sys

try:
    import sdl2
    import sdl2.ext as sdl2ext
except ImportError:
    import traceback
    traceback.print_exc()
    sys.exit(1)

from sdl2.ext import Resources
RESOURCES = Resources(__file__, "resources")

class ShapeWindow(sdl2ext.Window):
    def __init__(self, title, size, position=None, flags=None):
        if position is None:
            position = self.DEFAULTPOS
        if flags is None:
            flags = self.DEFAULTFLAGS
        window = sdl2.SDL_CreateShapedWindow(sdl2ext.compat.byteify(title, "utf-8"),
                                             position[0], position[1],
                                             size[0], size[1], flags)
        if not window:
            raise SDLError()
        self.window = window.contents
        self._renderer = None

sdl2ext.init()

# window = sdl2ext.Window("Hello World!", size=(800, 500))
window = ShapeWindow("Hello World!", size=(800, 500))
window.show()

renderer = sdl2ext.RenderContext(window)
factory = sdl2ext.SpriteFactory(sdl2ext.TEXTURE, renderer=renderer)

circle_sf = sdl2ext.load_image(RESOURCES.get_path("circle2.bmp"))
circle = factory.from_surface(circle_sf)

shape_mode = sdl2.SDL_WindowShapeMode()
shape_mode.mode = sdl2.ShapeModeColorKey
shape_mode.parameters.colorKey = (0, 0, 0, 255)
#sdl2.SDL_SetWindowShape(window.window, circle_sf, shape_mode)
sdl2.SDL_SetWindowShape(window.window, circle_sf, shape_mode)
sdl2.SDL_SetWindowSize(window.window, circle.size[0], circle.size[1])

spriterenderer = factory.create_sprite_renderer(window)
spriterenderer.render(circle)

processor = sdl2ext.TestEventProcessor()
processor.run(window)

sdl2ext.quit()
