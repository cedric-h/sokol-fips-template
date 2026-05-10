#ifndef __EAB_EASE_IMPL
#define __EAB_EASE_IMPL

static float ease_in_back(float x) {
    float c1 = 1.70158;
    float c3 = c1 + 1;
    return c3 * x * x * x - c1 * x * x;
}

static float ease_out_circ(float x) {
    return sqrtf(1 - powf(x - 1, 2));
}

static float ease_in_sine(float x) {
  return 1.0f - cosf((x * M_PI) / 2.0f);
}
static double ease_in_sine_double(double x) {
  return 1.0 - cos((x * M_PI) / 2.0);
}

static double ease_out_sine(double x) {
  return sinf((x * M_PI) / 2.0f);
}
static double ease_out_sine_double(double x) {
  return sin((x * M_PI) / 2);
}

#endif
