#version 330 core

out vec4 colourFrag;
uniform vec4 colour;

void main()
{
    colourFrag = colour;
}