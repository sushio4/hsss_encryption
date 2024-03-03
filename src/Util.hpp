#pragma once
#include <cstdint>
#include <string>
#include <vector>

uint8_t from_hex(char c) {
    if(c >= '0' && c <= '9')
        return c - '0';
    if(c >= 'A' && c <= 'F')
        return c - 'A' + 0xA;
    if(c >= 'a' && c <= 'f') 
        return c - 'a' + 0xa;
    return 255;
}

bool from_hex(std::string s, std::vector<uint8_t>& out) {
    bool first = true;
    uint8_t n;
    for(char c : s) {
        if(first) {
            n = from_hex(c);
            if(n == 255) return false;
            //high nibble
            n <<= 4;
            first = false;
        }
        else {
            if(from_hex(c) == 255) return false;
            //low nibble
            n += from_hex(c);
            first = true;
            out.push_back(n);
        }
    }
    return true;
}