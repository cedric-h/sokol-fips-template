#ifndef EAB_BASE_IMPL
#define EAB_BASE_IMPL

#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <assert.h>

#include "sokol_app.h"
#include "rand.h"

#define countof(arr) ( sizeof(arr) / sizeof((arr)[0]) )
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define sign(a) (((a) < 0) ? -1 : 1)
#define GOLDEN_RATIO 1.61803

#define EXE_PATH "./eab.app/Contents/MacOS"

float base_screen_size_x(void);
float base_screen_size_y(void);
double base_play_duration(void);
void base_set_cursor(sapp_mouse_cursor);

typedef struct { float x, y; } f2;

typedef struct { char *str; size_t len; } String;
#define STRING(x) ((String) { x, sizeof(x)-1 })

typedef struct { uint8_t r, g, b, a; } Color;

static float lerpf(float start, float end, float amount) {
    return start + amount*(end - start);
}
static float inv_lerpf(float min, float max, float p) {
    return (p - min) / (max - min);
}
static float lerp_rads(float a, float b, float t) {
    float difference = fmodf(b - a, M_PI*2.0),
            distance = fmodf(2.0 * difference, M_PI*2.0) - difference;
    return a + distance * t;
}

static double lerp(double start, double end, double amount) {
    return start + amount*(end - start);
}
static double inv_lerp(double min, double max, double p) {
    return (p - min) / (max - min);
}

/* https://bottosson.github.io/posts/oklab/#converting-from-linear-srgb-to-oklab */
typedef struct { float L, a, b; } color_Lab;
typedef struct { float r, g, b; } color_RGB;

static color_Lab color_linear_srgb_to_oklab(color_RGB c) {
    float l = 0.4122214708f * c.r + 0.5363325363f * c.g + 0.0514459929f * c.b;
    float m = 0.2119034982f * c.r + 0.6806995451f * c.g + 0.1073969566f * c.b;
    float s = 0.0883024619f * c.r + 0.2817188376f * c.g + 0.6299787005f * c.b;

    float l_ = cbrtf(l);
    float m_ = cbrtf(m);
    float s_ = cbrtf(s);

    return (color_Lab) {
        0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_,
        1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_,
        0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_,
    };
}

static color_RGB color_oklab_to_linear_srgb(color_Lab c) {
    float l_ = c.L + 0.3963377774f * c.a + 0.2158037573f * c.b;
    float m_ = c.L - 0.1055613458f * c.a - 0.0638541728f * c.b;
    float s_ = c.L - 0.0894841775f * c.a - 1.2914855480f * c.b;

    float l = l_*l_*l_;
    float m = m_*m_*m_;
    float s = s_*s_*s_;

    return (color_RGB) {
        +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
        -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
        -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
    };
}

static Color color_lerp(Color clr_a, Color clr_b, float t) {
    color_RGB rgb_a = { (float)(clr_a.r)/255.0f, (float)(clr_a.g)/255.0f, (float)(clr_a.b)/255.0f };
    color_RGB rgb_b = { (float)(clr_b.r)/255.0f, (float)(clr_b.g)/255.0f, (float)(clr_b.b)/255.0f };

    color_Lab lab_a = color_linear_srgb_to_oklab(rgb_a);
    color_Lab lab_b = color_linear_srgb_to_oklab(rgb_b);

    color_RGB ret = color_oklab_to_linear_srgb((color_Lab) {
        .L = lerp(lab_a.L, lab_b.L, t),
        .a = lerp(lab_a.a, lab_b.a, t),
        .b = lerp(lab_a.b, lab_b.b, t),
    });
    return (Color) {
        .r = roundf(ret.r*255.0f),
        .g = roundf(ret.g*255.0f),
        .b = roundf(ret.b*255.0f),
        .a = lerp(clr_a.a, clr_b.a, t),
    };
}
#endif
