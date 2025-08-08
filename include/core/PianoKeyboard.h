#pragma once

#include <array>
#include <bitset>

#include <windows.h>

#include "PianoKey.h"


class PianoKeyboard {
public:
    PianoKeyboard();

    int GetOptimalTranspose();
    bool GetEditLockStatus();

    void InitKeys();
    void LayoutKeys(int window_width, int window_height);
    void DrawKeys(HDC hdc);

    PianoKey* FindKeyAtPoint(POINT pt);
    void HandleClickAtPoint(POINT pt, bool is_right_click, HWND hwnd);
    void SyncColour(int key_index);

    void ResetKeys();
    void OptimiseKeys();
    void TransposeUp(int amount);
    void TransposeDown(int amount);

private:
    static constexpr int kTotalKeysCount = 88;
    static constexpr int kWhiteKeysCount = 52;
    static constexpr int kLastKeyIndex = kTotalKeysCount - 1;

    int white_key_width_ = 0;
    int white_key_height_ = 0;
    int black_key_width_ = 0;
    int black_key_height_ = 0;

    std::array<PianoKey, kTotalKeysCount> keys_{};

    std::bitset<kTotalKeysCount> left_clicks_;
    std::bitset<kTotalKeysCount> right_clicks_;

    int current_offset_ = 0;
    int optimal_transpose_ = 0;

    bool edit_lock_ = false;
};
