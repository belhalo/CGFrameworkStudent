/*  
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "button.h"

class Application
{
public:
    
    std::vector<Button> buttons;
    
	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;

	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);
    
    void HandleButton(ButtonType t);
    
    // Mouse position/buttons handling
    enum Tool { TOOL_LINE, TOOL_RECT, TOOL_TRIANGLE, TOOL_ERASER, TOOL_CIRCLE, TOOL_PENCIL};
    Tool currentTool = TOOL_PENCIL;

    Color borderColor = Color::BLACK;
    Color fillColor   = Color::BLACK;
    bool  fillShapes  = true;

    bool isDragging = false;
    Vector2 startPos;
    Vector2 currentPos;

    int triClicks = 0;
    Vector2 t0, t1;
    
    Vector2 MouseToCanvas(int mx, int my) const;
    
    // pencil tool
    bool pencilDown = false;
    Vector2 lastPencilPos;

	// CPU Global framebuffer
	Image framebuffer;

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init( void );
	void Render( void );
	void Update( float dt );

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
		this->framebuffer.Resize(width, height);
	}

	Vector2 GetWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(float(w), float(h));
	}
};
