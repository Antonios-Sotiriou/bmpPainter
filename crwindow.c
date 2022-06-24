#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static const void keypress(XEvent *event);
static const void atomsinit(void);
static const int board(void);

static int RUNNING = 1;
static void (*handler[LASTEvent]) (XEvent *event) = {
    [ClientMessage] = clientmessage,
    [ReparentNotify] = reparentnotify,
    [Expose] = expose,
    [KeyPress] = keypress,
};

Display *displ;
Window app;
XImage *image;
XWindowAttributes stat_app;
static Atom wmatom[Atom_Last];

static const void clientmessage(XEvent *event) {
    printf("ClientMessage event received\n");
    if (event->xclient.data.l[0] == wmatom[App_Close]) {

        XDestroyWindow(displ, app);
        XCloseDisplay(displ);

        RUNNING = 0;
    }
}
static const void reparentnotify(XEvent *event) {

    if (event->xreparent.parent != app) 

        XGetWindowAttributes(displ, app, &stat_app);
}
static const void expose(XEvent *event) {
    
    printf("Expose event received\n");

    GC gc = XCreateGC(displ, app, 0, NULL);
    BMP_Image bmp_image = bmp_parser();
    image = XCreateImage(displ, stat_app.visual, stat_app.depth, ZPixmap, 0, bmp_image.data, bmp_image.width, bmp_image.heigth, 32, 0);
    XPutImage(displ, app, gc, image, 0, 0, 0, 0, bmp_image.width, bmp_image.heigth);
    free(bmp_image.data);
    XFreeGC(displ, gc);
}
static const void keypress(XEvent *event) {

    /* Get user text input */
    XIM xim;
    XIC xic;
    char *failed_arg;
    XIMStyles *styles;

    xim = XOpenIM(displ, NULL, NULL, NULL);
    if (xim == NULL) {
        fprintf(stderr, "keypress() - XOpenIM()");
    }
    
    failed_arg = XGetIMValues(xim, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
        fprintf(stderr, "keypress() - XGetIMValues()");
    }
    XFree(failed_arg);

    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, app, NULL);
    if (xic == NULL) {
        fprintf(stderr, "keypress() - XreateIC()");
    }
    XSetICFocus(xic);

    KeySym keysym = 0;
    char buffer[32];
    Status status = 0;
    Xutf8LookupString(xic, &event->xkey, buffer, 32, &keysym, &status);
    if (status == XBufferOverflow) {
        fprintf(stderr, "Buffer Overflow...\n");
    }
    if (status != XLookupKeySym || status != XLookupBoth) {
        fprintf(stderr, "Status: %d\n", status);
    }
    if (keysym == 65293) {
        event->type = Expose;
        XSendEvent(displ, app, False, StructureNotifyMask, event);
    } else {
        return;
    }

    XDestroyIC(xic);
    XCloseIM(xim);
}
static const void atomsinit(void) {

    /* Delete window initializer area */
    wmatom[App_Close] = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, app, &wmatom[App_Close], 1);

    /* Change main window Title */
    wmatom[App_Name] = XInternAtom(displ, "WM_NAME", False);
    wmatom[Atom_Type] =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, app, wmatom[App_Name], wmatom[Atom_Type], 8, PropModeReplace, (unsigned char*)"BMP Parser", 10);
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

