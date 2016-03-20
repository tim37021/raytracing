#ifndef __RAY_MATH_TOOLKIT_H
#define __RAY_MATH_TOOLKIT_H

#include <math.h>
#include <stdio.h>
#include <assert.h>

static inline
void normalize(double *v)
{
    double d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    assert(d != 0.0 && "Error calculating normal");

    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
}

static inline
double length(const double *v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static inline
void add_vector(const double *a, const double *b, double *out)
{
    for (int i = 0; i < 3; i++)
        out[i] = a[i] + b[i];
}

static inline
void subtract_vector(const double *a, const double *b, double *out)
{
    for (int i = 0; i < 3; i++)
        out[i] = a[i] - b[i];
}

static inline
void multiply_vectors(const double *a, const double *b, double *out)
{
    for (int i = 0; i < 3; i++)
        out[i] = a[i] * b[i];
}

static inline
void multiply_vector(const double *a, double b, double *out)
{
    for (int i = 0; i < 3; i++)
        out[i] = a[i] * b;
}


static inline
void cross_product(const double *v1, const double *v2, double *out)
{
    out[0] = v1[1] * v2[2] - v1[2] * v2[1];
    out[1] = v1[2] * v2[0] - v1[0] * v2[2];
    out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

static inline
double dot_product(const double *v1, const double *v2)
{
    double dp = 0.0;
    for (int i = 0; i < 3; i++)
        dp += v1[i] * v2[i];
    return dp;
}

static inline
void scalar_triple_product(const double *u, const double *v, const double *w, double *out)
{
    cross_product(v, w, out);
    multiply_vectors(u, out, out);
}

static inline
double scalar_triple(const double *u, const double *v, const double *w)
{
    double tmp[3];
    cross_product(w, u, tmp);
    return dot_product(v, tmp);
}

/* @brief protect color value overflow */
static inline
void clamp(double *c)
{
    for (int i = 0; i < 3; i++)
        if (c[i] > 1.0) c[i] = 1.0;
}

static inline
void scalar_matrix(const double *scalar, double *v)
{
    v[0] = scalar[0];
    v[1] = 0;
    v[2] = 0.0;
    v[3] = 0.0;
    v[4] = scalar[1];
    v[5] = 0.0;
    v[6] = 0.0;
    v[7] = 0.0;
    v[8] = scalar[2];
}


static inline
void identity_matrix(double *v)
{
    scalar_matrix((double [3]) {
        1.0, 1.0, 1.0
    }, v);
}

static inline
void rotation_matrix_x(double t, double *v)
{
    v[0] = 1.0;
    v[1] = 0;
    v[2] = 0.0;
    v[3] = 0.0;
    v[4] = cos(-t);
    v[5] = -sin(-t);
    v[6] = 0.0;
    v[7] = sin(-t);
    v[8] = cos(-t);
}

static inline
void rotation_matrix_y(double t, double *v)
{
    v[0] = cos(-t);
    v[1] = 0;
    v[2] = sin(-t);
    v[3] = 0.0;
    v[4] = 1.0;
    v[5] = 0.0;
    v[6] = -sin(-t);
    v[7] = 0.0;
    v[8] = cos(-t);
}

static inline
void rotation_matrix_z(double t, double *v)
{
    v[0] = cos(-t);
    v[1] = -sin(-t);
    v[2] = 0.0;
    v[3] = sin(-t);
    v[4] = cos(-t);
    v[5] = 0.0;
    v[6] = 0.0;
    v[7] = 0.0;
    v[8] = 1.0;
}

static inline
void construct_mat3(const double *u, const double *v, const double *w, double *out)
{
    out[0] = u[0];
    out[1] = v[0];
    out[2] = w[0];
    out[3] = u[1];
    out[4] = v[1];
    out[5] = w[1];
    out[6] = u[2];
    out[7] = v[2];
    out[8] = w[2];
}

/* NOTE: out can't be overlap with u or v */
static inline
void multiply_mat3(const double *u, const double *v, double *out)
{
    out[0] = u[0] * v[0] + u[1] * v[3] + u[2] * v[6];
    out[1] = u[0] * v[1] + u[1] * v[4] + u[2] * v[7];
    out[2] = u[0] * v[2] + u[1] * v[5] + u[2] * v[8];
    out[3] = u[3] * v[0] + u[4] * v[3] + u[5] * v[6];
    out[4] = u[3] * v[1] + u[4] * v[4] + u[5] * v[7];
    out[5] = u[3] * v[2] + u[4] * v[5] + u[5] * v[8];
    out[6] = u[6] * v[0] + u[7] * v[3] + u[8] * v[6];
    out[7] = u[6] * v[1] + u[7] * v[4] + u[8] * v[7];
    out[8] = u[6] * v[2] + u[7] * v[5] + u[8] * v[8];
}

/* NOTE: out can't be overlap with v */
static inline
void multiply_mat3_vector(const double *M, const double *v, double *out)
{
    out[0] = M[0] * v[0] + M[1] * v[1] + M[2] * v[2];
    out[1] = M[3] * v[0] + M[4] * v[1] + M[5] * v[2];
    out[2] = M[6] * v[0] + M[7] * v[1] + M[8] * v[2];
}

#endif
