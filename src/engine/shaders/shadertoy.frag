#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float iTime;
    int iFrame;
    vec3 iResolution;
    vec4 iMouse;
    float effectOpacity;
} ubuf;

void mainImage(out vec4 color, in vec2 fragCoord)
{
    vec2 uv = fragCoord / max(ubuf.iResolution.xy, vec2(1.0));
    vec2 p = (2.0 * fragCoord - ubuf.iResolution.xy) / max(ubuf.iResolution.y, 1.0);

    float t = ubuf.iTime;
    float wave = 0.5 + 0.5 * cos(8.0 * length(p) - t * 2.4);
    vec3 base = 0.5 + 0.5 * cos(t + uv.xyx * 5.0 + vec3(0.0, 2.0, 4.0));
    vec3 accent = mix(vec3(0.02, 0.03, 0.05), vec3(0.2, 0.75, 0.95), wave);

    if (ubuf.iMouse.z > 0.0) {
        vec2 mouseUv = ubuf.iMouse.xy / max(ubuf.iResolution.xy, vec2(1.0));
        float cursor = smoothstep(0.22, 0.0, distance(uv, mouseUv));
        accent = mix(accent, vec3(1.0, 0.7, 0.25), cursor);
    }

    color = vec4(mix(base, accent, 0.65), 1.0);
}

void main()
{
    vec4 color;
    mainImage(color, qt_TexCoord0 * ubuf.iResolution.xy);
    fragColor = vec4(color.rgb, color.a * ubuf.qt_Opacity * ubuf.effectOpacity);
}
