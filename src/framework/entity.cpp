#include "entity.h"
#include <algorithm>
#include <cmath>

// lab2
// frustum reject helper in ndc space
// triangle rejected only when all three vertices fall outside same clipping plane
// avoids dropping triangles that straddle frustum boundary
bool Entity::TriangleOutsideNDC(const Vector3& a, const Vector3& b, const Vector3& c)
{
    // lab2
    // each test checks one plane of ndc cube, range is -1 to 1 in x y z after projection and divide
    if (a.x < -1 && b.x < -1 && c.x < -1) return true; // left
    if (a.x >  1 && b.x >  1 && c.x >  1) return true; // right
    if (a.y < -1 && b.y < -1 && c.y < -1) return true; // bottom
    if (a.y >  1 && b.y >  1 && c.y >  1) return true; // top
    if (a.z < -1 && b.z < -1 && c.z < -1) return true; // near in ndc
    if (a.z >  1 && b.z >  1 && c.z >  1) return true; // far in ndc
    return false;
}

// lab2
// entity initialization, store shared mesh pointer and initial placement matrix
// baseMatrix kept for animation so translation stays stable
void Entity::EntityAdd(Mesh* m, const Matrix44& M)
{
    mesh = m;
    baseMatrix = M;
    modelMatrix = M;
}

// lab2
// simple animation, accumulate time and rotate around y axis
// phase offsets rotation between entities in multi mode
void Entity::Update(float dt)
{
    totalTime += dt;

    Matrix44 R;
    R.MakeRotationMatrix(totalTime + phase, Vector3(0, 1, 0));

    // lab2
    // rotate relative to base placement, preserves original translation and orientation fix
    modelMatrix = baseMatrix * R;
}

// lab2
// cpu render pipeline, local to world, world to ndc, ndc to screen
// wireframe draws triangle edges in screen space
// triangle fill path calls image triangle rasterizer
// lab3 adds optional zbuffer test and optional texture sampling with uv interpolation
void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer)
{
    // lab2
    // guard against null pointers
    if (!mesh || !camera || !framebuffer) return;

    // lab2
    // vertex arrays read in triangle order, three vertices per triangle
    const auto& V  = mesh->GetVertices();

    // lab3
    // uv array optional, texturing only valid when uv count matches vertex count
    const auto& UV = mesh->GetUVs();
    bool hasUV = (UV.size() == V.size());

    // lab2
    // user rotation applied from mouse input, computed once per frame for efficiency
    Matrix44 Ry; Ry.MakeRotationMatrix(userYaw,   Vector3(0, 1, 0));
    Matrix44 Rx; Rx.MakeRotationMatrix(userPitch, Vector3(1, 0, 0));

    // lab2
    // final model transform combines user rotation and animated placement
    Matrix44 M = (Ry * Rx) * modelMatrix;

    // lab2
    // main triangle loop, assume obj loader outputs triangles already
    for (size_t i = 0; i + 2 < V.size(); i += 3)
    {
        // lab2
        // local to world space transform
        Vector3 w0 = M * V[i + 0];
        Vector3 w1 = M * V[i + 1];
        Vector3 w2 = M * V[i + 2];

        // lab2
        // world to ndc using camera viewprojection and perspective divide
        Vector3 c0 = camera->ProjectVector(w0);
        Vector3 c1 = camera->ProjectVector(w1);
        Vector3 c2 = camera->ProjectVector(w2);

        // lab2
        // skip only when triangle fully outside ndc cube
        if (TriangleOutsideNDC(c0, c1, c2))
            continue;

        // lab2
        // ndc to screen mapping, x y from -1 1 into 0 width and 0 height
        // z kept in ndc for zbuffer compare
        auto toScreen = [&](const Vector3& c) -> Vector3 {
            float sx = (c.x * 0.5f + 0.5f) * (framebuffer->width  - 1);
            float sy = (c.y * 0.5f + 0.5f) * (framebuffer->height - 1);
            return Vector3(sx, sy, c.z);
        };

        Vector3 s0 = toScreen(c0);
        Vector3 s1 = toScreen(c1);
        Vector3 s2 = toScreen(c2);

        // lab2
        // wireframe mode, draws edges only, helpful for debugging transforms and clipping
        if (renderMode == eRenderMode::WIREFRAME)
        {
            framebuffer->DrawLineDDA((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, Color::WHITE);
            continue;
        }

        // lab3
        // zbuffer pointer passed only when enabled, null disables occlusion test
        FloatImage* zb = (useZBuffer ? zbuffer : nullptr);

        // lab3
        // texture path requires toggle enabled, texture loaded, and valid uv data
        if (useTexture && texture && hasUV)
        {
            const Vector2& uv0 = UV[i + 0];
            const Vector2& uv1 = UV[i + 1];
            const Vector2& uv2 = UV[i + 2];

            // lab3
            // vertex colors not used by texture path, placeholders passed to match function signature
            Color dummy0(255, 255, 255), dummy1(255, 255, 255), dummy2(255, 255, 255);

            // lab3
            // rasterize triangle with barycentric weights, optional uv interpolation and zbuffer
            framebuffer->DrawTriangleInterpolated(
                s0, s1, s2,
                dummy0, dummy1, dummy2,
                zb,
                texture,
                uv0, uv1, uv2,
                interpolateUVs
            );
        }
        else
        {
            // lab3
            // color interpolation path, useful for verifying barycentric interpolation and uv layout
            Color col0, col1, col2;

            // lab3
            // uv visualization mode, encodes uv into rgb for debugging
            if (hasUV)
            {
                Vector2 uv0 = UV[i + 0];
                Vector2 uv1 = UV[i + 1];
                Vector2 uv2 = UV[i + 2];

                col0 = Color(uv0.x * 255.f, uv0.y * 255.f, 128.f);
                col1 = Color(uv1.x * 255.f, uv1.y * 255.f, 128.f);
                col2 = Color(uv2.x * 255.f, uv2.y * 255.f, 128.f);
            }
            else
            {
                // lab2
                // fallback gradient based on screen coords when uv not present
                col0 = Color(255.f * (s0.x / framebuffer->width), 255.f * (s0.y / framebuffer->height), 128.f);
                col1 = Color(255.f * (s1.x / framebuffer->width), 255.f * (s1.y / framebuffer->height), 128.f);
                col2 = Color(255.f * (s2.x / framebuffer->width), 255.f * (s2.y / framebuffer->height), 128.f);
            }

            // lab3
            // rasterize triangle with barycentric color interpolation, optional zbuffer
            framebuffer->DrawTriangleInterpolated(s0, s1, s2, col0, col1, col2, zb);
        }
    }
}
