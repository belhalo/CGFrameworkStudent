varying vec2 v_uv;
uniform float u_subtask;
uniform float u_task;
uniform float u_aspect;

void main()
{
    // problem precision of fs, that because we put the range of (0.5, 1.5)
    if (u_task > 0.5 && u_task < 1.5) { // Tasca 1
        if (u_subtask > 0.5 && u_subtask < 1.5) { // same here
            vec3 colorInici = vec3(1.0, 0.0, 0.0); // RED
            vec3 colorFinal = vec3(0.0, 0.0, 1.0); // BLUE
            // we want a gradient between red and blue
            color = mix(colorInici, colorFinal, v_uv.x);
        }
    }

    gl_FragColor = vec4(color, 1.0);
}