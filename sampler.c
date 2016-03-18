#include "sampler.h"
#include <stdio.h>
#include <stdlib.h>

sampler *create_sampler(int seed, int count)
{
	sampler *result = malloc(sizeof(sampler));
	srand(seed);
	result->samples = malloc(sizeof(double) * count);
	result->count = count;
	result->niddle = 0;
	while(count)
		result->samples[--count] = (double)rand()/RAND_MAX;
	return result;
}

void release_sampler(sampler *sampler)
{
	free(sampler->samples);
	free(sampler);
}

void regen_sampler(sampler *sampler, int seed, int count)
{
	srand(seed);
	sampler->count = count;
	sampler->niddle = 0;
	while(count)
		sampler->samples[--count] = (double)rand()/RAND_MAX;
}