#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "header_files/locale.h"
#include "header_files/bmp.h"
#include "header_files/bmp_parser.h"

enum { App_Close, App_Name, Atom_Type, Atom_Last};

#define WIDTH      800
#define HEIGHT     800

#define KEYBOARDMASKS             ( KeyPressMask )
#define EXPOSEMASKS               ( StructureNotifyMask | ExposureMask )

static const void clientmessage(XEvent *event);
static const void reparentnotify(XEvent *event);
static const void expose(XEvent *event);
static const void pixmapupdate(void);
static const void pixmapdisplay(void);
static const void atomsinit(void);
static const int board(void);

static int RUNNING = 1;
static void (*handler[LASTEvent]) (XEvent *event) = {
    [ClientMessage] = clientmessage,
    [ReparentNotify] = reparentnotify,
    [Expose] = expose,
};

Display *displ;
Window app;
XImage *image;
Pixmap pixmap;
XWindowAttributes stat_app;
static Atom wmatom[Atom_Last];

static const void clientmessage(XEvent *event) {

    if (event->xclient.data.l[0] == wmatom[App_Close]) {
        printf("Exiting...\n");
        XDestroyWindow(displ, app);
        XCloseDisplay(displ);

        RUNNING = 0;
    }
}
static const void reparentnotify(XEvent *event) {

    if (event->xreparent.parent != app) 

        XGetWindowAttributes(displ, app, &stat_app);

    XGCValues gc_vals;
    gc_vals.graphics_exposures = False;
    GC gc = XCreateGC(displ, app, GCGraphicsExposures, &gc_vals);

    BMP_Image bmp_image = bmp_parser();
    image = XCreateImage(displ, stat_app.visual, stat_app.depth, ZPixmap, 0, bmp_image.data, bmp_image.width, bmp_image.height, 32, 0);
    XPutImage(displ, app, gc, image, 0, 0, 0, 0, bmp_image.width, bmp_image.height);
    XRaiseWindow(displ, app);
    
    pixmapupdate();

    free(bmp_image.data);
    XFreeGC(displ, gc);
    XFree(image);
}
static const void expose(XEvent *event) {

    pixmapdisplay();
}
static const void pixmapupdate(void) {

    XGCValues gc_vals;
    gc_vals.graphics_exposures = False;
    GC pix = XCreateGC(displ, app, GCGraphicsExposures, &gc_vals);

    pixmap = XCreatePixmap(displ, app, stat_app.width, stat_app.height, stat_app.depth);
    XCopyArea(displ, app, pixmap, pix, 0, 0, stat_app.width, stat_app.height, 0, 0);
    XFreeGC(displ, pix);
}
static const void pixmapdisplay(void) {

    XGCValues gc_vals;
    gc_vals.graphics_exposures = False;
    GC pix = XCreateGC(displ, app, GCGraphicsExposures, &gc_vals);

    XCopyArea(displ, pixmap, app, pix, 0, 0, stat_app.width, stat_app.height, 0, 0);
    XFreeGC(displ, pix);
}
static const void atomsinit(void) {

    /* Delete window initializer area */
    wmatom[App_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, app, &wmatom[App_Close], 1);

    /* Change main window Title */
    wmatom[App_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, app, wmatom[App_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"BMP Painter", 11);
}
static const int board(void) {

    XEvent event;

    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Failed to open Display.\n");
        return EXIT_FAILURE;
    }
    int screen = XDefaultScreen(displ);

    /*  App main Window */
    app = XCreateSimpleWindow(displ, XRootWindow(displ, screen), 0, 0, WIDTH, HEIGHT, 0, XWhitePixel(displ, screen), XBlackPixel(displ, screen));
    XSelectInput(displ, app, EXPOSEMASKS | KEYBOARDMASKS);
    XMapWindow(displ, app);
    XLowerWindow(displ, app);

    atomsinit();

    while (RUNNING) {

        XNextEvent(displ, &event);

        if (handler[event.type])
            handler[event.type](&event);
    }
    return EXIT_SUCCESS;
}
int main(int argc, char *argv[]) {

    if (locale_init())
        fprintf(stderr, "Warning: Main -locale()\n");

    if (board())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

