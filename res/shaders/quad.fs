varying vec2 v_uv;

uniform float u_subtask;
uniform float u_task;
uniform float u_aspect;
uniform float u_time;

uniform sampler2D u_texture;
uniform vec2 u_texel_size;

// clamp helper
float saturate(float x) { return clamp(x, 0.0, 1.0); }

// returns 1 if x in a b range else 0
float inRange(float x, float a, float b)
{
    return step(a, x) * (1.0 - step(b, x));
}

// texture sampling helper
vec3 sampleTex(vec2 uv)
{
    return texture2D(u_texture, uv).rgb;
}

// rgb to luma
float luminance(vec3 c)
{
    return dot(c, vec3(0.2126, 0.7152, 0.0722));
}

void main()
{
    vec2 uv = v_uv;

    // aspect correction around centre for distance based shapes
    vec2 p = uv - 0.5;
    p.x *= u_aspect;
    vec2 uv_aspect = p + 0.5;

    vec3 color = vec3(0.0);

    // task switching allowed
    if (u_task > 0.5 && u_task < 1.5)
    {
        // task 1 placeholder gradient
        vec3 leftC  = vec3(0.0, 0.0, 1.0);
        vec3 rightC = vec3(1.0, 0.0, 0.0);
        color = mix(leftC, rightC, uv.x);
    }
    else if (u_task > 1.5 && u_task < 2.5)
    {
        // subtask masks a to f
        float ma = inRange(u_subtask, 0.5, 1.5);
        float mb = inRange(u_subtask, 1.5, 2.5);
        float mc = inRange(u_subtask, 2.5, 3.5);
        float md = inRange(u_subtask, 3.5, 4.5);
        float me = inRange(u_subtask, 4.5, 5.5);
        float mf = step(5.5, u_subtask);

        // a blue to red gradient
        vec3 col_a = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), uv.x);

        // b radial grayscale
        float d = distance(uv_aspect, vec2(0.5, 0.5));
        float t = saturate(d / 0.75);
        float g = mix(0.05, 0.75, t);
        vec3 col_b = vec3(g);

        // c grid bars using fract and step
        float freq = 6.0;
        float xcell = abs(fract(uv.x * freq) - 0.5);
        float ycell = abs(fract(uv.y * freq) - 0.5);
        float vx = 1.0 - step(0.42, xcell);
        float hy = 1.0 - step(0.42, ycell);
        vec3 col_c = clamp(vec3(1.0, 0.0, 0.0) * vx + vec3(0.0, 0.0, 1.0) * hy, 0.0, 1.0);

        // d uv quantisation blocks
        float N = 16.0;
        vec2 q = floor(uv * N) / N;
        vec3 col_d = vec3(q.x, q.y, 0.0);

        // e checkerboard from cell parity
        vec2 c = floor(uv * N);
        float checker = mod(c.x + c.y, 2.0);
        vec3 col_e = mix(vec3(0.0), vec3(1.0), checker);

        // f sine hill mask plus green gradients
        float yCurve = 0.55 + 0.18 * sin(uv.x * 6.28318);
        float inside = 1.0 - step(yCurve, uv.y);
        float bg = mix(0.10, 0.35, 1.0 - uv.y);
        vec3 bgCol = vec3(0.0, bg, 0.0);
        float depth = saturate((yCurve - uv.y) / 0.4);
        float hillG = mix(0.25, 0.85, depth);
        vec3 hillCol = vec3(0.0, hillG, 0.0);
        vec3 col_f = mix(bgCol, hillCol, inside);

        // combine using masks no conditionals
        color =
            col_a * ma +
            col_b * mb +
            col_c * mc +
            col_d * md +
            col_e * me +
            col_f * mf;
    }
    else if (u_task > 2.5 && u_task < 3.5)
    {
        // subtask masks a to f
        float ma = inRange(u_subtask, 0.5, 1.5);
        float mb = inRange(u_subtask, 1.5, 2.5);
        float mc = inRange(u_subtask, 2.5, 3.5);
        float md = inRange(u_subtask, 3.5, 4.5);
        float me = inRange(u_subtask, 4.5, 5.5);
        float mf = step(5.5, u_subtask);

        // base sample reused for simple filters
        vec3 base = sampleTex(uv);

        // a original
        vec3 col_a = base;

        // b grayscale
        float l = luminance(base);
        vec3 col_b = vec3(l);

        // c invert
        vec3 col_c = vec3(1.0) - base;

        // d threshold on luma
        float th = step(0.5, l);
        vec3 col_d = vec3(th);

        // e blur 3x3 with larger radius
        float k = 3.0;
        vec2 t2 = u_texel_size * k;

        vec3 b0 = sampleTex(uv + t2 * vec2(-1.0, -1.0));
        vec3 b1 = sampleTex(uv + t2 * vec2( 0.0, -1.0));
        vec3 b2 = sampleTex(uv + t2 * vec2( 1.0, -1.0));
        vec3 b3 = sampleTex(uv + t2 * vec2(-1.0,  0.0));
        vec3 b4 = sampleTex(uv + t2 * vec2( 0.0,  0.0));
        vec3 b5 = sampleTex(uv + t2 * vec2( 1.0,  0.0));
        vec3 b6 = sampleTex(uv + t2 * vec2(-1.0,  1.0));
        vec3 b7 = sampleTex(uv + t2 * vec2( 0.0,  1.0));
        vec3 b8 = sampleTex(uv + t2 * vec2( 1.0,  1.0));

        vec3 col_e = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + b7 + b8) / 9.0;

        // f sobel edges on blurred neighbourhood
        float s0 = luminance(b0);
        float s1 = luminance(b1);
        float s2 = luminance(b2);
        float s3 = luminance(b3);
        float s4 = luminance(b4);
        float s5 = luminance(b5);
        float s6 = luminance(b6);
        float s7 = luminance(b7);
        float s8 = luminance(b8);

        float gx = (-1.0 * s0) + ( 1.0 * s2)
                 + (-2.0 * s3) + ( 2.0 * s5)
                 + (-1.0 * s6) + ( 1.0 * s8);

        float gy = (-1.0 * s0) + (-2.0 * s1) + (-1.0 * s2)
                 + ( 1.0 * s6) + ( 2.0 * s7) + ( 1.0 * s8);

        float edge = saturate(sqrt(gx * gx + gy * gy));
        vec3 col_f = vec3(edge);

        // combine using masks
        color =
            col_a * ma +
            col_b * mb +
            col_c * mc +
            col_d * md +
            col_e * me +
            col_f * mf;
    }
    else if (u_task > 3.5 && u_task < 4.5)
    {
        // subtask masks a to f
        float ma = inRange(u_subtask, 0.5, 1.5);
        float mb = inRange(u_subtask, 1.5, 2.5);
        float mc = inRange(u_subtask, 2.5, 3.5);
        float md = inRange(u_subtask, 3.5, 4.5);
        float me = inRange(u_subtask, 4.5, 5.5);
        float mf = step(5.5, u_subtask);

        // common centre for transforms
        vec2 center = vec2(0.5, 0.5);

        // a original
        vec3 col_a = texture2D(u_texture, uv).rgb;

        // b rotation with time
        vec2 p = uv - center;
        float ang = u_time;
        float c = cos(ang);
        float s = sin(ang);
        vec2 uv_b = vec2(p.x * c - p.y * s, p.x * s + p.y * c) + center;
        vec3 col_b = texture2D(u_texture, uv_b).rgb;

        // c pixelisation with time
        float scale = 8.0 + 48.0 * abs(sin(u_time));
        vec2 uv_c = floor(uv * scale) / scale;
        vec3 col_c = texture2D(u_texture, uv_c).rgb;

        // d wave warp with time
        vec2 uv_d = uv;
        uv_d.x += 0.03 * sin(uv.y * 20.0 + u_time * 2.0);
        uv_d.y += 0.03 * sin(uv.x * 16.0 + u_time * 1.7);
        vec3 col_d = texture2D(u_texture, uv_d).rgb;

        // e swirl with aspect correction and time
        vec2 q = uv - center;
        q.x *= u_aspect;
        float r = length(q);
        float swirl = 2.5 * (0.35 - r) * sin(u_time);
        float cs = cos(swirl);
        float sn = sin(swirl);
        vec2 q2 = vec2(q.x * cs - q.y * sn, q.x * sn + q.y * cs);
        q2.x /= u_aspect;
        vec2 uv_e = q2 + center;
        vec3 col_e = texture2D(u_texture, uv_e).rgb;

        // f zoom pulse with time
        float z = 1.0 + 0.35 * sin(u_time * 2.0);
        vec2 uv_f = (uv - center) / z + center;
        vec3 col_f = texture2D(u_texture, uv_f).rgb;

        // combine using masks
        color =
            col_a * ma +
            col_b * mb +
            col_c * mc +
            col_d * md +
            col_e * me +
            col_f * mf;
    }
    else
    {
        // fallback colour
        color = vec3(0.2, 0.0, 0.2);

    }

    gl_FragColor = vec4(color, 1.0);
}
