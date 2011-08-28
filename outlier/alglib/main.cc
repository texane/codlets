#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "linalg.h"
#include "statistics.h"

typedef struct CvPoint
{
  int x, y;
} CvPoint;

static void gen_linear(std::vector<CvPoint>& points)
{
  static const unsigned int npoints = 100;

  // y = mx + p
  static const double m = 2;
  static const double p = 10;

  for (unsigned int i = 0; i < npoints; ++i)
  {
    const unsigned int x = i * 4;

    CvPoint point;
    point.x = x - 3 + (rand() % 6);
    point.y = m * (double)x + p - 3 + (rand() % 6);
    points.push_back(point);
  }
}

static void gen_outlier(std::vector<CvPoint>& points)
{
  // shuffle
  std::vector<unsigned int> map;
  map.resize(points.size());
  for (unsigned int i = 0; i < points.size(); ++i)
    map[i] = i;
  for (unsigned int i = 0; i < points.size(); ++i)
  {
    const unsigned int j = rand() % points.size();
    std::swap(map[i], map[j]);
  }

  const unsigned int count = points.size() / 10;
  for (unsigned int i = 0; i < count; ++i)
    points[map[i]].y = 0;
}


// mahalanobis distance

static int mahalanobis
(const double* a, unsigned int m, unsigned int n, double* dists)
{
  // compute the mahalanobis distance for the given dataset
  // a the dataset matrix
  // m observation count (a row count)
  // n the dimension count (a col count)
  // dists the resulting mahalanobis distances

  // compute the mean vector, foreach dimension n

  alglib::real_1d_array mu;
  mu.setlength(n);

  for (unsigned int i = 0; i < n; ++i)
  {
    // todo: move outside the loop
    alglib::real_1d_array tmp;
    tmp.setlength(m);
    for (unsigned int j = 0; j < m; ++j) tmp[j] = a[j * n + i];

    // compute moments
    double var, skew, kurt;
    alglib::samplemoments(tmp, mu[i], var, skew, kurt);
  }

  // compute the inversed covariance matrix

  alglib::real_2d_array x;
  x.setcontent(m, n, a);

  alglib::real_2d_array invcov;
  invcov.setlength(n, n);
  alglib::covm(x, invcov);

  alglib::ae_int_t info;
  alglib::matinvreport rep;
  alglib::rmatrixinverse(invcov, info, rep);

#if 0
  for (int i = 0; i < invcov.rows(); ++i)
  {
    for (int j = 0; j < invcov.cols(); ++j)
      printf(" %lf", invcov(i, j));
    printf("\n");
  }
#endif

  // compute dist[i] = (x[i] - mu)' * inv(cov) * (x[i] - mu)

  // x[i] - mu vector
  alglib::real_2d_array diff;
  diff.setlength(n, 1);

  for (unsigned int i = 0; i < m; ++i)
  {
    // compute x[i] - mu
    for (unsigned int j = 0; j < n; ++j)
      diff(j, 0) = x(i, j) - mu[j];

    static const alglib::ae_int_t nop = 0;
    static const alglib::ae_int_t transpose = 1;

    // tmp = (x[i] - mu)' * inv(cov)
    alglib::real_2d_array tmp;
    tmp.setlength(1, invcov.cols());
    alglib::rmatrixgemm
    (
     diff.cols(), invcov.cols(), invcov.rows(), 1,
     diff, 0, 0, transpose,
     invcov, 0, 0, nop,
     0, tmp, 0, 0
    );

    // res = tmp * (x[i] - mu)
    alglib::real_2d_array res;
    res.setlength(tmp.rows(), 1);
    alglib::rmatrixgemm
    (
     tmp.rows(), diff.cols(), tmp.cols(), 1,
     tmp, 0, 0, nop,
     diff, 0, 0, nop,
     0, res, 0, 0
    );

    dists[i] = res(0, 0);
  }

  return 0;
}

static int mahalanobis
(const std::vector<CvPoint>& points, std::vector<double>& dists)
{
  // compute the mahalanobis distance fo the given dataset

  double* const p = (double*)malloc(2 * points.size() * sizeof(double));
  if (p == NULL) return -1;
  for (unsigned int i = 0; i < points.size(); ++i)
  {
    p[i * 2 + 0] = (double)points[i].x;
    p[i * 2 + 1] = (double)points[i].y;
  }

  dists.resize(points.size());
  mahalanobis(p, points.size(), 2, &dists[0]);

  free(p);

  return 0;
}


int main(int ac, char** av)
{
  std::vector<CvPoint> points;
  std::vector<double> m;

  gen_linear(points);
  gen_outlier(points);

#if 0
  printf("x = [ ");
  for (unsigned int i = 0; i < points.size(); ++i)
    printf("%c %d %d", i ? ';' : ' ', points[i].x, points[i].y);
  printf("]\n");
#endif

  mahalanobis(points, m);

#if 1
  for (unsigned int i = 0; i < points.size(); ++i)
    printf("%d, %d, %lf\n", points[i].x, points[i].y, m[i]);
#endif

  return 0;
}
