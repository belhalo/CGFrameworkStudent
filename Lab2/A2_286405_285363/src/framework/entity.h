#pragma once
#include "framework.h"
#include "image.h"
#include "mesh.h"
#include "camera.h"

// lab2
// entity holds renderable object state, mesh pointer shared across instances, transform matrices define local to world placement

// lab3
// entity optionally holds texture pointer, render can use zbuffer and uv interpolation for textured triangle rasterization

class Entity {
private:
    // lab2
    // mesh pointer shared across entities, avoids loading same geometry multiple times
    Mesh* mesh = nullptr;

    // lab2
    // modelMatrix is current transform used for rendering, baseMatrix stores initial placement used for animation
    Matrix44 modelMatrix;
    Matrix44 baseMatrix;

    // lab2
    // helper for frustum reject in ndc, reject only when whole triangle is outside same plane
    // prevents partial triangles disappearing when only one vertex is outside
    static bool TriangleOutsideNDC(const Vector3& a, const Vector3& b, const Vector3& c);

public:
    // lab2
    // render mode for debugging or final raster, wireframe draws edges, triangles fills pixels
    enum class eRenderMode { WIREFRAME, TRIANGLES };
    eRenderMode renderMode = eRenderMode::TRIANGLES;

    // lab3
    // feature toggles controlled from application key input
    bool useTexture = false;     // 'T'
    bool useZBuffer = true;      // 'Z'
    bool interpolateUVs = true;  // 'C' true means correct uv interpolation

    // lab2
    // user controlled rotation applied on top of modelMatrix, updated from mouse drag
    float userYaw = 0.f;
    float userPitch = 0.f;
    void SetUserRotation(float yaw, float pitch) { userYaw = yaw; userPitch = pitch; }

    // lab2
    // time based animation state, phase offsets each entity to avoid identical motion
    float totalTime = 0.f;
    float phase = 0.f;

    // lab3
    // texture pointer, can be shared across entities, sampled using uv data in mesh
    Image* texture = nullptr;

    Entity()
    {
        // lab2
        // identity transforms before placement assigned in entityadd
        modelMatrix.SetIdentity();
        baseMatrix.SetIdentity();
    }

    // lab2
    // assign mesh pointer and initial transform, baseMatrix used to preserve translation while animating rotation
    void EntityAdd(Mesh* m, const Matrix44& M);

    // lab2
    // cpu render pipeline, local to world, world to ndc, ndc to screen, then wireframe or triangle raster
    // lab3 adds optional zbuffer and texture sampling
    void Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer);

    // lab2
    // update modelMatrix for animated mode, typically rotation over time using totalTime and phase
    void Update(float seconds_elapsed);
};
