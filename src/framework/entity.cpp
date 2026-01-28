// to be made for lab2
#include "entity.h"
#include "camera.h"
#include "main/includes.h"
#include "mesh.h"

// it takes 3D points and apply a transformation to can project them into the 2D screen
void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
	// (1) get vertices from the mesh and iter throught them 
	// (2) in each iter:
		// - transform the vertices from local -> world space using ENTITY MODEL MATRIX - fet
		// - project each of the world space vertices to clip space position using your current camera
		// - make sure to render only the projected triangles that lay inside the cube [-1,1]^3
		// - before drawing each of the triangle lines, convert the clip space position to screen space using the framebuffer w,h

	// get vertices from the mesh and iter throught them 
	// notice that getVertices returns a list of 3D vectors
	std::vector<Vector3> meshVertices = this->mesh->GetVertices();
	for (int i = 0; i < meshVertices.size(); i++) {
		// as we want multiply each point of the mesh by the model matrix, firstly we have to isolated 

		// local position 
		Vector3 vecLocal = meshVertices[i];

		// world position = modelMatrix * local position
		Vector3 vecWorld = this->modelMatrix * (vecLocal);

		// clip view position (the camera nows sees the object, because is in the range between -1 to 1 in all the axis
		Vector3 vecClip = camera->ProjectVector(vecWorld);
		if (-1 > vecClip.x || vecClip.x > 1 || -1 > vecClip.y || vecClip.y > 1 || - 1 > vecClip.z || vecClip.z > 1) {
			continue;
		}
		//////////////////////////////////////////////////////////////
		///////////////////// TESTING THIS CODE///////////////////////
		// 2. Map Clip Space [-1, 1] to Screen Space [pixels]
		float screenX = (vecClip.x + 1.0f) * 0.5f * framebuffer->width;
		float screenY = (1.0f - (vecClip.y + 1.0f) * 0.5f) * framebuffer->height;

		// Now you have the pixel coordinates to draw!
		framebuffer->SetPixel(screenX, screenY, c);
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////

	}
}


/*
The framework contains a function Camera::ProjectVector(Vector3 pos) that 
already projects a vector using the camera’s viewprojection matrix.

camera.ProjectVector( pos );
*/