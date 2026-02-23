varying vec2 v_uv;

void main()
{
    // UV range is [0.0, 1.0]
    v_uv = gl_MultiTexCoord0.xy;

    // fullscreen quad is already in clip space ([1, -1])
    gl_Position = vec4(gl_Vertex.xyz, 1.0);
}
