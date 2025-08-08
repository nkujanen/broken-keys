#include "ui/ObjectGrid.h"

#include <windows.h>

// Divide client area to uniform grid for clean controls placement
void ObjectGrid::CalculateGrid(HWND hwnd, int client_width, int client_height, int column_count, int row_count, int margin) {
    // Get client dimensions if not passed as arguments
    if (client_width <= 0 | client_height <= 0) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        client_width = (clientRect.right - clientRect.left);
        client_height = (clientRect.bottom - clientRect.top);
    }

    margin_ = margin;
    column_count_ = column_count;
    row_count_ = row_count;

    // Calculate total amount taken by margins
    int margin_total_width = margin_ * (column_count + 1);
    int margin_total_height = margin_ * (row_count + 1);

    // Divide actual client are into cells specified by column and row counts
    cell_width_ = (client_width - margin_total_width) / column_count_;
    cell_height_ = (client_height - margin_total_height) / row_count_;
};

// Place object in specified grid cell
void ObjectGrid::PlaceObject(HWND hwnd, int column, int row, int column_span, int row_span) {
    // If column or row number exceeds maximum allowed value, place object on (0,0)
    if(column >= column_count_ | row >= row_count_) {
        column = 0;
        row = 0;
    }
    
    int x = margin_ + column * (cell_width_ + margin_);
    int y = margin_ + row * (cell_height_ + margin_);
    int width = column_span * cell_width_ + (column_span - 1) * margin_;
    int height = row_span * cell_height_ + (row_span - 1) * margin_;

    MoveWindow(hwnd, x, y, width, height, FALSE);
};