#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"

// lab1
// image constructors set default size and allocate cpu pixel buffer
Image::Image()
{
    // lab1
    // zero size means no pixel allocation yet
    width = 0;
    height = 0;
    pixels = NULL;
}

// lab1
// allocate framebuffer with given resolution, clear pixels to zero
Image::Image(unsigned int width, unsigned int height)
{
    // lab1
    // store resolution and allocate rgb color array
    this->width = width;
    this->height = height;
    pixels = new Color[width * height];
    memset(pixels, 0, width * height * sizeof(Color));
}

// lab1
// deep copy, allocates new pixel buffer and copies content
Image::Image(const Image& c)
{
    // lab1
    // copy metadata first, pixels copied only if source exists
    pixels = NULL;
    width = c.width;
    height = c.height;
    bytes_per_pixel = c.bytes_per_pixel;

    if (c.pixels)
    {
        // lab1
        // allocate pixel buffer and copy raw bytes
        pixels = new Color[width * height];
        memcpy(pixels, c.pixels, width * height * bytes_per_pixel);
    }
}

// lab1
// assignment operator, frees old buffer then deep copies
Image& Image::operator=(const Image& c)
{
    // lab1
    // release old buffer to avoid leak
    if (pixels) delete[] pixels;
    pixels = NULL;

    width = c.width;
    height = c.height;
    bytes_per_pixel = c.bytes_per_pixel;

    if (c.pixels)
    {
        // lab1
        // allocate and copy raw pixel bytes
        pixels = new Color[width * height * bytes_per_pixel];
        memcpy(pixels, c.pixels, width * height * bytes_per_pixel);
    }
    return *this;
}

// lab1
// destructor releases cpu pixel buffer
Image::~Image()
{
    // lab1
    // delete safe on null guarded anyway
    if (pixels)
        delete[] pixels;
}

// lab1
// push cpu pixels to opengl backbuffer using glDrawPixels
void Image::Render()
{
    // lab1
    // alignment set to 1 so rows have no padding assumptions
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// lab1
// resize without resampling, keeps top left region from old buffer
void Image::Resize(unsigned int width, unsigned int height)
{
    // lab1
    // allocate new buffer, copy overlapping region, then replace pointer
    Color* new_pixels = new Color[width * height];
    unsigned int min_width = this->width > width ? width : this->width;
    unsigned int min_height = this->height > height ? height : this->height;

    for (unsigned int x = 0; x < min_width; ++x)
        for (unsigned int y = 0; y < min_height; ++y)
            new_pixels[y * width + x] = GetPixel(x, y);

    delete[] pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}

// lab1
// scale with nearest neighbor sampling
void Image::Scale(unsigned int width, unsigned int height)
{
    // lab1
    // allocate new buffer, sample from old coordinates, replace pointer
    Color* new_pixels = new Color[width * height];

    for (unsigned int x = 0; x < width; ++x)
        for (unsigned int y = 0; y < height; ++y)
            new_pixels[y * width + x] = GetPixel(
                (unsigned int)(this->width * (x / (float)width)),
                (unsigned int)(this->height * (y / (float)height))
            );

    delete[] pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}

// lab1
// extracts rectangular region and returns as new image
Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
    // lab1
    // bounds check prevents reading outside source image
    Image result(width, height);
    for (unsigned int x = 0; x < width; ++x)
        for (unsigned int y = 0; y < height; ++y)
            if ((x + start_x) < this->width && (y + start_y) < this->height)
                result.SetPixelUnsafe(x, y, GetPixel(x + start_x, y + start_y));
    return result;
}

// lab1
// flips image vertically, used for texture coordinate conventions
void Image::FlipY()
{
    // lab1
    // swap rows in place using temporary buffer
    int row_size = bytes_per_pixel * (int)width;
    Uint8* temp_row = new Uint8[row_size];

#pragma omp simd
    for (int y = 0; y < (int)(height * 0.5f); y += 1)
    {
        Uint8* pos = (Uint8*)pixels + y * row_size;
        memcpy(temp_row, pos, row_size);
        Uint8* pos2 = (Uint8*)pixels + ((int)height - y - 1) * row_size;
        memcpy(pos, pos2, row_size);
        memcpy(pos2, temp_row, row_size);
    }

    delete[] temp_row;
}

