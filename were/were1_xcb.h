#ifndef WERE1_XCB_H
#define WERE1_XCB_H

#include <xcb/xcb.h>
#include <xcb/shm.h>

// XXX2 Checks

struct were1_xcb_display
{
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_gcontext_t gc;
};

struct were1_xcb_window
{
    struct were1_xcb_display *display;
    int width;
    int height;
    xcb_drawable_t window;
    int fd;
    void *data;
    xcb_shm_seg_t shmseg;
};

#ifdef  __cplusplus
extern "C" {
#endif

struct were1_xcb_display *were1_xcb_display_open();
void were1_xcb_display_close(struct were1_xcb_display *display);
void were1_xcb_display_get_events(struct were1_xcb_display *display, void (*handler)(xcb_generic_event_t *event, void *user), void *user);
int were1_xcb_display_fd(struct were1_xcb_display *display);

struct were1_xcb_window *were1_xcb_window_create(struct were1_xcb_display *display, int width, int height);
void were1_xcb_window_destroy(struct were1_xcb_window *window);
void were1_xcb_window_commit(struct were1_xcb_window *window);
void were1_xcb_window_commit_with_damage(struct were1_xcb_window *window, int x1, int y1, int x2, int y2);
void were1_xcb_window_set_size(struct were1_xcb_window *window, int width, int height);

#ifdef  __cplusplus
}
#endif

#endif // WERE1_XCB_H
