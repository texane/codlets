//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Aug 17 17:06:48 2009 texane
// Last update Mon Oct 11 21:07:44 2010 texane
//



#ifndef GRAPHICS_X_H_INCLUDED
# define GRAPHICS_X_H_INCLUDED


#ifdef _WIN32
# include <memory.h>
# include <SDL.H>
#else // __linux__
# include <SDL/SDL.h>
#endif // _WIN32


#define CONFIG_TICK_MS 100
#define CONFIG_SPACE_SCALE 2


typedef struct x_color x_color_t;
struct x_event;
typedef SDL_Surface x_surface_t;


/* event types
 */

#define X_EVENT_TICK 0
#define X_EVENT_QUIT 1
#define X_EVENT_KUP_SPACE 2
#define X_EVENT_KDOWN_SPACE 3
#define X_EVENT_KDOWN_LEFT 4
#define X_EVENT_KDOWN_RIGHT 5
#define X_EVENT_KDOWN_UP 6
#define X_EVENT_KDOWN_DOWN 7


int x_initialize(unsigned int);
void x_cleanup(void);
int x_set_refresh_rate(unsigned int);
void x_loop(int (*)(const struct x_event*, void*), void*);
int x_alloc_color(const unsigned char*, const x_color_t**);
void x_remap_color(const unsigned char*, const x_color_t*);
void x_free_color(const x_color_t*);
void x_draw_pixel(x_surface_t*, int, int, const x_color_t*);
void x_draw_line(x_surface_t*, int, int, int, int, const x_color_t*);
void x_draw_hline(x_surface_t*, int, int, int, const x_color_t*);
void x_draw_square(x_surface_t*, int, int, int, const x_color_t*);
void x_draw_circle(x_surface_t*, int, int, int, const x_color_t*);
void x_draw_disk(x_surface_t*, int, int, int, const x_color_t*);
void x_draw_ellipse(x_surface_t*, int, int, int, int, const x_color_t*);
int x_event_get_type(const struct x_event*);
int x_get_width(void);
int x_get_height(void);
const x_color_t* x_get_transparency_color(void);
x_surface_t* x_create_surface(int, int);
x_surface_t* x_get_screen(void);
void x_free_surface(x_surface_t*);
void x_blit_surface(x_surface_t*, x_surface_t*);
void x_blit_surface_at(x_surface_t*, x_surface_t*, int, int);
void x_fill_surface(x_surface_t*, const x_color_t*);


#endif /* ! GRAPHICS_X_H_INCLUDED */
