/* orbit measurement from body obvervations

   from the book:
   elmentary linear algebra with applications
   howard anton, chris rorres
   section 11.1

   an homogenous system A has non trivial solutions iif det(A) = 0
   
   det(A) cofactor expansion along a row i:
   http://tutorial.math.lamar.edu/Classes/LinAlg/MethodOfCofactors.aspx
   det(A) = sum (aij * Cij)
   where Cij the cofactor of Aij
   with Cij = (-1)^(i + j) * Mij
   where Mij the minor of Aij, ie. the determinant of the matrix A prived of
   row i and col j.
   Thus, given row 1 is chosen, the recursive relation is:
   det(A) = sum(-1^j * a1j * det(M1j))

   ellipse explicit form:
   http://gilles.dubois10.free.fr/analyse_reelle/contimplicite.html
   x^2 / a^2 + y^2 / b^2 - 1 = 0
   
   from ellipses.pdf:
   an eq. of the form:
   ax^2 + 2bxy + cy^2 + 2dx + 2ey + f = 0
   defines an ellipses iif:
   ac - b^2 > 0 and
   (bd - ae)^2 - (d^2 - af)(b^2 - ac) > 0

   ephemeris references:
   http://www.cv.nrao.edu/~rfisher/Ephemerides/ephem_use.html
   http://www.cv.nrao.edu/~rfisher/Ephemerides/ephem_descr.html
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "x.h"


static const char* dtos(double d)
{
  static char buf[32];
  sprintf(buf, "%lf", d);
  return buf;
}

__attribute__((unused))
static void print
(
 const double* a,
 const int* cols,
 int lda,
 int n
)
{
  int i;
  int j;

  printf("--\n");

  for (i = 0; i < n; ++i)
  {
    const int k = i * lda;
    for (j = 0; j < n; ++j)
      printf(" %10s", dtos(a[k + ((cols == NULL) ? j : cols[j])]));
    printf(";\n");
  }

  printf("--\n");
}


static double det
(
 const int* cols,
 const double* a,
 int lda,
 int n
)
{
  /* compute A determinant
     a the original nxn square matrix
     the current matrix is mxm with
     a_ij is a[i * n + cols[j]]
   */

  static const double signs[] = { 1, -1 };

  int* new_cols;
  double sum;
  int i;
  int j;

/*   print(a, cols, lda, n); */
/*   getchar(); */

  if (n == 2)
  {
    return
      a[0 * lda + cols[0]] * a[1 * lda + cols[1]] -
      a[0 * lda + cols[1]] * a[1 * lda + cols[0]];
  }

  new_cols = malloc((n - 1) * sizeof(int));

  sum = 0;
  for (i = 0; i < n; ++i)
  {
    for (j = 0; j < i; ++j) new_cols[j] = cols[j];
    for (; j < n - 1; ++j) new_cols[j] = cols[j + 1];
    sum += signs[i & 1] * a[cols[i]] * det(new_cols, a + lda, lda, n - 1);
  }

  free(new_cols);

  return sum;
}


static void expand_cofactor
(
 double* x,
 const double* a,
 int n
)
{
  /* x the unknown coefficients
     a the coefficient matrix
     n the system dimension
   */

  static const double signs[] = { 1, -1 };

  int i;
  int j;

  int* cols;

#if 0
  print(a, NULL, n, n);
  getchar();
#endif

  cols = malloc((n - 1) * sizeof(int));
  
  for (i = 0; i < n; ++i)
  {
    /* remove col i */
    for (j = 0; j < i; ++j) cols[j] = j;
    for (; j < n - 1; ++j) cols[j] = j + 1;

    /* x_i = -1^(1 + i) * M_ij; */
    x[i] = signs[i & 1] * det(cols, a + n, n, n - 1);
  }

  free(cols);
}


