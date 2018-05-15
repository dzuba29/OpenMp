#ifndef __DIRICHLET_H__
#define __DIRICHLET_H__
#include "matrix.h"
double step(const size_t size);
Matrix solve(const size_t size, const double eps);
Matrix solve_omp(const size_t size, const double eps);
double conditions(const double x,const double y);
double function(const double x, const double y);
Matrix first_approx_f(const size_t size,const double h);
Matrix first_approx_u(const size_t size,const double h);
#endif // __DIRICHLE_H__