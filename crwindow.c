#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "header_files/bmp_parser.h"

//Window cr_button(Display *displ, Window win, int screen);

int main(int argc, char *argv[]) {

    Display *displ;
    int screen;
    Window win, in_frame, x_close;;
    XEvent event;

    /* BMP files parser which reurn a pointer to the image data array.Should return also height and width values.Maybe with a structure. */
    //char *pixel_array = bmp_parser();

    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Failed to open Display.\n");
        exit(1);
    }

    screen = XDefaultScreen(displ);
    printf("Default screen value: %d\n", screen);

    /* Window without Window Manager */
    XWindowAttributes winattr;
    XGetWindowAttributes(displ, XDefaultRootWindow(displ), &winattr);
    printf("Width: %d", winattr.width);
    printf("Heigth: %d", winattr.height);
    XSetWindowAttributes root_attr;
    root_attr.background_pixel = XBlackPixel(displ, screen);
    root_attr.event_mask = ExposureMask | StructureNotifyMask | SubstructureNotifyMask | ButtonPressMask | KeyPressMask | KeyReleaseMask | PointerMotionMask;
    root_attr.cursor = None;
    root_attr.override_redirect = True;

    win = XCreateWindow(displ, XDefaultRootWindow(displ), 440, 240, 800, 500, 0, winattr.depth, InputOutput, winattr.visual, CWCursor | CWEventMask | CWBackPixel | CWOverrideRedirect, &root_attr);
    XSelectInput(displ, win, root_attr.event_mask);
    XMapWindow(displ, win);

    /* X close button */
    x_close = XCreateSimpleWindow(displ, win, 775, 0, 15, 15, 5, 0xff0000ff, 0x00ff00ff00);
    XGCValues sign;
    sign.line_width = 4;
    sign.line_style = LineSolid;
    sign.fill_rule = WindingRule;
    sign.foreground = 0XFF0000;
    GC sign_x = XCreateGC(displ, win, GCForeground | GCLineWidth | GCLineStyle | GCFillRule, &sign);
    XSelectInput(displ, x_close, ButtonPressMask);
    XMapWindow(displ, x_close);
    XDrawLine(displ, x_close, sign_x, 0, 0, 15, 15);
    XDrawLine(displ, x_close, sign_x, 15, 0, 0, 15);

    /*  Parent Window */
    in_frame = XCreateSimpleWindow(displ, win, 0, 20, 790, 470, 5, 0xff0000ff, 0x00ff00ff00);
    //XSelectInput(displ, win, ExposureMask | KeyPressMask /*| PointerMotionMask*/);
    XMapWindow(displ, in_frame);

    /* Write Text on window area */
    XFontStruct *font;
    XTextItem text[1];

    /* Delete window initializer area */
    Atom wm_delete_window = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, win, &wm_delete_window, 1);

    /* Get user text input */
    XIM xim;
    XIC xic;
    char *failed_arg;
    XIMStyles *styles;
    //XIMStyle xim_requested_style;
    xim = XOpenIM(displ, NULL, NULL, NULL);
    if (xim == NULL) {
        fprintf(stderr, "Failed to open Input Method.\n");
        exit(2);
    }
    failed_arg = XGetIMValues(xim, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
        fprintf(stderr, "Failed to obtain input method's styles.\n");
        exit(3);
    }
    for (int i = 0; i < styles->count_styles; i++) {
        printf("Styles supported %lu.\n", styles->supported_styles[i]);
    }
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditArea | XIMStatusArea, XNClientWindow, win, NULL);
    if (xic == NULL) {
        fprintf(stderr, "Could not open xic.\n");
        exit(4);
    }
    XSetICFocus(xic);

    /* Change main window Title */
    Atom new_attr = XInternAtom(displ, "WM_NAME", False);
    Atom type =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, win, new_attr, type, 8, PropModeReplace, (unsigned char*)"GastGeber", 10);

    /* Child Window */
    // child_win = XCreateSimpleWindow(displ, win, 0, 0, 180, 80, 2, 0x000fff, 0xffffff);
    // XSelectInput(displ, child_win, ExposureMask | KeyPressMask);
    // XMapWindow(displ, child_win);

    // Window button = cr_button(displ, win, screen);
    //XSync(displ, False);

    /* Get window attributes */
    //XWindowAttributes winattr;
    //XGetWindowAttributes(displ, win, &winattr);
    fprintf(stdout, "Window attributtes(visual): %p\n", winattr.visual);

    /* Add grafical context to window */
    XGCValues values;
    values.line_width = 2;
    values.line_style = LineSolid;
    values.fill_rule = WindingRule;
    values.foreground =XWhitePixel(displ, screen);
    GC gc = XCreateGC(displ, win, GCForeground | GCLineWidth | GCLineStyle | GCFillRule, &values);
    //XImage *image = XCreateImage(displ, winattr.visual, winattr.depth, ZPixmap, 0, pixel_array, 640, 426, 32, 0);

    while (1) {
        while (XPending(displ) > 0) {
            XNextEvent(displ, &event);
            if (event.type == ButtonPress && event.xclient.window == x_close) {
                //printf("Child Window Event.\n");
                XClientMessageEvent destroy;

                destroy.type = ClientMessage;
                destroy.message_type = wm_delete_window;
                destroy.format = 32;
                destroy.data.l[0] = wm_delete_window;
                event.xclient = destroy;
                XSendEvent(displ, win, True, ClientMessage, &event);
            }
            if (event.type == ClientMessage) {
                printf("Client Message Event.\n");
                if (event.xclient.data.l[0] == wm_delete_window) {
                    printf("WM_DELETE_WINDOW");
                    XFreeGC(displ, gc);
                    XDestroyWindow(displ, win);
                    //free(pixel_array);
                    XCloseDisplay(displ);
                    return 0;
                }
            } else if (event.type == Expose && event.xclient.window == win) {
                XSetInputFocus(displ, win, RevertToPointerRoot, CurrentTime);
                //printf("Expose Event occured.\n");
                font = XLoadQueryFont(displ, "7x14");
                text[0].chars = "Press Me!";
                text[0].nchars = 9;
                text[0].delta = 0;
                text[0].font = font->fid;
                XDrawText(displ, win, gc, (800 - XTextWidth(font, text[0].chars, text[0].nchars)) / 2, (500 - (font->ascent + font->descent)) / 2 + font->ascent, text, 1);
                XUnloadFont(displ, font->fid);
            } else if (event.type == KeyPress && event.xclient.window == win) {  
                int count = 0;  
                KeySym keysym = 0;
                char buffer[32];
                Status status = 0;   
                count = Xutf8LookupString(xic, &event.xkey, buffer, 32, &keysym, &status);
                printf("Button pressed.\n");
                printf("Count %d.\n", count);
                if (status == XBufferOverflow) {
                    printf("Buffer Overflow...\n");
                }
                if (count) {
                    printf("The Button that was pressed is %s.\n", buffer);
                }
                if (status == XLookupKeySym || status == XLookupBoth) {
                    printf("Status: %d\n", status);
                 }
                printf("Pressed key: %lu.\n", keysym);
            } else {
                //printf("Main Window Event.\n");
                //printf("Event Type: %d\n", event.type);
                /* Draw some lines to experiment */
                XDrawLine(displ, win, gc, rand() % 800, rand() % 800, rand() % 500, rand() % 500);
            }
            //XPutImage(displ, win, gc, image, 0, 0, 0, 0, 640, 426);
            XSync(displ, False);
        }
    }

    return 0;
}

