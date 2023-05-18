#version 330 core

out vec4 fragColor;
in vec2 textureCoord_frag;

const float gridBrightness = 0.4; // 1.0 is white, 0.0 is black
const vec3 graphRed = vec3(0.878, 0.125, 0.125);
const vec3 graphGreen = vec3(0.427, 0.831, 0.0);
const float gridTilingFactor = 80.0; // number of tiles across but only works for even numbers, 80 makes each grid tile width of 0.25 for a matrix scale of 20
const float gridFadeoutDistance = 0.2; // 0.5 fades out to edges of plane, anything closer to 0.0 fades out sooner

/*  Outputs color for a grid to be drawn via UV texture coordinates.

    Based on ref 1, but modified not to use 3D coordinates, just 2D UV texture coordinates.
    If we wanted to add a z axis line later, we will want to move to 3D coordinates and
    will require unprojecting in 3D space.

    @ref https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/
    @ref https://madebyevan.com/shaders/grid/

    @param uvPos a UV texture coordinate position
    @param scale set the number of tiling repetitions (even numbers only)
*/
vec4 grid(vec2 uvPos, float scale)
{
    // XY position on plane determined via UV coordinate. Origin of plane
    // is (0.0, 0.0), top right is (0.5, 0.5), bottom left is (-0.5, -0.5), etc.
    vec2 xyPos = uvPos - vec2(0.5);

    // XY position determines color based on repeating grid pattern
    // Uses fwidth screen-space partial derivatives to provide anti-aliasing and consistent line thickness
    vec2 gridCoord = xyPos * scale;
    vec2 derivative = fwidth(gridCoord); // Can add line thickness by scaling this value
    vec2 grid = abs(fract(gridCoord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    vec4 color = vec4(gridBrightness, gridBrightness, gridBrightness, 1.0 - min(line, 1.0));

    float minimumY = min(derivative.y, 1.0);
    float minimumX = min(derivative.x, 1.0);
    float inverseScale = 1.0 / scale;
    // X and Y axis are swapped here based on the axes setup in ThreeDView
    float xAxisPresent = step(abs(xyPos.y), minimumY * inverseScale);
    float yAxisPresent = step(abs(xyPos.x), minimumX * inverseScale);
    color.rgb = mix(color.rgb, graphRed, xAxisPresent);   // If X-axis present at this position, color red
    color.rgb = mix(color.rgb, graphGreen, yAxisPresent); // If Y-axis present at this position, color green

    // Fade out edge of grid in the distance by setting
    // alpha (opacity) of color based on distance from origin
    float distanceFromOrigin = length(xyPos);
    // Remaps distance to a 0.0 to 1.0 range where 1.0 becomes the minimum distance
    // from the origin to an edge of the plane
    float normalizedDistanceFromOrigin = min(distanceFromOrigin, gridFadeoutDistance) / gridFadeoutDistance;
    color.a *= 1.0 - normalizedDistanceFromOrigin; // Closer to edges of plane become more transparent

    return color;
}

void main()
{
    fragColor = grid(textureCoord_frag, gridTilingFactor);
} 
