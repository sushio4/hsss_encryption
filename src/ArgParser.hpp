//  This is a small library for parsing cli arguments

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
#include <array>
#include <vector>
#include <cstring>
#include <ostream>

/**
 * @brief namespace for argument parser
*/
namespace ArgParser {
    /**
     * @brief Enum for indicating types of arguments.
    */
    enum class ArgType : unsigned char {
        //for simple flags
        regular,
        //for options with additional string like "-f filename"
        extended,
        //for help option
        help,
        //for version option
        version
    };

    /**
     * @brief Contains argument declaration
    */
    struct Argument {
        //One letter flag, used with single dash, user may combine them like "-tFv"
        char flag = '\0';
        //Full flag, used with ouble dash like "--version"
        const char* full_flag = nullptr;
        //Type of the option
        ArgType type = ArgType::regular;
        /**
         * Create compulsory groups for enforcing arguments.
         * For each compulsory group, at least one argument in them must be present.
         * Exceptions are help and version flags, that disable compulsory_group checking.
         * Hence separate ArgType for them.
        */
        int compulsory_group = 0;

        constexpr inline Argument(char flag, const char* full_flag, ArgType type = ArgType::regular, int compulsory_group = 0) 
        : flag(flag), full_flag(full_flag), type(type), compulsory_group(compulsory_group) {}

        constexpr inline Argument(char flag, const char* full_flag, int compulsory_group) 
        : flag(flag), full_flag(full_flag), compulsory_group(compulsory_group) {}  


        friend inline constexpr bool operator==(const Argument& lhs, const char* rhs) {
            if(lhs.full_flag == nullptr || rhs == nullptr || rhs[0] == '\0') return false;
            //long form
            return strcmp(lhs.full_flag, rhs) == 0;
        }

        friend inline constexpr bool operator==(const Argument& lhs, char rhs) {
            return lhs.flag == rhs;
        }
    };

    template<typename T, typename... Ts>
    struct array_type {
        using type = T;
    };

    /**
     * Function for making array of Arguments to pass to Parser constructor
     * 
     * Example:
     * ```
     * using Arg = ArgParser::Argument;
     * using ArgType = ArgParser::ArgType;
     *
     * constexpr auto args = ArgParser::make_args(
     *     Arg('h', "help", ArgType::help),
     *     Arg('v', "version", ArgType::version),
     *     Arg('V', "verbose")
     * );
     *
     * ArgParser::Parser parser(args);
     * ```
    */
    template<typename... Ts>
    constexpr auto make_args(const Ts&&... values) {
        using array_type = typename array_type<Ts...>::type;
        return std::array<array_type, sizeof...(Ts)>{values...};
    }

    /**
     * @brief Main class of the parser namespace. It parses arguments
    */
    template<typename Type, std::size_t Size>
    class Parser {
    public:
        /**
         * @param args array of ArgParser::Argument structs. It is recommended to make it using ```make_args``` function
        */
        constexpr Parser(const std::array<Type, Size>& args, const char* help_msg = nullptr, const char* version_msg = nullptr) : 
            _args{args}, _values{0}, _comp_groups(0), _help(help_msg), _version(version_msg) {
            //find max comp_group number, help and version args
            for(auto a : _args) {
                if(a.compulsory_group > _comp_groups) 
                    _comp_groups = a.compulsory_group;

                if(a.type == ArgParser::ArgType::help)
                    _help_ref = &a;
        
                if(a.type == ArgParser::ArgType::version)
                    _version_ref = &a;
            }
            _comp_groups++;
        }

        /**
         * @return returns true on success, false otherwise
        */
        bool parse(int argc, char* argv[]);

        /**
         * @return returns how many times the flag was set
        */
        template<typename AType>
        int set(AType arg) {
            for(std::size_t i = 0; i < Size; i++) {
                if(!(_args[i] == arg)) 
                    continue;

                if(_args[i].type == ArgParser::ArgType::extended) 
                    return 0;

                return _values[i].set;
            }
            return 0;
        }

        /**
         * @return returns string after an extended flag (--flag string) or nullptr
        */
        template<typename AType>
        const char* value(AType arg) {
            for(std::size_t i = 0; i < Size; i++) {
                if(!(_args[i] == arg)) 
                    continue;

                if(_args[i].type != ArgType::extended) 
                    return nullptr;

                return _values[i].str;
            }
            return nullptr;
        }

        /**
         * @brief read unnamed arguments
         * @return reference to std::vector<char*> of non-flag parameter passed to the program
        */
        inline auto unnamed_args() -> const std::vector<char*>& {
            return _unnamed;
        }

    private:
        /**
         * @brief searches for matching argument
         * @return index in array or -1 on failure
        */
        int search_args(const char* arg);
        /**
         * @brief searches for matching argument
         * @return index in array or -1 on failure
        */
        int search_args(char arg);

        void show_help(std::ostream& out);
        void show_version();
        void print_comp_group(int group);

        void show_special(ArgParser::ArgType type);

        union ArgValue{
            int   set = 0;
            char* str;
        };

        std::array<Type, Size>      _args;
        std::array<ArgValue, Size>  _values;
        std::vector<char*>          _unnamed;

        Argument* _help_ref = nullptr;
        Argument* _version_ref = nullptr;
        
        int _comp_groups;

        const char* _help;
        const char* _version;
    };

}

#include "ArgParser_impl.hpp"
