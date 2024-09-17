#include "Gol.h"

size_t Gol::get_num_neighbors(size_t x, size_t y) const {
    if (x >= this->horizontal_size || y >= this->vertical_size)
        throw std::runtime_error("Cell out of bounds line:" +
                                 std::to_string(__LINE__));

    size_t neighbor_start_x = (x != 0)              ? x - 1
                              : (this->wrap_around) ? this->horizontal_size - 1
                                                    : 0;
    size_t neighbor_start_y = (y != 0)              ? y - 1
                              : (this->wrap_around) ? this->vertical_size - 1
                                                    : 0;
    size_t neighbor_end_x = (x < (this->horizontal_size - 1)) ? x + 1
                            : (this->wrap_around)             ? 0
                                                              : x;
    size_t neighbor_end_y = (y < (this->vertical_size - 1)) ? y + 1
                            : (this->wrap_around)           ? 0
                                                            : y;
    size_t num_neighbors = 0;
    size_t neighbor_y = neighbor_start_y;
    while (neighbor_y != (neighbor_end_y + 1) % this->vertical_size) {
        size_t neighbor_x = neighbor_start_x;
        while (neighbor_x != (neighbor_end_x + 1) % this->horizontal_size) {
            if (neighbor_x == x && neighbor_y == y) {
                neighbor_x = (neighbor_x + 1) % this->horizontal_size;
                continue;
            }
            if (get_cell(neighbor_x, neighbor_y)) num_neighbors++;
            neighbor_x = (neighbor_x + 1) % this->horizontal_size;
        }
        neighbor_y = (neighbor_y + 1) % this->vertical_size;
    }
    return num_neighbors;
}

bool Gol::should_be_alive(size_t x, size_t y) const {
    if (x >= this->horizontal_size || y >= this->vertical_size)
        throw std::runtime_error("Cell out of bounds line" +
                                 std::to_string(__LINE__));
    const size_t num_neighbors = get_num_neighbors(x, y);
    if (get_cell(x, y)) {
        if (num_neighbors > this->alive_high || num_neighbors < this->alive_low)
            return false;
        return true;
    } else {
        if (num_neighbors > this->reproduction_high ||
            num_neighbors < reproduction_low)
            return false;
        return true;
    }
}

void Gol::set_cell(bool alive, size_t x, size_t y) {
    if (x >= this->horizontal_size || y >= this->vertical_size)
        throw std::runtime_error("Cell out of bounds line: " +
                                 std::to_string(__LINE__));
    this->cells_inactive[x + y * horizontal_size] = alive;
    return;
}

bool Gol::get_cell(size_t x, size_t y) const {
    if (x >= this->horizontal_size || y >= this->vertical_size)
        throw std::runtime_error("Cell out of bounds line: " +
                                 std::to_string(__LINE__));
    return this->cells_active[x + y * horizontal_size];
}

Gol::Gol(size_t horizontal_size, size_t vertical_size, std::string starting_file_path)
    : horizontal_size(horizontal_size), vertical_size(vertical_size) {
    this->cells_active = new bool[horizontal_size * vertical_size]{0};
    this->cells_inactive = new bool[horizontal_size * vertical_size]{0};
    if(starting_file_path != ""){
        init_field_RLE(starting_file_path);
    }
    else init_field_random(20);
}

Gol::~Gol() {
    delete[] cells_active;
    delete[] cells_inactive;
}

void Gol::init_field_random(unsigned char alive_chance_procent, size_t seed) {
    srand(seed);
    for (size_t y = 0; y < this->vertical_size; y++) {
        for (size_t x = 0; x < this->horizontal_size; x++) {
            set_cell(rand() % 100 < alive_chance_procent, x, y);
        }
    }
    std::swap(this->cells_active, this->cells_inactive);
}

void Gol::init_field_RLE(std::string filepath){
    std::ifstream input_file(filepath);
    if(!input_file.is_open()) throw std::runtime_error("Failed to get file");
    std::string currentInput;
    static const size_t x_start = 75;
    static const size_t y_start = 20;
    size_t x = x_start;
    size_t y = y_start;
    while(input_file >> currentInput){
        for (size_t i = 0; i < currentInput.size(); i++)
        {
            std::string number_string = "";
            while(currentInput[i] <= '9' && currentInput[i] >= '0'){
                number_string += currentInput[i];
                i++;
            }
            size_t cellcount = (number_string != "") ? std::stoul(number_string) : 1;
            switch (currentInput[i]){
                case 'o':
                    for (size_t j = 0; j < cellcount; j++)
                    {
                        set_cell(true, x++, y);
                        if(x >= this->horizontal_size) throw std::runtime_error("Board too small horizontal: " + std::to_string(__LINE__));
                    }
                    break;
                case 'b':
                    for (size_t j = 0; j < cellcount; j++)
                    {
                        set_cell(false, x++, y);
                        if(x >= this->horizontal_size) throw std::runtime_error("Board too small horizontal: " + std::to_string(__LINE__));
                    }
                    break;
                case '$':
                    y++;
                    if(y >= this->vertical_size) throw std::runtime_error("Board too small vertical: " + std::to_string(__LINE__));
                    x = x_start;
                    break;
                case '!':
                    std::swap(this->cells_active, this->cells_inactive);
                    return;
                default:
                    throw std::runtime_error("Invalid input file, Line: " + std::to_string(__LINE__));
            }
        }

    }
}


std::string Gol::get_board_string() const {
    std::string board_string = "";
    board_string.resize(this->vertical_size * (this->horizontal_size + 1) *
                        this->alive_cell_string.size());
    for (size_t y = 0; y < this->vertical_size; y++) {
        for (size_t x = 0; x < this->horizontal_size; x++) {
            board_string += (get_cell(x, y)) ? this->alive_cell_string
                                             : this->dead_cell_string;
        }
        board_string += '\n';
    }
    return board_string;
}

bool* Gol::get_board_array() const {
    bool* board_copy = new bool[this->vertical_size * this->horizontal_size];
    for (size_t y = 0; y < this->vertical_size; y++) {
        for (size_t x = 0; x < this->horizontal_size; x++) {
            board_copy[x + horizontal_size * y] = get_cell(x, y);
        }
    }
    return board_copy;
}

void Gol::do_tick() {
    for (size_t y = 0; y < this->vertical_size; y++) {
        for (size_t x = 0; x < this->horizontal_size; x++) {
            if (should_be_alive(x, y))
                set_cell(true, x, y);
            else
                set_cell(false, x, y);
        }
    }
    std::swap(this->cells_active, this->cells_inactive);
}