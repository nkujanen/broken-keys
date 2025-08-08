#pragma once

#include <windows.h>

enum class KeyType { white_, black_ };

class PianoKey {
public:

    PianoKey() = default;

    void SetIndex(int i);
    void SetRect(int x, int y, int width, int height);
    void SetType(KeyType new_type);

    int GetIndex();
    const RECT& GetRect() const;
    int GetX() const;
    KeyType GetType() const;

    bool ContainsPoint(POINT pt) const;
    void DrawKey(HDC hdc) const;

    void UpdateColour(bool left_click, bool right_click);

private:
    int index_ = 0;
    RECT rect_{};
    KeyType type_ = KeyType::white_;
    COLORREF colour_ = RGB(200, 200, 200);
};