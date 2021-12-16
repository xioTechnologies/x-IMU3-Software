in float colourScale;
out vec4 colourFrag;
uniform vec4 colour;

uniform bool isBorder;
uniform bool isVertical;

void main()
{
    if (!isBorder)
    {
        if (isVertical)
        {
            if ((int(gl_FragCoord.y / 2) % 2) == 1)
            {
                discard;
            }
        }

        else if (!isVertical)
        {
            if ((int(gl_FragCoord.x / 2) % 2) == 1)
            {
                discard;
            }
        }
    }

    colourFrag = colour * colourScale;
}