static int get_ellipse_from_quadratic
(
 const double* p,
 double* x,
 double* y,
 double* major,
 double* minor,
 double* alpha
)
{
  /* http://mathworld.wolfram.com/Ellipse.html
     p the quadratic polynomial coefficients
     x, y  the ellipse center
     major, minor the axis lenghts
     alpha the ellipse rotation
     return -1 if the this is not an ellipse
   */

  const double a = p[0];
  const double b = p[1] / 2;
  const double c = p[2];
  const double d = p[3] / 2;
  const double f = p[4] / 2;
  const double g = p[5];

  const double bb = b * b;
  const double dd = d * d;
  const double ff = f * f;
  const double bbac = bb - a * c;

  double m[3 * 3];
  double detm;
  const int cols[3] = { 0, 1, 2 };

  double q;

  /* check it is an ellipse */

  m[0 * 3 + 0] = a;
  m[0 * 3 + 1] = b;
  m[0 * 3 + 2] = d;
  m[1 * 3 + 0] = b;
  m[1 * 3 + 1] = c;
  m[1 * 3 + 2] = f;
  m[2 * 3 + 0] = d;
  m[2 * 3 + 1] = f;
  m[2 * 3 + 2] = g;
  detm = det(cols, m, 3, 3);

  if (detm == 0) return -1;
  if (detm / (a + c) >= 0) return -1;

  m[0 * 2 + 0] = a;
  m[0 * 2 + 1] = b;
  m[1 * 2 + 0] = b;
  m[1 * 2 + 1] = c;
  if (det(cols, m, 2, 2) <= 0) return -1;

  /* center */

  *x = (c * d - b * f) / bbac;
  *y = (a * f - b * d) / bbac;

  /* axis lengths */

  q = a * ff + c * dd + g * bb  - 2 * b * d * f - a * c * g;
  *minor = 2 * sqrt( (2 * q) / ( bbac * ( +1 * sqrt(pow(a - c, 2) + 4 * bb) - (a + c)) ) );
  *major = 2 * sqrt( (2 * q) / ( bbac * ( -1 * sqrt(pow(a - c, 2) + 4 * bb) - (a + c)) ) );

  /* compute alpha */

  if (b == 0)
  {
    if (a < c) *alpha = 0;
    else *alpha = M_PI / 2;
  }
  else
  {
    *alpha = tanh(a - c / (2 * b));
    if (a > c) *alpha += M_PI / 2;
  }

  return 0;
}


static void points_to_a
(
 double* a,
 const double* points,
 int npoints
)
{
  const int n = npoints + 1;

  int i;

  for (i = 0; i < n; ++i) a[i] = 1;

  /* a starts at second line, refer above comment */
  for (i = 0; i < npoints; ++i)
  {
    const double x = points[i * 2 + 0];
    const double y = points[i * 2 + 1];

    a[(i + 1) * n + 0] = x * x;
    a[(i + 1) * n + 1] = x * y;
    a[(i + 1) * n + 2] = y * y;
    a[(i + 1) * n + 3] = x;
    a[(i + 1) * n + 4] = y;
    a[(i + 1) * n + 5] = 1;
  }
}


static inline double rtod(double r)
{
  /* radians to degrees */
  return (r * 180) / M_PI;
}


static const unsigned char red_rgb[] = { 0xff, 0, 0 };
const x_color_t* red_col = NULL;

struct info
{
  double x;
  double y;
  double d0;
  double d1;
  double a;

  const double* points;
  int npoints;
};

