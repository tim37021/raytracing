#ifndef SAMPLER_H_
#define SAMPLER_H_

#include "math-toolkit.h"
#include "primitives.h"

typedef struct _SAMPLER_TYPEDEF {
    double *samples;
    int count;
    int niddle;
} sampler;
sampler *create_sampler(int seed, int count);
void release_sampler(sampler *);
void regen_sampler(sampler *, int seed, int count);

static inline
double sampler_fetch(sampler *sampler)
{
    sampler->niddle%=sampler->count;
    return sampler->samples[sampler->niddle++];
}

static inline
void hemisphere_sampling(sampler *sampler, double cosine_power, point3 normal, point3 out)
{
    /* uniform hemisphere sampling */
    double sample_x, sample_y;
    sample_x = sampler_fetch(sampler);
    sample_y  = sampler_fetch(sampler);

    double cos_phi = cos(2.0 * 3.14159 * sample_x);
    double sin_phi = sin(2.0 * 3.14159 * sample_x);
    double cos_theta = pow(1.0 - sample_y, 1.0 / (cosine_power + 1.0));
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    point3 sample;
    sample[0] = sin_theta * cos_phi;
    sample[1] = sin_theta * sin_phi;
    sample[2] = cos_theta;

    point3 rvec, tangent, bitangent;
    rvec[0]=sampler_fetch(sampler) * 2.0 - 1.0;
    rvec[1]=sampler_fetch(sampler) * 2.0 - 1.0;
    rvec[2]=sampler_fetch(sampler) * 2.0 - 1.0;
    multiply_vector(normal, dot_product(rvec, normal), tangent);
    subtract_vector(rvec, tangent, tangent);
    normalize(tangent);
    cross_product(normal, tangent, bitangent);

    /* construct TBN (change of basis matrix) */
    mat3 tbn;
    construct_mat3(tangent, bitangent, normal, tbn);
    multiply_mat3_vector(tbn, sample, out);
}

#endif