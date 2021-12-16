in vec4 position;
out float colourScale;

uniform vec4 window;
uniform vec4 offsetAndScale;

void main()
{
    vec2 finalPosition = (window.xy + ((offsetAndScale.xy + position.xy) * offsetAndScale.zw)) * window.zw;

    gl_Position = vec4(finalPosition.x, finalPosition.y, position.z, 1.0);

    colourScale = position.w;
}