static int on_event(const struct x_event* ev, void* p)
{
  switch (x_event_get_type(ev))
  {
  case X_EVENT_KDOWN_SPACE:
    break;

  case X_EVENT_KDOWN_LEFT:
    break;

  case X_EVENT_KDOWN_RIGHT:
    break;

  case X_EVENT_KDOWN_UP:
    break;

  case X_EVENT_KDOWN_DOWN:
    break;

  case X_EVENT_MOUSE_BUTTON:
    {
      static int ibutton = 0;
      static double buttons[5 * 2];
      const int w = x_get_width();
      const int h = x_get_height();
      int posx, posy;
      x_event_get_xy(ev, &posx, &posy);
      buttons[ibutton * 2 + 0] = posx - w / 2;
      buttons[ibutton * 2 + 1] = posy - h / 2;

      if (((++ibutton) % 5) == 0)
      {
	static const int npoints = 5;

	struct info* const i = p;
	ibutton = 0;

	double* a;
	double* x;
	int n;

	n = npoints + 1;
	a = malloc(n * n * sizeof(double));
	x = malloc(n * sizeof(double));
	points_to_a(a, buttons, npoints);
	expand_cofactor(x, a, n);

	double _x, _y;
	double major, minor;
	double alpha;
	if (get_ellipse_from_quadratic(x, &_x, &_y, &major, &minor, &alpha))
	{
	  printf("not an ellipse\n");
	  break ;
	}

	i->x = _x;
	i->y = _y;
	i->d0 = major;
	i->d1 = minor;
	i->a = alpha;
	i->points = buttons;
	i->npoints = npoints;
      }
    }

  case X_EVENT_TICK:
    {
      struct info* const i = p;

      const int w = x_get_width();
      const int h = x_get_height();

      int j;

      x_draw_ellipse
      (
       x_get_screen(),
       w / 2,
       h / 2,
#if 0
       i->x * 10,
       i->y * 10,
       i->d0 * 10,
       i->d1 * 10,
#else
       i->x,
       i->y,
       i->d0,
       i->d1,
#endif
       i->a,
       red_col
      );

      for (j = 0; j < i->npoints; ++j)
      {
#if 0
	const double x = w / 2 + i->points[j * 2 + 0] * 10;
	const double y = h / 2 + i->points[j * 2 + 1] * 10;
#else
	const double x = w / 2 + i->points[j * 2 + 0];
	const double y = h / 2 + i->points[j * 2 + 1];
#endif
	x_draw_circle(x_get_screen(), x, y, 5, red_col);
      }

      x_draw_circle(x_get_screen(), w / 2, h / 2, 5, red_col);
    }
    break;

  case X_EVENT_QUIT:
    x_cleanup();
    exit(-1);
    break;

  default:
    break;
  }

  return 0;
}

static void draw_ellipse
(
 double x,
 double y,
 double d0,
 double d1,
 double a,
 const double* points,
 int npoints
)
{
  struct info i;

  if (d1 > d0)
  {
    const double tmp = d1;
    d1 = d0;
    d0 = tmp;
  }

  x_initialize(CONFIG_TICK_MS);
  x_alloc_color(red_rgb, &red_col);
  i.x = x;
  i.y = y;
  i.d0 = d0;
  i.d1 = d1;
  i.a = a;
  i.points = points;
  i.npoints = npoints;
  x_loop(on_event, &i);
  x_cleanup();
}


int main(int ac, char** av)
{
  double points[] =
  {
    8.025, 8.310,
    10.170, 6.355,
    11.202, 3.212,
    10.736, 0.375,
    9.092, -2.267
  };

  static const int npoints = 5;

  double* a;
  double* x;
  int n;
  int i;

  n = npoints + 1;
  a = malloc(n * n * sizeof(double));
  x = malloc(n * sizeof(double));

  points_to_a(a, points, npoints);

#if 0
  {
    a[0 + 0] = -1;
    a[0 + 1] = 3;
    a[0 + 2] = 9;

    a[1 * 3 + 0] = 5;
    a[1 * 3 + 1] = -1;
    a[1 * 3 + 2] = 6;

    a[2 * 3 + 0] = 3;
    a[2 * 3 + 1] = 1;
    a[2 * 3 + 2] = -2;

    int n = 3;

    int* cols = malloc(n * sizeof(int));
    for (i = 0; i < n; ++i) cols[i] = i;
    print(a, cols, n, n);
    printf("det: %lf\n", det(cols, a, n, n));
    free(cols);
  }
#endif

  expand_cofactor(x, a, n);
  for (i = 0; i < n; ++i)
    printf("%s%lf", i ? " + " : "", x[i]);
  printf("\n");

  {
    double _x, _y;
    double major, minor;
    double alpha;
    if (get_ellipse_from_quadratic(x, &_x, &_y, &major, &minor, &alpha))
      printf("not an ellipse\n");
    else
      printf("(%lf, %lf) (%lf, %lf) %lf\n", _x, _y, major, minor, rtod(alpha));
    draw_ellipse(_x, _y, major, minor, rtod(alpha), points, npoints);
  }

  free(a);
  free(x);

  return 0;
}
