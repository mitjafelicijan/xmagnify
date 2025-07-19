#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>
#include <X11/keysym.h>

#define DEFAULT_ZOOM_LEVEL 2
#define DEFAULT_WINDOW_SIZE 600

Display *display;
Window zoom_window;
int screen;
int screen_width, screen_height;
int running = 1;
int zoom_level = DEFAULT_ZOOM_LEVEL;
int window_size = DEFAULT_WINDOW_SIZE;

void print_usage(const char *program_name) {
	printf("Usage: %s [OPTIONS]\n", program_name);
	printf("Options:\n");
	printf("  -z, --zoom LEVEL    Zoom level (default: %d)\n", DEFAULT_ZOOM_LEVEL);
	printf("  -s, --size SIZE     Window size in pixels (default: %d)\n", DEFAULT_WINDOW_SIZE);
	printf("  -h, --help          Show this help message\n");
	printf("  -q, --quit          Quit the application\n");
}

void parse_arguments(int argc, char *argv[]) {
	int opt;
	const char *short_options = "z:s:hq";
	struct option long_options[] = {
		{"zoom", required_argument, 0, 'z'},
		{"size", required_argument, 0, 's'},
		{"help", no_argument, 0, 'h'},
		{"quit", no_argument, 0, 'q'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch (opt) {
			case 'z':
				zoom_level = atoi(optarg);
				if (zoom_level <= 0) {
					fprintf(stderr, "Error: Zoom level must be positive\n");
					exit(1);
				}
				break;
			case 's':
				window_size = atoi(optarg);
				if (window_size <= 0) {
					fprintf(stderr, "Error: Window size must be positive\n");
					exit(1);
				}
				break;
			case 'h':
				print_usage(argv[0]);
				exit(0);
			case 'q':
				exit(0);
			default:
				print_usage(argv[0]);
				exit(1);
		}
	}
}

void init_x11() {
	display = XOpenDisplay(NULL);
	if (!display) {
		fprintf(stderr, "Cannot open X display\n");
		exit(1);
	}
	screen = DefaultScreen(display);
	screen_width = DisplayWidth(display, screen);
	screen_height = DisplayHeight(display, screen);
}

void create_zoom_window() {
	zoom_window = XCreateSimpleWindow(
		display,
		RootWindow(display, screen),
		0, 0,
		window_size, window_size,
		1,
		BlackPixel(display, screen),
		WhitePixel(display, screen)
	);
	XStoreName(display, zoom_window, "Xmagnify");
	XSelectInput(display, zoom_window, KeyPressMask);
	XMapWindow(display, zoom_window);
}

void clamp_coordinates(int *x, int *y, int width, int height) {
	*x = (*x < 0) ? 0 : *x;
	*y = (*y < 0) ? 0 : *y;
	*x = (*x > screen_width - width) ? screen_width - width : *x;
	*y = (*y > screen_height - height) ? screen_height - height : *y;
}

void handle_keypress(XEvent *event) {
	KeySym keysym = XLookupKeysym(&event->xkey, 0);
	if (keysym == XK_Escape || keysym == XK_q || keysym == XK_Q) {
		running = 0;
	}
}

void update_zoom() {
	XFixesCursorImage *cursor = XFixesGetCursorImage(display);
	if (!cursor) {
		fprintf(stderr, "Failed to get cursor position\n");
		return;
	}

	int capture_width = window_size / zoom_level;
	int capture_height = window_size / zoom_level;
	int capture_x = cursor->x - capture_width/2;
	int capture_y = cursor->y - capture_height/2;

	clamp_coordinates(&capture_x, &capture_y, capture_width, capture_height);

	XImage *src_image = XGetImage(
		display,
		RootWindow(display, screen),
		capture_x, capture_y,
		capture_width, capture_height,
		AllPlanes,
		ZPixmap
	);

	if (!src_image) {
		fprintf(stderr, "XGetImage failed\n");
		XFree(cursor);
		return;
	}

	XImage *dest_image = XCreateImage(
		display,
		DefaultVisual(display, screen),
		DefaultDepth(display, screen),
		ZPixmap,
		0,
		malloc(window_size * window_size * 4),
		window_size, window_size,
		32,
		0
	);

	for (int y = 0; y < window_size; y++) {
		for (int x = 0; x < window_size; x++) {
			int src_x = x / zoom_level;
			int src_y = y / zoom_level;
			XPutPixel(dest_image, x, y, XGetPixel(src_image, src_x, src_y));
		}
	}

	GC gc = XCreateGC(display, zoom_window, 0, NULL);
	XPutImage(display, zoom_window, gc, dest_image, 0, 0, 0, 0, window_size, window_size);

	XFreeGC(display, gc);
	XDestroyImage(src_image);
	free(dest_image->data);
	dest_image->data = NULL;
	XDestroyImage(dest_image);
	XFree(cursor);
}

int main(int argc, char *argv[]) {
	parse_arguments(argc, argv);

	init_x11();

	int event_base, error_base;
	if (!XFixesQueryExtension(display, &event_base, &error_base)) {
		fprintf(stderr, "XFixes not available\n");
		XCloseDisplay(display);
		return 1;
	}

	create_zoom_window();

	while (running) {
		while (XPending(display)) {
			XEvent event;
			XNextEvent(display, &event);
			if (event.type == KeyPress) {
				handle_keypress(&event);
			}
		}

		update_zoom();
		XFlush(display);
		usleep(16000);
	}

	XCloseDisplay(display);
	return 0;
}
