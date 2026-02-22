#pragma once

#include <string.h>
#include <stdio.h>
#include <iostream>
#include "framework.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#endif

class FloatImage;

// lab1
// image represents cpu framebuffer storing rgb pixels
// used for drawing primitives and final display output
// lab3 extends usage with triangle rasterization and zbuffer interaction
class Image
{
    // lab1
    // internal tga loading helper storing image metadata and raw data pointer
    typedef struct sTGAInfo {
        unsigned int width;
        unsigned int height;
        unsigned int bpp;
        unsigned char* data;
    } TGAInfo;

public:
    // lab1
    // image resolution and color storage parameters
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int bytes_per_pixel = 3;

    // lab1
    // pointer to pixel array, stored row major
    Color* pixels = nullptr;

    // lab1
    // constructors and memory management functions
    Image();
    Image(unsigned int width, unsigned int height);
    Image(const Image& c);
    Image& operator = (const Image& c);
    ~Image();

    // lab1
    // upload framebuffer content to window or gpu texture
    void Render();

    // lab1
    // direct pixel access without bounds checking
    Color GetPixel(unsigned int x, unsigned int y) const { return pixels[y * width + x]; }
    Color& GetPixelRef(unsigned int x, unsigned int y) { return pixels[y * width + x]; }

    // lab1
    // safe pixel read with clamping to valid image range
    Color GetPixelSafe(unsigned int x, unsigned int y) const
    {
        x = clamp(x, 0u, width - 1);
        y = clamp(y, 0u, height - 1);
        return pixels[y * width + x];
    }

    // lab1
    // safe pixel write ignoring out of bounds coordinates
    void SetPixel(unsigned int x, unsigned int y, const Color& c)
    {
        if (x >= width) return;
        if (y >= height) return;
        pixels[y * width + x] = c;
    }

    // lab1
    // fast pixel write without bounds checks
    inline void SetPixelUnsafe(unsigned int x, unsigned int y, const Color& c)
    {
        pixels[y * width + x] = c;
    }

    // lab1
    // resize image allocating new pixel storage
    void Resize(unsigned int width, unsigned int height);

    // lab1
    // scale image to new resolution using resampling
    void Scale(unsigned int width, unsigned int height);

    // lab1
    // vertical flip used after loading textures or images
    void FlipY();

    // lab1
    // fill entire image with single color
    void Fill(const Color& c)
    {
        for (unsigned int pos = 0; pos < width * height; ++pos)
            pixels[pos] = c;
    }

    // lab1
    // copy rectangular region from image
    Image GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height);

    // lab1
    // load png image into pixel storage
    bool LoadPNG(const char* filename, bool flip_y = true);

    // lab1
    // load tga image optionally flipping vertically
    bool LoadTGA(const char* filename, bool flip_y = false);

    // lab1
    // save image as tga file
    bool SaveTGA(const char* filename);

    // lab1
    // primitive drawing functions for early labs
    void DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c);
    void DrawRect(int x, int y, int w, int h, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor);
    void DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor);
    void DrawCircle(int cx, int cy, int radius, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor);
    void DrawImage(const Image& image, int x, int y);

    // lab3
    // barycentric triangle rasterization with color interpolation and optional zbuffer
    void DrawTriangleInterpolated(
        const Vector3& p0, const Vector3& p1, const Vector3& p2,
        const Color& c0, const Color& c1, const Color& c2,
        FloatImage* zbuffer
    );

    // lab3
    // triangle rasterization with texture sampling and optional perspective correct uv interpolation
    void DrawTriangleInterpolated(
        const Vector3& p0, const Vector3& p1, const Vector3& p2,
        const Color& c0, const Color& c1, const Color& c2,
        FloatImage* zbuffer,
        Image* texture,
        const Vector2& uv0, const Vector2& uv1, const Vector2& uv2,
        bool interpolateUVs
    );

#ifndef IGNORE_LAMBDAS
    // lab1
    // apply callback function to every pixel and replace value
    template <typename F>
    Image& ForEachPixel(F callback)
    {
        for (unsigned int pos = 0; pos < width * height; ++pos)
            pixels[pos] = callback(pixels[pos]);
        return *this;
    }
#endif
};

// lab3
// floatimage used as depth buffer storing one float per pixel
// smaller depth means pixel closer to camera
class FloatImage
{
public:
    // lab3
    // depth buffer resolution and storage
    unsigned int width = 0;
    unsigned int height = 0;
    float* pixels = nullptr;

    FloatImage() {}
    FloatImage(unsigned int width, unsigned int height);
    FloatImage(const FloatImage& c);
    FloatImage& operator = (const FloatImage& c);
    ~FloatImage();

    // lab3
    // fill entire depth buffer with constant value
    void Fill(const float& v)
    {
        for (unsigned int pos = 0; pos < width * height; ++pos)
            pixels[pos] = v;
    }

    // lab3
    // direct pixel read and write access
    float GetPixel(unsigned int x, unsigned int y) const { return pixels[y * width + x]; }
    float& GetPixelRef(unsigned int x, unsigned int y) { return pixels[y * width + x]; }

    // lab3
    // safe pixel write with bounds check
    void SetPixel(unsigned int x, unsigned int y, const float& v)
    {
        if (x >= width) return;
        if (y >= height) return;
        pixels[y * width + x] = v;
    }

    // lab3
    // fast write without bounds checking
    inline void SetPixelUnsafe(unsigned int x, unsigned int y, const float& v)
    {
        pixels[y * width + x] = v;
    }

    // lab3
    // resize depth buffer allocating new storage
    void Resize(unsigned int width, unsigned int height);
};
