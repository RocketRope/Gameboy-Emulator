#ifndef _CPU_H_
#define _CPU_H_

// std
#include <array>

// el
#include <el/easylogging++.h>

// gbe
#include "mcu.h"
#include "types.h"
#include "registers.h"
#include "instructions.h"

class LR35902
{
    public:

        // Constructor/Destructor //

        LR35902(MCU& _mcu);
        ~LR35902();

        // Controll functions //

        void reset();
        void step();
        void run(uint16 break_pc_postion = 0x00);

        // Flags // 

        enum Flag : uint8
        {
            carry      = 0x10,
            half_carry = 0x20, 
            sub        = 0x40, 
            zero       = 0x80
        };

        bool get_flag(Flag flag);
        void set_flag(Flag flag, bool value = true);

        // interrupt enable flag // 
        bool interrupt_enable;

        Registers reg;


    private:

        el::Logger* log;

        MCU& mcu;

        // Cycles counters //
        uint64 t_cycles = 0;
        uint64 m_cycles = 0;
        
        //

        void execute_opcode(uint8 opcode);

        // Misc functions //

        void daa();
        void cpl();
        void ccf();
        void scf();
        void di();
        void ei();
        void stop();
        void halt();

        // Jump functions //

        void call(uint16 address);
        void ret();
        void rst(uint16 address);
        void jr(bool condition);

        // Load functions //

        uint16 pop();
        void   push(uint16 source);

        // ALU functions //

        void add_8bit(uint8 source, bool with_carry = false);
        void sub_8bit(uint8 source, bool with_carry = false);
        void inc_8bit(uint8& source);
        void dec_8bit(uint8& source);

        void add_16bit(uint16 source);
        void inc_16bit(uint16& source);
        void dec_16bit(uint16& source);
        
        void and_8bit(uint8 source);
        void or_8bit(uint8 source);
        void xor_8bit(uint8 source);
        void cp_8bit(uint8 source);

        // Rotates & Shifts functions //

        void rotate_left_8bit(uint8& source, bool through_carry);
        void rotate_right_8bit(uint8& source, bool through_carry);

        // Array of instruction functions pointers //

