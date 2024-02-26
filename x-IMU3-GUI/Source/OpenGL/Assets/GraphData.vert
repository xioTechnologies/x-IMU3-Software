#version 330 core

layout (location = 0) in vec2 position_vert;

uniform vec2 axisLimitsRange;
uniform vec2 axisLimitsMin;

// Nasty fix because we are using GL_LINES for line rendering and some graphics driver implementations
// throw away lines drawn on the top and left edges of the NDC XY plane. We scale down (inset) our NDC
// coordinates by just a little bit to ensure all edges of the NDC XY plane are always drawn.
const float SCALE_FOR_GL_LINES_FIX = 0.999;

void main()
{
    vec2 positionNdc = (((position_vert - axisLimitsMin) / axisLimitsRange) * 2.0) - 1.0;
    gl_Position = vec4(positionNdc.xy * SCALE_FOR_GL_LINES_FIX, 0.0, 1.0);
}
