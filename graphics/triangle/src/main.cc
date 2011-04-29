//
// Made by fabien le mentec <texane@gmail.com>
// 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.hh"
#include "x.hh"


static const x_color_t* red_color = NULL;

static void init_stuffs(void)
{
  static const unsigned char red_rgb[] = { 0xff, 0, 0 };
  x_alloc_color(red_rgb, &red_color);
}


typedef struct vertex
{
  int x, y;
} vertex_t;

typedef struct triangle
{
  vertex_t dots[3];
} triangle_t;

static inline double radians(double a)
{
  return (a * M_PI) / 180;
}

static inline triangle_t triangle_make
(unsigned int x, unsigned int y, unsigned int r)
{
  // make an equilateral triangle or radius r

  triangle_t t;

  // start from top, clockwise

  t.dots[0].x = x + r * cos(radians(90));
  t.dots[0].y = y + r * sin(radians(90));

  t.dots[1].x = x + r * cos(radians(210));
  t.dots[1].y = y + r * sin(radians(210));

  t.dots[2].x = x + r * cos(radians(330));
  t.dots[2].y = y + r * sin(radians(330));

  return t;
}


static inline void draw_hline
(unsigned int x0, unsigned int x1, unsigned int y, const x_color_t* c)
{
  x_draw_line(x0, y, x1, y, c);
}

static void fill_top
(triangle_t* t, const unsigned int* sorted, const x_color_t* c)
{
  int scany;
  double tx, ty;
  double ly, ry;
  double tmp;
  double lx, rx;
  double la, ra;

  /* top, left, right points coords */
  tx = (double)t->dots[sorted[0]].x;
  ty = (double)t->dots[sorted[0]].y;
  lx = (double)t->dots[sorted[1]].x;
  ly = (double)t->dots[sorted[1]].y;
  rx = (double)t->dots[sorted[2]].x;
  ry = (double)t->dots[sorted[2]].y;

  if (lx > rx)
  {
    tmp = rx;
    rx = lx;
    lx = tmp;

    tmp = ry;
    ry = ly;
    ly = tmp;
  }

  /* left, right deltas */
  la = (tx - lx) / (ty - ly);
  ra = (tx - rx) / (ty - ry);

  /* scanning coord y */
  if (ly > ry)
  {
    scany = ly;
    rx += ra * (ly - ry);

    /* update triangle */
    t->dots[sorted[0]].x = floor(rx);
    t->dots[sorted[0]].y = scany;
  }
  else
  {
    scany = ry;
    lx += la * (ry - ly);

    /* update triangle */
    t->dots[sorted[0]].x = floor(lx);
    t->dots[sorted[0]].y = scany;
  }

  /* iterate over y */
  for (; scany <= (int)ty; ++scany)
  {
    draw_hline((int)ceil(lx), (int)ceil(rx), scany, c);
    lx += la;
    rx += ra;
  }
}

static void fill_down
(const triangle_t* t, const unsigned int* sorted, const x_color_t* c)
{
  double dx, dy;
  double lx, rx;
  double ly, ry;
  double la, ra;
  double tmp;
  int scany;

  /* down, left, right points coords */
  dx = (double)t->dots[sorted[2]].x;
  dy = (double)t->dots[sorted[2]].y;
  lx = (double)t->dots[sorted[1]].x;
  ly = (double)t->dots[sorted[1]].y;
  rx = (double)t->dots[sorted[0]].x;
  ry = (double)t->dots[sorted[0]].y;

  if (lx > rx)
  {
    tmp = rx;
    rx = lx;
    lx = tmp;

    tmp = ry;
    ry = ly;
    ly = tmp;
  }

  /* left, right deltas */
  la = (lx - dx) / (ly - dy);
  ra = (rx - dx) / (ry - dy);

  /* scanning coord y */
  scany = (int)ly;

  /* iterate over y */
  for (; scany >= (int)dy; --scany)
  {
    draw_hline((int)ceil(lx), (int)ceil(rx), scany, c);
    lx -= la;
    rx -= ra;
  }
}