// Window cr_button(Display *displ, Window win, int screen) {

//     int x, y, width, height;
//     Window button;

//     Colormap colormap;
//     XColor button_color, lightgray_color, darkgray_color;
//     XGCValues gcv_lightgray, gcv_darkgray;
//     GC gc_lightgray, gc_darkgray;
    
//     button = XCreateSimpleWindow(displ, win, 622, 352, 50, 20, 4, 0x000fff, 0xffffff);
//     XSelectInput(displ, button, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);
//     XMapWindow(displ, button);

//     colormap = DefaultColormap(displ, screen);
//     XParseColor(displ, colormap, "rbg:cc/cc/cc", &button_color);
//     XAllocColor(displ, colormap, &button_color);

//     XParseColor(displ, colormap, "rbg:ee/ee/ee", &lightgray_color);
//     XAllocColor(displ, colormap, &lightgray_color);
//     gcv_lightgray.foreground = lightgray_color.pixel;
//     gcv_lightgray.background = button_color.pixel;
//     gc_lightgray = XCreateGC(displ, RootWindow(displ, screen), GCForeground | GCBackground, &gcv_lightgray);

//     XParseColor(displ, colormap, "rbg:88/88/88", &darkgray_color);
//     XAllocColor(displ, colormap, &darkgray_color);
//     gcv_darkgray.foreground = darkgray_color.pixel;
//     gcv_darkgray.background = button_color.pixel;
//     gc_darkgray = XCreateGC(displ, RootWindow(displ, screen), GCForeground | GCBackground, &gcv_darkgray);

//     return button;
// }

