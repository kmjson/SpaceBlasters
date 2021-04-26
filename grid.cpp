// Copyright 2021 Jason Kim jasonk@bu.edu
// Copyright 2021 Harin Lee harinlee@bu.edu
// Copyright 2021 Raymond Lim oscar338@bu.edu
// Copyright 2021 Ji Young Chung jery1106@bu.edu

#include<iostream>
#include<string>
#include<vector>
using std::cout;
using std::cin;
using std::stoi;
using std::vector;
using std::string;

class Grid {
    public:
        bool on_grid(int x, int y);
        void update_grid(int x, int y, char c);
        void update_grid(int i, char c);
        char get_on_grid(int x, int y);
        char get_on_grid(int i);
        vector<char> get_grid();
        void replace_grid(vector<char> replace);
        bool clear();
        bool is_empty(int x, int y);
        void create(int x, int y, int size);
    private:
        int length;
        int x_pos;
        int y_pos;
        vector<char> grid;
};

// Creates a grid filled with 'o'.
void Grid::create(int x, int y, int size) {
    x_pos = x;
    y_pos = y;
    length = size;
    for (int i = 0; i < 100; i++) {
        grid.push_back('o');
    }
}

// Checks if a coordinate is on the grid.
bool Grid::on_grid(int x, int y) {
    return x < x_pos + length && x >= x_pos &&
           y < y_pos + length && y >= y_pos;
}

// If a coordinate is on the grid, check what char is on that coordinate.
char Grid::get_on_grid(int x, int y) {
    if (on_grid(x,y)) {
        int box_length = length/10;
        int col = (x-x_pos)/box_length;
        int row = (y-y_pos)/box_length;
        int index = (row*10) + col;
        return grid.at(index);
    }
    return '0';
}

// Check what char is on the grid based on index.
char Grid::get_on_grid(int i) {
    return grid.at(i);
}

// Update the grid by converting the coordinate into the index and put the input character.
void Grid::update_grid(int x, int y, char c) {
    if (on_grid(x,y)) {
        int box_length = length/10;
        int col = (x-x_pos)/box_length;
        int row = (y-y_pos)/box_length;
        int index = (row*10) + col;
        grid.at(index) = c;
    }
}

// Update the grid at index i and put the input character.
void Grid::update_grid(int i, char c) {
   grid.at(i) = c;
}

// Getter method for the grid.
vector<char> Grid::get_grid() {
    return grid;
}

// Replace grid with another grid.
void Grid::replace_grid(vector<char> replace) {
    for (int i = 0; i < 100; i++) {
        grid.at(i) = replace.at(i);
    }
}

// Clear the grid.
bool Grid::clear() {
    for (int i = 0; i < 100; i++) {
        grid.at(i) = 'o';
    }
    return true;
}

// Check if the grid at a given coordinate is 'o'.
bool Grid::is_empty(int x, int y) {
    if (on_grid(x,y)) {
        int box_length = length/10;
        int col = (x-x_pos)/box_length;
        int row = (y-y_pos)/box_length;
        int index = (row*10) + col;
        return grid.at(index) == 'o';
    }
    return false;
}