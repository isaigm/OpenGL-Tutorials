#version 420 core
in vec3 fcolor;
out vec4 fragColor;
void main()
{
    fragColor = vec4(fcolor, 1.0f);
}