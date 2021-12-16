in vec2 position;
uniform vec4 window;
uniform vec4 offsetAndScale;

void main()
{
    vec2 finalPosition = (window.xy + ((offsetAndScale.xy + position) * offsetAndScale.zw)) * window.zw;

    gl_Position = vec4(finalPosition.x, finalPosition.y, 0.5, 1.0);
}