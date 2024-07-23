layout (location = 0) in float vertexPosX;
layout (location = 1) in float vertexPosY;
layout (location = 2) in float vertexPosZ;
layout (location = 3) in float vertexVelX;
layout (location = 4) in float vertexVelY;
layout (location = 5) in float vertexVelZ;
layout (location = 6) in float mass;

out vec3 pos;
out float speed;

// TODO: combine view and projection
// into single matrix.
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform float fovScale;

void main()
{
    pos = vec3(vertexPosX, vertexPosY, vertexPosZ);
    vec3 vel = vec3(vertexVelX, vertexVelY, vertexVelZ);
    
    gl_Position = projection * view * vec4(pos, 1.0);

    // distance from view position.
    // Because camera position is currently fixed to origin,
    // it is just length of the vector.
    float d = length(pos - cameraPos);

    // need to make independent from screen resolution.
    // Scaling of point size with distance does not take in
    // acount perspective projection.
    gl_PointSize = 50 * sqrt(mass*6.67e-11) * fovScale / d;

    speed = length(vel);
}