// lab1
// load png using picopng decoder, converts to 3 channels rgb
bool Image::LoadPNG(const char* filename, bool flip_y)
{
    // lab1
    // absResPath resolves relative resource path inside project
    std::string sfullPath = absResPath(filename);
    std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

    std::streamsize size = 0;
    if (file.seekg(0, std::ios::end).good()) size = file.tellg();
    if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

    if (!size)
    {
        std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
        return false;
    }

    // lab1
    // read whole file into memory buffer for decoder
    std::vector<unsigned char> buffer;
    buffer.resize((size_t)size);
    file.read((char*)(&buffer[0]), size);

    std::vector<unsigned char> out_image;

    if (decodePNG(out_image, width, height, &buffer[0], (unsigned long)buffer.size(), true) != 0)
    {
        std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
        return false;
    }

    size_t bufferSize = out_image.size();
    unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);

    // lab1
    // store as rgb only, alpha dropped if present
    bytes_per_pixel = 3;

    if (originalBytesPerPixel == 3)
    {
        // lab1
        // input already rgb, copy raw bytes
        if (pixels) delete[] pixels;
        pixels = new Color[bufferSize];
        memcpy(pixels, &out_image[0], bufferSize);
    }
    else if (originalBytesPerPixel == 4)
    {
        // lab1
        // input rgba, convert to rgb by skipping alpha
        if (pixels) delete[] pixels;

        unsigned int newBufferSize = width * height * bytes_per_pixel;
        pixels = new Color[newBufferSize];

        unsigned int k = 0;
        for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel)
        {
            pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
            k++;
        }
    }

    // lab1
    // optional vertical flip to match uv origin expectations
    if (flip_y) FlipY();

    std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;
    return true;
}

// lab1
// load uncompressed tga, supports 24 or 32 bpp, converts to rgb buffer
bool Image::LoadTGA(const char* filename, bool flip_y)
{
    // lab1
    // validate header matches expected uncompressed true color format
    unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char TGAcompare[12];
    unsigned char header[6];
    unsigned int imageSize;
    unsigned int bytesPerPixel;

    std::string sfullPath = absResPath(filename);

    FILE* file = fopen(sfullPath.c_str(), "rb");
    if (file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
        memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
        fread(header, 1, sizeof(header), file) != sizeof(header))
    {
        std::cerr << "--- File not found: " << sfullPath.c_str() << std::endl;
        if (file) fclose(file);
        return false;
    }

    TGAInfo* tgainfo = new TGAInfo;

    tgainfo->width = header[1] * 256 + header[0];
    tgainfo->height = header[3] * 256 + header[2];

    if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
    {
        std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
        fclose(file);
        delete tgainfo;
        return false;
    }

    tgainfo->bpp = header[4];
    bytesPerPixel = tgainfo->bpp / 8;
    imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;

    tgainfo->data = new unsigned char[imageSize];

    if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
    {
        std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
        if (tgainfo->data) delete[] tgainfo->data;
        fclose(file);
        delete tgainfo;
        return false;
    }

    fclose(file);

    // lab1
    // replace existing pixel buffer with new resolution
    if (pixels) delete[] pixels;

    width = tgainfo->width;
    height = tgainfo->height;
    pixels = new Color[width * height];

    // lab1
    // tga stores bgr or bgra, alpha ignored in this conversion
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
            if ((pos + 2) < imageSize)
            {
                SetPixelUnsafe(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
            }
        }
    }

    // lab1
    // optional second flip, useful depending on source tga orientation
    if (flip_y) FlipY();

    delete[] tgainfo->data;
    delete tgainfo;

    std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;
    return true;
}

// lab1
// save current rgb pixels to uncompressed 24 bit tga
bool Image::SaveTGA(const char* filename)
{
    // lab1
    // write minimal tga header and then raw bgr pixel data
    unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::string fullPath = absResPath(filename);
    FILE* file = fopen(fullPath.c_str(), "wb");
    if (file == NULL)
    {
        std::cerr << "--- Failed to save file: " << fullPath.c_str() << std::endl;
        return false;
    }

    unsigned short header_short[3];
    header_short[0] = (unsigned short)width;
    header_short[1] = (unsigned short)height;
    unsigned char* header = (unsigned char*)header_short;
    header[4] = 24;
    header[5] = 0;

    fwrite(TGAheader, 1, sizeof(TGAheader), file);
    fwrite(header, 1, 6, file);

    // lab1
    // tga expects bgr order, convert from stored rgb
    unsigned char* bytes = new unsigned char[width * height * 3];
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x)
        {
            Color c = pixels[y * width + x];
            unsigned int pos = (y * width + x) * 3;
            bytes[pos + 2] = c.r;
            bytes[pos + 1] = c.g;
            bytes[pos + 0] = c.b;
        }

    fwrite(bytes, 1, width * height * 3, file);
    fclose(file);

    delete[] bytes;

    std::cout << "+++ File saved: " << fullPath.c_str() << std::endl;
    return true;
}

