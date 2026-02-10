/*
	+ This file defines the class Image that allows to manipulate images.
	+ It defines all the need operators for Color and Image
 
 - Stores a CPU framebuffer (Color per pixel)
 - Provides drawing primitives (line, rect, triangle, etc.)
 - Lab3 adds a triangle rasterizer using barycentric interpolation:
     * Interpolate vertex colors across the triangle
*/

#pragma once

#include <string.h>
#include <stdio.h>
#include <iostream>
#include "framework.h"

//remove unsafe warnings
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#endif

class FloatImage;
class Entity;
class Camera;

// Image: RGB framebuffer in CPU memory
// A matrix of pixels
class Image
{
	// A general struct to store all the information about a TGA file
	typedef struct sTGAInfo {
		unsigned int width;
		unsigned int height;
		unsigned int bpp; // Bits per pixel
		unsigned char* data; // Bytes with the pixel information
	} TGAInfo;

public:
	unsigned int width;
	unsigned int height;
	unsigned int bytes_per_pixel = 3; // Bits per pixel (rgb)

	Color* pixels;

	// Constructors / copy / assign
	Image();
	Image(unsigned int width, unsigned int height);
	Image(const Image& c);
	Image& operator = (const Image& c); // Assign operator

	// Destructor
	~Image();
    
    // sends CPU pixels to OpenGL window
	void Render();

	// Get the pixel at position x,y
	Color GetPixel(unsigned int x, unsigned int y) const { return pixels[ y * width + x ]; }
	Color& GetPixelRef(unsigned int x, unsigned int y)	{ return pixels[ y * width + x ]; }
    
    // safe version (clamps coords)
	Color GetPixelSafe(unsigned int x, unsigned int y) const {
		x = clamp((unsigned int)x, 0, width-1); 
		y = clamp((unsigned int)y, 0, height-1); 
		return pixels[ y * width + x ]; 
	}

	// Set the pixel at position x,y with value C (bounds check)
	void SetPixel(unsigned int x, unsigned int y, const Color& c) { if(x < 0 || x > width-1) return; if(y < 0 || y > height-1) return; pixels[ y * width + x ] = c; }
    // no bounds check
	inline void SetPixelUnsafe(unsigned int x, unsigned int y, const Color& c) { pixels[ y * width + x ] = c; }

    // resize without scaling content (keeps top left area)
	void Resize(unsigned int width, unsigned int height);
    
    // resize scaling existing content
	void Scale(unsigned int width, unsigned int height);
	
    // flip image vertically (for textures)
	void FlipY();

	// Fill the image with the color C
	void Fill(const Color& c) { for(unsigned int pos = 0; pos < width*height; ++pos) pixels[pos] = c; }

	// Returns a new image with the area from (startx,starty) of size width,height
    // extract sub-rectangle of image
	Image GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height);

	// Save or load images from the hard drive
	bool LoadPNG(const char* filename, bool flip_y = true);
	bool LoadTGA(const char* filename, bool flip_y = false);
	bool SaveTGA(const char* filename);

    // lab1
	// Draws lines (using DDA algorithm!)
	void DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c);

    // lab1
	// Draws a rectangle
	void DrawRect(int x, int y, int w, int h, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor);
    
	// Draws a triangle
	void DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor);
    
    // lab3
    // version1: no texture
    void DrawTriangleInterpolated(
        const Vector3& p0, const Vector3& p1, const Vector3& p2,
        const Color& c0, const Color& c1, const Color& c2,
        FloatImage* zbuffer
    );
    // version2: with texture
    // - If texture != nullptr and interpolateUVs=true:
    //      barycentrically interpolate UV per pixel, then sample texture
    // - If interpolateUVs=false:
    //      "wrong mode": uses only uv0 for the whole triangle
    void DrawTriangleInterpolated(
        const Vector3& p0, const Vector3& p1, const Vector3& p2,
        const Color& c0, const Color& c1, const Color& c2,
        FloatImage* zbuffer,
        Image* texture,
        const Vector2& uv0, const Vector2& uv1, const Vector2& uv2,
        bool interpolateUVs
    );
    
    // lab1
    // Draws a circle using Midpoint Circle algorithm
    void DrawCircle(int cx, int cy, int radius, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor);

	// Draws the selected image
	void DrawImage(const Image& image, int x, int y);

	// Used to easy code
	#ifndef IGNORE_LAMBDAS

	// Applies an algorithm to every pixel in an image
	// you can use lambda sintax:   img.forEachPixel( [](Color c) { return c*2; });
	// or callback sintax:   img.forEachPixel( mycallback ); //the callback has to be Color mycallback(Color c) { ... }
	template <typename F>
	Image& ForEachPixel( F callback )
	{
		for(unsigned int pos = 0; pos < width*height; ++pos)
			pixels[pos] = callback(pixels[pos]);
		return *this;
	}
	#endif
};

// Image storing one float per pixel instead of a 3 or 4 component Color
// 1 float per pixel (used as z-buffer in lab3)
class FloatImage
{
public:
	unsigned int width;
	unsigned int height;
	float* pixels;

	// CONSTRUCTORS 
	FloatImage() { width = height = 0; pixels = NULL; }
	FloatImage(unsigned int width, unsigned int height);
	FloatImage(const FloatImage& c);
	FloatImage& operator = (const FloatImage& c); //assign operator

	//destructor
	~FloatImage();

    // fill z-buffer with some big value
	void Fill(const float& v) { for (unsigned int pos = 0; pos < width * height; ++pos) pixels[pos] = v; }

	//get the pixel at position x,y
	float GetPixel(unsigned int x, unsigned int y) const { return pixels[y * width + x]; }
	float& GetPixelRef(unsigned int x, unsigned int y) { return pixels[y * width + x]; }

	//set the pixel at position x,y with value C
	void SetPixel(unsigned int x, unsigned int y, const float& v) { if (x < 0 || x > width - 1) return; if (y < 0 || y > height - 1) return; pixels[y * width + x] = v; }
	inline void SetPixelUnsafe(unsigned int x, unsigned int y, const float& v) { pixels[y * width + x] = v; }

    // keep the top left (like image::resize)
	void Resize(unsigned int width, unsigned int height);
};
