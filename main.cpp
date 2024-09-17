#include <chrono>
#include <iostream>
#include <thread>
#include <cstdlib>

#include "Gol.h"

int main(int argc, char* args[]) {

    Gol game(200, 50, "./light_weight_spaceship");
    float target_fps = 5;
    while (1) {
        std::system("clear");
        std::cout << game.get_board_string();
        auto frame_start_time = std::chrono::high_resolution_clock::now();
        game.do_tick();
        auto frame_end_time = std::chrono::high_resolution_clock::now();
        auto delta_time =
            std::chrono::duration(frame_end_time - frame_start_time);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(size_t(1000 / target_fps)) -
            std::chrono::duration_cast<std::chrono::milliseconds>(delta_time));
    }

}