#include "core/PianoKeyboard.h"

#include <windows.h>

#include <algorithm>
#include <bitset>
#include <tuple>
#include <vector>

#include "core/PianoKey.h"

PianoKeyboard::PianoKeyboard() {
    InitKeys();
}

int PianoKeyboard::GetOptimalTranspose() {
    return optimal_transpose_;
}

bool PianoKeyboard::GetEditLockStatus() {
    return edit_lock_;
}

// Set up key types and indices in correct pattern
void PianoKeyboard::InitKeys() {
    constexpr std::array<int, 12> kBlackKeyPattern = {0,1,0,0,1,0,1,0,0,1,0,1};

    for (int i = 0; i < kTotalKeysCount; ++i) {
        keys_[i].SetType(kBlackKeyPattern[i % 12] == 1
            ? KeyType::black_
            : KeyType::white_
        );
        keys_[i].SetIndex(i);
    }
}

// Set size and position of keys based on window size
void PianoKeyboard::LayoutKeys(int window_width, int window_height) {
    // Calculate key widths and lengths
    white_key_width_ = window_width / kWhiteKeysCount;
    white_key_height_ = white_key_width_ * 3;
    black_key_width_ = white_key_width_ * 2 / 3;
    black_key_height_ = black_key_width_ * 3;

    // Set first key at (0, 0)
    keys_[0].SetRect(
        0,
        0,
        white_key_width_,
        white_key_height_
    );

    // Maths to set the black keys between the white keys
    for (int i = 1; i < kTotalKeysCount; ++i){
        if (keys_[i].GetType() == KeyType::black_) {
            int x_position = keys_[i-1].GetX() + white_key_width_ - black_key_width_ / 2;
            keys_[i].SetRect(
                x_position,
                0,
                black_key_width_,
                black_key_height_
            );
        } else if (keys_[i-1].GetType() == KeyType::black_) {
            int x_position = keys_[i-1].GetX() + black_key_width_ / 2;
            keys_[i].SetRect(
                x_position,
                0,
                white_key_width_,
                white_key_height_
            );
        } else {
            int x_position = keys_[i-1].GetX() + white_key_width_;
            keys_[i].SetRect(
                x_position,
                0,
                white_key_width_,
                white_key_height_
            );
        }
    }
}

void PianoKeyboard::DrawKeys(HDC hdc) {
    // Sync key colours with click states
    for (int i = 0; i < kTotalKeysCount; ++i) {
        SyncColour(i);
    }

    // Draw white keys first
    for (const auto& key : keys_) {
        if (key.GetType() == KeyType::white_) {
            key.DrawKey(hdc);
        }
    }
    // Draw black keys on top
    for (const auto& key : keys_) {
        if (key.GetType() == KeyType::black_) {
            key.DrawKey(hdc);
        }
    }
}

// Check if there is a key under the cursor during clicking
PianoKey* PianoKeyboard::FindKeyAtPoint(POINT pt) {
    // Check black keys first as they lay on top of white keys
    for (auto& key : keys_) {
        if (key.GetType() == KeyType::black_ && key.ContainsPoint(pt)) {
            return &key;
        }
    }
    // Check white keys if black keys were not clicked
    for (auto& key : keys_) {
        if (key.GetType() == KeyType::white_ && key.ContainsPoint(pt)) {
            return &key;
        }
    }
    return nullptr;
}

// Store right and left clicks in their corresponding bitsets
void PianoKeyboard::HandleClickAtPoint(POINT pt, bool is_right_click, HWND hwnd) {
    PianoKey* key = FindKeyAtPoint(pt);
    // Exit if no key was clicked or if editing is disabled
    if (!key | edit_lock_) return;

    int key_index = key->GetIndex();
    if (is_right_click) {
        right_clicks_.flip(key_index);
    } else {
        left_clicks_.flip(key_index);
    }
    
    InvalidateRect(hwnd, &key->GetRect(), TRUE); // Only redraw clicked key
}

// Synchronise key colours with click state stored in bitsets
void PianoKeyboard::SyncColour(int key_index) {
    bool left_click = left_clicks_[key_index];
    bool right_click = right_clicks_[key_index];
    keys_[key_index].UpdateColour(left_click, right_click);
}

// Reset internal variables and allow editing if disabled
void PianoKeyboard::ResetKeys() {
    right_clicks_.reset();
    left_clicks_.reset();
    current_offset_ = 0;
    optimal_transpose_ = 0;
    edit_lock_ = false;
}

// Calculate the optimal transpose by minimising overlapped left and right clicked keys
void PianoKeyboard::OptimiseKeys() {
    // Exit if no keys are clicked or if editing is disabled
    if (left_clicks_.none() | edit_lock_) return;

    std::vector<std::tuple<int, int, int>> evaluator;
    int left_offset = 0;
    int right_offset = 0;
    current_offset_ = 0;

    // Calculate offset to left end of keyboard
    for (int i = 0; i < kTotalKeysCount; ++i) {
        if (left_clicks_[i]) {
            left_offset = i;
            break;
        }
    }

    // Calculate offset to right end of keyboard
    for (int i = kLastKeyIndex; i >= 0; --i) {
        if (left_clicks_[i]) {
            right_offset = kLastKeyIndex - i;
            break;
        }
    }

    // Transpose to left end of keyboard
    TransposeDown(left_offset);

    // Go through keyboard from left to right
    // Store left and right click overlap count
    for (int i = 0; i <= left_offset + right_offset; ++i) {
        int overlap = (right_clicks_ & left_clicks_).count();
        int distance = std::abs(current_offset_);
        evaluator.emplace_back(overlap, distance, current_offset_);

        TransposeUp(1);
    }

    // Transpose to starting point
    TransposeDown(right_offset);

    // Sort first by overlap count and second by offset
    std::sort(evaluator.begin(), evaluator.end());

    // Pick first element as optimal transpose
    optimal_transpose_ = std::get<2>(evaluator[0]);

    // Move to optimal transpose
    if (optimal_transpose_ < 0) {
        TransposeDown(-optimal_transpose_);
    } else {
        TransposeUp(optimal_transpose_);
    }

    // Disable editing
    edit_lock_ = true;
}

void PianoKeyboard::TransposeUp(int amount) {
    bool overflow = false;
    // Check that transpose does not cause overflow in bitset
    for (int i = kLastKeyIndex; i > kLastKeyIndex - amount; --i) {
        if (left_clicks_[i]) {
            overflow = true;
            break;
        }
    }
    if (overflow) return;

    left_clicks_ <<= amount;
    current_offset_ += amount;
}

void PianoKeyboard::TransposeDown(int amount) {
    bool overflow = false;
    // Check that transpose does not cause overflow in bitset
    for (int i = 0; i < amount; ++i) {
        if (left_clicks_[i]) {
            overflow = true;
            break;
        }
    }
    if (overflow) return;

    left_clicks_ >>= amount;
    current_offset_ -= amount;
}