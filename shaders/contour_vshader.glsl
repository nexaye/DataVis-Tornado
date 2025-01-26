#version 330
uniform mat4 mvpMatrix;
uniform float currentSlice;
in vec4 vertexPosition;

void main()
{

    // Calculate vertex position in screen space.
    gl_Position = mvpMatrix * vec4(vertexPosition.x, vertexPosition.y, currentSlice, 1);
}