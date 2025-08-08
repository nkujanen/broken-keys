#pragma once

#include <windows.h>

class ObjectGrid {
    public:
        ObjectGrid() = default;

        void CalculateGrid(HWND hwnd, int client_width, int client_height, int column_count, int row_count, int margin);
        void PlaceObject(HWND hwnd, int column, int row, int column_span, int row_span);

    private:
        int cell_width_ = 0;
        int cell_height_ = 0;
        int column_count_ = 0;
        int row_count_ = 0;
        int margin_ = 0;
};