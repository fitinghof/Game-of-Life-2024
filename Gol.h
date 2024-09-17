#ifndef GOL_H
#define GOL_H

#include <algorithm>
#include <string>
#include <cstddef>
#include <stdexcept>
#include <fstream>
#include <iostream>

class Gol
{
private:
    size_t horizontal_size;
    size_t vertical_size;
    bool wrap_around = false;

    std::string dead_cell_string = "\u25AF";
    std::string alive_cell_string = "\u25AE";

    bool* cells_active;
    bool* cells_inactive;

    size_t alive_low = 2;
    size_t alive_high = 3;

    size_t reproduction_low = 3;
    size_t reproduction_high = 3;

    size_t get_num_neighbors(size_t x, size_t y) const;
    bool should_be_alive(size_t x, size_t y) const;
    void set_cell(bool alive, size_t x, size_t y);
    bool get_cell(size_t x, size_t y) const;
public:
    Gol(size_t horizontal_size = 100, size_t vertical_size = 25, std::string starting_file_path = "");
    ~Gol();

    void init_field_random(unsigned char alive_chance_procent, size_t seed = 0);
    void init_field_RLE(std::string filepath);

    std::string get_board_string() const;
    bool* get_board_array() const;
    void do_tick();
};

#endif