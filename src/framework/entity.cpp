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
    const auto& meshVertices = this->mesh->GetVertices();
	// we can go 3 by 3 because we are counting three vertices for each triangle
	for (int i = 0; i < meshVertices.size(); i = i + 3) {
		// as we want multiply each point of the mesh by the model matrix, firstly we have to isolated 

		// local position - these are the three vertices that we are use to operate with them
		Vector3 vecLocal0 = meshVertices[i];
		Vector3 vecLocal1 = meshVertices[i + 1];
		Vector3 vecLocal2 = meshVertices[i + 2];

		// world position = modelMatrix * local position
		Vector3 vecWorld0 = this->modelMatrix * (vecLocal0);
		Vector3 vecWorld1 = this->modelMatrix * (vecLocal1);
		Vector3 vecWorld2 = this->modelMatrix * (vecLocal2);


		// clip view position (the camera nows sees the object, because is in the range between -1 to 1 in all the axis
		Vector3 vecClip0 = camera->ProjectVector(vecWorld0);
		Vector3 vecClip1 = camera->ProjectVector(vecWorld1);
		Vector3 vecClip2 = camera->ProjectVector(vecWorld2);

		// make sure that all the vectors are inside the clip space
		if (isInsideClip(vecClip0) && isInsideClip(vecClip1) && isInsideClip(vecClip2)){
			// map clip space [-1, 1] to Screen Space [pixels]
			float screenX0 = (vecClip0.x + 1.0f) * 0.5f * framebuffer->width;
			float screenY0 = (1.0f - (vecClip0.y + 1.0f) * 0.5f) * framebuffer->height;

			float screenX1 = (vecClip1.x + 1.0f) * 0.5f * framebuffer->width;
			float screenY1 = (1.0f - (vecClip1.y + 1.0f) * 0.5f) * framebuffer->height;

			float screenX2 = (vecClip2.x + 1.0f) * 0.5f * framebuffer->width;
			float screenY2 = (1.0f - (vecClip2.y + 1.0f) * 0.5f) * framebuffer->height;

			// Now you have the pixel coordinates to draw!
			framebuffer->DrawLineDDA(screenX0, screenY0, screenX1, screenY1, c);
			framebuffer->DrawLineDDA(screenX1, screenY1, screenX2, screenY2, c);
			framebuffer->DrawLineDDA(screenX2, screenY2, screenX0, screenY0, c);
		}
	}
}

bool Entity::isInsideClip(Vector3 vect) {
	// if the condition is satisfied -> function will return true, if not, false
	return (-1 < vect.x) && (vect.x < 1)
		&& (-1 < vect.y) && (vect.y < 1)
		&& (-1 < vect.z) && (vect.z < 1);
}

void Entity::EntityAdd(Mesh* m, const Matrix44& M)
{
    mesh = m;
    baseMatrix = M;
    modelMatrix = M;
}

void Entity::Update(float seconds_elapsed)
{
    static float total = 0.f;
    total += seconds_elapsed;

    Matrix44 R;
    R.MakeRotationMatrix(total, Vector3(0,1,0)); // radians

    modelMatrix = R * baseMatrix; // rotate but keep original translation
}

