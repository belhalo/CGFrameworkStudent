#pragma once

#include "image.h"
#include "utils.h"   // where Vector2 lives

enum class ButtonType
{
    Clear,
    Load,
    Save,
    Eraser,
    Line,
    Rectangle,
    Triangle,
    Circle,
    Pencil,

    ColorBlack,
    ColorWhite,
    ColorPink,
    ColorYellow,
    ColorRed,
    ColorBlue,
    ColorCyan,
    ColorGreen
};

class Button {
public:
    ButtonType type;
    Vector2 pos;   // bottom-left in framebuffer coords
    Image icon;

    Button() = default;
    
    Button(ButtonType t, const Vector2& p, const Image& img)
        : type(t), pos(p), icon(img) {}

    bool IsMouseInside(const Vector2& m) const
    {
        return m.x >= pos.x && m.x < (pos.x + icon.width) &&
               m.y >= pos.y && m.y < (pos.y + icon.height);
    }

    void Render(Image& framebuffer) const
    {
        framebuffer.DrawImage(icon, (int)pos.x, (int)pos.y);
    }
};
