#ifndef _DECODE_H_
#define _DECODE_H_

#include "types.h"

#include <vector>
#include <string>

#include <fstream>
#include <iostream>

struct Instruction
{
    int size;
    int clocks;

    std::string mnemonic;
};

Instruction instructions[256] = {
    {1,  4,                  "NOP"}, {3, 12,            "LD BC,d16"}, {1,  8,            "LD (BC),A"}, {1,  8,               "INC BC"}, {1,  4,                "INC B"}, {1,  4,                "DEC B"}, {2,  8,              "LD B,d8"}, {1,  4,                 "RLCA"},
    {3, 20,          "LD (a16),SP"}, {1,  8,            "ADD HL,BC"}, {1,  8,            "LD A,(BC)"}, {1,  8,               "DEC BC"}, {1,  4,                "INC C"}, {1,  4,                "DEC C"}, {2,  8,              "LD C,d8"}, {1,  4,                 "RRCA"},
    {2,  4,               "STOP 0"}, {3, 12,            "LD DE,d16"}, {1,  8,            "LD (DE),A"}, {1,  8,               "INC DE"}, {1,  4,                "INC D"}, {1,  4,                "DEC D"}, {2,  8,              "LD D,d8"}, {1,  4,                  "RLA"},
    {2, 12,                "JR r8"}, {1,  8,            "ADD HL,DE"}, {1,  8,            "LD A,(DE)"}, {1,  8,               "DEC DE"}, {1,  4,                "INC E"}, {1,  4,                "DEC E"}, {2,  8,              "LD E,d8"}, {1,  4,                  "RRA"},
    {2,  8,             "JR NZ,r8"}, {3, 12,            "LD HL,d16"}, {1,  8,           "LD (HL+),A"}, {1,  8,               "INC HL"}, {1,  4,                "INC H"}, {1,  4,                "DEC H"}, {2,  8,              "LD H,d8"}, {1,  4,                  "DAA"},
    {2,  8,              "JR Z,r8"}, {1,  8,            "ADD HL,HL"}, {1,  8,           "LD A,(HL+)"}, {1,  8,               "DEC HL"}, {1,  4,                "INC L"}, {1,  4,                "DEC L"}, {2,  8,              "LD L,d8"}, {1,  4,                  "CPL"},
    {2,  8,             "JR NC,r8"}, {3, 12,            "LD SP,d16"}, {1,  8,           "LD (HL-),A"}, {1,  8,               "INC SP"}, {1, 12,             "INC (HL)"}, {1, 12,             "DEC (HL)"}, {2, 12,           "LD (HL),d8"}, {1,  4,                  "SCF"},
    {2,  8,              "JR C,r8"}, {1,  8,            "ADD HL,SP"}, {1,  8,           "LD A,(HL-)"}, {1,  8,               "DEC SP"}, {1,  4,                "INC A"}, {1,  4,                "DEC A"}, {2,  8,              "LD A,d8"}, {1,  4,                  "CCF"},
    {1,  4,               "LD B,B"}, {1,  4,               "LD B,C"}, {1,  4,               "LD B,D"}, {1,  4,               "LD B,E"}, {1,  4,               "LD B,H"}, {1,  4,               "LD B,L"}, {1,  8,            "LD B,(HL)"}, {1,  4,               "LD B,A"},
    {1,  4,               "LD C,B"}, {1,  4,               "LD C,C"}, {1,  4,               "LD C,D"}, {1,  4,               "LD C,E"}, {1,  4,               "LD C,H"}, {1,  4,               "LD C,L"}, {1,  8,            "LD C,(HL)"}, {1,  4,               "LD C,A"},
    {1,  4,               "LD D,B"}, {1,  4,               "LD D,C"}, {1,  4,               "LD D,D"}, {1,  4,               "LD D,E"}, {1,  4,               "LD D,H"}, {1,  4,               "LD D,L"}, {1,  8,            "LD D,(HL)"}, {1,  4,               "LD D,A"},
    {1,  4,               "LD E,B"}, {1,  4,               "LD E,C"}, {1,  4,               "LD E,D"}, {1,  4,               "LD E,E"}, {1,  4,               "LD E,H"}, {1,  4,               "LD E,L"}, {1,  8,            "LD E,(HL)"}, {1,  4,               "LD E,A"},
    {1,  4,               "LD H,B"}, {1,  4,               "LD H,C"}, {1,  4,               "LD H,D"}, {1,  4,               "LD H,E"}, {1,  4,               "LD H,H"}, {1,  4,               "LD H,L"}, {1,  8,            "LD H,(HL)"}, {1,  4,               "LD H,A"},
    {1,  4,               "LD L,B"}, {1,  4,               "LD L,C"}, {1,  4,               "LD L,D"}, {1,  4,               "LD L,E"}, {1,  4,               "LD L,H"}, {1,  4,               "LD L,L"}, {1,  8,            "LD L,(HL)"}, {1,  4,               "LD L,A"},
    {1,  8,            "LD (HL),B"}, {1,  8,            "LD (HL),C"}, {1,  8,            "LD (HL),D"}, {1,  8,            "LD (HL),E"}, {1,  8,            "LD (HL),H"}, {1,  8,            "LD (HL),L"}, {1,  4,                 "HALT"}, {1,  8,            "LD (HL),A"},
    {1,  4,               "LD A,B"}, {1,  4,               "LD A,C"}, {1,  4,               "LD A,D"}, {1,  4,               "LD A,E"}, {1,  4,               "LD A,H"}, {1,  4,               "LD A,L"}, {1,  8,            "LD A,(HL)"}, {1,  4,               "LD A,A"},
    {1,  4,              "ADD A,B"}, {1,  4,              "ADD A,C"}, {1,  4,              "ADD A,D"}, {1,  4,              "ADD A,E"}, {1,  4,              "ADD A,H"}, {1,  4,              "ADD A,L"}, {1,  8,           "ADD A,(HL)"}, {1,  4,              "ADD A,A"},
    {1,  4,              "ADC A,B"}, {1,  4,              "ADC A,C"}, {1,  4,              "ADC A,D"}, {1,  4,              "ADC A,E"}, {1,  4,              "ADC A,H"}, {1,  4,              "ADC A,L"}, {1,  8,           "ADC A,(HL)"}, {1,  4,              "ADC A,A"},
    {1,  4,                "SUB B"}, {1,  4,                "SUB C"}, {1,  4,                "SUB D"}, {1,  4,                "SUB E"}, {1,  4,                "SUB H"}, {1,  4,                "SUB L"}, {1,  8,             "SUB (HL)"}, {1,  4,                "SUB A"},
    {1,  4,              "SBC A,B"}, {1,  4,              "SBC A,C"}, {1,  4,              "SBC A,D"}, {1,  4,              "SBC A,E"}, {1,  4,              "SBC A,H"}, {1,  4,              "SBC A,L"}, {1,  8,           "SBC A,(HL)"}, {1,  4,              "SBC A,A"},
    {1,  4,                "AND B"}, {1,  4,                "AND C"}, {1,  4,                "AND D"}, {1,  4,                "AND E"}, {1,  4,                "AND H"}, {1,  4,                "AND L"}, {1,  8,             "AND (HL)"}, {1,  4,                "AND A"},
    {1,  4,                "XOR B"}, {1,  4,                "XOR C"}, {1,  4,                "XOR D"}, {1,  4,                "XOR E"}, {1,  4,                "XOR H"}, {1,  4,                "XOR L"}, {1,  8,             "XOR (HL)"}, {1,  4,                "XOR A"},
    {1,  4,                 "OR B"}, {1,  4,                 "OR C"}, {1,  4,                 "OR D"}, {1,  4,                 "OR E"}, {1,  4,                 "OR H"}, {1,  4,                 "OR L"}, {1,  8,              "OR (HL)"}, {1,  4,                 "OR A"},
    {1,  4,                 "CP B"}, {1,  4,                 "CP C"}, {1,  4,                 "CP D"}, {1,  4,                 "CP E"}, {1,  4,                 "CP H"}, {1,  4,                 "CP L"}, {1,  8,              "CP (HL)"}, {1,  4,                 "CP A"},
    {1,  8,               "RET NZ"}, {1, 12,               "POP BC"}, {3, 12,            "JP NZ,a16"}, {3, 16,               "JP a16"}, {3, 12,          "CALL NZ,a16"}, {1, 16,              "PUSH BC"}, {2,  8,             "ADD A,d8"}, {1, 16,              "RST 00H"},
    {1,  8,                "RET Z"}, {1, 16,                  "RET"}, {3, 12,             "JP Z,a16"}, {1,  4,            "PREFIX CB"}, {3, 12,           "CALL Z,a16"}, {3, 24,             "CALL a16"}, {2,  8,             "ADC A,d8"}, {1, 16,              "RST 08H"},
    {1,  8,               "RET NC"}, {1, 12,               "POP DE"}, {3, 12,            "JP NC,a16"}, {1,  4,  "Invalid instruction"}, {3, 12,          "CALL NC,a16"}, {1, 16,              "PUSH DE"}, {2,  8,               "SUB d8"}, {1, 16,              "RST 10H"},
    {1,  8,                "RET C"}, {1, 16,                 "RETI"}, {3, 12,             "JP C,a16"}, {1,  4,  "Invalid instruction"}, {3, 12,           "CALL C,a16"}, {1,  4,  "Invalid instruction"}, {2,  8,             "SBC A,d8"}, {1, 16,              "RST 18H"},
    {2, 12,           "LDH (a8),A"}, {1, 12,               "POP HL"}, {2,  8,             "LD (C),A"}, {1,  4,  "Invalid instruction"}, {1,  4,  "Invalid instruction"}, {1, 16,              "PUSH HL"}, {2,  8,               "AND d8"}, {1, 16,              "RST 20H"},
    {2, 16,            "ADD SP,r8"}, {1,  4,              "JP (HL)"}, {3, 16,           "LD (a16),A"}, {1,  4,  "Invalid instruction"}, {1,  4,  "Invalid instruction"}, {1,  4,  "Invalid instruction"}, {2,  8,               "XOR d8"}, {1, 16,              "RST 28H"},
    {2, 12,           "LDH A,(a8)"}, {1, 12,               "POP AF"}, {2,  8,             "LD A,(C)"}, {1,  4,                   "DI"}, {1,  4,  "Invalid instruction"}, {1, 16,              "PUSH AF"}, {2,  8,                "OR d8"}, {1, 16,              "RST 30H"},
    {2, 12,          "LD HL,SP+r8"}, {1,  8,             "LD SP,HL"}, {3, 16,           "LD A,(a16)"}, {1,  4,                   "EI"}, {1,  4,  "Invalid instruction"}, {1,  4,  "Invalid instruction"}, {2,  8,                "CP d8"}, {1, 16,              "RST 38H"},
 };

struct Symbol
{
    int bank;
    int address;

    std::string name;

    bool operator < (const Symbol& rhs) const
    {
        if ( bank < rhs.bank )
            return true;
        
        if ( bank > rhs.bank )
            return false;

        if ( address < rhs.address )
            return true;

        return false;
    }
};

sorted_vector<Symbol> symbols;

void load_symbols(const std::string& filename)
{


    for ( auto& s : symbols )
    {
        std::cout << s.bank << " " << s.address << " " << s.name << std::endl;    
    }
}

void decode()
{
    load_symbols("game.sym");
}

class Decoder
{
    public:

        Decoder() {}

        void load_symbols(const std::string& filename)
        {
            std::ifstream ifs(filename);

            if ( !ifs.is_open() )
                return;

            std::string line;

            while ( std::getline(ifs, line, '\n') )
            {
                if ( (line[0] != ';') && (line.size() > 8) )
                {
                    int bank    = std::stoi(line.substr(0, 2));
                    int address = std::stoi(line.substr(3, 4));
                    
                    std::string name = line.substr(8);

                    symbols.insert( {bank, address, name} );
                }
            }
        }

    private:

        sorted_vector<Symbol> symbols;

};

#endif
