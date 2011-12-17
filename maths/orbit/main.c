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
 */


#include <stdio.h>
#include <stdlib.h>


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


static void points_to_a
(
 double* a,
 const double* points,
 int npoints
)
{
  const int n = npoints + 1;

  int i;

  /* a_0i = { x^2, y^2, xy, x, y} = 1 */
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

  free(a);
  free(x);

  return 0;
}
