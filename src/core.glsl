@vs vs
in vec2 pos0;
in vec2 uv0;
in vec4 color0;
in float size0;

out vec4 color;
out vec2 uv;
out float size;

void main() {
    gl_Position = vec4(pos0, 0.0, 1.0);
    color = color0;
    uv = uv0;
    size = size0;
}
@end

@fs fs
layout(binding=0) uniform texture2D font_tex;
layout(binding=0) uniform sampler font_smp;

in vec4 color;
in vec2 uv;
in float size;
out vec4 frag_color;

void main() {
    float dist = texture(
        sampler2D(font_tex, font_smp),
        vec2(uv.x, uv.y)
    ).r;

    dist = pow(abs(dist), 2.2);

    float dbuffer = 0.5f;

    float raw_gamma = 2.0f * 1.4142;
    float gamma = raw_gamma / size;
    float alpha = smoothstep(dbuffer - gamma, dbuffer + gamma, dist);
    frag_color = color * color.a * alpha; 
}
@end

@program core vs fs
