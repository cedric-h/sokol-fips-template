@vs vs
in vec4 position;
in vec4 color0;

out vec4 color;

void main() {
    gl_Position = position;
    color = color0;
}
@end

@fs fs
layout(binding=0) uniform texture2D triangle_tex;
layout(binding=0) uniform sampler triangle_smp;

in vec4 color;
out vec4 frag_color;

void main() {
    frag_color = texture(
        sampler2D(triangle_tex,triangle_smp),
        color.xy
    ) * color;
}
@end

@program triangle vs fs
