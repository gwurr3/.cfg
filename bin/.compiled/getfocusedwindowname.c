#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

int main(int argc, char *argv[]) {
	Display *display;
	Window focus;
	int revert;
	display = XOpenDisplay(NULL);
	XGetInputFocus(display, &focus, &revert);

	// XGetInputFocus is weird. see https://tronche.com/gui/x/xlib/input/XGetInputFocus.html
	if ( focus == PointerRoot || focus == None ) { // is there even a focused window?
		printf("<   >");
		return EXIT_SUCCESS;
	} else {
		char *window_name = NULL;
		XFetchName(display, focus, &window_name);

		if (window_name) {
			printf("< %s >", window_name);
			return EXIT_SUCCESS;
			// If there isn't a simple WM_NAME, we have to do a whole lot of shit:
		} else {
			Atom prop_type, prop_req_type, da;
			int di;
			int status;
			unsigned char *prop_ret = NULL;
			unsigned long dl;

			prop_type = XInternAtom(display, "_NET_WM_NAME", False);
			prop_req_type = XInternAtom(display ,"UTF8_STRING",False);

			status = XGetWindowProperty(display, focus, prop_type, 0L, sizeof (Atom), 
				False, prop_req_type, &da, &di, &dl, &dl, &prop_ret);

			if (status == Success && prop_ret) {
				printf("< %s >", prop_ret);
				return EXIT_SUCCESS;
			} else {
				printf("< NULL >"); // Still couldn't get a name. Need to say something
				return EXIT_FAILURE;
			}
		}
	}
}
