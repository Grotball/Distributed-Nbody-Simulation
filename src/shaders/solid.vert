layout (location = 0) in vec3 vertexPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(vertexPos.x, vertexPos.y, 0.0, 1.0);
}