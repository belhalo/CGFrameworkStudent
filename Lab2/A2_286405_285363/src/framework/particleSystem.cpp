#include "particleSystem.h"

#define windowWidth 1280
#define windowHeight 720

// we init the particles in random positions
void ParticleSystem::Init() {
    int widthWindow = 1280;
    int heightWindow = 720;
    for (int i = 0; i < this->MAX_PARTICLES; i++) {
        // for each of the particles, we init them into a random position between the dimensions of the window
        // also, we have to transform it into a float because (x,y) belong to Vector2D that has defined its positions as floats
        this->particles[i].position.x = (float)(rand() % widthWindow);
        this->particles[i].position.y = (float)(rand() % heightWindow);

        // set the velocity and acceleration
        this->particles[i].velocity = { 1.0, -30.0};
        this->particles[i].acceleration = 0.5;

        this->particles[i].color = Color::WHITE;

        // the inactive attr is a bool flag that will determine if the particle x is going to appers
        // as not all the particles appear at once!
        this->particles[i].inactive = false;
        this->particles[i].ttl = 0.01;
    }
}

// draw the particles in the framebuffer 
void ParticleSystem::Render(Image* framebuffer) {
    // max values of the window considering that we cannot paint in the toolbar
    int maxX = 1280;
    int minX = 0;
    int maxY = 50;
    int minY = 720;

    int currentX, currentY, lastX, lastY;
    // iter in all the particles
    for (int i = 0; i < this->MAX_PARTICLES; i++) {
        if (this->particles[i].inactive == false) {
            // we store the current position per each axis
            currentX = (int)this->particles[i].position.x;
            currentY = (int)this->particles[i].position.y;

            // we also store the old position per each axis -> we are using the formula to calculate it
            lastX = (int)(this->particles[i].position.x - this->particles[i].velocity.x * 0.1f);
            lastY = (int)(this->particles[i].position.y - this->particles[i].velocity.y * 0.1f);

            if (lastX >= minX && lastX < maxX && lastY > maxY && lastY < minY) {
                //we delete the part that we do not want by painting w/ the same colour as the window
                framebuffer->SetPixel(lastX, lastY, Color::BLACK);
            }

            // drawing the snow
            if (currentX >= minX && currentX < maxX && currentY > maxY && currentY < minY) {
                framebuffer->SetPixel(currentX, currentY, this->particles[i].color);
            }
        }
    }
}

// change some particle property
void ParticleSystem::Update(float dt){
    for (int i = 0; i < MAX_PARTICLES; i++) {
        // update the current position using the velocity, time and acceleration
        this->particles[i].position.x += this->particles[i].velocity.x * dt * this->particles[i].acceleration;
        this->particles[i].position.y += this->particles[i].velocity.y * dt * this->particles[i].acceleration;

        // we check some bounderies to see if it is out of the window and then charge again the position to apper in it
        // we also consider the toolbar to not draw in it
        if (this->particles[i].position.y < 50.0f) {
            this->particles[i].position.y = 719.0f; // goes up
            this->particles[i].position.x = (float)(rand() % 1280); // random position (as in the init)
        }

        // same as then but with the sides
        if (this->particles[i].position.x < 0){
            this->particles[i].position.x = 1279.0f;
        }

        if (this->particles[i].position.x > 1280) { 
            this->particles[i].position.x = 0.0f; 
        }
    }
}