        void (LR35902::*instructions[0xFF + 1])(void) = 
        {
            &LR35902::instr_0x00, &LR35902::instr_0x01, &LR35902::instr_0x02, &LR35902::instr_0x03,
            &LR35902::instr_0x04, &LR35902::instr_0x05, &LR35902::instr_0x06, &LR35902::instr_0x07,
            &LR35902::instr_0x08, &LR35902::instr_0x09, &LR35902::instr_0x0A, &LR35902::instr_0x0B,
            &LR35902::instr_0x0C, &LR35902::instr_0x0D, &LR35902::instr_0x0E, &LR35902::instr_0x0F,
            &LR35902::instr_0x10, &LR35902::instr_0x11, &LR35902::instr_0x12, &LR35902::instr_0x13,
            &LR35902::instr_0x14, &LR35902::instr_0x15, &LR35902::instr_0x16, &LR35902::instr_0x17,
            &LR35902::instr_0x18, &LR35902::instr_0x19, &LR35902::instr_0x1A, &LR35902::instr_0x1B,
            &LR35902::instr_0x1C, &LR35902::instr_0x1D, &LR35902::instr_0x1E, &LR35902::instr_0x1F,
            &LR35902::instr_0x20, &LR35902::instr_0x21, &LR35902::instr_0x22, &LR35902::instr_0x23,
            &LR35902::instr_0x24, &LR35902::instr_0x25, &LR35902::instr_0x26, &LR35902::instr_0x27,
            &LR35902::instr_0x28, &LR35902::instr_0x29, &LR35902::instr_0x2A, &LR35902::instr_0x2B,
            &LR35902::instr_0x2C, &LR35902::instr_0x2D, &LR35902::instr_0x2E, &LR35902::instr_0x2F,
            &LR35902::instr_0x30, &LR35902::instr_0x31, &LR35902::instr_0x32, &LR35902::instr_0x33,
            &LR35902::instr_0x34, &LR35902::instr_0x35, &LR35902::instr_0x36, &LR35902::instr_0x37,
            &LR35902::instr_0x38, &LR35902::instr_0x39, &LR35902::instr_0x3A, &LR35902::instr_0x3B,
            &LR35902::instr_0x3C, &LR35902::instr_0x3D, &LR35902::instr_0x3E, &LR35902::instr_0x3F,
            &LR35902::instr_0x40, &LR35902::instr_0x41, &LR35902::instr_0x42, &LR35902::instr_0x43,
            &LR35902::instr_0x44, &LR35902::instr_0x45, &LR35902::instr_0x46, &LR35902::instr_0x47,
            &LR35902::instr_0x48, &LR35902::instr_0x49, &LR35902::instr_0x4A, &LR35902::instr_0x4B,
            &LR35902::instr_0x4C, &LR35902::instr_0x4D, &LR35902::instr_0x4E, &LR35902::instr_0x4F,
            &LR35902::instr_0x50, &LR35902::instr_0x51, &LR35902::instr_0x52, &LR35902::instr_0x53,
            &LR35902::instr_0x54, &LR35902::instr_0x55, &LR35902::instr_0x56, &LR35902::instr_0x57,
            &LR35902::instr_0x58, &LR35902::instr_0x59, &LR35902::instr_0x5A, &LR35902::instr_0x5B,
            &LR35902::instr_0x5C, &LR35902::instr_0x5D, &LR35902::instr_0x5E, &LR35902::instr_0x5F,
            &LR35902::instr_0x60, &LR35902::instr_0x61, &LR35902::instr_0x62, &LR35902::instr_0x63,
            &LR35902::instr_0x64, &LR35902::instr_0x65, &LR35902::instr_0x66, &LR35902::instr_0x67,
            &LR35902::instr_0x68, &LR35902::instr_0x69, &LR35902::instr_0x6A, &LR35902::instr_0x6B,
            &LR35902::instr_0x6C, &LR35902::instr_0x6D, &LR35902::instr_0x6E, &LR35902::instr_0x6F,
            &LR35902::instr_0x70, &LR35902::instr_0x71, &LR35902::instr_0x72, &LR35902::instr_0x73,
            &LR35902::instr_0x74, &LR35902::instr_0x75, &LR35902::instr_0x76, &LR35902::instr_0x77,
            &LR35902::instr_0x78, &LR35902::instr_0x79, &LR35902::instr_0x7A, &LR35902::instr_0x7B,
            &LR35902::instr_0x7C, &LR35902::instr_0x7D, &LR35902::instr_0x7E, &LR35902::instr_0x7F,
            &LR35902::instr_0x80, &LR35902::instr_0x81, &LR35902::instr_0x82, &LR35902::instr_0x83,
            &LR35902::instr_0x84, &LR35902::instr_0x85, &LR35902::instr_0x86, &LR35902::instr_0x87,
            &LR35902::instr_0x88, &LR35902::instr_0x89, &LR35902::instr_0x8A, &LR35902::instr_0x8B,
            &LR35902::instr_0x8C, &LR35902::instr_0x8D, &LR35902::instr_0x8E, &LR35902::instr_0x8F,
            &LR35902::instr_0x90, &LR35902::instr_0x91, &LR35902::instr_0x92, &LR35902::instr_0x93,
            &LR35902::instr_0x94, &LR35902::instr_0x95, &LR35902::instr_0x96, &LR35902::instr_0x97,
            &LR35902::instr_0x98, &LR35902::instr_0x99, &LR35902::instr_0x9A, &LR35902::instr_0x9B,
            &LR35902::instr_0x9C, &LR35902::instr_0x9D, &LR35902::instr_0x9E, &LR35902::instr_0x9F,
            &LR35902::instr_0xA0, &LR35902::instr_0xA1, &LR35902::instr_0xA2, &LR35902::instr_0xA3,
            &LR35902::instr_0xA4, &LR35902::instr_0xA5, &LR35902::instr_0xA6, &LR35902::instr_0xA7,
            &LR35902::instr_0xA8, &LR35902::instr_0xA9, &LR35902::instr_0xAA, &LR35902::instr_0xAB,
            &LR35902::instr_0xAC, &LR35902::instr_0xAD, &LR35902::instr_0xAE, &LR35902::instr_0xAF,
            &LR35902::instr_0xB0, &LR35902::instr_0xB1, &LR35902::instr_0xB2, &LR35902::instr_0xB3,
            &LR35902::instr_0xB4, &LR35902::instr_0xB5, &LR35902::instr_0xB6, &LR35902::instr_0xB7,
            &LR35902::instr_0xB8, &LR35902::instr_0xB9, &LR35902::instr_0xBA, &LR35902::instr_0xBB,
            &LR35902::instr_0xBC, &LR35902::instr_0xBD, &LR35902::instr_0xBE, &LR35902::instr_0xBF,
            &LR35902::instr_0xC0, &LR35902::instr_0xC1, &LR35902::instr_0xC2, &LR35902::instr_0xC3,
            &LR35902::instr_0xC4, &LR35902::instr_0xC5, &LR35902::instr_0xC6, &LR35902::instr_0xC7,
            &LR35902::instr_0xC8, &LR35902::instr_0xC9, &LR35902::instr_0xCA, &LR35902::instr_0xCB,
            &LR35902::instr_0xCC, &LR35902::instr_0xCD, &LR35902::instr_0xCE, &LR35902::instr_0xCF,
            &LR35902::instr_0xD0, &LR35902::instr_0xD1, &LR35902::instr_0xD2, &LR35902::instr_0xD3,
            &LR35902::instr_0xD4, &LR35902::instr_0xD5, &LR35902::instr_0xD6, &LR35902::instr_0xD7,
            &LR35902::instr_0xD8, &LR35902::instr_0xD9, &LR35902::instr_0xDA, &LR35902::instr_0xDB,
            &LR35902::instr_0xDC, &LR35902::instr_0xDD, &LR35902::instr_0xDE, &LR35902::instr_0xDF,
            &LR35902::instr_0xE0, &LR35902::instr_0xE1, &LR35902::instr_0xE2, &LR35902::instr_0xE3,
            &LR35902::instr_0xE4, &LR35902::instr_0xE5, &LR35902::instr_0xE6, &LR35902::instr_0xE7,
            &LR35902::instr_0xE8, &LR35902::instr_0xE9, &LR35902::instr_0xEA, &LR35902::instr_0xEB,
            &LR35902::instr_0xEC, &LR35902::instr_0xED, &LR35902::instr_0xEE, &LR35902::instr_0xEF,
            &LR35902::instr_0xF0, &LR35902::instr_0xF1, &LR35902::instr_0xF2, &LR35902::instr_0xF3,
            &LR35902::instr_0xF4, &LR35902::instr_0xF5, &LR35902::instr_0xF6, &LR35902::instr_0xF7,
            &LR35902::instr_0xF8, &LR35902::instr_0xF9, &LR35902::instr_0xFA, &LR35902::instr_0xFB,
            &LR35902::instr_0xFC, &LR35902::instr_0xFD, &LR35902::instr_0xFE, &LR35902::instr_0xFF
        };

