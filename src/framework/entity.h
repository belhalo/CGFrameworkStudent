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

class Entity {
    Mesh * mesh = new Mesh();
    mesh->loadOBJ("meshes/lee.obj");
};
