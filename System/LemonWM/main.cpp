#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <assert.h>

#include <core/msghandler.h>
#include <gui/window.h>
#include <core/fb.h>
#include <core/input.h>

#include <time.h>

#ifdef __lemon__
    #include <lemon/spawn.h>
#endif

#include "lemonwm.h"

surface_t fbSurface;
surface_t renderSurface;

extern rgba_colour_t backgroundColor;

int main(){
    printf("Starting WM\n");

    CreateFramebufferSurface(fbSurface);
    renderSurface = fbSurface;
    renderSurface.buffer = new uint8_t[fbSurface.width * fbSurface.height * 4];
    
    sockaddr_un srvAddr;
    strcpy(srvAddr.sun_path, Lemon::GUI::wmSocketAddress);
    srvAddr.sun_family = AF_UNIX;
    WMInstance wm = WMInstance(renderSurface, srvAddr);

    Lemon::Graphics::DrawRect(0, 0, renderSurface.width, renderSurface.height, 0, 0, 0, &fbSurface);

    if(int e = Lemon::Graphics::LoadImage("/initrd/winbuttons.bmp", &wm.compositor.windowButtons)){
        printf("LemonWM: Warning: Error %d loading buttons.\n", e);
    }

    if(int e = Lemon::Graphics::LoadImage("/initrd/mouse.png", &wm.compositor.mouseCursor)){
        printf("LemonWM: Warning: Error %d loading mouse cursor.\n", e);
    }

    wm.compositor.backgroundImage = renderSurface;
    wm.compositor.backgroundImage.buffer = new uint8_t[renderSurface.width * renderSurface.height * 4];
    if(int e = Lemon::Graphics::LoadImage("/initrd/bg3.png", 0, 0, renderSurface.width, renderSurface.height, &wm.compositor.backgroundImage, true)){
        printf("LemonWM: Warning: Error %d loading background image.\n", e);
        wm.compositor.useImage = false;
    }
    
	timespec t;
	clock_gettime(CLOCK_BOOTTIME, &t);

    timespec _t; 
    double diff;
    double horizontalAnimationTime = 200;
    double animationTime = 1200;

    // Horizontal Animation
    while((diff = ({clock_gettime(CLOCK_BOOTTIME, &_t); ((_t.tv_sec - t.tv_sec) * 1000 + (_t.tv_nsec - t.tv_nsec) / 1000000.0);})) < horizontalAnimationTime){
        int columns = (fbSurface.width / horizontalAnimationTime) * diff;
        Lemon::Graphics::surfacecpy(&fbSurface, &wm.surface, {(fbSurface.width / 2) - (columns / 2), (fbSurface.height / 2)}, {{(fbSurface.width / 2) - (columns / 2), (fbSurface.height / 2)}, {columns, 1}});

        wm.Update();
    }

    // Vertical Animation
    while((diff = ({clock_gettime(CLOCK_BOOTTIME, &_t); ((_t.tv_sec - t.tv_sec) * 1000 + (_t.tv_nsec - t.tv_nsec) / 1000000.0);})) < animationTime){
        int lines = (fbSurface.height / animationTime) * diff;
        Lemon::Graphics::surfacecpy(&fbSurface, &wm.surface, {0, (fbSurface.height / 2) - (lines / 2)}, {{0, (fbSurface.height / 2) - (lines / 2)}, {fbSurface.width, lines}});

        wm.Update();
    }

    wm.screenSurface = fbSurface;

    for(;;){
        wm.Update();
    }
}