// lab1
// helper for writing pixels with int coords and bounds checks
static inline void SetPixelSafeInt(Image* img, int x, int y, const Color& c)
{
    // lab1
    // ignore writes outside image bounds
    if (x < 0 || y < 0) return;
    if (x >= (int)img->width || y >= (int)img->height) return;
    img->SetPixel((unsigned int)x, (unsigned int)y, c);
}

// lab1
// dda line rasterization, works with any framebuffer size
void Image::DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c)
{
    // lab1
    // compute number of steps based on max axis delta
    int dx = x1 - x0;
    int dy = y1 - y0;
    int d = std::max(std::abs(dx), std::abs(dy));
    if (d == 0)
    {
        SetPixelSafeInt(this, x0, y0, c);
        return;
    }

    float xInc = (float)dx / (float)d;
    float yInc = (float)dy / (float)d;

    // lab1
    // step along segment, round to nearest pixel per step
    for (int i = 0; i <= d; ++i)
    {
        int x = (int)floorf(x0 + xInc * i);
        int y = (int)floorf(y0 + yInc * i);
        SetPixelSafeInt(this, x, y, c);
    }
}

// lab1
// rectangle drawing with optional fill and variable border thickness
void Image::DrawRect(int x, int y, int w, int h, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor)
{
    // lab1
    // reject invalid sizes
    if (w <= 0 || h <= 0) return;

    borderWidth = std::max(borderWidth, 1);

    // lab1
    // fill first so border remains visible on top
    if (isFilled)
    {
        for (int j = 0; j < h; ++j)
            for (int i = 0; i < w; ++i)
                SetPixelSafeInt(this, x + i, y + j, fillColor);
    }

    // lab1
    // draw border as four lines, expanded by thickness
    for (int t = 0; t < borderWidth; ++t)
    {
        DrawLineDDA(x - t,     y - t,     x + w + t, y - t,     borderColor);
        DrawLineDDA(x - t,     y - t,     x - t,     y + h + t, borderColor);
        DrawLineDDA(x + w + t, y - t,     x + w + t, y + h + t, borderColor);
        DrawLineDDA(x - t,     y + h + t, x + w + t, y + h + t, borderColor);
    }
}

