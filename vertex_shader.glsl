#version 420 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
out vec3 fcolor;
vec2 pow2(vec2 z)
{
    return vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y);
}
float len(vec2 z)
{

    return sqrt(z.x * z.x + z.y * z.y);
}

void main()
{
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    fcolor = color;
    vec2 z = vec2(0, 0);
    int max_iter = 150;
    int i;
    for(i = 0; i < max_iter; i++)
    {
        if(len(z) > 2)
        {
            break;
        }
        z = pow2(z) + vec2(pos.x, pos.y);
    }
    if (i < max_iter && i > 0) {
        int idx = i % 16;
        vec3 mapping[16];
        mapping[0]  = vec3(66.0f/255.0f, 30.0f/255.0f, 15.0f/255.0f);
        mapping[1]  = vec3(25.0f/255.0f, 7.0f/255.0f, 26.0f/255.0f);
        mapping[2]  = vec3(9.0f/255.0f, 1.0f/255.0f, 47.0f/255.0f);
        mapping[3]  = vec3(4.0f/255.0f, 4.0f/255.0f, 73.0f/255.0f);
        mapping[4]  = vec3(0.0f/255.0f, 7.0f/255.0f, 100.0f/255.0f);
        mapping[5]  = vec3(12.0f/255.0f, 44.0f/255.0f, 138.0f/255.0f);
        mapping[6]  = vec3(24.0f/255.0f, 82.0f/255.0f, 177.0f/255.0f);
        mapping[7]  = vec3(57.0f/255.0f, 125.0f/255.0f, 209.0f/255.0f);
        mapping[8]  = vec3(134.0f/255.0f, 181.0f/255.0f, 229.0f/255.0f);
        mapping[9]  = vec3(211.0f/255.0f, 236.0f/255.0f, 248.0f/255.0f);
        mapping[10] = vec3(241.0f/255.0f, 233.0f/255.0f, 191.0f/255.0f);
        mapping[11] = vec3(248.0f/255.0f, 201.0f/255.0f, 95.0f/255.0f);
        mapping[12] = vec3(255.0f/255.0f, 170.0f/255.0f, 0.0f/255.0f);
        mapping[13] = vec3(204.0f/255.0f, 128.0f/255.0f, 0.0f/255.0f);
        mapping[14] = vec3(153.0f/255.0f, 87.0f/255.0f, 0.0f/255.0f);
        mapping[15] = vec3(106.0f/255.0f, 52.0f/255.0f, 3.0f/255.0f);
        fcolor = mapping[i];
    }
    else fcolor = vec3(0.0f, 0.0f, 0.0f);
}
