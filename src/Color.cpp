#include "Color.h"

#define A 54059
#define B 76963
#define C 86969
#define MIN_COLOR 0
#define MAX_COLOR 256

unsigned int hash_str(std::string key) {
    unsigned int h = 31;
    for (int i = 0; i < key.size(); ++ i) {
        h = (h * A) ^ (key[i] * B);
    }
    return h;
}

unsigned int hash_str(std::string key, unsigned int min, unsigned int max) {
    return hash_str(key) % (max - min + 1) + min;
}

std::string colorText(std::string text, std::string key) {
    return std::string("\033[38;5;") + std::to_string(hash_str(key, MIN_COLOR, MAX_COLOR)) + "m" + text +
            + "\033[0m";
            //return std::string("\033[0m") + text;
}
