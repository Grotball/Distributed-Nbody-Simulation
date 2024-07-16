layout (location = 0) in float vertexPosX;
layout (location = 1) in float vertexPosY;
layout (location = 2) in float vertexPosZ;
layout (location = 3) in float vertexVelX;
layout (location = 4) in float vertexVelY;
layout (location = 5) in float vertexVelZ;
layout (location = 6) in float mass;

out float speed;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 pos = vec3(vertexPosX, vertexPosY, vertexPosZ);
    vec3 vel = vec3(vertexVelX, vertexVelY, vertexVelZ);
    
    gl_Position = projection * view * vec4(pos, 1.0);

    gl_PointSize = sqrt(mass);

    speed = length(vel);
}