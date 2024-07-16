in float speed;

out vec4 FragCol;

void main()
{
    FragCol = vec4(vec3(min(speed, 10)), 1.0);

    if (length(gl_PointCoord - vec2(0.5)) > 0.5)
        discard;
}