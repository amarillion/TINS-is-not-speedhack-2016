#include <math.h>

double
srgb_to_linear (double x)
{
  if (x <= 0.04045)
    return x / 12.92;
  else
    return pow (((x + 0.055) / 1.055), 2.4);
}

double
linear_to_srgb (double x)
{
  if (x <= 0.0031308)
    return x * 12.92;
  else
    return (pow (x, (1 / 2.4)) * 1.055) - 0.055;
}

double
amplitude_to_db (double x)
{
  if (x > 0)
    return 20 * log10(x);
  else
    return -INFINITY
  /* negative ratios are in error */
}

double
db_to_amplitude (double x)
{
  return pow(10, (x/20));
}
