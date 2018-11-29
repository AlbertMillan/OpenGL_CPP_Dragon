#version 330 core

uniform vec3 color;

void main()
{
    gl_FragColor = vec4(color, 1.0); // set all 4 vector values to 1.0
}