        // Instructions functions // 
        
        void instr_0x00(); void instr_0x01(); void instr_0x02(); void instr_0x03();
        void instr_0x04(); void instr_0x05(); void instr_0x06(); void instr_0x07();
        void instr_0x08(); void instr_0x09(); void instr_0x0A(); void instr_0x0B();
        void instr_0x0C(); void instr_0x0D(); void instr_0x0E(); void instr_0x0F();
        void instr_0x10(); void instr_0x11(); void instr_0x12(); void instr_0x13();
        void instr_0x14(); void instr_0x15(); void instr_0x16(); void instr_0x17();
        void instr_0x18(); void instr_0x19(); void instr_0x1A(); void instr_0x1B();
        void instr_0x1C(); void instr_0x1D(); void instr_0x1E(); void instr_0x1F();
        void instr_0x20(); void instr_0x21(); void instr_0x22(); void instr_0x23();
        void instr_0x24(); void instr_0x25(); void instr_0x26(); void instr_0x27();
        void instr_0x28(); void instr_0x29(); void instr_0x2A(); void instr_0x2B();
        void instr_0x2C(); void instr_0x2D(); void instr_0x2E(); void instr_0x2F();
        void instr_0x30(); void instr_0x31(); void instr_0x32(); void instr_0x33();
        void instr_0x34(); void instr_0x35(); void instr_0x36(); void instr_0x37();
        void instr_0x38(); void instr_0x39(); void instr_0x3A(); void instr_0x3B();
        void instr_0x3C(); void instr_0x3D(); void instr_0x3E(); void instr_0x3F();
        void instr_0x40(); void instr_0x41(); void instr_0x42(); void instr_0x43();
        void instr_0x44(); void instr_0x45(); void instr_0x46(); void instr_0x47();
        void instr_0x48(); void instr_0x49(); void instr_0x4A(); void instr_0x4B();
        void instr_0x4C(); void instr_0x4D(); void instr_0x4E(); void instr_0x4F();
        void instr_0x50(); void instr_0x51(); void instr_0x52(); void instr_0x53();
        void instr_0x54(); void instr_0x55(); void instr_0x56(); void instr_0x57();
        void instr_0x58(); void instr_0x59(); void instr_0x5A(); void instr_0x5B();
        void instr_0x5C(); void instr_0x5D(); void instr_0x5E(); void instr_0x5F();
        void instr_0x60(); void instr_0x61(); void instr_0x62(); void instr_0x63();
        void instr_0x64(); void instr_0x65(); void instr_0x66(); void instr_0x67();
        void instr_0x68(); void instr_0x69(); void instr_0x6A(); void instr_0x6B();
        void instr_0x6C(); void instr_0x6D(); void instr_0x6E(); void instr_0x6F();
        void instr_0x70(); void instr_0x71(); void instr_0x72(); void instr_0x73();
        void instr_0x74(); void instr_0x75(); void instr_0x76(); void instr_0x77();
        void instr_0x78(); void instr_0x79(); void instr_0x7A(); void instr_0x7B();
        void instr_0x7C(); void instr_0x7D(); void instr_0x7E(); void instr_0x7F();
        void instr_0x80(); void instr_0x81(); void instr_0x82(); void instr_0x83();
        void instr_0x84(); void instr_0x85(); void instr_0x86(); void instr_0x87();
        void instr_0x88(); void instr_0x89(); void instr_0x8A(); void instr_0x8B();
        void instr_0x8C(); void instr_0x8D(); void instr_0x8E(); void instr_0x8F();
        void instr_0x90(); void instr_0x91(); void instr_0x92(); void instr_0x93();
        void instr_0x94(); void instr_0x95(); void instr_0x96(); void instr_0x97();
        void instr_0x98(); void instr_0x99(); void instr_0x9A(); void instr_0x9B();
        void instr_0x9C(); void instr_0x9D(); void instr_0x9E(); void instr_0x9F();
        void instr_0xA0(); void instr_0xA1(); void instr_0xA2(); void instr_0xA3();
        void instr_0xA4(); void instr_0xA5(); void instr_0xA6(); void instr_0xA7();
        void instr_0xA8(); void instr_0xA9(); void instr_0xAA(); void instr_0xAB();
        void instr_0xAC(); void instr_0xAD(); void instr_0xAE(); void instr_0xAF();
        void instr_0xB0(); void instr_0xB1(); void instr_0xB2(); void instr_0xB3();
        void instr_0xB4(); void instr_0xB5(); void instr_0xB6(); void instr_0xB7();
        void instr_0xB8(); void instr_0xB9(); void instr_0xBA(); void instr_0xBB();
        void instr_0xBC(); void instr_0xBD(); void instr_0xBE(); void instr_0xBF();
        void instr_0xC0(); void instr_0xC1(); void instr_0xC2(); void instr_0xC3();
        void instr_0xC4(); void instr_0xC5(); void instr_0xC6(); void instr_0xC7();
        void instr_0xC8(); void instr_0xC9(); void instr_0xCA(); void instr_0xCB();
        void instr_0xCC(); void instr_0xCD(); void instr_0xCE(); void instr_0xCF();
        void instr_0xD0(); void instr_0xD1(); void instr_0xD2(); void instr_0xD3();
        void instr_0xD4(); void instr_0xD5(); void instr_0xD6(); void instr_0xD7();
        void instr_0xD8(); void instr_0xD9(); void instr_0xDA(); void instr_0xDB();
        void instr_0xDC(); void instr_0xDD(); void instr_0xDE(); void instr_0xDF();
        void instr_0xE0(); void instr_0xE1(); void instr_0xE2(); void instr_0xE3();
        void instr_0xE4(); void instr_0xE5(); void instr_0xE6(); void instr_0xE7();
        void instr_0xE8(); void instr_0xE9(); void instr_0xEA(); void instr_0xEB();
        void instr_0xEC(); void instr_0xED(); void instr_0xEE(); void instr_0xEF();
        void instr_0xF0(); void instr_0xF1(); void instr_0xF2(); void instr_0xF3();
        void instr_0xF4(); void instr_0xF5(); void instr_0xF6(); void instr_0xF7();
        void instr_0xF8(); void instr_0xF9(); void instr_0xFA(); void instr_0xFB();
        void instr_0xFC(); void instr_0xFD(); void instr_0xFE(); void instr_0xFF();
};

#endif // _CPU_H_