// lab1
// triangle border plus optional fill using scanline between border hits
void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor)
{
    // lab1
    // draw triangle outline using dda lines
    DrawLineDDA((int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y, borderColor);
    DrawLineDDA((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, borderColor);
    DrawLineDDA((int)p2.x, (int)p2.y, (int)p0.x, (int)p0.y, borderColor);

    if (!isFilled) return;

    // lab1
    // bounding box limits fill work to local region
    int maxX = (int)std::ceil(std::max({ p0.x, p1.x, p2.x }));
    int maxY = (int)std::ceil(std::max({ p0.y, p1.y, p2.y }));
    int minX = (int)std::floor(std::min({ p0.x, p1.x, p2.x }));
    int minY = (int)std::floor(std::min({ p0.y, p1.y, p2.y }));

    minX = std::max(minX, 0);
    minY = std::max(minY, 0);
    maxX = std::min(maxX, (int)width - 1);
    maxY = std::min(maxY, (int)height - 1);

    // lab1
    // per scanline find border span, then fill interior pixels
    for (int y = minY; y <= maxY; ++y)
    {
        int borderMin = -1;
        int borderMax = -1;

        for (int x = minX; x <= maxX; ++x)
        {
            Color p = GetPixel((unsigned int)x, (unsigned int)y);
            if (p.r == borderColor.r && p.g == borderColor.g && p.b == borderColor.b)
            {
                if (borderMin == -1) borderMin = x;
                borderMax = x;
            }
        }

        if (borderMin >= 0 && borderMax > borderMin)
        {
            for (int x = borderMin + 1; x < borderMax; ++x)
                SetPixelSafeInt(this, x, y, fillColor);
        }
    }
}

// lab1
// blit another image into current image using safe pixel writes
void Image::DrawImage(const Image& image, int x, int y)
{
    // lab1
    // sample source using clamped reads to avoid out of range
    for (unsigned int j = 0; j < image.height; ++j)
        for (unsigned int i = 0; i < image.width; ++i)
        {
            Color c = image.GetPixelSafe(i, j);
            SetPixelSafeInt(this, x + (int)i, y + (int)j, c);
        }
}

// lab1
// circle symmetry helper, plots eight octant points with bounds checks
static inline void Plot8(Image* img, int cx, int cy, int x, int y, const Color& c)
{
    SetPixelSafeInt(img, cx + x, cy + y, c);
    SetPixelSafeInt(img, cx - x, cy + y, c);
    SetPixelSafeInt(img, cx + x, cy - y, c);
    SetPixelSafeInt(img, cx - x, cy - y, c);

    SetPixelSafeInt(img, cx + y, cy + x, c);
    SetPixelSafeInt(img, cx - y, cy + x, c);
    SetPixelSafeInt(img, cx + y, cy - x, c);
    SetPixelSafeInt(img, cx - y, cy - x, c);
}

// lab1
// draw horizontal span used for filled circle scanlines
static inline void DrawSpan(Image* img, int x0, int x1, int y, const Color& c)
{
    // lab1
    // clamp span to row and image bounds
    if (y < 0 || y >= (int)img->height) return;
    if (x0 > x1) std::swap(x0, x1);

    x0 = std::max(x0, 0);
    x1 = std::min(x1, (int)img->width - 1);

    for (int x = x0; x <= x1; ++x)
        img->SetPixel((unsigned int)x, (unsigned int)y, c);
}

// lab1
// midpoint circle algorithm, optional fill using spans, border thickness via concentric circles
void Image::DrawCircle(int cx, int cy, int radius, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor)
{
    // lab1
    // radius check avoids infinite loops and invalid memory writes
    if (radius <= 0) return;
    borderWidth = std::max(borderWidth, 1);

    if (isFilled)
    {
        // lab1
        // spans between symmetric points fill circle interior
        int x = radius;
        int y = 0;
        int d = 1 - radius;

        while (x >= y)
        {
            DrawSpan(this, cx - x, cx + x, cy + y, fillColor);
            DrawSpan(this, cx - x, cx + x, cy - y, fillColor);
            DrawSpan(this, cx - y, cx + y, cy + x, fillColor);
            DrawSpan(this, cx - y, cx + y, cy - x, fillColor);

            y++;

            if (d < 0) d += 2 * y + 1;
            else { x--; d += 2 * (y - x) + 1; }
        }
    }

    // lab1
    // border drawn as multiple circles shrinking radius for thickness
    for (int t = 0; t < borderWidth; ++t)
    {
        int r = radius - t;
        if (r <= 0) break;

        int x = r;
        int y = 0;
        int d = 1 - r;

        while (x >= y)
        {
            Plot8(this, cx, cy, x, y, borderColor);
            y++;
            if (d < 0) d += 2 * y + 1;
            else { x--; d += 2 * (y - x) + 1; }
        }
    }
}

// lab3
// floatimage stores depth values aligned to framebuffer resolution
FloatImage::FloatImage(unsigned int width, unsigned int height)
{
    // lab3
    // allocate and clear depth buffer values
    this->width = width;
    this->height = height;
    pixels = new float[width * height];
    memset(pixels, 0, width * height * sizeof(float));
}

// lab3
// deep copy of float buffer
FloatImage::FloatImage(const FloatImage& c)
{
    // lab3
    // copy dimensions, allocate new float buffer, copy values
    pixels = NULL;
    width = c.width;
    height = c.height;
    if (c.pixels)
    {
        pixels = new float[width * height];
        memcpy(pixels, c.pixels, width * height * sizeof(float));
    }
}

// lab3
// assignment operator for depth buffer
FloatImage& FloatImage::operator=(const FloatImage& c)
{
    // lab3
    // free old buffer, then deep copy from source
    if (pixels) delete[] pixels;
    pixels = NULL;

    width = c.width;
    height = c.height;
    if (c.pixels)
    {
        pixels = new float[width * height];
        memcpy(pixels, c.pixels, width * height * sizeof(float));
    }
    return *this;
}

// lab3
// destructor frees float buffer
FloatImage::~FloatImage()
{
    // lab3
    // delete safe on null guarded anyway
    if (pixels) delete[] pixels;
}

// lab3
// resize without resampling, keeps top left region of old depth buffer
void FloatImage::Resize(unsigned int width, unsigned int height)
{
    // lab3
    // allocate new buffer, copy overlap, replace pointer
    float* new_pixels = new float[width * height];

    unsigned int min_width = this->width > width ? width : this->width;
    unsigned int min_height = this->height > height ? height : this->height;

    for (unsigned int x = 0; x < min_width; ++x)
        for (unsigned int y = 0; y < min_height; ++y)
            new_pixels[y * width + x] = GetPixel(x, y);

    delete[] pixels;
    this->width = width;
    this->height = height;
    pixels = new_pixels;
}

// lab3
// edge function returns signed area, used for barycentric coordinates
static inline float EdgeFunction(const Vector2& a, const Vector2& b, const Vector2& c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

// lab3
// clamp int index to valid unsigned range
static inline unsigned int ClampUInt(int v, unsigned int lo, unsigned int hi)
{
    if (v < (int)lo) return lo;
    if (v > (int)hi) return hi;
    return (unsigned int)v;
}

// lab3
// color only rasterizer, forwards into texture path with texture null
void Image::DrawTriangleInterpolated(
    const Vector3& p0, const Vector3& p1, const Vector3& p2,
    const Color& c0, const Color& c1, const Color& c2,
    FloatImage* zbuffer
)
{
    DrawTriangleInterpolated(
        p0, p1, p2,
        c0, c1, c2,
        zbuffer,
        nullptr,
        Vector2(0.f, 0.f), Vector2(0.f, 0.f), Vector2(0.f, 0.f),
        false
    );
}

// lab3
// barycentric rasterizer, supports zbuffer and optional texture sampling
void Image::DrawTriangleInterpolated(
    const Vector3& P0, const Vector3& P1, const Vector3& P2,
    const Color& C0, const Color& C1, const Color& C2,
    FloatImage* zbuffer,
    Image* texture,
    const Vector2& UV0, const Vector2& UV1, const Vector2& UV2,
    bool interpolateUVs
)
{
    // lab3
    // use 2d positions for barycentric math, keep z separately for depth
    Vector2 p0(P0.x, P0.y);
    Vector2 p1(P1.x, P1.y);
    Vector2 p2(P2.x, P2.y);

    // lab3
    // bounding box reduces pixels tested to minimal rectangle
    int minX = (int)floorf(std::min({ p0.x, p1.x, p2.x }));
    int minY = (int)floorf(std::min({ p0.y, p1.y, p2.y }));
    int maxX = (int)ceilf (std::max({ p0.x, p1.x, p2.x }));
    int maxY = (int)ceilf (std::max({ p0.y, p1.y, p2.y }));

    if (width == 0 || height == 0) return;

    // lab3
    // clamp bounding box to framebuffer bounds
    minX = std::max(minX, 0);
    minY = std::max(minY, 0);
    maxX = std::min(maxX, (int)width - 1);
    maxY = std::min(maxY, (int)height - 1);

    if (minX > maxX || minY > maxY) return;

    // lab3
    // triangle area used for barycentric normalization, near zero means degenerate
    float area = EdgeFunction(p0, p1, p2);
    if (fabsf(area) < 1e-8f) return;

    // lab3
    // pixel center sampling reduces edge cracks between triangles
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            Vector2 p((float)x + 0.5f, (float)y + 0.5f);

            float w0 = EdgeFunction(p1, p2, p);
            float w1 = EdgeFunction(p2, p0, p);
            float w2 = EdgeFunction(p0, p1, p);

            // lab3
            // inside test supports both winding orders via area sign
            bool inside = (area > 0.0f) ? (w0 >= 0 && w1 >= 0 && w2 >= 0)
                                        : (w0 <= 0 && w1 <= 0 && w2 <= 0);
            if (!inside) continue;

            // lab3
            // normalize barycentric weights so sum equals 1
            w0 /= area;
            w1 /= area;
            w2 /= area;

            // lab3
            // interpolate ndc z for zbuffer test
            float z = w0 * P0.z + w1 * P1.z + w2 * P2.z;

            if (zbuffer)
            {
                float& zref = zbuffer->GetPixelRef((unsigned int)x, (unsigned int)y);
                if (z >= zref) continue;
                zref = z;
            }

            Color out;

            if (texture)
            {
                // lab3
                // uv either interpolated or held constant for interactivity toggle
                Vector2 uv;
                if (interpolateUVs)
                    uv = UV0 * w0 + UV1 * w1 + UV2 * w2;
                else
                    uv = UV0;

                // lab3
                // map uv range to texel coordinates, nearest sampling
                float txf = uv.x * (texture->width - 1);
                float tyf = uv.y * (texture->height - 1);

                int tx = (int)floorf(txf + 0.5f);
                int ty = (int)floorf(tyf + 0.5f);

                tx = (int)ClampUInt(tx, 0, texture->width - 1);
                ty = (int)ClampUInt(ty, 0, texture->height - 1);

                out = texture->GetPixel((unsigned int)tx, (unsigned int)ty);
            }
            else
            {
                // lab3
                // interpolate vertex colors linearly using barycentric weights
                float r = w0 * C0.r + w1 * C1.r + w2 * C2.r;
                float g = w0 * C0.g + w1 * C1.g + w2 * C2.g;
                float b = w0 * C0.b + w1 * C1.b + w2 * C2.b;
                out = Color(r, g, b);
            }

            SetPixel((unsigned int)x, (unsigned int)y, out);
        }
    }
}
