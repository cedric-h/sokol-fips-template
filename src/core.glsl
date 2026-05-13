@vs vs
in vec2  pos0;
in vec2  uv0;
in vec4  color0;
in uvec4 bytes0;

out vec4  color;
out vec2  uv;
flat out uvec4 bytes;

void main() {
    gl_Position = vec4(pos0.x, -pos0.y, 0.0, 1.0);
    color = color0;
    uv = uv0;
    bytes = bytes0;
}
@end

@fs fs
layout(binding=0) uniform texture2D font_tex;
layout(binding=0) uniform sampler font_smp;

layout(binding=1) uniform texture2D tex_tex;
layout(binding=1) uniform sampler tex_smp;

in vec4 color;
in vec2 uv;
flat in uvec4 bytes;
out vec4 frag_color;

void main() {
    frag_color = vec4(1, 0, 1, 1);
    switch (bytes.x) {
        case 0: {
            float dist = texture(
                sampler2D(font_tex, font_smp),
                uv
            ).r;

            dist = pow(abs(dist), 2.2);

            float dbuffer = 0.575f;

            float raw_gamma = 2.0f * 1.4142;
            float size = float(bytes[1]);
            float gamma = raw_gamma / size;
            float alpha = smoothstep(dbuffer - gamma, dbuffer + gamma, dist);
            frag_color = color * color.a * alpha; 
        }; break;

        case 1: {
            frag_color = color;
        }; break;

        case 2: {
            frag_color = texture(sampler2D(tex_tex, tex_smp), uv);
            frag_color *= color;
            frag_color *= frag_color.a;
        }; break;
    }
}
@end

@program core vs fs