static void sort_vertices(const triangle_t* t, unsigned int* sorted)
{
  /* sort triangle vertices by y */

  if (t->dots[0].y < t->dots[1].y) /* 0 < 1 */
  {
    sorted[2] = 0;

    if (t->dots[1].y > t->dots[2].y) /* 1 > (0, 2) */
    {
      sorted[0] = 1;
      sorted[1] = 2;

      if (t->dots[0].y > t->dots[2].y) /* 1 > 0 > 2 */
      {
	sorted[1] = 0;
	sorted[2] = 2;
      }
    }
    else /* 2 > 1 > 0 */
    {
      sorted[0] = 2;
      sorted[1] = 1;
    }
  }
  else if (t->dots[0].y < t->dots[2].y) /* 0 < (1,2) */
  {
    sorted[2] = 0;
    sorted[0] = 1;
    sorted[1] = 2;

    if (t->dots[2].y > t->dots[1].y)
    {
      sorted[0] = 2;
      sorted[1] = 1;
    }
  }
  else /* 0 > (1,2) */
  {
    sorted[0] = 0;
    sorted[1] = 2;
    sorted[2] = 1;

    if (t->dots[1].y > t->dots[2].y)
    {
      sorted[1] = 1;
      sorted[2] = 2;
    }
  }
}

static void triangle_fill(triangle_t* t, const x_color_t* c)
{
  unsigned int sorted[3];

  /* top vertex index */
  sort_vertices(t, sorted);

  if (t->dots[sorted[0]].y != t->dots[sorted[1]].y)
    fill_top(t, sorted, c);
  fill_down(t, sorted, c);
}

static void triangle_wire(const triangle_t* t, const x_color_t* c)
{
  x_draw_line(t->dots[0].x, t->dots[0].y, t->dots[1].x, t->dots[1].y, c);
  x_draw_line(t->dots[1].x, t->dots[1].y, t->dots[2].x, t->dots[2].y, c);
  x_draw_line(t->dots[2].x, t->dots[2].y, t->dots[0].x, t->dots[0].y, c);
}

static void redraw(void*)
{
  triangle_t fu;

  fu = triangle_make(100, 100, 40);
  triangle_wire(&fu, red_color);
  triangle_fill(&fu, red_color);

  fu = triangle_make(200, 100, 40);
  fu.dots[0].y -= 100;
  triangle_wire(&fu, red_color);
  triangle_fill(&fu, red_color);

  fu = triangle_make(300, 100, 20);
  fu.dots[0].x -= 40;
  triangle_wire(&fu, red_color);
  triangle_fill(&fu, red_color);

  fu = triangle_make(400, 100, 20);
  fu.dots[0].x -= 20;
  fu.dots[0].y += 40;
  fu.dots[2].y -= 40;
  triangle_wire(&fu, red_color);
  triangle_fill(&fu, red_color);

  fu = triangle_make(400, 300, 20);
  fu.dots[0].x -= 40;
  fu.dots[0].y += 40;
  fu.dots[2].y -= 40;
  triangle_wire(&fu, red_color);
  triangle_fill(&fu, red_color);

  fu = triangle_make(200, 300, 20);
  fu.dots[0].x += 10;
  fu.dots[2].x -= 40;
  fu.dots[2].y -= 40;
  triangle_wire(&fu, red_color);
  triangle_fill(&fu, red_color);
}

// x event handlers

static int on_event(const struct x_event* ev, void* arg)
{
  switch (x_event_get_type(ev))
  {
  case X_EVENT_TICK:
    redraw(arg);
    break;

  case X_EVENT_QUIT:
    x_cleanup();
    ::exit(-1);
    break;

  default:
    break;
  }

  return 0;
}


// main

int main(int ac, char** av)
{
  // trigger every 40ms
  if (x_initialize(CONFIG_TICK_MS) == -1)
    return -1;

  init_stuffs();

  // loop until done
  x_loop(on_event, (void*)NULL);

  return 0;
}
