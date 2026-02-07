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

// entity class represents an object and it includes mesh and model matrix
class Entity {
private:
    // we declare mesh as a pointer because if we need to load more than one time the same mesh
    // at the memory we will not have x copies of the same mesh, in instead, we will have x number of the same mesh
    // but at the locally all of the them will be pointed with the same pointer 

    Mesh* mesh;             // stores the geometry information of the object itself
    Matrix44 modelMatrix;   // it defines the transformations of the object; such as the translation, location, rotation, scale...
    Matrix44 baseMatrix;
public:
    // entity constructor
    void EntityAdd(Mesh* m, const Matrix44& M);
    void Render(Image* framebuffer, Camera* camera, const Color& c);
    void Update(float seconds_elapsed);
    bool isInsideClip(Vector3 vect);
};
