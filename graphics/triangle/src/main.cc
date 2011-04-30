//
// Made by fabien le mentec <texane@gmail.com>
// 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.hh"
#include "x.hh"
#include "stl.h"


/* helper */

static const x_color_t* red_color = NULL;
static const x_color_t* blu_color = NULL;
static const x_color_t* green_color = NULL;
static const x_color_t* yellow_color = NULL;

static const x_color_t* colors[4];

static void init_stuffs(void)
{
  static const unsigned char red_rgb[] = { 0xff, 0, 0 };
  static const unsigned char blu_rgb[] = { 0, 0, 0xff };
  static const unsigned char green_rgb[] = { 0, 0xff, 0x00 };
  static const unsigned char yellow_rgb[] = { 0xff, 0xff, 0x00 };

  x_alloc_color(red_rgb, &red_color);
  x_alloc_color(blu_rgb, &blu_color);
  x_alloc_color(green_rgb, &green_color);
  x_alloc_color(yellow_rgb, &yellow_color);

  colors[0] = red_color;
  colors[1] = blu_color;
  colors[2] = green_color;
  colors[3] = yellow_color;
}


typedef struct vertex
{
  int x, y;
} vertex_t;

typedef struct triangle
{
  vertex_t dots[3];
} triangle_t;


static void fill_top
(triangle_t* t, const unsigned int* sorted, const x_color_t* c)
{
  int scany;
  double tx, ty;
  double ly, ry;
  double lx, rx;
  double la, ra;

  /* top, left, right points coords */
  tx = (double)t->dots[sorted[0]].x;
  ty = (double)t->dots[sorted[0]].y;
  lx = (double)t->dots[sorted[1]].x;
  ly = (double)t->dots[sorted[1]].y;
  rx = (double)t->dots[sorted[2]].x;
  ry = (double)t->dots[sorted[2]].y;

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
    x_draw_hline((int)ceil(lx), (int)ceil(rx), scany, c);
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
  int scany;

  /* down, left, right points coords */
  dx = (double)t->dots[sorted[2]].x;
  dy = (double)t->dots[sorted[2]].y;
  lx = (double)t->dots[sorted[1]].x;
  ly = (double)t->dots[sorted[1]].y;
  rx = (double)t->dots[sorted[0]].x;
  ry = (double)t->dots[sorted[0]].y;

  /* left, right deltas */
  la = (lx - dx) / (ly - dy);
  ra = (rx - dx) / (ry - dy);

  /* scanning coord y */
  scany = (int)ly;

  /* iterate over y */
  for (; scany >= (int)dy; --scany)
  {
    x_draw_hline((int)ceil(lx), (int)ceil(rx), scany, c);
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

  /* check triangle bounds */
  {
    unsigned int i;
    for (i = 0; i < 3; ++i)
    {
      if (t->dots[i].x < 0 || t->dots[i].x > 500)
      {
	/* printf("x == %d\n", t->dots[i].x); */
	break ;
      }
      else if (t->dots[i].y < 0 || t->dots[i].y > 500)
      {
	/* printf("y == %d\n", t->dots[i].y); */
	break ;
      }
    }

    if (i != 3) return ;
  }

  /* printf("ok: %d %d %d\n", t->dots[0].x, t->dots[1].x, t->dots[2].x); */
  /* printf("ok: %d %d %d\n", t->dots[0].y, t->dots[1].y, t->dots[2].y); */

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


/* 3d routines */

typedef struct vertex3
{
  double x;
  double y;
  double z;
} vertex3_t;

typedef struct triangle3
{
  vertex3_t points[3];
} triangle3_t;


/* camera transform 3d to 2d projection */

typedef struct view
{
  /* camera position */
  double cx;
  double cy;
  double cz;

  /* camera angle */
  double alpha;
  double beta;
  double gamma;

  /* viewer position relative to display */
  double vx;
  double vy;
  double vz;

} view_t;

static inline void view_set_defaults(view_t* view)
{
  view->cx = 0;
  view->cy = 0;
  view->cz = 0;

  view->alpha = 0;
  view->beta = 0;
  view->gamma = 0;

  view->vx = 0;
  view->vy = 0;
  view->vz = 0;
}

static inline void view_set_viewer_depth(view_t* view, double z)
{
  view->vz = z;
}

static inline void view_set_camera_pos
(view_t* view, double x, double y, double z)
{
  view->cx = x;
  view->cy = y;
  view->cz = z;
}

static inline void view_set_camera_angle
(view_t* view, double alpha, double beta, double gamma)
{
  view->alpha = alpha;
  view->beta = beta;
  view->gamma = gamma;
}

static void view_project_triangles
(const view_t* view, const triangle3_t* tri, unsigned int count)
{
  const double cosa = cos(view->alpha);
  const double cosb = cos(view->beta);
  const double cosy = cos(view->gamma);

  const double sina = sin(view->alpha);
  const double sinb = sin(view->beta);
  const double siny = sin(view->gamma);

  triangle_t tri2;
  double fu, bar;
  double diffx, diffy, diffz;
  double dx, dy, dz;
  unsigned int i;

  for (; count; ++tri, --count)
  {
    for (i = 0; i < 3; ++i)
    {
      diffx = tri->points[i].x - view->cx;
      diffy = tri->points[i].y - view->cy;
      diffz = tri->points[i].z - view->cz;

      /* apply view transformation */
      fu = cosb * diffz + sinb * (siny * diffy + cosy * diffx);
      bar = cosy * diffy - siny * diffx;

      dx = cosb * (siny * diffy + cosy * diffx) - sinb * diffz;
      dy = sina * fu + cosa * bar;
      dz = cosa * fu - sina * bar;

      /* printf("%lf %lf %lf\n", dx, dy, dz); */

      /* project object to 2d surface */
      tri2.dots[i].x = (dx - view->vx) * (view->vz / dz);
      tri2.dots[i].y = (dy - view->vy) * (view->vz / dz);
    }

    /* draw the triangle */
    triangle_fill(&tri2, colors[count & 3]);
  }
}


/* redraw the scene */

static inline double radians(double a)
{
  return (a * M_PI) / 180;
}

static inline triangle3_t make_tri3
(unsigned int x, unsigned int y, unsigned int r)
{
  /* make an equilateral triangle or radius r */

  triangle3_t t;

  /* start from top, clockwise */

  t.points[0].x = x + r * cos(radians(90));
  t.points[0].y = y + r * sin(radians(90));
  t.points[0].z = 50;

  t.points[1].x = x + r * cos(radians(210));
  t.points[1].y = y + r * sin(radians(210));
  t.points[1].z = 50;

  t.points[2].x = x + r * cos(radians(330));
  t.points[2].y = y + r * sin(radians(330));
  t.points[2].z = 50;

  return t;
}


static double cam_x = -200;
static double cam_y = -200;
static double cam_z = -200;

static double view_alpha = 0;
static double view_beta = 0;
static double view_gamma = 0;

static void redraw(void*)
{
  view_t view;

  /* generate objects */
#if 0 /* triangles */
  triangle3_t tri[32];
  unsigned int n;

  n = 0;

  tri[n] = make_tri3(100, 100, 40);
  ++n;

  tri[n].points[0].x = 100 + 40 * cos(radians(90));
  tri[n].points[0].y = 50;
  tri[n].points[0].z = 50;

  tri[n].points[1].x = 100 + 40 * cos(radians(210));
  tri[n].points[1].y = 50;
  tri[n].points[1].z = 50;

  tri[n].points[2].x = 100 + 40 * cos(radians(330));
  tri[n].points[2].y = 50;
  tri[n].points[2].z = 51;
  ++n;

#if 0
  tri[n] = make_tri3(200, 100, 40);
  tri[n].points[0].y -= 100;

  ++n;

  tri[n] = make_tri3(300, 100, 20);
  tri[n].points[0].x -= 40;

  ++n;

  tri[n] = make_tri3(400, 100, 20);
  tri[n].points[0].x -= 20;
  tri[n].points[0].y += 40;
  tri[n].points[2].y -= 40;

  ++n;

  tri[n] = make_tri3(400, 300, 20);
  tri[n].points[0].x -= 40;
  tri[n].points[0].y += 40;
  tri[n].points[2].y -= 40;

  ++n;
#endif /* 0 */
#else /* read from stl file */
  static triangle3_t* tri = NULL;
  static unsigned int n = 0;

  if (tri == NULL)
  {
    stl_list_t list;
    if (stl_read_file("../../stl/data/stl/2_8.stl", &list) == 0)
    {
      /* convert to triangle3_t array */
      stl_list_elem_t* pos;
      unsigned int i;

      tri = (triangle3_t*)malloc(list.count * sizeof(triangle3_t));

      n = 0;
      for (pos = list.head; pos != NULL; pos = pos->next, ++n)
      {
	for (i = 0; i < 3; ++i)
	{
	  tri[n].points[i].x = pos->vertices[i * 3 + 0];
	  tri[n].points[i].y = pos->vertices[i * 3 + 1];
	  tri[n].points[i].z = pos->vertices[i * 3 + 2];
	}
      }

      stl_free_list(&list);
    }

    printf("read %u\n", n);
  }
#endif /* stl file */

  /* project to screen */
  view_set_defaults(&view);
  view_set_camera_pos(&view, cam_x, cam_y, cam_z);
  view_set_viewer_depth(&view, 200);
  view_set_camera_angle(&view, view_alpha, view_beta, view_gamma);
  view_project_triangles(&view, tri, n);
}

// x event handlers

static int on_event(const struct x_event* ev, void* arg)
{
  switch (x_event_get_type(ev))
  {
  case X_EVENT_KDOWN_SPACE:
    /* view_alpha += radians(3); */
    /* view_beta += radians(3); */
    view_gamma += radians(3);
    break;

  case X_EVENT_KDOWN_LEFT:
    cam_x -= 10;
    break;

  case X_EVENT_KDOWN_RIGHT:
    cam_x += 10;
    break;

  case X_EVENT_KDOWN_UP:
    cam_z += 1;
/*     cam_y += 1;  */
    break;

  case X_EVENT_KDOWN_DOWN:
    cam_z -= 1;
/*     cam_y -= 1;  */
    break;

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
