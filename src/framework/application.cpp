#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	// init the window colours
	framebuffer.Fill(Color::WHITE);
	framebuffer.DrawRect(0, 0, 1280, 50, Color::GRAY, 0, true, Color::GRAY);

	// init the toolbar
	Image clear;
	if (clear.LoadPNG("images\\clear.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(clear, 15, 10);

	Image load;
	if (load.LoadPNG("images\\load.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(load, 55, 10);

	Image save;
	if (save.LoadPNG("images\\save.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(save, 95, 10);

	Image eraser;
	if (eraser.LoadPNG("images\\eraser.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(eraser, 135, 10);

	Image line;
	if (line.LoadPNG("images\\line.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(line, 175, 10);

	Image rectangle;
	if (rectangle.LoadPNG("images\\rectangle.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(rectangle, 215, 10);

	Image circle;
	if (circle.LoadPNG("images\\circle.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(circle, 255, 10);

	Image triangle;
	if (triangle.LoadPNG("images\\triangle.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(triangle, 295, 10);

	Image black;
	if (black.LoadPNG("images\\black.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(black, 335, 10);

	Image white;
	if (white.LoadPNG("images\\white.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(white, 375, 10);

	Image pink;
	if (pink.LoadPNG("images\\pink.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(pink, 415, 10);

	Image yellow;
	if (yellow.LoadPNG("images\\yellow.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(yellow, 455, 10);

	Image red;
	if (red.LoadPNG("images\\red.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(red, 495, 10);

	Image blue;
	if (blue.LoadPNG("images\\blue.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(blue, 535, 10);

	Image cyan;
	if (cyan.LoadPNG("images\\cyan.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(cyan, 575, 10);

}

// Render one frame

	// ancho del rectangulo
	// sin() and cos()
void Application::Render(void)
{
	// remainder that w = 1280, h = 720
	// set up the window
	



	//framebuffer.DrawLineDDA(100, 100, 300, 300, Color::CYAN);
	//framebuffer.DrawLineDDA(230, 300, 230 + 100 * cos(time), 300 + 100 * sin(time), Color::CYAN);

	//framebuffer.DrawRect(100, 100, 45, 1280, Color::GRAY, 0, true, Color::GRAY);
	//framebuffer.DrawTriangle({ 100, 100 }, { 200, 200 }, { 300, 100 }, Color::BLUE, true, Color::RED);
	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{

}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// ...
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}