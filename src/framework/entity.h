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
#include "mesh.h"

// in entity class we will include mesh and model matrix
class Entity {
private:
    // (1) Create an Entity class which contains a mesh and a model matrix.
    // (2) Instantiate a new entity and assign a model matrix and a mesh.
    Mesh* mesh;
    Matrix44* modelMatrix;

public: 
    // entity constructor
    Entity() {
        this->mesh = new Mesh();
        this->modelMatrix = new Matrix44();
    }

};
