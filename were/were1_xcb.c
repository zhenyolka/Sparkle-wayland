#include "were1_xcb.h"
#include "were1_tmpfile.h"
#include <stdlib.h>
#include <unistd.h>

#define SHM


struct were1_xcb_display *were1_xcb_display_open()
{
    uint32_t mask = 0;
    uint32_t values[2];

    struct were1_xcb_display *display = malloc(sizeof(struct were1_xcb_display));

    display->connection = xcb_connect(NULL, NULL);

    display->screen = xcb_setup_roots_iterator(xcb_get_setup(display->connection)).data;

    xcb_drawable_t window = display->screen->root;
    display->gc = xcb_generate_id(display->connection);
    mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    values[0] = display->screen->black_pixel;
    values[1] = 0;
    xcb_create_gc(display->connection, display->gc, window, mask, values);

    return display;
}

void were1_xcb_display_close(struct were1_xcb_display *display)
{
    xcb_free_gc(display->connection, display->gc);
    xcb_disconnect(display->connection);
}

void were1_xcb_display_get_events(struct were1_xcb_display *display, void (*handler)(xcb_generic_event_t *event, void *user), void *user)
{
    xcb_generic_event_t *event;

    while ((event = xcb_poll_for_event(display->connection)))
    {
        handler(event, user);
        free(event);
    }
}

int were1_xcb_display_fd(struct were1_xcb_display *display)
{
    return xcb_get_file_descriptor(display->connection);
}

struct were1_xcb_window *were1_xcb_window_create(struct were1_xcb_display *display, int width, int height)
{
    uint32_t mask = 0;
    uint32_t values[2];

    struct were1_xcb_window *window = malloc(sizeof(struct were1_xcb_window));

    window->display = display;
    window->width = width;
    window->height = height;

    window->window = xcb_generate_id(display->connection);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = display->screen->black_pixel;
    values[1] =     XCB_EVENT_MASK_EXPOSURE |
                    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
                    XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
                    XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
    xcb_create_window(display->connection,              /* Connection          */
                     XCB_COPY_FROM_PARENT,              /* depth               */
                     window->window,                    /* window Id           */
                     display->screen->root,             /* parent window       */
                     0, 0,                              /* x, y                */
                     width, height,                     /* width, height       */
                     0,                                 /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT,     /* class               */
                     display->screen->root_visual,      /* visual              */
                     mask, values);                     /* masks */
    xcb_map_window(display->connection, window->window);

    xcb_flush(display->connection);

    //xcb_shm_query_version_reply_t *reply;
    //reply = xcb_shm_query_version_reply(display->connection, xcb_shm_query_version(display->connection), NULL);

    //if(!reply || !reply->shared_pixmaps)
    //    return -1;

#ifdef SHM
    window->fd = were1_tmpfile_create(width * height * 4);
    were1_tmpfile_map(&window->data, width * height * 4, window->fd);

    window->shmseg = xcb_generate_id(display->connection);
    xcb_shm_attach_fd(display->connection, window->shmseg, window->fd, 0);
#else
    window->data = NULL;
#endif

    return window;
}

void were1_xcb_window_destroy(struct were1_xcb_window *window)
{
#ifdef SHM
    xcb_shm_detach(window->display->connection, window->shmseg);
    were1_tmpfile_unmap(&window->data, window->width * window->height * 4);
    close(window->fd);
#endif
    xcb_unmap_window(window->display->connection, window->window);
    xcb_destroy_window(window->display->connection, window->window);
    xcb_flush(window->display->connection);
}

void were1_xcb_window_commit(struct were1_xcb_window *window)
{
#ifdef SHM
    xcb_void_cookie_t cookie = xcb_shm_put_image_checked(window->display->connection, window->window, window->display->gc, window->width, window->height, 0, 0, window->width, window->height, 0, 0, window->display->screen->root_depth, XCB_IMAGE_FORMAT_Z_PIXMAP, 0, window->shmseg, 0);
    xcb_request_check(window->display->connection, cookie);
#else
#warning "no shm"
    xcb_void_cookie_t cookie = xcb_put_image_checked(window->display->connection, XCB_IMAGE_FORMAT_Z_PIXMAP, window->window, window->display->gc, window->width, window->height, 0, 0, 0, window->display->screen->root_depth, window->width * window->height * 4, window->data);
    xcb_request_check(window->display->connection, cookie);
#endif
    //xcb_flush(window->display->connection);
}
