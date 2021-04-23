// Copyright 2021 J.Carruthers jbc@bu.edu

// socket programming using UDP and SFML
// g++ networking_send.cpp -l sfml-network -o sendmsg

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

void Grid::create(int x, int y, int size) {
    x_pos = x;
    y_pos = y;
    length = size;
    for (int i = 0; i < 100; i++) {
        grid.push_back('o');
    }
}

bool Grid::on_grid(int x, int y) {
    return x < x_pos + length && x >= x_pos &&
           y < y_pos + length && y >= y_pos;
}

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

char Grid::get_on_grid(int i) {
    return grid.at(i);
}

void Grid::update_grid(int x, int y, char c) {
    if (on_grid(x,y)) {
        int box_length = length/10;
        int col = (x-x_pos)/box_length;
        int row = (y-y_pos)/box_length;
        int index = (row*10) + col;
        grid.at(index) = c;
    }
}

void Grid::update_grid(int i, char c) {
   grid.at(i) = c;
}

vector<char> Grid::get_grid() {
    return grid;
}


void Grid::replace_grid(vector<char> replace) {
    for (int i = 0; i < 100; i++) {
        grid.at(i) = replace.at(i);
    }
}

bool Grid::clear() {
    for (int i = 0; i < 100; i++) {
        grid.at(i) = 'o';
    }
    return true;
}

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