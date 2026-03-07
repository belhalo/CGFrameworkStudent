varying vec3 v_light_color;
varying vec2 v_uv;

void main()
{
    gl_FragColor = vec4(v_light_color, 1.0);
}
