#ifndef FUNCS_H
#define FUNCS_H

#include "funcs.h"
#include "data.h"
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

Result *find_overlaps(Picture picture, Object object, int *num_results, float threshold);

#endif