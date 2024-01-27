#ifndef cubicSpline_h
#define cubicSpline_h


#include <stddef.h>

#include "vec3.h"


/*
** Let y_0, ..., y_{n-1} in R^m be a series of points with n > 1, m > 0.
** We would like to construct n cubic polynomials of the form
**     f_i(t) = a_i t^3 + b_i t^2 + c_i t + d_i,
** for i in {0, ..., n-2}, where a_i, b_i, c_i, d_i in R^m, such that
**     1. f_i(0) = y_i,            i in {0, ..., n-2};
**     2. f_i(1) = y_{i+1},        i in {0, ..., n-2};
**     3. f_i'(1)  = f_{i+1}'(0),  i in {0, ..., n-3};
**     4. f_i''(1) = f_{i+1}''(0), i in {0, ..., n-3}.
** In other words, the f_i form a C^2 interpolating curve between the y_i.
** Note that we have 4(n-1) unknown coefficients, but only 4n-6 equations.
** We have several choices for these last two constraints, but we choose
** those giving the so-called "natural" spline. This requires that we set
**     5. f_0''(0) = 0,
**     6. f_{n-1}''(1) = 0.
** With these equations, the f_i may be uniquely determined.
**
** We begin by showing that if each c_i is known, then the remaining
** coefficients may be uniquely determined. We begin by noting that
** c_i = f_i'(0) by construction, so equations (1-3) give
**     y_i = f_i(0) = d_i,
**     y_{i+1} = f_i(1) = a_i + b_i + c_i + d_i,
**     3a_i + 2b_i + c_i = f_i'(1) = f_{i+1}'(0) = c_{i+1}.
** It's worth noting that while the first two equations here hold for all
** i in {0, ..., n-2}, the last does not hold for i = n-2. Nevertheless,
** using these identities, we find that
**     7. d_i = y_i,                               i in {0, ..., n-2}
**     8. b_i = 3(y_{i+1} - y_i) - 2c_i - c_{i+1}, i in {0, ..., n-3};
**     9. a_i = 2(y_i - y_{i+1}) + c_i + c_{i+1},  i in {0, ..., n-3}.
** This can be extended to the i = n-2 case using a suitable definition
** of c_{n-1}. To further motivate this definition, we note that from our
** second natural spline condition (6), we may write
**     0 = f_{n-2}''(1) = 6a_{n-2} + 2b_{n-2},
**     => b_{n-2} = -3a_{n-2},
** Using (2), we may thus define c_{n-1} using the expression
**     2a_{n-2} = (y_{n-2} - y_{n-1}) + c_{n-2},
**     => c_{n-2} + 2c_{n-1} = 3(y_{n-1} - y_{n-2}).
** It can be verified that using this definition and (6), identities
** (8-9) now hold for i = n-2, as required. This work demonstrates that
** if c_0, ..., c_{n-1} can be determined, then a_i, b_i, d_i can be
** computed for each interpolant function f_i.
**
** In order to compute the c_i, we require n equations relating them
** only to the y_i. As we have not used our constraints on the second
** derivative yet, this seems a sensible place to start. From (4), we get
**     10. 6a_{i-1} + 2b_{i-1} = 2b_i,
** for i in {1, ..., n-2}. Using identities (8-10), we can write the c_i
** just in terms of the y_i for all i in {1, ..., n-2}, whence we find
**     0 = 6a_{i-1} + 2b_{i-1} - 2b_i
**       = 6(y_{i-1} - y_{i+1}) + 2c_{i-1} + 8c_i + 2c_{i+1}
**     => c_{i-1} + 4c_i + c_{i+1} = 3(y_{i+1} - y_{i-1}).
** Including our definition of c_{n-1}, we now have n equations. We need
** only one more equation, and the only information we have not yet used
** is the first natural spline condition (5). Combining it with (9) for
** i = 0, we obtain
**     0 = f_0''(0) = 2b_0,
**     => 2c_0 + c_1 = 3(y_1 - y_0).
** This final equation completes the system of equations, which we expect
** to have a unique solution.
**
** It is worth noting that the system constructed above is particularly
** efficient to solve numerically, as it may be represented by a simple
** tridiagonal matrix equation,
**     [2, 1, 0, 0, 0,  ...][  c_0  ]   [    3(y_1 - y_0)    ]
**     [1, 4, 1, 0, 0,  ...][  c_1  ]   [    3(y_2 - y_0)    ]
**     [0, 1, 4, 1, 0,  ...][  c_2  ]   [    3(y_3 - y_1)    ]
**     [        ...        ][  ...  ] = [         ...        ].
**     [  ... 0, 1, 4, 1, 0][c_{n-3}]   [3(y_{n-2} - y_{n-4})]
**     [  ... 0, 0, 1, 4, 1][c_{n-2}]   [3(y_{n-1} - y_{n-3})]
**     [  ... 0, 0, 0, 1, 2][c_{n-1}]   [3(y_{n-1} - y_{n-2})]
** This is handy, as it is now easily solved using the Thomas algorithm.
** After solving for the c_i, we may compute a_i, b_i, d_i using (7-9).
*/


typedef struct cubicSpline {
	vec3 *a;
	vec3 *b;
	vec3 *c;
	vec3 *d;
} cubicSpline;


void cubicSplineInit(
	cubicSpline *const restrict spline,
	const vec3 *const restrict y, const size_t n
);
void cubicSplineEvaluate(
	const cubicSpline *const restrict spline,
	const size_t i, const float t, vec3 *const restrict out
);
void cubicSplineDelete(cubicSpline *const restrict spline);


#endif