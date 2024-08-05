in vec3 pos;
in float speed;

out vec4 FragCol;

void main()
{
    vec2 q = 2 * (gl_PointCoord - vec2(0.5));
    float l = length(q);

    if (l > 1.0)
        discard;
 
 
    vec3 col = vec3(mix(0.0, 1.0, 0.1*speed));

    FragCol = vec4(col, 1.0);
}