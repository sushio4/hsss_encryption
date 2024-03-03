/*  Copyright (C) 2024  Maciej Suski  suskimaciej@interia.pl

    This code is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/



#pragma once
#include "ArgParser.hpp"
#include <iostream>

namespace ArgParser {

    template<typename Type, std::size_t Size>
    int Parser<Type, Size>::search_args(char arg) {
        for(std::size_t i = 0; i < Size; i++) {
            if(_args[i] == arg) return i;
        }
        std::cerr << "Unrecognized option: -" << arg << "\n\n";

        show_help(std::cerr);

        return -1;
    }   

    template<typename Type, std::size_t Size>
    int Parser<Type, Size>::search_args(const char* arg) {
        for(std::size_t i = 0; i < Size; i++) {
            if(_args[i] == arg) return i;
        }
        std::cerr << "Unrecognized option: --" << arg << "\n\n";

        show_help(std::cerr);
        
        return -1;
    }   

    template<typename Type, std::size_t Size>
    void Parser<Type, Size>::show_help(std::ostream& out) {
        if(_help == nullptr) return;
        out << _help;
    }

    template<typename Type, std::size_t Size>
    void Parser<Type, Size>::show_version() {
        if(_version == nullptr) return;
        std::cout << _version;
    }

    template<typename Type, std::size_t Size>
    void Parser<Type, Size>::show_special(ArgParser::ArgType type) {
        if(type == ArgParser::ArgType::help)
            show_help(std::cout);
        else if(type == ArgParser::ArgType::version)
            show_version();
    }

    template<typename Type, std::size_t Size>
    void Parser<Type, Size>::print_comp_group(int group) {

        bool first = true;

        for(std::size_t i = 0; i < Size; i++) {
            if(_args[i].compulsory_group != group)
                continue;

            if(first) {
                std::cerr << '-' << _args[i].flag << " |--" << _args[i].full_flag;
                first = false;
                continue;
            }
            std::cerr << ",  -" << _args[i].flag << " |--" << _args[i].full_flag;
        }
        std::cerr << "\n\n";
        show_help(std::cerr);
    }


    template<typename Type, std::size_t Size>
    bool Parser<Type, Size>::parse(int argc, char* argv[]) {
        /**array for checking compulsory groups
         * since groups start from 1, the 0 value tells whether to ignore the rest 
         */
        std::vector<bool> comp_satisfied(_comp_groups);

        for(int i = 1; i < argc; i++) {
            //check for unnamed args
            if(argv[i][0] != '-') {
                _unnamed.push_back(argv[i]);
                continue;
            }

            auto parse_arg = [&](int index) {
                using ArgType = ArgParser::ArgType;

                if(_args[index].type == ArgType::extended) {
                    i++;
                    //not provided
                    if(argc <= i) {
                        std::cerr << "Must provide a parameter after -" << _args[index].flag << "|--" <<
                            _args[index].full_flag << " option.\n\n";
                        show_help(std::cerr);
                        return false;
                    }
                    _values[index].str = argv[i];
                } 
                else {
                    _values[index].set++;
                }
                
                //special treatment for help/version
                if(_args[index].type == ArgType::help ||
                   _args[index].type == ArgType::version) {
                    comp_satisfied[0] = true;
                    
                    show_special(_args[index].type);

                    return true;
                }

                //compulsory group check
                if(_args[index].compulsory_group != 0) {
                    comp_satisfied[_args[index].compulsory_group] = true;
                }
                return true;
            };

            //check for long
            if(argv[i][1] == '-') {
                int index = search_args(argv[i] + 2);
                if(index == -1) return false;
                
                if(!parse_arg(index))
                    return false;
                continue;
            } 

            //short
            for(auto p = argv[i] + 1; *p; p++) {
                int index = search_args(*p);
                if(index == -1) return false;

                if(!parse_arg(index))
                    return false;
            }
        }

        //comp check
        if(comp_satisfied[0]) return true;

        for(int i = 1; i < _comp_groups; i++) {
            if(comp_satisfied[i]) 
                continue;

            std::cerr << "At least one of the following arguments has to be included:\n";
            print_comp_group(i);
            
            return false;      
        }
        return true;
    }

}