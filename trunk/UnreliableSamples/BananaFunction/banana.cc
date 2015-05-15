#include <cmath>

#include "banana.h"

double banana(double x1, double x2)
{
    return 100.0 * pow(x2 - pow(x1, 2), 2) + pow((1 - x1), 2);
}
