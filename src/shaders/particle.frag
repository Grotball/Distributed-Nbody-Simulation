in vec3 pos;
in float speed;

out vec4 FragCol;

void main()
{
    vec2 q = 2 * (gl_PointCoord - vec2(0.5));
    float l = length(q);

    if (l > 1.0)
        discard;
 

    // The surface normal vector 
    // does not account for perspective projection.
    vec3 normal = vec3(q.x, -q.y, sqrt(1 - l*l));

    vec3 lightDir = normalize(vec3(5, 5, 15) - pos);
    float w = max(dot(normal, lightDir), 0);
    vec3 col =  w * vec3(1.0, 0.2, 0.1);

    FragCol = vec4(col, 1.0);
}