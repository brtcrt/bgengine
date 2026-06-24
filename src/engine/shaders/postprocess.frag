#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D sourceTexture;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float iTime;
    int iFrame;
    vec3 iResolution;
    vec4 iMouse;
    float effectOpacity;
} ubuf;

void main()
{
    vec2 uv = qt_TexCoord0;
    vec2 centered = uv - vec2(0.5);
    float vignette = smoothstep(0.85, 0.25, dot(centered, centered));
    vec3 color = texture(sourceTexture, uv).rgb;
    color = mix(color * 0.92, color, vignette);
    color += 0.015 * sin((uv.x + uv.y + ubuf.iTime) * 80.0);
    fragColor = vec4(color, ubuf.qt_Opacity * ubuf.effectOpacity);
}
