#version 330 core

in float brightness_frag;
out vec4 fragColour;

void main()
{
    fragColour = vec4(1.0) * brightness_frag;
}