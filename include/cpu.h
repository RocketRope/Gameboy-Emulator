#ifndef _CPU_H_
#define _CPU_H_

// el
#include <el/easylogging++.h>

// gbe
#include "mcu.h"
#include "types.h"
#include "registers.h"

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

        // Interrupt enable flag // 
        bool interrupt_enable;

        Registers reg;


    private:

        el::Logger* log;

        MCU& mcu;

        // Cycles counters //
        uint64 t_cycles = 0;
        uint64 m_cycles = 0;
        
        //

        void execute_opcode(uint8 opcode, bool CB_prefix = false);

        // Misc functions //

        void daa();
        void cpl();
        void swap(uint8 &source);

        void ccf();
        void scf();

        void di();
        void ei();

        void stop();
        void halt();

        // Bit Opcodes //

        void bit(uint8 n, uint8 source);
        void set(uint8 n, uint8& source);
        void res(uint8 n, uint8& source);

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

        void rlc_8bit(uint8& source);
        void rl_8bit(uint8& source);
        void rrc_8bit(uint8& source);
        void rr_8bit(uint8& source);

        void sla_8bit(uint8& source);
        void sra_8bit(uint8& source);
        void srl_8bit(uint8& source);

        // Jump table for opcode instructions //

        void (LR35902::*instr_table[0xFF + 1])(void) = 
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

        void (LR35902::*instrCB_table[0xFF + 1])(void) = 
        {
            &LR35902::instr_0xCB00, &LR35902::instr_0xCB01, &LR35902::instr_0xCB02, &LR35902::instr_0xCB03,
            &LR35902::instr_0xCB04, &LR35902::instr_0xCB05, &LR35902::instr_0xCB06, &LR35902::instr_0xCB07,
            &LR35902::instr_0xCB08, &LR35902::instr_0xCB09, &LR35902::instr_0xCB0A, &LR35902::instr_0xCB0B,
            &LR35902::instr_0xCB0C, &LR35902::instr_0xCB0D, &LR35902::instr_0xCB0E, &LR35902::instr_0xCB0F,
            &LR35902::instr_0xCB10, &LR35902::instr_0xCB11, &LR35902::instr_0xCB12, &LR35902::instr_0xCB13,
            &LR35902::instr_0xCB14, &LR35902::instr_0xCB15, &LR35902::instr_0xCB16, &LR35902::instr_0xCB17,
            &LR35902::instr_0xCB18, &LR35902::instr_0xCB19, &LR35902::instr_0xCB1A, &LR35902::instr_0xCB1B,
            &LR35902::instr_0xCB1C, &LR35902::instr_0xCB1D, &LR35902::instr_0xCB1E, &LR35902::instr_0xCB1F,
            &LR35902::instr_0xCB20, &LR35902::instr_0xCB21, &LR35902::instr_0xCB22, &LR35902::instr_0xCB23,
            &LR35902::instr_0xCB24, &LR35902::instr_0xCB25, &LR35902::instr_0xCB26, &LR35902::instr_0xCB27,
            &LR35902::instr_0xCB28, &LR35902::instr_0xCB29, &LR35902::instr_0xCB2A, &LR35902::instr_0xCB2B,
            &LR35902::instr_0xCB2C, &LR35902::instr_0xCB2D, &LR35902::instr_0xCB2E, &LR35902::instr_0xCB2F,
            &LR35902::instr_0xCB30, &LR35902::instr_0xCB31, &LR35902::instr_0xCB32, &LR35902::instr_0xCB33,
            &LR35902::instr_0xCB34, &LR35902::instr_0xCB35, &LR35902::instr_0xCB36, &LR35902::instr_0xCB37,
            &LR35902::instr_0xCB38, &LR35902::instr_0xCB39, &LR35902::instr_0xCB3A, &LR35902::instr_0xCB3B,
            &LR35902::instr_0xCB3C, &LR35902::instr_0xCB3D, &LR35902::instr_0xCB3E, &LR35902::instr_0xCB3F,
            &LR35902::instr_0xCB40, &LR35902::instr_0xCB41, &LR35902::instr_0xCB42, &LR35902::instr_0xCB43,
            &LR35902::instr_0xCB44, &LR35902::instr_0xCB45, &LR35902::instr_0xCB46, &LR35902::instr_0xCB47,
            &LR35902::instr_0xCB48, &LR35902::instr_0xCB49, &LR35902::instr_0xCB4A, &LR35902::instr_0xCB4B,
            &LR35902::instr_0xCB4C, &LR35902::instr_0xCB4D, &LR35902::instr_0xCB4E, &LR35902::instr_0xCB4F,
            &LR35902::instr_0xCB50, &LR35902::instr_0xCB51, &LR35902::instr_0xCB52, &LR35902::instr_0xCB53,
            &LR35902::instr_0xCB54, &LR35902::instr_0xCB55, &LR35902::instr_0xCB56, &LR35902::instr_0xCB57,
            &LR35902::instr_0xCB58, &LR35902::instr_0xCB59, &LR35902::instr_0xCB5A, &LR35902::instr_0xCB5B,
            &LR35902::instr_0xCB5C, &LR35902::instr_0xCB5D, &LR35902::instr_0xCB5E, &LR35902::instr_0xCB5F,
            &LR35902::instr_0xCB60, &LR35902::instr_0xCB61, &LR35902::instr_0xCB62, &LR35902::instr_0xCB63,
            &LR35902::instr_0xCB64, &LR35902::instr_0xCB65, &LR35902::instr_0xCB66, &LR35902::instr_0xCB67,
            &LR35902::instr_0xCB68, &LR35902::instr_0xCB69, &LR35902::instr_0xCB6A, &LR35902::instr_0xCB6B,
            &LR35902::instr_0xCB6C, &LR35902::instr_0xCB6D, &LR35902::instr_0xCB6E, &LR35902::instr_0xCB6F,
            &LR35902::instr_0xCB70, &LR35902::instr_0xCB71, &LR35902::instr_0xCB72, &LR35902::instr_0xCB73,
            &LR35902::instr_0xCB74, &LR35902::instr_0xCB75, &LR35902::instr_0xCB76, &LR35902::instr_0xCB77,
            &LR35902::instr_0xCB78, &LR35902::instr_0xCB79, &LR35902::instr_0xCB7A, &LR35902::instr_0xCB7B,
            &LR35902::instr_0xCB7C, &LR35902::instr_0xCB7D, &LR35902::instr_0xCB7E, &LR35902::instr_0xCB7F,
            &LR35902::instr_0xCB80, &LR35902::instr_0xCB81, &LR35902::instr_0xCB82, &LR35902::instr_0xCB83,
            &LR35902::instr_0xCB84, &LR35902::instr_0xCB85, &LR35902::instr_0xCB86, &LR35902::instr_0xCB87,
            &LR35902::instr_0xCB88, &LR35902::instr_0xCB89, &LR35902::instr_0xCB8A, &LR35902::instr_0xCB8B,
            &LR35902::instr_0xCB8C, &LR35902::instr_0xCB8D, &LR35902::instr_0xCB8E, &LR35902::instr_0xCB8F,
            &LR35902::instr_0xCB90, &LR35902::instr_0xCB91, &LR35902::instr_0xCB92, &LR35902::instr_0xCB93,
            &LR35902::instr_0xCB94, &LR35902::instr_0xCB95, &LR35902::instr_0xCB96, &LR35902::instr_0xCB97,
            &LR35902::instr_0xCB98, &LR35902::instr_0xCB99, &LR35902::instr_0xCB9A, &LR35902::instr_0xCB9B,
            &LR35902::instr_0xCB9C, &LR35902::instr_0xCB9D, &LR35902::instr_0xCB9E, &LR35902::instr_0xCB9F,
            &LR35902::instr_0xCBA0, &LR35902::instr_0xCBA1, &LR35902::instr_0xCBA2, &LR35902::instr_0xCBA3,
            &LR35902::instr_0xCBA4, &LR35902::instr_0xCBA5, &LR35902::instr_0xCBA6, &LR35902::instr_0xCBA7,
            &LR35902::instr_0xCBA8, &LR35902::instr_0xCBA9, &LR35902::instr_0xCBAA, &LR35902::instr_0xCBAB,
            &LR35902::instr_0xCBAC, &LR35902::instr_0xCBAD, &LR35902::instr_0xCBAE, &LR35902::instr_0xCBAF,
            &LR35902::instr_0xCBB0, &LR35902::instr_0xCBB1, &LR35902::instr_0xCBB2, &LR35902::instr_0xCBB3,
            &LR35902::instr_0xCBB4, &LR35902::instr_0xCBB5, &LR35902::instr_0xCBB6, &LR35902::instr_0xCBB7,
            &LR35902::instr_0xCBB8, &LR35902::instr_0xCBB9, &LR35902::instr_0xCBBA, &LR35902::instr_0xCBBB,
            &LR35902::instr_0xCBBC, &LR35902::instr_0xCBBD, &LR35902::instr_0xCBBE, &LR35902::instr_0xCBBF,
            &LR35902::instr_0xCBC0, &LR35902::instr_0xCBC1, &LR35902::instr_0xCBC2, &LR35902::instr_0xCBC3,
            &LR35902::instr_0xCBC4, &LR35902::instr_0xCBC5, &LR35902::instr_0xCBC6, &LR35902::instr_0xCBC7,
            &LR35902::instr_0xCBC8, &LR35902::instr_0xCBC9, &LR35902::instr_0xCBCA, &LR35902::instr_0xCBCB,
            &LR35902::instr_0xCBCC, &LR35902::instr_0xCBCD, &LR35902::instr_0xCBCE, &LR35902::instr_0xCBCF,
            &LR35902::instr_0xCBD0, &LR35902::instr_0xCBD1, &LR35902::instr_0xCBD2, &LR35902::instr_0xCBD3,
            &LR35902::instr_0xCBD4, &LR35902::instr_0xCBD5, &LR35902::instr_0xCBD6, &LR35902::instr_0xCBD7,
            &LR35902::instr_0xCBD8, &LR35902::instr_0xCBD9, &LR35902::instr_0xCBDA, &LR35902::instr_0xCBDB,
            &LR35902::instr_0xCBDC, &LR35902::instr_0xCBDD, &LR35902::instr_0xCBDE, &LR35902::instr_0xCBDF,
            &LR35902::instr_0xCBE0, &LR35902::instr_0xCBE1, &LR35902::instr_0xCBE2, &LR35902::instr_0xCBE3,
            &LR35902::instr_0xCBE4, &LR35902::instr_0xCBE5, &LR35902::instr_0xCBE6, &LR35902::instr_0xCBE7,
            &LR35902::instr_0xCBE8, &LR35902::instr_0xCBE9, &LR35902::instr_0xCBEA, &LR35902::instr_0xCBEB,
            &LR35902::instr_0xCBEC, &LR35902::instr_0xCBED, &LR35902::instr_0xCBEE, &LR35902::instr_0xCBEF,
            &LR35902::instr_0xCBF0, &LR35902::instr_0xCBF1, &LR35902::instr_0xCBF2, &LR35902::instr_0xCBF3,
            &LR35902::instr_0xCBF4, &LR35902::instr_0xCBF5, &LR35902::instr_0xCBF6, &LR35902::instr_0xCBF7,
            &LR35902::instr_0xCBF8, &LR35902::instr_0xCBF9, &LR35902::instr_0xCBFA, &LR35902::instr_0xCBFB,
            &LR35902::instr_0xCBFC, &LR35902::instr_0xCBFD, &LR35902::instr_0xCBFE, &LR35902::instr_0xCBFF
        };

        // Opcode functions // 
        
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

        void instr_0xCB00(); void instr_0xCB01(); void instr_0xCB02(); void instr_0xCB03();
        void instr_0xCB04(); void instr_0xCB05(); void instr_0xCB06(); void instr_0xCB07();
        void instr_0xCB08(); void instr_0xCB09(); void instr_0xCB0A(); void instr_0xCB0B();
        void instr_0xCB0C(); void instr_0xCB0D(); void instr_0xCB0E(); void instr_0xCB0F();
        void instr_0xCB10(); void instr_0xCB11(); void instr_0xCB12(); void instr_0xCB13();
        void instr_0xCB14(); void instr_0xCB15(); void instr_0xCB16(); void instr_0xCB17();
        void instr_0xCB18(); void instr_0xCB19(); void instr_0xCB1A(); void instr_0xCB1B();
        void instr_0xCB1C(); void instr_0xCB1D(); void instr_0xCB1E(); void instr_0xCB1F();
        void instr_0xCB20(); void instr_0xCB21(); void instr_0xCB22(); void instr_0xCB23();
        void instr_0xCB24(); void instr_0xCB25(); void instr_0xCB26(); void instr_0xCB27();
        void instr_0xCB28(); void instr_0xCB29(); void instr_0xCB2A(); void instr_0xCB2B();
        void instr_0xCB2C(); void instr_0xCB2D(); void instr_0xCB2E(); void instr_0xCB2F();
        void instr_0xCB30(); void instr_0xCB31(); void instr_0xCB32(); void instr_0xCB33();
        void instr_0xCB34(); void instr_0xCB35(); void instr_0xCB36(); void instr_0xCB37();
        void instr_0xCB38(); void instr_0xCB39(); void instr_0xCB3A(); void instr_0xCB3B();
        void instr_0xCB3C(); void instr_0xCB3D(); void instr_0xCB3E(); void instr_0xCB3F();
        void instr_0xCB40(); void instr_0xCB41(); void instr_0xCB42(); void instr_0xCB43();
        void instr_0xCB44(); void instr_0xCB45(); void instr_0xCB46(); void instr_0xCB47();
        void instr_0xCB48(); void instr_0xCB49(); void instr_0xCB4A(); void instr_0xCB4B();
        void instr_0xCB4C(); void instr_0xCB4D(); void instr_0xCB4E(); void instr_0xCB4F();
        void instr_0xCB50(); void instr_0xCB51(); void instr_0xCB52(); void instr_0xCB53();
        void instr_0xCB54(); void instr_0xCB55(); void instr_0xCB56(); void instr_0xCB57();
        void instr_0xCB58(); void instr_0xCB59(); void instr_0xCB5A(); void instr_0xCB5B();
        void instr_0xCB5C(); void instr_0xCB5D(); void instr_0xCB5E(); void instr_0xCB5F();
        void instr_0xCB60(); void instr_0xCB61(); void instr_0xCB62(); void instr_0xCB63();
        void instr_0xCB64(); void instr_0xCB65(); void instr_0xCB66(); void instr_0xCB67();
        void instr_0xCB68(); void instr_0xCB69(); void instr_0xCB6A(); void instr_0xCB6B();
        void instr_0xCB6C(); void instr_0xCB6D(); void instr_0xCB6E(); void instr_0xCB6F();
        void instr_0xCB70(); void instr_0xCB71(); void instr_0xCB72(); void instr_0xCB73();
        void instr_0xCB74(); void instr_0xCB75(); void instr_0xCB76(); void instr_0xCB77();
        void instr_0xCB78(); void instr_0xCB79(); void instr_0xCB7A(); void instr_0xCB7B();
        void instr_0xCB7C(); void instr_0xCB7D(); void instr_0xCB7E(); void instr_0xCB7F();
        void instr_0xCB80(); void instr_0xCB81(); void instr_0xCB82(); void instr_0xCB83();
        void instr_0xCB84(); void instr_0xCB85(); void instr_0xCB86(); void instr_0xCB87();
        void instr_0xCB88(); void instr_0xCB89(); void instr_0xCB8A(); void instr_0xCB8B();
        void instr_0xCB8C(); void instr_0xCB8D(); void instr_0xCB8E(); void instr_0xCB8F();
        void instr_0xCB90(); void instr_0xCB91(); void instr_0xCB92(); void instr_0xCB93();
        void instr_0xCB94(); void instr_0xCB95(); void instr_0xCB96(); void instr_0xCB97();
        void instr_0xCB98(); void instr_0xCB99(); void instr_0xCB9A(); void instr_0xCB9B();
        void instr_0xCB9C(); void instr_0xCB9D(); void instr_0xCB9E(); void instr_0xCB9F();
        void instr_0xCBA0(); void instr_0xCBA1(); void instr_0xCBA2(); void instr_0xCBA3();
        void instr_0xCBA4(); void instr_0xCBA5(); void instr_0xCBA6(); void instr_0xCBA7();
        void instr_0xCBA8(); void instr_0xCBA9(); void instr_0xCBAA(); void instr_0xCBAB();
        void instr_0xCBAC(); void instr_0xCBAD(); void instr_0xCBAE(); void instr_0xCBAF();
        void instr_0xCBB0(); void instr_0xCBB1(); void instr_0xCBB2(); void instr_0xCBB3();
        void instr_0xCBB4(); void instr_0xCBB5(); void instr_0xCBB6(); void instr_0xCBB7();
        void instr_0xCBB8(); void instr_0xCBB9(); void instr_0xCBBA(); void instr_0xCBBB();
        void instr_0xCBBC(); void instr_0xCBBD(); void instr_0xCBBE(); void instr_0xCBBF();
        void instr_0xCBC0(); void instr_0xCBC1(); void instr_0xCBC2(); void instr_0xCBC3();
        void instr_0xCBC4(); void instr_0xCBC5(); void instr_0xCBC6(); void instr_0xCBC7();
        void instr_0xCBC8(); void instr_0xCBC9(); void instr_0xCBCA(); void instr_0xCBCB();
        void instr_0xCBCC(); void instr_0xCBCD(); void instr_0xCBCE(); void instr_0xCBCF();
        void instr_0xCBD0(); void instr_0xCBD1(); void instr_0xCBD2(); void instr_0xCBD3();
        void instr_0xCBD4(); void instr_0xCBD5(); void instr_0xCBD6(); void instr_0xCBD7();
        void instr_0xCBD8(); void instr_0xCBD9(); void instr_0xCBDA(); void instr_0xCBDB();
        void instr_0xCBDC(); void instr_0xCBDD(); void instr_0xCBDE(); void instr_0xCBDF();
        void instr_0xCBE0(); void instr_0xCBE1(); void instr_0xCBE2(); void instr_0xCBE3();
        void instr_0xCBE4(); void instr_0xCBE5(); void instr_0xCBE6(); void instr_0xCBE7();
        void instr_0xCBE8(); void instr_0xCBE9(); void instr_0xCBEA(); void instr_0xCBEB();
        void instr_0xCBEC(); void instr_0xCBED(); void instr_0xCBEE(); void instr_0xCBEF();
        void instr_0xCBF0(); void instr_0xCBF1(); void instr_0xCBF2(); void instr_0xCBF3();
        void instr_0xCBF4(); void instr_0xCBF5(); void instr_0xCBF6(); void instr_0xCBF7();
        void instr_0xCBF8(); void instr_0xCBF9(); void instr_0xCBFA(); void instr_0xCBFB();
        void instr_0xCBFC(); void instr_0xCBFD(); void instr_0xCBFE(); void instr_0xCBFF();

};

#endif // _CPU_H_