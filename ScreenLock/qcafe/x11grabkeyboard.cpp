#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

void block();
void unblock();
bool isBlock = false;
Display *display;
Screen *screen;
Window window;
char password[6];

void order_password(){
  for (int i = 0; i < 6; i++){
    password[i] = password[i+1];
  }
  password[6]=0;
}

void handle_event(XEvent event){
  KeySym keysym = XKeycodeToKeysym(display, event.xkey.keycode,0);
  char str[1];
  if (event.type == KeyReleaseMask){
    int len = strlen(password);
    if (len == 6){
      order_password();
    }
    len = strlen(password);
    XLookupString(&event.xkey,str,1,NULL,NULL);
    password[len] = str[0];
    if (strcmp(password, "hello") == 0){
      if (isBlock == false){
        memset(&password, 0, sizeof(password));
        block();
      }else{
        memset(&password, 0, sizeof(password));
        unblock();
      }
    }
  }
}

void block(){
  isBlock = true;
  XGrabKeyboard(display, window,true, GrabModeAsync, GrabModeAsync, CurrentTime);
  XEvent event;
  while (1){
    XNextEvent(display, &event);
    std::cout << "You can't type anything" << std::endl;
    XAllowEvents(display, AsyncKeyboard, CurrentTime);
    XAllowEvents(display, AsyncPointer, CurrentTime);
    handle_event(event);
  }
}

void unblock(){
  isBlock = false;
  XUngrabKeyboard(display, CurrentTime);
  XEvent event;
  while (1){
    XNextEvent(display, &event);
    XSendEvent(display,window,true, 0, &event);
    std::cout << "Now, you can type" << std::endl;
    XAllowEvents(display, AsyncKeyboard, CurrentTime);
    XAllowEvents(display, AsyncPointer, CurrentTime);
    handle_event(event);
  }
}

int main(int argc,char **argv){
  display = XOpenDisplay(NULL);
  screen = ScreenOfDisplay(display, 0);

  window = DefaultRootWindow(display);
  isBlock = false;
  if (isBlock == false){
    block();
  }
}
