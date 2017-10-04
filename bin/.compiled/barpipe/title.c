// this whole file is pretty much stolen from https://github.com/baskerville/xtitle
// just changed around a bit to fit into my program
#include <err.h>
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <wchar.h>
#include <xcb/xcb.h>
#include <xcb/xcb_event.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>


#include <confuse.h>

#include "common.h"
#include "config.h"


#include "title.h"

xcb_connection_t *dpy;
xcb_ewmh_connection_t *ewmh;
int default_screen;
xcb_window_t root;
bool visible;

static void setup(void)
{
	dpy = xcb_connect(NULL, &default_screen);
	if (xcb_connection_has_error(dpy)) {
		errx(EXIT_FAILURE, "can't open display.");
	}
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;
	if (screen == NULL) {
		errx(EXIT_FAILURE, "can't acquire screen.");
	}
	root = screen->root;
	ewmh = malloc(sizeof(xcb_ewmh_connection_t));
	if (xcb_ewmh_init_atoms_replies(ewmh, xcb_ewmh_init_atoms(dpy, ewmh), NULL) == 0) {
		errx(EXIT_FAILURE, "can't initialize EWMH atoms.");
	}
}


static void watch(xcb_window_t win, bool state)
{
	if (win == XCB_NONE)
		return;
	uint32_t value = (state ? XCB_EVENT_MASK_PROPERTY_CHANGE : XCB_EVENT_MASK_NO_EVENT);
	xcb_change_window_attributes(dpy, win, XCB_CW_EVENT_MASK, &value);
}

static void get_window_title(xcb_window_t win, wchar_t *title, size_t len) {
	xcb_ewmh_get_utf8_strings_reply_t ewmh_txt_prop;
	xcb_icccm_get_text_property_reply_t icccm_txt_prop;
	ewmh_txt_prop.strings = icccm_txt_prop.name = NULL;
	title[0] = L'\0';
	if (win != XCB_NONE && ((visible && xcb_ewmh_get_wm_visible_name_reply(ewmh, xcb_ewmh_get_wm_visible_name(ewmh, win), &ewmh_txt_prop, NULL) == 1) || xcb_ewmh_get_wm_name_reply(ewmh, xcb_ewmh_get_wm_name(ewmh, win), &ewmh_txt_prop, NULL) == 1 || xcb_icccm_get_wm_name_reply(dpy, xcb_icccm_get_wm_name(dpy, win), &icccm_txt_prop, NULL) == 1)) {
		char *src = NULL;
		size_t title_len = 0;
		if (ewmh_txt_prop.strings != NULL) {
			src = ewmh_txt_prop.strings;
			title_len = MIN(len, ewmh_txt_prop.strings_len);
		} else if (icccm_txt_prop.name != NULL) {
			src = icccm_txt_prop.name;
			title_len = MIN(len, icccm_txt_prop.name_len);
		}
		if (src != NULL) {
			title_len = mbsnrtowcs(title, (const char**)&src, title_len, SHARED_STRING_LEN, NULL);
			if (title_len == (size_t)-1) {
				warnx("can't decode the title of 0x%08lX.", (unsigned long)win);
				title_len = 0;
			}
			title[title_len] = L'\0';
		}
	}
	if (ewmh_txt_prop.strings != NULL)
		xcb_ewmh_get_utf8_strings_reply_wipe(&ewmh_txt_prop);
	if (icccm_txt_prop.name != NULL)
		xcb_icccm_get_text_property_reply_wipe(&icccm_txt_prop);
}

static void output_title(char* string, xcb_window_t win, wchar_t *title, size_t len)
{
	get_window_title(win, title, len);
	wchar_t wcharbuf[SHARED_STRING_LEN];

	swprintf(wcharbuf, len, TITLEFORMAT, title);

	wcstombs(string, wcharbuf, len);
}

static bool get_active_window(xcb_window_t *win)
{
	return (xcb_ewmh_get_active_window_reply(ewmh, xcb_ewmh_get_active_window(ewmh, default_screen), win, NULL) == 1);
}

static bool title_changed(xcb_generic_event_t *evt, xcb_window_t *win, xcb_window_t *last_win)
{
	if (XCB_EVENT_RESPONSE_TYPE(evt) == XCB_PROPERTY_NOTIFY) {
		xcb_property_notify_event_t *pne = (xcb_property_notify_event_t *) evt;
		if (pne->atom == ewmh->_NET_ACTIVE_WINDOW) {
			watch(*last_win, false);
			if (get_active_window(win)) {
				watch(*win, true);
				*last_win = *win;
			} else {
				*win = *last_win = XCB_NONE;
			}
			return true;
		} else if (*win != XCB_NONE && pne->window == *win && ((visible && pne->atom == ewmh->_NET_WM_VISIBLE_NAME) || pne->atom == ewmh->_NET_WM_NAME || pne->atom == XCB_ATOM_WM_NAME)) {
			return true;
		}
	}
	return false;
}

pid_t titleworker(char *string, BOOL *running)
{


	pid_t pid = fork();

	switch (pid) {
		case -1:
			errx(1, "could not fork worker proc");

		case 0:
			dpy = NULL;
			ewmh = NULL;
			visible = false;

			setlocale(LC_ALL, "");
			setup();
			wchar_t title[SHARED_STRING_LEN] = {0};
			xcb_window_t win = XCB_NONE;

			watch(root, true);

			watch(win, true);
			xcb_window_t last_win = XCB_NONE;
			fd_set descriptors;
			int fd = xcb_get_file_descriptor(dpy);
			xcb_flush(dpy);

			if (get_active_window(&win))
				output_title(string, win, title, SHARED_STRING_LEN);

			while (*running) {
				FD_ZERO(&descriptors);
				FD_SET(fd, &descriptors);
				if (select(fd + 1, &descriptors, NULL, NULL, NULL) > 0) {
					xcb_generic_event_t *evt;
					while ((evt = xcb_poll_for_event(dpy)) != NULL) {
						if (title_changed(evt, &win, &last_win))
							output_title(string, win, title, SHARED_STRING_LEN);
						free(evt);
					}
				}
				if (xcb_connection_has_error(dpy)) {
					errx(1,"the server closed the connection.");
				}
			}

			xcb_ewmh_connection_wipe(ewmh);
			free(ewmh);
			xcb_disconnect(dpy);
			exit(EXIT_SUCCESS);
		
		default:
			warnx("Worker proc %d forked", pid);
			return pid;
	}
	return -1;
}


