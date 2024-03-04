#pragma once
#include <cstdint>
#include <type_traits>
#include <random>
#include <algorithm>

namespace hsss {

    /**
     * Calculates polynomial of x where the bits of a are coefficients and adds a and b
    */
    uint8_t compress(uint8_t a, uint8_t b) {
        uint8_t res = 0;
        //the lowest bit
        uint8_t bit = 1;
        while(bit) {
            if(bit & a) {
                res += b;
            }
            b *= b;
            bit <<= 1;
        }
        return res + a + b;
    }

    /**
     * Takes container of uint8_t
    */
    template<typename Iter>
    uint8_t hash(Iter begin, Iter end) {        
        //implementation defined seed
        uint8_t current = 0xff;

        for(auto it = begin; it != end; ++it) {
            uint8_t block = static_cast<uint8_t>(*it);
            current = compress(current, block);
        }

        return current;
    }

    template<typename Iter>
    void generate_salt(Iter begin, Iter end) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<uint8_t> dist(0, 255);

        for(auto it = begin; it != end; ++it) {
            (*it) = dist(rng);
        }
    }

    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data, std::string password) {
        const std::size_t salt_size = 16;
        std::vector<uint8_t> result(salt_size);
        
        generate_salt(result.begin(), result.end());

        password.reserve(password.size() + salt_size);

        auto pbegin = password.begin();
        auto pend = password.end();
        //copy salt at the end of the password
        password.insert(password.end(), result.begin(), result.end());

        for(auto msg : data) {
            //hash salt shift but it's actually salt hash shift
            uint8_t hashed = hash(password.begin(), password.end());
            
            result.push_back(msg + hashed);
            
            std::rotate(pbegin, pbegin + 1, pend);
        }
        
        return result;
    }

    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data, std::string password) {
        const std::size_t salt_size = 16;
        std::vector<uint8_t> result;
        if(data.size() <= 16) return std::vector<uint8_t>(3,'x');
        result.reserve(data.size() - salt_size);
        
        password.reserve(password.size() + salt_size);
        
        auto pbegin = password.begin();
        auto pend = password.end();
        //copy salt at the end of the password
        password.insert(password.end(), data.begin(), data.begin() + salt_size);

        for(auto it = data.begin() + salt_size; it != data.end(); ++it) {
            //hash salt shift but it's actually salt hash shift
            uint8_t hashed = hash(password.begin(), password.end());
            
            result.push_back(*it - hashed);
            
            std::rotate(pbegin, pbegin + 1, pend);
        }

        return result;
    }
};