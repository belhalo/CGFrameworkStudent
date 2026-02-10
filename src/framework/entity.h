#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "framework.h"
#include "image.h"
#include "mesh.h"
#include "camera.h"
#include "main/includes.h"

/*
    Entity (Lab2 + Lab3):
    - represents an object in the scene
    - holds:
        * Mesh* (geometry: vertices + uvs)
        * modelMatrix (transform local -> world)
        * baseMatrix (original placement, used so rotation keeps translation)
        * texture (Lab3)
    - render does pipeline:
        local -> world -> clip -> screen -> rasterize
*/


class Entity {
private:
    // lab2
    // mesh pointer so multiple entities can share same loaded mesh in memory
    Mesh* mesh = nullptr;

    // lab2
    // model matrix for this entity
    Matrix44 modelMatrix;

    // lab2
    // base matrix = original transform
    // used so when we rotate we still keep og translation
    Matrix44 baseMatrix;

public:
    // lab3
    // selectable render style
    enum class eRenderMode { WIREFRAME, TRIANGLES };
    
    // lab3
    // default (wireframe as debug)
    eRenderMode renderMode = eRenderMode::TRIANGLES;

    // lab3
    // toggles controlled by Application keys
    bool useTexture      = false;  // key 'T'
    bool useZBuffer      = true;   // key 'Z'
    bool interpolateUVs  = true;   // key 'C' (true=correct UV barycentric)

    // lab3
    // texture (loaded once and shared)
    Image* texture = nullptr;

    Entity() {
        modelMatrix.SetIdentity();
        baseMatrix.SetIdentity();
    }

    // lab2
    // initialize mesh pointer and starting transform
    void EntityAdd(Mesh* m, const Matrix44& M);

    // lab3
    // render needs a zbuffer pointer (may be null if occlusion disabled)
    void Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer);

    // lab2
    // animate by rotating over time
    void Update(float seconds_elapsed);

    // lab2/3
    // simple frustum reject in clip space
    bool isInsideClip(Vector3 v);
};
