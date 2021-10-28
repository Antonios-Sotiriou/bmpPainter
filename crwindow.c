#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

char bmp_parser();

Window cr_button(Display *displ, Window win, int screen);
typedef struct {
    unsigned char RedPixel;
    unsigned char BluePixel;
    unsigned char GreenPixel;
} PIXEL;

int main(int argc, char *argv[]) {

    Display *displ;
    int screen;
    Window win, child_win;
    XEvent event;
    PIXEL *pixel_arr, pixel;
    

    FILE *fp;
    fp = fopen("/home/as/Desktop/sample.bmp", "rb");
    //fp = fopen("/home/as/Desktop/triangles.bmp", "rb");
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 138, SEEK_SET);

    char *pixel_array = malloc(sizeof(char));
    pixel_arr = malloc(sizeof(PIXEL));
    if (pixel_array == NULL) {
        printf("Can't create Pixel array!\n");
    }
    /* setting memory values to 0, must include <string.h> for that to work */
    //memset(pixel_array, 0, sizeof(char) * file_size);
    pixel_array = realloc(pixel_array, sizeof(char) * 640 * 426 * 4);
    char c;
    int i = 0, dynamic_inc = 2, x = 0;
    while (1) {
        fread(&pixel, sizeof(PIXEL), 1, fp);
        if (feof(fp)) {
            printf("End of file.\n");
            printf("Size of pixel: %d\n", sizeof(unsigned char));
            break;
        }
        //printf("Reading pixel array...\n");
        pixel_arr = realloc(pixel_arr, sizeof(PIXEL) * dynamic_inc);
        pixel_arr[x] = pixel;
        pixel_array[i] += pixel_arr[x].RedPixel;
        pixel_array[i + 1] += pixel_arr[x].BluePixel;
        pixel_array[i + 2] += pixel_arr[x].GreenPixel;
        //pixel_array[i] = 0xFF000000; /* AlphaMask */
        //pixel_array[i] = ((rand() % 255 - c) + (rand() % 255) * 100000) + (rand() % 255) * 1000; /* Red Color */
        //pixel_array[i] = 0xFF00FF00; /* Green Color */
        ///pixel_array[i] = 0x000000FF; /* BLue color */
        //pixel_array[i] = c;
        dynamic_inc++;
        i += 4;
        x++;
        //i++;
    }
    printf("Pixel iterations: %d", x);
    fclose(fp);

    displ = XOpenDisplay(NULL);
    if (displ == NULL) {
        fprintf(stderr, "Can't open Display.\n");
        exit(1);
    }

    screen = DefaultScreen(displ);
    printf("Default screen value: %d\n", screen);

    /*  Parent Window */
    win = XCreateSimpleWindow(displ, XRootWindow(displ, screen), 0, 0, 800, 500, 0, XWhitePixel(displ, screen), XBlackPixel(displ, screen));
    XSelectInput(displ, win, ExposureMask | KeyPressMask /*| PointerMotionMask*/);
    XMapWindow(displ, win);

    /* Delete window initializer area */
    Atom wm_delete_window = XInternAtom(displ, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(displ, win, &wm_delete_window, 1);

    /* Change main window Title */
    Atom new_attr = XInternAtom(displ, "WM_NAME", False);
    Atom type =  XInternAtom(displ, "STRING", False);
    XChangeProperty(displ, win, new_attr, type, 8, PropModeReplace, "GastGeber", 10);

    /* Child Window */
    // child_win = XCreateSimpleWindow(displ, win, 0, 0, 180, 80, 2, 0x000fff, 0xffffff);
    // XSelectInput(displ, child_win, ExposureMask | KeyPressMask);
    // XMapWindow(displ, child_win);

    // Window button = cr_button(displ, win, screen);
    //XSync(displ, False);

    /* Get window attributes */
    XWindowAttributes winattr;
    XGetWindowAttributes(displ, win, &winattr);
    fprintf(stdout, "Window attributtes(visual): %p\n", winattr.visual);

    /* Add grafical context to window */
    //XGCValues values;
    //values.graphics_exposures = True;
    GC gc = XCreateGC(displ, win, 0, 0);
    XImage *image = XCreateImage(displ, winattr.visual, winattr.depth, ZPixmap, 0, pixel_array, 640, 426, 32, 0);
    for (int i = 0; i <= 640 * 426; i++) {
        printf("Pixel array entries: %0x- pos %d\n", pixel_array[i], i);
    }

    while (1) {
        while (XPending(displ) > 0) {
            XNextEvent(displ, &event);
            // if (event.xmotion.subwindow == child_win || event.xany.window == child_win) {
            //     printf("Child Window Event.\n");
            //     printf("Event Type: %d\n", event.type);
            // } else if (event.type == ButtonPress && event.xclient.window == button) {
            //     printf("A button was Presed\n");
            // } else if (event.type == ButtonRelease && event.xclient.window == button) {
            //     printf("A button was Released\n");
            if (event.type == ClientMessage) {
                if (event.xclient.data.l[0] == wm_delete_window) {
                    printf("WM_DELETE_WINDOW");
                    XFreeGC(displ, gc);
                    XCloseDisplay(displ);
                    return 0;
                }
            } else if (event.type == Expose && event.xclient.window == win) {
                printf("Expose Event occured.\n");
            } else {
                printf("Main Window Event.\n");
                printf("Event Type: %d\n", event.type);
                /* Draw some lines to experiment */
                //XDrawLine(displ, child_win, DefaultGC(displ, screen), rand() % 180, rand() % 80, rand() % 60, rand() % 60);
            }
            XPutImage(displ, win, gc, image, 0, 0, 0, 0, 640, 426);
            XSync(displ, False);
        }
    }
    free(pixel_array);
    //free(pixel_arr);
    return 0;
}

Window cr_button(Display *displ, Window win, int screen) {

    int x, y, width, height;
    Window button;

    Colormap colormap;
    XColor button_color, lightgray_color, darkgray_color;
    XGCValues gcv_lightgray, gcv_darkgray;
    GC gc_lightgray, gc_darkgray;
    
    button = XCreateSimpleWindow(displ, win, 622, 352, 50, 20, 4, 0x000fff, 0xffffff);
    XSelectInput(displ, button, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);
    XMapWindow(displ, button);

    colormap = DefaultColormap(displ, screen);
    XParseColor(displ, colormap, "rbg:cc/cc/cc", &button_color);
    XAllocColor(displ, colormap, &button_color);

    XParseColor(displ, colormap, "rbg:ee/ee/ee", &lightgray_color);
    XAllocColor(displ, colormap, &lightgray_color);
    gcv_lightgray.foreground = lightgray_color.pixel;
    gcv_lightgray.background = button_color.pixel;
    gc_lightgray = XCreateGC(displ, RootWindow(displ, screen), GCForeground | GCBackground, &gcv_lightgray);

    XParseColor(displ, colormap, "rbg:88/88/88", &darkgray_color);
    XAllocColor(displ, colormap, &darkgray_color);
    gcv_darkgray.foreground = darkgray_color.pixel;
    gcv_darkgray.background = button_color.pixel;
    gc_darkgray = XCreateGC(displ, RootWindow(displ, screen), GCForeground | GCBackground, &gcv_darkgray);

    return button;
}

