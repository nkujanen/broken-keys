#include "core/PianoKey.h"

#include <windows.h>

void PianoKey::SetIndex(int i) {
    index_ = i;
}

// Function to calculate key rect based on position and size
void PianoKey::SetRect(int x, int y, int width, int height) {
    rect_.left = x;
    rect_.top = y;
    rect_.right = x + width;
    rect_.bottom = y + height;
}

void PianoKey::SetType(KeyType new_type) {
    type_ = new_type;
}

int PianoKey::GetIndex() {
    return index_;
}

const RECT& PianoKey::GetRect() const {
    return rect_;
}

// Get the x position of key
int PianoKey::GetX() const {
    return rect_.left;
}

KeyType PianoKey::GetType() const {
    return type_;
}

// Function to check if point is contained within key
bool PianoKey::ContainsPoint(POINT pt) const {
    return PtInRect(&rect_, pt);
}

void PianoKey::DrawKey(HDC hdc) const {
    HBRUSH brush = CreateSolidBrush(colour_);
    FillRect(hdc, &rect_, brush);
    FrameRect(hdc, &rect_, (HBRUSH)GetStockObject(BLACK_BRUSH));
    DeleteObject(brush);
}

// Function to update key colour depending on clicked state
void PianoKey::UpdateColour(bool left_click, bool right_click) {
    if (left_click && right_click) {
        colour_ = RGB(0, 100, 100); // Both clicks
    } else if (left_click) {
        colour_ = RGB(100, 255, 255); // Only left click
    } else if (right_click) {
        colour_ = RGB(160, 160, 160); // Only right click
    } else {
        colour_ = (type_ == KeyType::white_) ? RGB(255, 255, 255) : RGB(0, 0, 0); // Default
    }
}