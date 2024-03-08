#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "hsss_lib.hpp"
#include "ArgParser.hpp"
#include "Util.hpp"

using Arg = ArgParser::Argument;
constexpr auto args = ArgParser::make_args(
    Arg('h', "help", ArgParser::ArgType::help),
    Arg('v', "version", ArgParser::ArgType::version),
    Arg('t', "text", ArgParser::ArgType::extended),
    Arg('e', "encrypt", ArgParser::ArgType::extended, 1),
    Arg('d', "decrypt", ArgParser::ArgType::extended, 1),
    Arg('r', "remove")
);

const char* help_msg = 
    "Hash Salt Shift by Suski encryption algorithm v1.0\n"
    "Usage: hsss [-e|-d] (password) [(filenames)|-t (text)]\n\n"
    "Available options:\n"
    " -e --encrypt   encrypts and sets the password\n"
    " -d --decrypt   decrypts and sets the password\n"
    " -r --remove    removes processed files\n"
    " -t --text      processes text instead of files. For encyrption its plain text, for decryption it should be in hex.\n"
    " -h --help      shows this message\n"
    " -v --version   shows version\n\n"
    "by Maciej Suski 2024";

const char* version_msg = 
    "Hash Salt Shift by Suski encryption algorithm v1.0\n"
    "by Maciej Suski 2024\n";


int main(int argc, char* argv[]) {
    auto ap = ArgParser::Parser(args, help_msg, version_msg);
    if(!ap.parse(argc, argv)) {
        return 1;
    }

    bool encrypt = ap.value('e') != nullptr;

    //we work on text given as an argument
    if(ap.value('t') != nullptr) {
        std::string text = ap.value('t');
        
        if(encrypt) {
            std::vector<uint8_t> out = hsss::encrypt(text.begin(), text.end(), ap.value('e'));
            std::cout << "Encrypted data (in hex):\n" << std::hex << std::setfill('0');
            for(auto n : out) {
                std::cout << std::setw(2) << (uint16_t)n;
            }
            std::cout << std::endl;
            return 0;
        }
        //decrypt
        std::vector<uint8_t> in;

        if(!from_hex(text, in)) {
            std::cout << "Invalid (non hex) character!\n";
            return 1;
        }

        std::vector<uint8_t> out = hsss::decrypt(in.begin(), in.end(), ap.value('d'));
        std::cout << "Decrypted data:\n";
        for(auto c : out) {
            std::cout << c;
        }
        std::cout << std::endl;
        return 0;
    }

    //we process files
    for(auto filename : ap.unnamed_args()) {
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if(!file) {
            std::cout << "Error! file " << filename << " cannot be opened for reading!\n";
            continue;
        }

        std::string ofilename(filename);
        if(encrypt) {
            ofilename += ".hsss";
        }
        else {
            //if ends with .hsss
            if(ofilename.length() > 5 &&
              0 == ofilename.compare(ofilename.length() - 5, 5, ".hsss")) {
                //remove the suffix
                ofilename = std::string(ofilename.begin(), ofilename.end() - 5);
            }
            else {
                //ask the user
                std::cout << "Enter the name of the file to decrypt " << ofilename << " to (leave blank to overwrite " << ofilename << "):\n";
                std::getline(std::cin, ofilename);
                if(ofilename == "")
                    ofilename = filename;
            }
        }
        
        std::ofstream ofile(ofilename, std::ios::out | std::ios::binary);
        if(!ofile) {
            std::cout << "Error! file " << ofilename << " could not be opened or created for write!\n";
            file.close();
            continue;
        }

        if(encrypt) {
            hsss::encrypt_stream(file, ap.value('e'), ofile);
        }
        else {
            hsss::decrypt_stream(file, ap.value('d'), ofile);
        }

        if(ap.set('r') && filename != ofilename) {
            std::remove(filename);
        }

        std::cout << "File " << filename << " successfully " << (encrypt ? "encrypted" : "decrypted") << " to " << ofilename << '\n';
    }
    return 0;
}
