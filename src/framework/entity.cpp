#include "entity.h"
#include "camera.h"
#include "main/includes.h"
#include "mesh.h"

// Render (Lab2 + Lab3 pipeline)
// for every triangle in the mesh:
//      1) Local -> World using modelMatrix
//      2) World -> Clip using camera->ProjectVector
//      3) Clip test in [-1,1]^3
//      4) Clip -> Screen pixel coords
//      5) Depending on renderMode:
//          - WIREFRAME: draw 3 edges
//          - TRIANGLES: draw filled using Image::DrawTriangleInterpolated
//              * barycentric interpolation
//              * Z-buffer occlusion
//              * texture sampling (UV interpolation)
void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer)
{
    if (!mesh || !camera || !framebuffer)
        return;

    const auto& V  = mesh->GetVertices();
    const auto& UV = mesh->GetUVs();

    // Lab3
    // can only do texture mapping if UVs exist for each vertex
    bool hasUV = (UV.size() == V.size());

    // loop every triangle (3 vertices at a time)
    for (size_t i = 0; i + 2 < V.size(); i += 3)
    {
        // 1) local -> world transform
        Vector3 w0 = modelMatrix * V[i + 0];
        Vector3 w1 = modelMatrix * V[i + 1];
        Vector3 w2 = modelMatrix * V[i + 2];

        // 2) world -> clip
        // camera->ProjectVector does viewprojection and perspective divide
        // Result should be in clip cube [-1,1]
        Vector3 c0 = camera->ProjectVector(w0);
        Vector3 c1 = camera->ProjectVector(w1);
        Vector3 c2 = camera->ProjectVector(w2);

        // 3) clip test (simple reject)
        if (!(isInsideClip(c0) && isInsideClip(c1) && isInsideClip(c2)))
            continue;

        // 4) clip -> screen
        // clip x,y in [-1,1] maps to pixel [0..W-1], [0..H-1]
        // framebuffer origin is bottom-left
        auto toScreen = [&](const Vector3& c) -> Vector3 {
            float sx = (c.x * 0.5f + 0.5f) * (framebuffer->width  - 1);
            float sy = (c.y * 0.5f + 0.5f) * (framebuffer->height - 1);

            // keep z in the Vector3 since need depth for zbuffer
            // use clip-space z here
            return Vector3(sx, sy, c.z);
        };

        Vector3 s0 = toScreen(c0);
        Vector3 s1 = toScreen(c1);
        Vector3 s2 = toScreen(c2);

        // 5) build vertex colors (for "color per vertex" mode)
        // 3 colors per triangle and barycentric interpolation
        // encode UV into color (so to see patterns)
        Color col0, col1, col2;
        if (hasUV)
        {
            Vector2 uv0 = UV[i + 0];
            Vector2 uv1 = UV[i + 1];
            Vector2 uv2 = UV[i + 2];

            // map uv in [0,1] to RGB in [0,255]
            col0 = Color(uv0.x * 255.f, uv0.y * 255.f, 128.f);
            col1 = Color(uv1.x * 255.f, uv1.y * 255.f, 128.f);
            col2 = Color(uv2.x * 255.f, uv2.y * 255.f, 128.f);
        }
        else
        {
            // fallback based on screen position
            col0 = Color(255.f * (s0.x / framebuffer->width), 255.f * (s0.y / framebuffer->height), 128.f);
            col1 = Color(255.f * (s1.x / framebuffer->width), 255.f * (s1.y / framebuffer->height), 128.f);
            col2 = Color(255.f * (s2.x / framebuffer->width), 255.f * (s2.y / framebuffer->height), 128.f);
        }

        // 6) wireframe mode
        if (renderMode == eRenderMode::WIREFRAME)
        {
            framebuffer->DrawLineDDA((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, Color::WHITE);
            continue;
        }

        // 7) triangle fill mode (Lab3)
        // - decide whether zbuffer is active
        // - decide whether texture is active
        FloatImage* zb = (useZBuffer ? zbuffer : nullptr);

        if (useTexture && texture && hasUV)
        {
            // Lab3
            // texturing
            const Vector2& uv0 = UV[i + 0];
            const Vector2& uv1 = UV[i + 1];
            const Vector2& uv2 = UV[i + 2];

            framebuffer->DrawTriangleInterpolated(
                s0, s1, s2,
                col0, col1, col2,       // colors are unused if texture != null
                zb,
                texture,
                uv0, uv1, uv2,
                interpolateUVs          // toggle correct vs wrong UV interpolation
            );
        }
        else
        {
            // Lab3
            // color barycentric interpolation
            // zbuffer occlusion if zb != null
            framebuffer->DrawTriangleInterpolated(
                s0, s1, s2,
                col0, col1, col2,
                zb
            );
        }
    }
}

// after projecting a world point with the camera we get clip coords in [-1,1]
// if a vertex is outside, the triangle is partially outside the frustum
// reject triangles when any vertex is outside

/*bool Entity::isInsideClip(Vector3 v)
{
    return (v.x >= -1.f && v.x <= 1.f) &&
           (v.y >= -1.f && v.y <= 1.f) &&
           (v.z >= -1.f && v.z <= 1.f);
}*/

bool Entity::isInsideClip(Vector3 v)
{
    return (v.x >= -1.f && v.x <= 1.f) &&
           (v.y >= -1.f && v.y <= 1.f);
    // don't test z for now
}

// called once during Init() in Application
// assigns mesh pointer and sets initial placement
void Entity::EntityAdd(Mesh* m, const Matrix44& M)
{
    mesh = m;
    baseMatrix = M;
    modelMatrix = M;
}

// Update (Lab2 animation):
// - total time increases each frame
// - build a rotation matrix around Y axis
// - modelMatrix = R * baseMatrix so:
//      * rotation changes with time
//      * translation from baseMatrix stays
void Entity::Update(float seconds_elapsed)
{
    static float total = 0.f;
    total += seconds_elapsed;

    Matrix44 R;
    R.MakeRotationMatrix(total, Vector3(0,1,0)); // radians

    modelMatrix = R * baseMatrix;
}
