#include "cpu.h"

// Constructor/Destructor // 

LR35902::LR35902(MCU* _mcu)
    : mcu{_mcu}
{
    log = el::Loggers::getLogger("CPU");

    reset();
}
LR35902::~LR35902()
{
   el::Loggers::unregisterLogger("CPU");
}

// Controll functions //

void LR35902::reset()
{    
    reg.af = 0x1180; // 0x01B0;
    reg.bc = 0x0000; // 0x0013;
    reg.de = 0xFF56; // 0x00D8;
    reg.hl = 0x000D; // 0x014D;
    reg.sp = 0xFFFE;
    reg.pc = 0x0100;
}

void LR35902::step()
{
    uint8 opcode = mcu->read_8bit(reg.pc);

    if ( opcode != 0xCB )
    {
        execute_opcode(opcode, false);
    }
    else
    {
        reg.pc++;
        opcode = mcu->read_8bit(reg.pc);
        execute_opcode(opcode, true);
    }
}

// Flags //

bool LR35902::get_flag(Flag flag)
{
    return (reg.f & flag) ? true : false;
}
void LR35902::set_flag(Flag flag, bool value)
{
    if ( value )
        reg.f |= flag;
    else
        reg.f &= ~flag;
}

void LR35902::execute_opcode(uint8 opcode, bool CB_prefix)
{
    if ( CB_prefix )
        (this->*instrCB_table[opcode])();
    else
        (this->*instr_table[opcode])();
}

// Misc functions //

void LR35902::daa()
{
    uint8 correction = 0;

    // least significant bcd nibble > 9
    if ( ( ((reg.a & 0x0F) > 0x09) && !get_flag(Flag::subtraction) ) || get_flag(Flag::half_carry) )
        correction |= 0x06;

    // Most significant bcd nibble > 9
    if ( ( (reg.a > 0x99) && !get_flag(Flag::subtraction) ) || get_flag(Flag::carry) )
    {
        correction |= 0x60;
        set_flag(Flag::carry, true);
    }

    if ( get_flag(Flag::subtraction) )
        reg.a -= correction;
    else
        reg.a += correction;

    set_flag(Flag::zero, reg.a == 0);

    set_flag(Flag::half_carry, false);
}
void LR35902::cpl()
{
    reg.a ^= 0xFF;

    set_flag(Flag::subtraction, true);
    set_flag(Flag::half_carry, true);
}
void LR35902::swap(uint8 &source)
{
    uint8 low_nibble = source & 0x0F;

    source = (source >> 4) | (low_nibble << 4);

    set_flag(Flag::zero, source == 0);
    set_flag(Flag::subtraction, false);
    set_flag(Flag::half_carry, false);
    set_flag(Flag::carry, false);
}

void LR35902::ccf()
{
    set_flag(Flag::carry, !get_flag(Flag::carry));
    
    set_flag(Flag::subtraction, false);
    set_flag(Flag::half_carry, false);

}
void LR35902::scf()
{
    set_flag(Flag::carry, true);
    
    set_flag(Flag::subtraction, false);
    set_flag(Flag::half_carry, false);
}
void LR35902::di()
{
    interrupt_enable = false;
}
void LR35902::ei()
{
    interrupt_enable = true;
}
void LR35902::stop()
{

}
void LR35902::halt()
{

}

// Bit Opcodes //

void LR35902::bit(uint8 n, uint8 source)
{
    set_flag(Flag::zero, !get_bit(n, source));

    set_flag(Flag::subtraction, false);
    set_flag(Flag::half_carry, true);
}
void LR35902::set(uint8 n, uint8& source)
{
    set_bit(n, source, true);
}
void LR35902::res(uint8 n, uint8& source)
{
    set_bit(n, source, false);
}

// Jump functions //

void LR35902::call(uint16 address)
{
    push(reg.pc + 3);
    reg.pc = address;
}
void LR35902::ret()
{
    reg.pc = pop();
}
void LR35902::rst(uint16 address)
{
    push(reg.pc + 1);
    reg.pc = address;
}

// Load functions //

uint16 LR35902::pop()
{
    uint16 value = mcu->read_16bit(reg.sp);
    reg.sp += 2;
    
    return value;
}
void LR35902::push(uint16 source)
{
    reg.sp -= 2;
    mcu->write_16bit(reg.sp, source);
}

// ALU functions //

void LR35902::add_8bit(uint8 source, bool with_carry)
{
    // Add lower nibbles (to check half carry)
    uint16 result = (reg.a & 0x0F) + (source & 0x0F);

    if ( with_carry && get_flag(Flag::carry) )
        result++;

    // Update h flag
    set_flag(Flag::half_carry, get_bit(4, result));

    // High nibbles
    result += (reg.a & 0xF0) + (source & 0xF0);

    // Update c flag
    set_flag(Flag::carry, get_bit(8, result));

    reg.a = static_cast<uint8>( result );

    // Update z flag
    set_flag(Flag::zero, reg.a == 0);

    // Clear n flag
    set_flag(Flag::subtraction, false);
}
void LR35902::sub_8bit(uint8 source, bool with_carry)
{
   // Sub lower nibbles (to check half carry)
    uint16 result = (reg.a & 0x0F) - (source & 0x0F);

    if ( with_carry && get_flag(Flag::carry) )
        result--;

    // Update h flag
    set_flag(Flag::half_carry, get_bit(4, result));

    // High nibbles
    result += (reg.a & 0xF0) - (source & 0xF0);

    // Update c flag
    set_flag(Flag::carry, get_bit(8, result));

    reg.a = static_cast<uint8>( result );

    // Update z flag
    set_flag(Flag::zero, reg.a == 0);

    // Set n flag
    set_flag(Flag::subtraction, true);
}

void LR35902::inc_8bit(uint8& source)
{
    // Add lower nibbles (to check half carry)
    uint8 result_low = (source & 0x0F) + 1;

    // Update h flag
    set_flag(Flag::half_carry, get_bit(4, result_low));

    // 
    source++;

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Update n flag
    set_flag(Flag::subtraction, false);
}
void LR35902::dec_8bit(uint8& source)
{
    uint8 result_low = (source & 0x0F) - 1;

    // Update h flag
    set_flag(Flag::half_carry, get_bit(4, result_low));

    //
    source--;

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Update n flag
    set_flag(Flag::subtraction, true);
}

void LR35902::add_16bit(uint16 source)
{
    // Add lower byte (to check half carry)
    uint32 result = (reg.hl & 0x0FFF) + (source & 0x0FFF);

    // Update h flag
    set_flag(Flag::half_carry, get_bit(12, result));

    // 
    result = reg.hl + source;

    // Update c flag
    set_flag(Flag::carry, get_bit(16, result));

    reg.hl = static_cast<uint16>( result );

    // Clear n flag
    set_flag(Flag::subtraction, false);
}
void LR35902::inc_16bit(uint16& source)
{
    source++;
}
void LR35902::dec_16bit(uint16& source)
{
    source--;
}

void LR35902::and_8bit(uint8 source)
{
    reg.a = reg.a & source;

    set_flag(Flag::zero, reg.a == 0);

    set_flag(Flag::subtraction, false);
    set_flag(Flag::carry, false);
    set_flag(Flag::half_carry, true);
}
void LR35902::or_8bit(uint8 source)
{
    reg.a = reg.a | source;

    set_flag(Flag::zero, reg.a == 0);

    set_flag(Flag::subtraction, false);
    set_flag(Flag::carry, false);
    set_flag(Flag::half_carry, false);
}
void LR35902::xor_8bit(uint8 source)
{
    reg.a = reg.a ^ source;

    set_flag(Flag::zero, reg.a == 0);

    set_flag(Flag::subtraction, false);
    set_flag(Flag::carry, false);
    set_flag(Flag::half_carry, false);
}
void LR35902::cp_8bit(uint8 source)
{
    uint8 temp_a = reg.a;

    sub_8bit(source, false);

    reg.a = temp_a;
}

// Rotates & Shifts functions //

void LR35902::rlc_8bit(uint8& source)
{
    // Bit 7 to carry
    set_flag(Flag::carry, get_bit(7, source));

    // Shift
    source <<= 1;

    // Bit 0 same as old bit 7
    if ( get_flag(Flag::carry) )
        set_bit(0, source, true);

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Clear h and n flag
    set_flag(Flag::half_carry, false);
    set_flag(Flag::subtraction, false);
}
void LR35902::rl_8bit(uint8& source)
{
    bool old_carry = get_flag(Flag::carry);

    // Bit 7 to carry
    set_flag(Flag::carry, get_bit(7, source));

    // Shift
    source <<= 1;

    // Bit 0 same as old carry
    if ( old_carry )
        set_bit(0, source, true);

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Clear h and n flag
    set_flag(Flag::half_carry, false);
    set_flag(Flag::subtraction, false);
}
void LR35902::rrc_8bit(uint8& source)
{
    // Bit 0 to carry
    set_flag(Flag::carry, get_bit(0, source));

    // Shift
    source >>= 1;

    // Bit 7 same as old bit 0
    if ( get_flag(Flag::carry) )
        set_bit(7, source, true);

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Clear h and n flag
    set_flag(Flag::half_carry, false);
    set_flag(Flag::subtraction, false);
}
void LR35902::rr_8bit(uint8& source)
{
    bool old_carry = get_flag(Flag::carry);

    // Bit 0 to carry
    set_flag(Flag::carry, get_bit(0, source));

    // Shift
    source >>= 1;

    // Bit 7 same as old carry
    if ( old_carry )
        set_bit(7, source, true);

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Clear h and n flag
    set_flag(Flag::half_carry, false);
    set_flag(Flag::subtraction, false);
}

void LR35902::sla_8bit(uint8& source)
{
    // Bit 7 to carry
    set_flag(Flag::carry, get_bit(7, source));

    // Shift 
    source <<= 1;

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Clear h and n flag
    set_flag(Flag::half_carry, false);
    set_flag(Flag::subtraction, false);
}
void LR35902::sra_8bit(uint8& source)
{
    // Save bit 7
    bool old_bit7 = get_bit(7, source);

    // Bit 0 to carry
    set_flag(Flag::carry, get_bit(0, source));

    // Shift
    source >>= 1;

    // Set bit 7 to old bit 7
    if ( old_bit7 )
        set_bit(7, source, true);

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Clear h and n flag
    set_flag(Flag::half_carry, false);
    set_flag(Flag::subtraction, false);

}
void LR35902::srl_8bit(uint8& source)
{
    // Bit 0 to carry
    set_flag(Flag::carry, get_bit(0, source));

    // Shift
    source >>= 1;

    // Update z flag
    set_flag(Flag::zero, source == 0);

    // Clear h and n flag
    set_flag(Flag::half_carry, false);
    set_flag(Flag::subtraction, false);
}

// Instructions functions //

void LR35902::instr_0x00() // NOP
{
    log->trace("%v : 0x00 - NOP", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x01() // LD BC,d16
{
    log->trace("%v : 0x01 - LD BC,%v", reg.pc, mcu->read_16bit(reg.pc + 1));

    reg.bc = mcu->read_16bit(reg.pc + 1);
    
    reg.pc   += 3;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x02() // LD (BC),A
{
    log->trace("%v : 0x02 - LD (BC),A", reg.pc);

    mcu->write_8bit(reg.bc, reg.a);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x03() // INC BC
{
    log->trace("%v : 0x03 - INC BC", reg.pc);

    inc_16bit(reg.bc);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x04() // INC B
{
    log->trace("%v : 0x04 - INC B", reg.pc);

    inc_8bit(reg.b);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x05() // DEC B
{
    log->trace("%v : 0x05 - DEC B", reg.pc);

    dec_8bit(reg.b);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x06() // LD B,d8
{
    log->trace("%v : 0x06 - LD B,d8", reg.pc);

    reg.b = mcu->read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x07() // RLCA
{
    log->trace("%v : 0x07 - RLCA", reg.pc);

    rlc_8bit(reg.a);

    set_flag(Flag::zero, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x08() // LD (a16),SP
{
    log->trace("%v : 0x08 - LD (a16),SP", reg.pc);

    mcu->write_16bit(mcu->read_16bit(reg.pc + 1), reg.sp);

    reg.pc   += 3;
    t_cycles += 20;
    m_cycles += 5;
}
void LR35902::instr_0x09() // ADD HL,BC
{
    log->trace("%v : 0x09 - ADD HL,BC", reg.pc);

    add_16bit(reg.bc);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x0A() // LD A,(BC)
{
    log->trace("%v : 0x0A - LD A,(BC)", reg.pc);

    reg.a = mcu->read_8bit(reg.bc);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x0B() // DEC BC
{
    log->trace("%v : 0x0B - DEC BC", reg.pc);

    dec_16bit(reg.bc);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x0C() // INC C
{
    log->trace("%v : 0x0C - INC C", reg.pc);

    inc_8bit(reg.c);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x0D() // DEC C
{
    log->trace("%v : 0x0D - DEC C", reg.pc);

    dec_8bit(reg.c);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x0E() // LD C,d8
{
    log->trace("%v : 0x0E - LD C,d8", reg.pc);

    reg.c = mcu->read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x0F() // RRCA
{
    log->trace("%v : 0x0F - RRCA", reg.pc);

    rrc_8bit(reg.a);

    set_flag(Flag::zero, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x10() // STOP 0
{
    log->trace("%v : 0x10 - STOP 0", reg.pc);

    stop();

    reg.pc   += 2;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x11() // LD DE,d16
{
    log->trace("%v : 0x11 - LD DE,d16", reg.pc);

    reg.de = mcu->read_16bit(reg.pc + 1);

    reg.pc   += 3;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x12() // LD (DE),A
{
    log->trace("%v : 0x12 - LD (DE),A", reg.pc);

    mcu->write_8bit(reg.de, reg.a);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x13() // INC DE
{
    log->trace("%v : 0x13 - INC DE", reg.pc);

    inc_16bit(reg.de);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x14() // INC D
{
    log->trace("%v : 0x14 - INC D", reg.pc);

    inc_8bit(reg.d);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x15() // DEC D
{
    log->trace("%v : 0x15 - DEC D", reg.pc);

    dec_8bit(reg.d);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x16() // LD D,d8
{
    log->trace("%v : 0x16 - LD D,d8", reg.pc);

    reg.d = mcu->read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x17() // RLA
{
    log->trace("%v : 0x17 - RLA", reg.pc);

    rl_8bit(reg.a);

    set_flag(Flag::zero, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x18() // JR r8
{
    log->trace("%v : 0x18 - JR r8", reg.pc);

    reg.pc += static_cast<int8>( mcu->read_8bit(reg.pc + 1) );
    t_cycles += 12;
    m_cycles += 3;

    reg.pc   += 2; 
}
void LR35902::instr_0x19() // ADD HL,DE
{
    log->trace("%v : 0x19 - ADD HL,DE", reg.pc);

    add_16bit(reg.de);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x1A() // LD A,(DE)
{
    log->trace("%v : 0x1A - LD A,(DE)", reg.pc);

    reg.a = mcu->read_8bit(reg.de);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x1B() // DEC DE
{
    log->trace("%v : 0x1B - DEC DE", reg.pc);

    dec_16bit(reg.de);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x1C() // INC E
{
    log->trace("%v : 0x1C - INC E", reg.pc);

    inc_8bit(reg.e);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x1D() // DEC E
{
    log->trace("%v : 0x1D - DEC E", reg.pc);

    dec_8bit(reg.e);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x1E() // LD E,d8
{
    log->trace("%v : 0x1E - LD E,d8", reg.pc);

    reg.e = mcu->read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x1F() // RRA
{
    log->trace("%v : 0x1F - RRA", reg.pc);

    rr_8bit(reg.a);

    set_flag(Flag::zero, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x20() // JR NZ,r8
{
    log->trace("%v : 0x20 - JR NZ,r8", reg.pc);

    if ( get_flag(Flag::zero) == false )
    {
        reg.pc += static_cast<int8>( mcu->read_8bit(reg.pc + 1) );
        t_cycles += 12;
        m_cycles += 3;
    }
    else
    {
        t_cycles += 8;
        m_cycles += 2;
    }

    reg.pc   += 2;   
}
void LR35902::instr_0x21() // LD HL,d16
{
    log->trace("%v : 0x21 - LD HL,d16", reg.pc);

    reg.hl = mcu->read_16bit(reg.pc + 1);

    reg.pc   += 3;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x22() // LD (HL+),A
{
    log->trace("%v : 0x22 - LD (HL+),A", reg.pc);

    mcu->write_16bit(reg.hl, reg.a);
    reg.hl++;

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x23() // INC HL
{
    log->trace("%v : 0x23 - INC HL", reg.pc);

    inc_16bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x24() // INC H
{
    log->trace("%v : 0x24 - INC H", reg.pc);

    inc_8bit(reg.h);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x25() // DEC H
{
    log->trace("%v : 0x25 - DEC H", reg.pc);

    dec_8bit(reg.h);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x26() // LD H,d8
{
    log->trace("%v : 0x26 - LD H,d8", reg.pc);

    reg.h = mcu->read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x27() // DAA
{
    log->trace("%v : 0x27 - DAA", reg.pc);

    daa();

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x28() // JR Z,r8
{
    log->trace("%v : 0x28 - JR Z,r8", reg.pc);

    if ( get_flag(Flag::zero) == true )
    {
        reg.pc += static_cast<int8>( mcu->read_8bit(reg.pc + 1) );
        t_cycles += 12;
        m_cycles += 3;
    }
    else
    {
        t_cycles += 8;
        m_cycles += 2;
    }

    reg.pc   += 2; 

    
}
void LR35902::instr_0x29() // ADD HL,HL
{
    log->trace("%v : 0x29 - ADD HL,HL", reg.pc);

    add_16bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x2A() // LD A,(HL+)
{
    log->trace("%v : 0x2A - LD A,(HL+)", reg.pc);

    reg.a = mcu->read_8bit(reg.hl);
    reg.hl++;

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x2B() // DEC HL
{
    log->trace("%v : 0x2B - DEC HL", reg.pc);

    dec_16bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x2C() // INC L
{
    log->trace("%v : 0x2C - INC L", reg.pc);

    inc_8bit(reg.l);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x2D() // DEC L
{
    log->trace("%v : 0x2D - DEC L", reg.pc);

    dec_8bit(reg.l);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x2E() // LD L,d8
{
    log->trace("%v : 0x2E - LD L,d8", reg.pc);

    reg.l = mcu->read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x2F() // CPL
{
    log->trace("%v : 0x2F - CPL", reg.pc);

    cpl();

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x30() // JR NC,r8
{
    log->trace("%v : 0x30 - JR NC,r8", reg.pc);

    if ( get_flag(Flag::carry) == false )
    {
        reg.pc += static_cast<int8>( mcu->read_8bit(reg.pc + 1) );
        t_cycles += 12;
        m_cycles += 3;
    }
    else
    {
        t_cycles += 8;
        m_cycles += 2;
    }

    reg.pc   += 2; 
}
void LR35902::instr_0x31() // LD SP,d16
{
    log->trace("%v : 0x31 - LD SP,d16", reg.pc);

    reg.sp = mcu->read_16bit(reg.pc + 1);

    reg.pc   += 3;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x32() // LD (HL-),A
{
    log->trace("%v : 0x32 - LD (HL-),A", reg.pc);

    mcu->write_8bit(reg.hl, reg.a);
    reg.hl--;

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x33() // INC SP
{
    log->trace("%v : 0x33 - INC SP", reg.pc);

    inc_16bit(reg.sp);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x34() // INC (HL)
{
    log->trace("%v : 0x34 - INC (HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

    inc_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x35() // DEC (HL)
{
    log->trace("%v : 0x35 - DEC (HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

    dec_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x36() // LD (HL),d8
{
    log->trace("%v : 0x36 - LD (HL),d8", reg.pc);

    mcu->write_8bit(reg.hl, mcu->read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x37() // SCF
{
    log->trace("%v : 0x37 - SCF", reg.pc);

    scf();

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x38() // JR C,r8
{
    log->trace("%v : 0x38 - JR C,r8", reg.pc);

    if ( get_flag(Flag::carry) == true )
    {
        reg.pc += static_cast<int8>( mcu->read_8bit(reg.pc + 1) );
        t_cycles += 12;
        m_cycles += 3;
    }
    else
    {
        t_cycles += 8;
        m_cycles += 2;
    }

    reg.pc   += 2; 
}
void LR35902::instr_0x39() // ADD HL,SP
{
    log->trace("%v : 0x39 - ADD HL,SP", reg.pc);

    add_16bit(reg.sp);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x3A() // LD A,(HL-)
{
    log->trace("%v : 0x3A - LD A,(HL-)", reg.pc);

    reg.a = mcu->read_8bit(reg.hl);
    reg.hl--;

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x3B() // DEC SP
{
    log->trace("%v : 0x3B - DEC SP", reg.pc);

    dec_16bit(reg.sp);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x3C() // INC A
{
    log->trace("%v : 0x3C - INC A", reg.pc);

    inc_8bit(reg.a);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x3D() // DEC A
{
    log->trace("%v : 0x3D - DEC A", reg.pc);

    dec_8bit(reg.a);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x3E() // LD A,d8
{
    log->trace("%v : 0x3E - LD A,d8", reg.pc);

    reg.a = mcu->read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x3F() // CCF
{
    log->trace("%v : 0x3F - CCF", reg.pc);

    ccf();

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x40() // LD B,B
{
    log->trace("%v : 0x40 - LD B,B", reg.pc);

    reg.b = reg.b;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x41() // LD B,C
{
    log->trace("%v : 0x41 - LD B,C", reg.pc);

    reg.b = reg.c;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x42() // LD B,D
{
    log->trace("%v : 0x42 - LD B,D", reg.pc);

    reg.b = reg.d;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x43() // LD B,E
{
    log->trace("%v : 0x43 - LD B,E", reg.pc);

    reg.b = reg.e;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x44() // LD B,H
{
    log->trace("%v : 0x44 - LD B,H", reg.pc);

    reg.b = reg.h;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x45() // LD B,L
{
    log->trace("%v : 0x45 - LD B,L", reg.pc);

    reg.b = reg.l;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x46() // LD B,(HL)
{
    log->trace("%v : 0x46 - LD B,(HL)", reg.pc);

    reg.b = mcu->read_8bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x47() // LD B,A
{
    log->trace("%v : 0x47 - LD B,A", reg.pc);

    reg.b = reg.a;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x48() // LD C,B
{
    log->trace("%v : 0x48 - LD C,B", reg.pc);

    reg.c = reg.b;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x49() // LD C,C
{
    log->trace("%v : 0x49 - LD C,C", reg.pc);

    reg.c = reg.c;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x4A() // LD C,D
{
    log->trace("%v : 0x4A - LD C,D", reg.pc);

    reg.c = reg.d;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x4B() // LD C,E
{
    log->trace("%v : 0x4B - LD C,E", reg.pc);

    reg.c = reg.e;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x4C() // LD C,H
{
    log->trace("%v : 0x4C - LD C,H", reg.pc);

    reg.c = reg.h;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x4D() // LD C,L
{
    log->trace("%v : 0x4D - LD C,L", reg.pc);

    reg.c = reg.l;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x4E() // LD C,(HL)
{
    log->trace("%v : 0x4E - LD C,(HL)", reg.pc);

    reg.c = mcu->read_8bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x4F() // LD C,A
{
    log->trace("%v : 0x4F - LD C,A", reg.pc);

    reg.c = reg.a;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x50() // LD D,B
{
    log->trace("%v : 0x50 - LD D,B", reg.pc);

    reg.d = reg.b;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x51() // LD D,C
{
    log->trace("%v : 0x51 - LD D,C", reg.pc);

    reg.d = reg.c;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x52() // LD D,D
{
    log->trace("%v : 0x52 - LD D,D", reg.pc);

    reg.d = reg.d;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x53() // LD D,E
{
    log->trace("%v : 0x53 - LD D,E", reg.pc);

    reg.d = reg.e;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x54() // LD D,H
{
    log->trace("%v : 0x54 - LD D,H", reg.pc);

    reg.d = reg.h;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x55() // LD D,L
{
    log->trace("%v : 0x55 - LD D,L", reg.pc);

    reg.d = reg.l;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x56() // LD D,(HL)
{
    log->trace("%v : 0x56 - LD D,(HL)", reg.pc);

    reg.d = mcu->read_8bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x57() // LD D,A
{
    log->trace("%v : 0x57 - LD D,A", reg.pc);

    reg.d = reg.a;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x58() // LD E,B
{
    log->trace("%v : 0x58 - LD E,B", reg.pc);

    reg.e = reg.b;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x59() // LD E,C
{
    log->trace("%v : 0x59 - LD E,C", reg.pc);

    reg.e = reg.c;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x5A() // LD E,D
{
    log->trace("%v : 0x5A - LD E,D", reg.pc);

    reg.e = reg.d;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x5B() // LD E,E
{
    log->trace("%v : 0x5B - LD E,E", reg.pc);

    reg.e = reg.e;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x5C() // LD E,H
{
    log->trace("%v : 0x5C - LD E,H", reg.pc);

    reg.e = reg.h;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x5D() // LD E,L
{
    log->trace("%v : 0x5D - LD E,L", reg.pc);

    reg.e = reg.l;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x5E() // LD E,(HL)
{
    log->trace("%v : 0x5E - LD E,(HL)", reg.pc);

    reg.e = mcu->read_8bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x5F() // LD E,A
{
    log->trace("%v : 0x5F - LD E,A", reg.pc);

    reg.e = reg.a;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x60() // LD H,B
{
    log->trace("%v : 0x60 - LD H,B", reg.pc);

    reg.h = reg.b;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x61() // LD H,C
{
    log->trace("%v : 0x61 - LD H,C", reg.pc);

    reg.h = reg.c;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x62() // LD H,D
{
    log->trace("%v : 0x62 - LD H,D", reg.pc);

    reg.h = reg.d;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x63() // LD H,E
{
    log->trace("%v : 0x63 - LD H,E", reg.pc);

    reg.h = reg.e;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x64() // LD H,H
{
    log->trace("%v : 0x64 - LD H,H", reg.pc);

    reg.h = reg.h;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x65() // LD H,L
{
    log->trace("%v : 0x65 - LD H,L", reg.pc);

    reg.h = reg.l;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x66() // LD H,(HL)
{
    log->trace("%v : 0x66 - LD H,(HL)", reg.pc);

    reg.h = mcu->read_8bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x67() // LD H,A
{
    log->trace("%v : 0x67 - LD H,A", reg.pc);

    reg.h = reg.a;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x68() // LD L,B
{
    log->trace("%v : 0x68 - LD L,B", reg.pc);

    reg.l = reg.b;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x69() // LD L,C
{
    log->trace("%v : 0x69 - LD L,C", reg.pc);

    reg.l = reg.c;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x6A() // LD L,D
{
    log->trace("%v : 0x6A - LD L,D", reg.pc);

    reg.l = reg.d;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x6B() // LD L,E
{
    log->trace("%v : 0x6B - LD L,E", reg.pc);

    reg.l = reg.e;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x6C() // LD L,H
{
    log->trace("%v : 0x6C - LD L,H", reg.pc);

    reg.l = reg.h;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x6D() // LD L,L
{
    log->trace("%v : 0x6D - LD L,L", reg.pc);

    reg.l = reg.l;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x6E() // LD L,(HL)
{
    log->trace("%v : 0x6E - LD L,(HL)", reg.pc);

    reg.l = mcu->read_8bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x6F() // LD L,A
{
    log->trace("%v : 0x6F - LD L,A", reg.pc);

    reg.l = reg.a;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x70() // LD (HL),B
{
    log->trace("%v : 0x70 - LD (HL),B", reg.pc);

    mcu->write_8bit(reg.hl, reg.b);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x71() // LD (HL),C
{
    log->trace("%v : 0x71 - LD (HL),C", reg.pc);

    mcu->write_8bit(reg.hl, reg.c);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x72() // LD (HL),D
{
    log->trace("%v : 0x72 - LD (HL),D", reg.pc);

    mcu->write_8bit(reg.hl, reg.d);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x73() // LD (HL),E
{
    log->trace("%v : 0x73 - LD (HL),E", reg.pc);

    mcu->write_8bit(reg.hl, reg.e);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x74() // LD (HL),H
{
    log->trace("%v : 0x74 - LD (HL),H", reg.pc);

    mcu->write_8bit(reg.hl, reg.h);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x75() // LD (HL),L
{
    log->trace("%v : 0x75 - LD (HL),L", reg.pc);

    mcu->write_8bit(reg.hl, reg.l);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x76() // HALT
{
    log->trace("%v : 0x76 - HALT", reg.pc);

    halt();

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x77() // LD (HL),A
{
    log->trace("%v : 0x77 - LD (HL),A", reg.pc);

    mcu->write_8bit(reg.hl, reg.a);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x78() // LD A,B
{
    log->trace("%v : 0x78 - LD A,B", reg.pc);

    reg.a = reg.b;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x79() // LD A,C
{
    log->trace("%v : 0x79 - LD A,C", reg.pc);

    reg.a = reg.c;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x7A() // LD A,D
{
    log->trace("%v : 0x7A - LD A,D", reg.pc);

    reg.a = reg.d;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x7B() // LD A,E
{
    log->trace("%v : 0x7B - LD A,E", reg.pc);

    reg.a = reg.e;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x7C() // LD A,H
{
    log->trace("%v : 0x7C - LD A,H", reg.pc);

    reg.a = reg.h;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x7D() // LD A,L
{
    log->trace("%v : 0x7D - LD A,L", reg.pc);

    reg.a = reg.l;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x7E() // LD A,(HL)
{
    log->trace("%v : 0x7E - LD A,(HL)", reg.pc);

    reg.a = mcu->read_8bit(reg.hl);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x7F() // LD A,A
{
    log->trace("%v : 0x7F - LD A,A", reg.pc);

    reg.a = reg.a;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x80() // ADD A,B
{
    log->trace("%v : 0x80 - ADD A,B", reg.pc);

    add_8bit(reg.b, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x81() // ADD A,C
{
    log->trace("%v : 0x81 - ADD A,C", reg.pc);

    add_8bit(reg.c, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x82() // ADD A,D
{
    log->trace("%v : 0x82 - ADD A,D", reg.pc);

    add_8bit(reg.d, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x83() // ADD A,E
{
    log->trace("%v : 0x83 - ADD A,E", reg.pc);

    add_8bit(reg.e, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x84() // ADD A,H
{
    log->trace("%v : 0x84 - ADD A,H", reg.pc);

    add_8bit(reg.h, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x85() // ADD A,L
{
    log->trace("%v : 0x85 - ADD A,L", reg.pc);

    add_8bit(reg.l, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x86() // ADD A,(HL)
{
    log->trace("%v : 0x86 - ADD A,(HL)", reg.pc);

    add_8bit(mcu->read_8bit(reg.hl), false);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x87() // ADD A,A
{
    log->trace("%v : 0x87 - ADD A,A", reg.pc);

    add_8bit(reg.a, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x88() // ADC A,B
{
    log->trace("%v : 0x88 - ADC A,B", reg.pc);

    add_8bit(reg.b, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x89() // ADC A,C
{
    log->trace("%v : 0x89 - ADC A,C", reg.pc);

    add_8bit(reg.c, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x8A() // ADC A,D
{
    log->trace("%v : 0x8A - ADC A,D", reg.pc);

    add_8bit(reg.d, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x8B() // ADC A,E
{
    log->trace("%v : 0x8B - ADC A,E", reg.pc);

    add_8bit(reg.e, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x8C() // ADC A,H
{
    log->trace("%v : 0x8C - ADC A,H", reg.pc);

    add_8bit(reg.h, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x8D() // ADC A,L
{
    log->trace("%v : 0x8D - ADC A,L", reg.pc);

    add_8bit(reg.l, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x8E() // ADC A,(HL)
{
    log->trace("%v : 0x8E - ADC A,(HL)", reg.pc);

    add_8bit(mcu->read_8bit(reg.hl), true);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x8F() // ADC A,A
{
    log->trace("%v : 0x8F - ADC A,A", reg.pc);

    add_8bit(reg.a, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x90() // SUB B
{
    log->trace("%v : 0x90 - SUB B", reg.pc);

    sub_8bit(reg.b, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x91() // SUB C
{
    log->trace("%v : 0x91 - SUB C", reg.pc);

    sub_8bit(reg.c, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x92() // SUB D
{
    log->trace("%v : 0x92 - SUB D", reg.pc);

    sub_8bit(reg.d, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x93() // SUB E
{
    log->trace("%v : 0x93 - SUB E", reg.pc);

    sub_8bit(reg.e, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x94() // SUB H
{
    log->trace("%v : 0x94 - SUB H", reg.pc);

    sub_8bit(reg.h, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x95() // SUB L
{
    log->trace("%v : 0x95 - SUB L", reg.pc);

    sub_8bit(reg.l, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x96() // SUB (HL)
{
    log->trace("%v : 0x96 - SUB (HL)", reg.pc);

    sub_8bit(mcu->read_8bit(reg.hl), false);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x97() // SUB A
{
    log->trace("%v : 0x97 - SUB A", reg.pc);

    sub_8bit(reg.a, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x98() // SBC A,B
{
    log->trace("%v : 0x98 - SBC A,B", reg.pc);

    sub_8bit(reg.b, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x99() // SBC A,C
{
    log->trace("%v : 0x99 - SBC A,C", reg.pc);

    sub_8bit(reg.c, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x9A() // SBC A,D
{
    log->trace("%v : 0x9A - SBC A,D", reg.pc);

    sub_8bit(reg.d, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x9B() // SBC A,E
{
    log->trace("%v : 0x9B - SBC A,E", reg.pc);

    sub_8bit(reg.e, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x9C() // SBC A,H
{
    log->trace("%v : 0x9C - SBC A,H", reg.pc);

    sub_8bit(reg.h, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x9D() // SBC A,L
{
    log->trace("%v : 0x9D - SBC A,L", reg.pc);

    sub_8bit(reg.l, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x9E() // SBC A,(HL)
{
    log->trace("%v : 0x9E - SBC A,(HL)", reg.pc);

    sub_8bit(mcu->read_8bit(reg.hl), true);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x9F() // SBC A,A
{
    log->trace("%v : 0x9F - SBC A,A", reg.pc);

    sub_8bit(reg.a, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA0() // AND B
{
    log->trace("%v : 0xA0 - AND B", reg.pc);

    and_8bit(reg.b);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA1() // AND C
{
    log->trace("%v : 0xA1 - AND C", reg.pc);

    and_8bit(reg.c);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA2() // AND D
{
    log->trace("%v : 0xA2 - AND D", reg.pc);

    and_8bit(reg.d);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA3() // AND E
{
    log->trace("%v : 0xA3 - AND E", reg.pc);

    and_8bit(reg.e);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA4() // AND H
{
    log->trace("%v : 0xA4 - AND H", reg.pc);

    and_8bit(reg.h);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA5() // AND L
{
    log->trace("%v : 0xA5 - AND L", reg.pc);

    and_8bit(reg.l);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA6() // AND (HL)
{
    log->trace("%v : 0xA6 - AND (HL)", reg.pc);

    and_8bit(mcu->read_8bit(reg.hl));

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xA7() // AND A
{
    log->trace("%v : 0xA7 - AND A", reg.pc);

    and_8bit(reg.a);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA8() // XOR B
{
    log->trace("%v : 0xA8 - XOR B", reg.pc);

    xor_8bit(reg.b);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xA9() // XOR C
{
    log->trace("%v : 0xA9 - XOR C", reg.pc);

    xor_8bit(reg.c);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xAA() // XOR D
{
    log->trace("%v : 0xAA - XOR D", reg.pc);

    xor_8bit(reg.d);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xAB() // XOR E
{
    log->trace("%v : 0xAB - XOR E", reg.pc);

    xor_8bit(reg.e);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xAC() // XOR H
{
    log->trace("%v : 0xAC - XOR H", reg.pc);

    xor_8bit(reg.h);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xAD() // XOR L
{
    log->trace("%v : 0xAD - XOR L", reg.pc);

    xor_8bit(reg.l);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xAE() // XOR (HL)
{
    log->trace("%v : 0xAE - XOR (HL)", reg.pc);

    xor_8bit(mcu->read_8bit(reg.hl));

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xAF() // XOR A
{
    log->trace("%v : 0xAF - XOR A", reg.pc);

    xor_8bit(reg.a);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB0() // OR B
{
    log->trace("%v : 0xB0 - OR B", reg.pc);

    or_8bit(reg.b);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB1() // OR C
{
    log->trace("%v : 0xB1 - OR C", reg.pc);

    or_8bit(reg.c);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB2() // OR D
{
    log->trace("%v : 0xB2 - OR D", reg.pc);

    or_8bit(reg.d);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB3() // OR E
{
    log->trace("%v : 0xB3 - OR E", reg.pc);

    or_8bit(reg.e);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB4() // OR H
{
    log->trace("%v : 0xB4 - OR H", reg.pc);

    or_8bit(reg.h);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB5() // OR L
{
    log->trace("%v : 0xB5 - OR L", reg.pc);

    or_8bit(reg.l);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB6() // OR (HL)
{
    log->trace("%v : 0xB6 - OR (HL)", reg.pc);

    or_8bit(mcu->read_8bit(reg.hl));

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xB7() // OR A
{
    log->trace("%v : 0xB7 - OR A", reg.pc);

    or_8bit(reg.a);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB8() // CP B
{
    log->trace("%v : 0xB8 - CP B", reg.pc);

    cp_8bit(reg.b);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xB9() // CP C
{
    log->trace("%v : 0xB9 - CP C", reg.pc);

    cp_8bit(reg.c);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xBA() // CP D
{
    log->trace("%v : 0xBA - CP D", reg.pc);

    cp_8bit(reg.d);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xBB() // CP E
{
    log->trace("%v : 0xBB - CP E", reg.pc);

    cp_8bit(reg.e);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xBC() // CP H
{
    log->trace("%v : 0xBC - CP H", reg.pc);

    cp_8bit(reg.h);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xBD() // CP L
{
    log->trace("%v : 0xBD - CP L", reg.pc);

    cp_8bit(reg.l);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xBE() // CP (HL)
{
    log->trace("%v : 0xBE - CP (HL)", reg.pc);

    cp_8bit(mcu->read_8bit(reg.hl));

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xBF() // CP A
{
    log->trace("%v : 0xBF - CP A", reg.pc);

    cp_8bit(reg.a);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xC0() // RET NZ
{
    log->trace("%v : 0xC0 - RET NZ", reg.pc);

    if ( get_flag(Flag::zero) == false )
    {
        ret();
        t_cycles += 20;
        m_cycles += 5;
    }
    else
    {
        reg.pc   += 1;
        t_cycles += 8;
        m_cycles += 2;
    }
}
void LR35902::instr_0xC1() // POP BC
{
    log->trace("%v : 0xC1 - POP BC", reg.pc);

    reg.bc = pop();

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xC2() // JP NZ,a16
{
    log->trace("%v : 0xC2 - JP NZ,a16", reg.pc);

    if ( get_flag(Flag::zero) == false )
    {
        reg.pc = mcu->read_16bit(reg.pc + 1);
        t_cycles += 16;
        m_cycles += 5;
    }
    else
    {
        reg.pc   += 3;
        t_cycles += 12;
        m_cycles += 3;
    }
}
void LR35902::instr_0xC3() // JP a16
{
    log->trace("%v : 0xC3 - JP a16", reg.pc);

    reg.pc = mcu->read_16bit(reg.pc + 1);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xC4() // CALL NZ,a16
{
    log->trace("%v : 0xC4 - CALL NZ,a16", reg.pc);

    if ( get_flag(Flag::zero) == false )
    {
        call(mcu->read_16bit(reg.pc + 1));
        t_cycles += 24;
        m_cycles += 6;
    }
    else
    {
        reg.pc   += 3;
        t_cycles += 12;
        m_cycles += 3;      
    }
}
void LR35902::instr_0xC5() // PUSH BC
{
    log->trace("%v : 0xC5 - PUSH BC", reg.pc);

    push(reg.bc);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xC6() // ADD A,d8
{
    log->trace("%v : 0xC6 - ADD A,d8", reg.pc);

    add_8bit(mcu->read_8bit(reg.pc + 1), false);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xC7() // RST 0x0000
{
    log->trace("%v : 0xC7 - RST 0x0000", reg.pc);

    rst(MCU::Addr::rst_0x00);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xC8() // RET Z
{
    log->trace("%v : 0xC8 - RET Z", reg.pc);

    if ( get_flag(Flag::zero) == true )
        ret();
    else
        reg.pc   += 1;

    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xC9() // RET
{
    log->trace("%v : 0xC9 - RET", reg.pc);

    ret();

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xCA() // JP Z,a16
{
    log->trace("%v : 0xCA - JP Z,a16", reg.pc);

    if ( get_flag(Flag::zero) == true )
    {
        reg.pc = mcu->read_16bit(reg.pc + 1);
        t_cycles += 16;
        m_cycles += 4;
    }
    else
    {
        reg.pc   += 3;
        t_cycles += 12;
        m_cycles += 3;    
    }
}
void LR35902::instr_0xCB() // PREFIX CB
{
    log->error("%v : 0xCB - PREFIX CB", reg.pc);
}
void LR35902::instr_0xCC() // CALL Z,a16
{
    log->trace("%v : 0xCC - CALL Z,a16", reg.pc);

    if ( get_flag(Flag::zero) == true )
    {
        call(mcu->read_16bit(reg.pc + 1));
        t_cycles += 24;
        m_cycles += 6;
    }
    else
    {
        reg.pc   += 3;
        t_cycles += 12;
        m_cycles += 3;    
    }
}
void LR35902::instr_0xCD() // CALL a16
{
    log->trace("%v : 0xCD - CALL a16", reg.pc);

    call(mcu->read_16bit(reg.pc + 1));

    t_cycles += 24;
    m_cycles += 6;
}
void LR35902::instr_0xCE() // ADC A,d8
{
    log->trace("%v : 0xCE - ADC A,d8", reg.pc);

    add_8bit(mcu->read_8bit(reg.pc + 1), true);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xCF() // RST 08H
{
    log->trace("%v : 0xCF - RST 08H", reg.pc);

    rst(MCU::Addr::rst_0x08);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xD0() // RET NC
{
    log->trace("%v : 0xD0 - RET NC", reg.pc);

    if ( get_flag(Flag::carry) == false )
    {
        ret();
        t_cycles += 20;
        m_cycles += 5;
    }
    else
    {
        reg.pc   += 1;
        t_cycles += 8;
        m_cycles += 2;
    }
}
void LR35902::instr_0xD1() // POP DE
{
    log->trace("%v : 0xD1 - POP DE", reg.pc);

    reg.de = pop();

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xD2() // JP NC,a16
{
    log->trace("%v : 0xD2 - JP NC,a16", reg.pc);

    if ( get_flag(Flag::carry) == false )
    {
        reg.pc = mcu->read_16bit(reg.pc + 1);
        t_cycles += 16;
        m_cycles += 4;
    } 
    else
    {
        reg.pc   += 3;
        t_cycles += 12;
        m_cycles += 3;
    }
}
void LR35902::instr_0xD3() // Invalid instruction
{
    log->error("%v : 0xD3 - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xD4() // CALL NC,a16
{
    log->trace("%v : 0xD4 - CALL NC,a16", reg.pc);

    if ( get_flag(Flag::carry) == false )
    {
        call(mcu->read_16bit(reg.pc + 1));
        t_cycles += 24;
        m_cycles += 6;
    }
    else
    {
        reg.pc   += 3;
        t_cycles += 12;
        m_cycles += 3;
    }
}
void LR35902::instr_0xD5() // PUSH DE
{
    log->trace("%v : 0xD5 - PUSH DE", reg.pc);

    push(reg.de);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xD6() // SUB d8
{
    log->trace("%v : 0xD6 - SUB d8", reg.pc);

    sub_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xD7() // RST 10H
{
    log->trace("%v : 0xD7 - RST 10H", reg.pc);

    rst(MCU::Addr::rst_0x10);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xD8() // RET C
{
    log->trace("%v : 0xD8 - RET C", reg.pc);

    if ( get_flag(Flag::carry) == true )
    {
        ret();
        t_cycles += 20;    
        m_cycles += 5;
    }
    else
    {
        reg.pc   += 1;
        t_cycles += 8;
        m_cycles += 2;    
    }
}
void LR35902::instr_0xD9() // RETI
{
    log->trace("%v : 0xD9 - RETI", reg.pc);

    ret();
    ei();

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xDA() // JP C,a16
{
    log->trace("%v : 0xDA - JP C,a16", reg.pc);

    if ( get_flag(Flag::carry) == true )
    {
        reg.pc = mcu->read_16bit(reg.pc + 1);
        t_cycles += 16;
        m_cycles += 4;
    }
    else
    {
        reg.pc   += 3;
        t_cycles += 12;
        m_cycles += 3;
    }
}
void LR35902::instr_0xDB() // Invalid instruction
{
    log->error("%v : 0xDB - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xDC() // CALL C,a16
{
    log->trace("%v : 0xDC - CALL C,a16", reg.pc);

    if ( get_flag(Flag::carry) == true )
    {
        call(mcu->read_16bit(reg.pc + 1));
        t_cycles += 24;
        m_cycles += 6;
    }
    else
    {
        reg.pc   += 3;
        t_cycles += 12;
        m_cycles += 3;
    }
}
void LR35902::instr_0xDD() // Invalid instruction
{
    log->error("%v : 0xDD - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xDE() // SBC A,d8
{
    log->trace("%v : 0xDE - SBC A,d8", reg.pc);

    sub_8bit(mcu->read_8bit(reg.pc + 1), true);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xDF() // RST 18H
{
    log->trace("%v : 0xDF - RST 18H", reg.pc);

    rst(MCU::Addr::rst_0x18);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xE0() // LDH (a8),A
{
    log->trace("%v : 0xE0 - LDH (a8),A", reg.pc);

    mcu->write_8bit(0xFF00 + mcu->read_8bit(reg.pc + 1), reg.a);

    reg.pc   += 2;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xE1() // POP HL
{
    log->trace("%v : 0xE1 - POP HL", reg.pc);

    reg.hl = pop();

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xE2() // LD (C),A
{
    log->trace("%v : 0xE2 - LD (C),A", reg.pc);

    mcu->write_8bit(0xFF00 + reg.c, reg.a);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xE3() // Invalid instruction
{
    log->error("%v : 0xE3 - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xE4() // Invalid instruction
{
    log->error("%v : 0xE4 - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xE5() // PUSH HL
{
    log->trace("%v : 0xE5 - PUSH HL", reg.pc);

    push(reg.hl);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xE6() // AND d8
{
    log->trace("%v : 0xE6 - AND d8", reg.pc);

    and_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xE7() // RST 20H
{
    log->trace("%v : 0xE7 - RST 20H", reg.pc);

    rst(MCU::Addr::rst_0x20);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xE8() // ADD SP,r8
{
    log->trace("%v : 0xE8 - ADD SP,r8", reg.pc);

    int8 e = static_cast<int8>( mcu->read_8bit(reg.pc + 1) );

    // Invert h and c flags if e < 0
    if ( e > 0 )
    {
        set_flag(Flag::half_carry, get_bit(4, (reg.sp & 0x000F) + e));
        set_flag(Flag::carry, get_bit(8, (reg.sp & 0x00FF) + e));
    }
    else
    {
        set_flag(Flag::half_carry, !get_bit(4, (reg.sp & 0x000F) + e));
        set_flag(Flag::carry, !get_bit(8, (reg.sp & 0x00FF) + e));
    }

    reg.sp = reg.sp + e;

    set_flag(Flag::zero, false);
    set_flag(Flag::subtraction, false);

    reg.pc   += 2;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xE9() // JP (HL)
{
    log->trace("%v : 0xE9 - JP (HL)", reg.pc);

    reg.pc = reg.hl;

    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xEA() // LD (a16),A
{
    log->trace("%v : 0xEA - LD (a16),A", reg.pc);

    mcu->write_8bit(mcu->read_16bit(reg.pc + 1), reg.a);

    reg.pc   += 3;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xEB() // Invalid instruction
{
    log->error("%v : 0xEB - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xEC() // Invalid instruction
{
    log->error("%v : 0xEC - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xED() // Invalid instruction
{
    log->error("%v : 0xED - Invalid instruction", reg.pc);

    // // ***************// *************** //

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xEE() // XOR d8
{
    log->trace("%v : 0xEE - XOR d8", reg.pc);

    xor_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xEF() // RST 28H
{
    log->trace("%v : 0xEF - RST 28H", reg.pc);

    rst(MCU::Addr::rst_0x28);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xF0() // LDH A,(a8)
{
    log->trace("%v : 0xF0 - LDH A,(a8)", reg.pc);

    reg.a = mcu->read_8bit(0xFF00 + mcu->read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xF1() // POP AF
{
    log->trace("%v : 0xF1 - POP AF", reg.pc);

    reg.af = pop();

    // Clear lower nibble
    reg.f = reg.f & 0xF0;

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xF2() // LD A,(C)
{
    log->trace("%v : 0xF2 - LD A,(C)", reg.pc);

    reg.a = mcu->read_8bit(0xFF00 + reg.c);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xF3() // DI
{
    log->trace("%v : 0xF3 - DI", reg.pc);

    di();

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xF4() // Invalid instruction
{
    log->error("%v : 0xF4 - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xF5() // PUSH AF
{
    log->trace("%v : 0xF5 - PUSH AF", reg.pc);

    push(reg.af);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xF6() // OR d8
{
    log->trace("%v : 0xF6 - OR d8", reg.pc);

    or_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xF7() // RST 30H
{
    log->trace("%v : 0xF7 - RST 30H", reg.pc);

    rst(MCU::Addr::rst_0x30);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xF8() // LD HL,SP+r8
{
    log->trace("%v : 0xF8 - LD HL,SP+r8", reg.pc);

    int8 e = static_cast<int8>( mcu->read_8bit(reg.pc + 1) );

    // Invert h and c flags if e < 0
    if ( e > 0 )
    {
        set_flag(Flag::half_carry, get_bit(4, (reg.sp & 0x000F) + e));
        set_flag(Flag::carry, get_bit(8, (reg.sp & 0x00FF) + e));
    }
    else
    {
        set_flag(Flag::half_carry, !get_bit(4, (reg.sp & 0x000F) + e));
        set_flag(Flag::carry, !get_bit(8, (reg.sp & 0x00FF) + e));
    }

    reg.hl = reg.sp + e;

    set_flag(Flag::zero, false);
    set_flag(Flag::subtraction, false);

    reg.pc   += 2;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xF9() // LD SP,HL
{
    log->trace("%v : 0xF9 - LD SP,HL", reg.pc);

    reg.sp = reg.hl;

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xFA() // LD A,(a16)
{
    log->trace("%v : 0xFA - LD A,(a16)", reg.pc);

    reg.a = mcu->read_8bit(mcu->read_16bit(reg.pc + 1));

    reg.pc   += 3;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xFB() // EI
{
    log->trace("%v : 0xFB - EI", reg.pc);

    ei();

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xFC() // Invalid instruction
{
    log->error("%v : 0xFC - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xFD() // Invalid instruction
{
    log->error("%v : 0xFD - Invalid instruction", reg.pc);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xFE() // CP d8
{
    log->trace("%v : 0xFE - CP d8", reg.pc);

    cp_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xFF() // RST 38H
{
    log->trace("%v : 0xFF - RST 38H", reg.pc);

    rst(MCU::Addr::rst_0x38);

    t_cycles += 16;
    m_cycles += 4;
}

void LR35902::instr_0xCB00() // RLC B
{
	log->trace("%v : 0xCB00 - RLC B", reg.pc);

	rlc_8bit(reg.b);

	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB01() // RLC C
{
	log->trace("%v : 0xCB01 - RLC C", reg.pc);

	rlc_8bit(reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB02() // RLC D
{
	log->trace("%v : 0xCB02 - RLC D", reg.pc);

	rlc_8bit(reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB03() // RLC E
{
	log->trace("%v : 0xCB03 - RLC E", reg.pc);

	rlc_8bit(reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB04() // RLC H
{
	log->trace("%v : 0xCB04 - RLC H", reg.pc);

	rlc_8bit(reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB05() // RLC L
{
	log->trace("%v : 0xCB05 - RLC L", reg.pc);

	rlc_8bit(reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB06() // RLC (HL)
{
	log->trace("%v : 0xCB06 - RLC (HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	rlc_8bit(value);

    mcu->write_8bit(reg.hl, value);
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB07() // RLC A
{
	log->trace("%v : 0xCB07 - RLC A", reg.pc);

	rlc_8bit(reg.a);

	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB08() // RRC B
{
	log->trace("%v : 0xCB08 - RRC B", reg.pc);

	rrc_8bit(reg.b);

	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB09() // RRC C
{
	log->trace("%v : 0xCB09 - RRC C", reg.pc);

	rrc_8bit(reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB0A() // RRC D
{
	log->trace("%v : 0xCB0A - RRC D", reg.pc);

	rrc_8bit(reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB0B() // RRC E
{
	log->trace("%v : 0xCB0B - RRC E", reg.pc);

	rrc_8bit(reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB0C() // RRC H
{
	log->trace("%v : 0xCB0C - RRC H", reg.pc);

	rrc_8bit(reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB0D() // RRC L
{
	log->trace("%v : 0xCB0D - RRC L", reg.pc);

	rrc_8bit(reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB0E() // RRC (HL)
{
	log->trace("%v : 0xCB0E - RRC (HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);
    
    rrc_8bit(value);

    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB0F() // RRC A
{
	log->trace("%v : 0xCB0F - RRC A", reg.pc);

	rrc_8bit(reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB10() // RL B
{
	log->trace("%v : 0xCB10 - RL B", reg.pc);

	rl_8bit(reg.b);

	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB11() // RL C
{
	log->trace("%v : 0xCB11 - RL C", reg.pc);

	rl_8bit(reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB12() // RL D
{
	log->trace("%v : 0xCB12 - RL D", reg.pc);

	rl_8bit(reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB13() // RL E
{
	log->trace("%v : 0xCB13 - RL E", reg.pc);

	rl_8bit(reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB14() // RL H
{
	log->trace("%v : 0xCB14 - RL H", reg.pc);

	rl_8bit(reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB15() // RL L
{
	log->trace("%v : 0xCB15 - RL L", reg.pc);

	rl_8bit(reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB16() // RL (HL)
{
	log->trace("%v : 0xCB16 - RL (HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	rl_8bit(value);

    mcu->write_8bit(reg.hl, value);
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB17() // RL A
{
	log->trace("%v : 0xCB17 - RL A", reg.pc);

	rl_8bit(reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB18() // RR B
{
	log->trace("%v : 0xCB18 - RR B", reg.pc);

	rr_8bit(reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB19() // RR C
{
	log->trace("%v : 0xCB19 - RR C", reg.pc);

	rr_8bit(reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB1A() // RR D
{
	log->trace("%v : 0xCB1A - RR D", reg.pc);

	rr_8bit(reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB1B() // RR E
{
	log->trace("%v : 0xCB1B - RR E", reg.pc);

	rr_8bit(reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB1C() // RR H
{
	log->trace("%v : 0xCB1C - RR H", reg.pc);

	rr_8bit(reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB1D() // RR L
{
	log->trace("%v : 0xCB1D - RR L", reg.pc);

	rr_8bit(reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB1E() // RR (HL)
{
	log->trace("%v : 0xCB1E - RR (HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	rr_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB1F() // RR A
{
	log->trace("%v : 0xCB1F - RR A", reg.pc);

	rr_8bit(reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB20() // SLA B
{
	log->trace("%v : 0xCB20 - SLA B", reg.pc);

	sla_8bit(reg.b);

	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB21() // SLA C
{
	log->trace("%v : 0xCB21 - SLA C", reg.pc);

	sla_8bit(reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB22() // SLA D
{
	log->trace("%v : 0xCB22 - SLA D", reg.pc);

	sla_8bit(reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB23() // SLA E
{
	log->trace("%v : 0xCB23 - SLA E", reg.pc);

	sla_8bit(reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB24() // SLA H
{
	log->trace("%v : 0xCB24 - SLA H", reg.pc);

	sla_8bit(reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB25() // SLA L
{
	log->trace("%v : 0xCB25 - SLA L", reg.pc);

	sla_8bit(reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB26() // SLA (HL)
{
	log->trace("%v : 0xCB26 - SLA (HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	sla_8bit(value);

    mcu->write_8bit(reg.hl, value);
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB27() // SLA A
{
	log->trace("%v : 0xCB27 - SLA A", reg.pc);

	sla_8bit(reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB28() // SRA B
{
	log->trace("%v : 0xCB28 - SRA B", reg.pc);

	sra_8bit(reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB29() // SRA C
{
	log->trace("%v : 0xCB29 - SRA C", reg.pc);

	sra_8bit(reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB2A() // SRA D
{
	log->trace("%v : 0xCB2A - SRA D", reg.pc);

	sra_8bit(reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB2B() // SRA E
{
	log->trace("%v : 0xCB2B - SRA E", reg.pc);

	sra_8bit(reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB2C() // SRA H
{
	log->trace("%v : 0xCB2C - SRA H", reg.pc);

	sra_8bit(reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB2D() // SRA L
{
	log->trace("%v : 0xCB2D - SRA L", reg.pc);

	sra_8bit(reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB2E() // SRA (HL)
{
	log->trace("%v : 0xCB2E - SRA (HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	sra_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB2F() // SRA A
{
	log->trace("%v : 0xCB2F - SRA A", reg.pc);

	sra_8bit(reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB30() // SWAP B
{
	log->trace("%v : 0xCB30 - SWAP B", reg.pc);

	swap(reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB31() // SWAP C
{
	log->trace("%v : 0xCB31 - SWAP C", reg.pc);

	swap(reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB32() // SWAP D
{
	log->trace("%v : 0xCB32 - SWAP D", reg.pc);

	swap(reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB33() // SWAP E
{
	log->trace("%v : 0xCB33 - SWAP E", reg.pc);

	swap(reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB34() // SWAP H
{
	log->trace("%v : 0xCB34 - SWAP H", reg.pc);

	swap(reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB35() // SWAP L
{
	log->trace("%v : 0xCB35 - SWAP L", reg.pc);

	swap(reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB36() // SWAP (HL)
{
	log->trace("%v : 0xCB36 - SWAP (HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);

    swap(value);

    mcu->write_8bit(reg.hl, value);

	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB37() // SWAP A
{
	log->trace("%v : 0xCB37 - SWAP A", reg.pc);

	swap(reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB38() // SRL B
{
	log->trace("%v : 0xCB38 - SRL B", reg.pc);

	srl_8bit(reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB39() // SRL C
{
	log->trace("%v : 0xCB39 - SRL C", reg.pc);

	srl_8bit(reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB3A() // SRL D
{
	log->trace("%v : 0xCB3A - SRL D", reg.pc);

	srl_8bit(reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB3B() // SRL E
{
	log->trace("%v : 0xCB3B - SRL E", reg.pc);

	srl_8bit(reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB3C() // SRL H
{
	log->trace("%v : 0xCB3C - SRL H", reg.pc);

	srl_8bit(reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB3D() // SRL L
{
	log->trace("%v : 0xCB3D - SRL L", reg.pc);

	srl_8bit(reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB3E() // SRL (HL)
{
	log->trace("%v : 0xCB3E - SRL (HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

    srl_8bit(value);

    mcu->write_8bit(reg.hl, value);
	
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB3F() // SRL A
{
	log->trace("%v : 0xCB3F - SRL A", reg.pc);

	srl_8bit(reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB40() // BIT 0,B
{
	log->trace("%v : 0xCB40 - BIT 0,B", reg.pc);

	bit(0, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB41() // BIT 0,C
{
	log->trace("%v : 0xCB41 - BIT 0,C", reg.pc);

	bit(0, reg.c);

	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB42() // BIT 0,D
{
	log->trace("%v : 0xCB42 - BIT 0,D", reg.pc);

	bit(0, reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB43() // BIT 0,E
{
	log->trace("%v : 0xCB43 - BIT 0,E", reg.pc);

	bit(0, reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB44() // BIT 0,H
{
	log->trace("%v : 0xCB44 - BIT 0,H", reg.pc);

	bit(0, reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB45() // BIT 0,L
{
	log->trace("%v : 0xCB45 - BIT 0,L", reg.pc);

	bit(0, reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB46() // BIT 0,(HL)
{
	log->trace("%v : 0xCB46 - BIT 0,(HL)", reg.pc);

    bit(0, mcu->read_8bit(reg.hl));

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB47() // BIT 0,A
{
	log->trace("%v : 0xCB47 - BIT 0,A", reg.pc);

	bit(0, reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB48() // BIT 1,B
{
	log->trace("%v : 0xCB48 - BIT 1,B", reg.pc);

	bit(1, reg.b);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB49() // BIT 1,C
{
	log->trace("%v : 0xCB49 - BIT 1,C", reg.pc);

	bit(1, reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB4A() // BIT 1,D
{
	log->trace("%v : 0xCB4A - BIT 1,D", reg.pc);

	bit(1, reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB4B() // BIT 1,E
{
	log->trace("%v : 0xCB4B - BIT 1,E", reg.pc);

	bit(1, reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB4C() // BIT 1,H
{
	log->trace("%v : 0xCB4C - BIT 1,H", reg.pc);

	bit(1, reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB4D() // BIT 1,L
{
	log->trace("%v : 0xCB4D - BIT 1,L", reg.pc);

	bit(1, reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB4E() // BIT 1,(HL)
{
	log->trace("%v : 0xCB4E - BIT 1,(HL)", reg.pc);

	bit(1, mcu->read_8bit(reg.hl));
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB4F() // BIT 1,A
{
	log->trace("%v : 0xCB4F - BIT 1,A", reg.pc);

	bit(1, reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB50() // BIT 2,B
{
	log->trace("%v : 0xCB50 - BIT 2,B", reg.pc);

	bit(2, reg.b);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB51() // BIT 2,C
{
	log->trace("%v : 0xCB51 - BIT 2,C", reg.pc);

	bit(2, reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB52() // BIT 2,D
{
	log->trace("%v : 0xCB52 - BIT 2,D", reg.pc);

	bit(2, reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB53() // BIT 2,E
{
	log->trace("%v : 0xCB53 - BIT 2,E", reg.pc);

	bit(2, reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB54() // BIT 2,H
{
	log->trace("%v : 0xCB54 - BIT 2,H", reg.pc);

	bit(2, reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB55() // BIT 2,L
{
	log->trace("%v : 0xCB55 - BIT 2,L", reg.pc);

	bit(2, reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB56() // BIT 2,(HL)
{
	log->trace("%v : 0xCB56 - BIT 2,(HL)", reg.pc);

	bit(2, mcu->read_8bit(reg.hl));
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB57() // BIT 2,A
{
	log->trace("%v : 0xCB57 - BIT 2,A", reg.pc);

	bit(2, reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB58() // BIT 3,B
{
	log->trace("%v : 0xCB58 - BIT 3,B", reg.pc);

	bit(3, reg.b);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB59() // BIT 3,C
{
	log->trace("%v : 0xCB59 - BIT 3,C", reg.pc);

	bit(3, reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB5A() // BIT 3,D
{
	log->trace("%v : 0xCB5A - BIT 3,D", reg.pc);

	bit(3, reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB5B() // BIT 3,E
{
	log->trace("%v : 0xCB5B - BIT 3,E", reg.pc);

	bit(3, reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB5C() // BIT 3,H
{
	log->trace("%v : 0xCB5C - BIT 3,H", reg.pc);

	bit(3, reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB5D() // BIT 3,L
{
	log->trace("%v : 0xCB5D - BIT 3,L", reg.pc);

	bit(3, reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB5E() // BIT 3,(HL)
{
	log->trace("%v : 0xCB5E - BIT 3,(HL)", reg.pc);

	bit(3, mcu->read_8bit(reg.hl));
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB5F() // BIT 3,A
{
	log->trace("%v : 0xCB5F - BIT 3,A", reg.pc);

	bit(3, reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB60() // BIT 4,B
{
	log->trace("%v : 0xCB60 - BIT 4,B", reg.pc);

	bit(4, reg.b);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB61() // BIT 4,C
{
	log->trace("%v : 0xCB61 - BIT 4,C", reg.pc);

	bit(4, reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB62() // BIT 4,D
{
	log->trace("%v : 0xCB62 - BIT 4,D", reg.pc);

	bit(4, reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB63() // BIT 4,E
{
	log->trace("%v : 0xCB63 - BIT 4,E", reg.pc);

	bit(4, reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB64() // BIT 4,H
{
	log->trace("%v : 0xCB64 - BIT 4,H", reg.pc);

	bit(4, reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB65() // BIT 4,L
{
	log->trace("%v : 0xCB65 - BIT 4,L", reg.pc);

	bit(4, reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB66() // BIT 4,(HL)
{
	log->trace("%v : 0xCB66 - BIT 4,(HL)", reg.pc);

	bit(4, mcu->read_8bit(reg.hl));
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB67() // BIT 4,A
{
	log->trace("%v : 0xCB67 - BIT 4,A", reg.pc);

	bit(4, reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB68() // BIT 5,B
{
	log->trace("%v : 0xCB68 - BIT 5,B", reg.pc);

	bit(5, reg.b);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB69() // BIT 5,C
{
	log->trace("%v : 0xCB69 - BIT 5,C", reg.pc);

	bit(5, reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB6A() // BIT 5,D
{
	log->trace("%v : 0xCB6A - BIT 5,D", reg.pc);

	bit(5, reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB6B() // BIT 5,E
{
	log->trace("%v : 0xCB6B - BIT 5,E", reg.pc);

	bit(5, reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB6C() // BIT 5,H
{
	log->trace("%v : 0xCB6C - BIT 5,H", reg.pc);

	bit(5, reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB6D() // BIT 5,L
{
	log->trace("%v : 0xCB6D - BIT 5,L", reg.pc);

	bit(5, reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB6E() // BIT 5,(HL)
{
	log->trace("%v : 0xCB6E - BIT 5,(HL)", reg.pc);

	bit(5, mcu->read_8bit(reg.hl));
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB6F() // BIT 5,A
{
	log->trace("%v : 0xCB6F - BIT 5,A", reg.pc);

	bit(5, reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB70() // BIT 6,B
{
	log->trace("%v : 0xCB70 - BIT 6,B", reg.pc);

	bit(6, reg.b);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB71() // BIT 6,C
{
	log->trace("%v : 0xCB71 - BIT 6,C", reg.pc);

	bit(6, reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB72() // BIT 6,D
{
	log->trace("%v : 0xCB72 - BIT 6,D", reg.pc);

	bit(6, reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB73() // BIT 6,E
{
	log->trace("%v : 0xCB73 - BIT 6,E", reg.pc);

	bit(6, reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB74() // BIT 6,H
{
	log->trace("%v : 0xCB74 - BIT 6,H", reg.pc);

	bit(6, reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB75() // BIT 6,L
{
	log->trace("%v : 0xCB75 - BIT 6,L", reg.pc);

	bit(6, reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB76() // BIT 6,(HL)
{
	log->trace("%v : 0xCB76 - BIT 6,(HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

    bit(6, value);

    mcu->write_8bit(reg.hl, value);
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB77() // BIT 6,A
{
	log->trace("%v : 0xCB77 - BIT 6,A", reg.pc);

	bit(6, reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB78() // BIT 7,B
{
	log->trace("%v : 0xCB78 - BIT 7,B", reg.pc);

	bit(7, reg.b);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB79() // BIT 7,C
{
	log->trace("%v : 0xCB79 - BIT 7,C", reg.pc);

	bit(7, reg.c);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB7A() // BIT 7,D
{
	log->trace("%v : 0xCB7A - BIT 7,D", reg.pc);

	bit(7, reg.d);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB7B() // BIT 7,E
{
	log->trace("%v : 0xCB7B - BIT 7,E", reg.pc);

	bit(7, reg.e);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB7C() // BIT 7,H
{
	log->trace("%v : 0xCB7C - BIT 7,H", reg.pc);

	bit(7, reg.h);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB7D() // BIT 7,L
{
	log->trace("%v : 0xCB7D - BIT 7,L", reg.pc);

	bit(7, reg.l);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB7E() // BIT 7,(HL)
{
	log->trace("%v : 0xCB7E - BIT 7,(HL)", reg.pc);

	bit(7, mcu->read_8bit(reg.hl));
    
	reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB7F() // BIT 7,A
{
	log->trace("%v : 0xCB7F - BIT 7,A", reg.pc);

	bit(7, reg.a);
    
	reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB80() // RES 0,B
{
	log->trace("%v : 0xCB80 - RES 0,B", reg.pc);

	res(0, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB81() // RES 0,C
{
	log->trace("%v : 0xCB81 - RES 0,C", reg.pc);

	res(0, reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB82() // RES 0,D
{
	log->trace("%v : 0xCB82 - RES 0,D", reg.pc);

	res(0, reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB83() // RES 0,E
{
	log->trace("%v : 0xCB83 - RES 0,E", reg.pc);

	res(0, reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB84() // RES 0,H
{
	log->trace("%v : 0xCB84 - RES 0,H", reg.pc);

	res(0, reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB85() // RES 0,L
{
	log->trace("%v : 0xCB85 - RES 0,L", reg.pc);

	res(0, reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB86() // RES 0,(HL)
{
	log->trace("%v : 0xCB86 - RES 0,(HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	res(0, value);

    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB87() // RES 0,A
{
	log->trace("%v : 0xCB87 - RES 0,A", reg.pc);

	res(0, reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB88() // RES 1,B
{
	log->trace("%v : 0xCB88 - RES 1,B", reg.pc);

	res(1, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB89() // RES 1,C
{
	log->trace("%v : 0xCB89 - RES 1,C", reg.pc);

	res(1, reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB8A() // RES 1,D
{
	log->trace("%v : 0xCB8A - RES 1,D", reg.pc);

	res(1, reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB8B() // RES 1,E
{
	log->trace("%v : 0xCB8B - RES 1,E", reg.pc);

	res(1, reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB8C() // RES 1,H
{
	log->trace("%v : 0xCB8C - RES 1,H", reg.pc);

	res(1, reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB8D() // RES 1,L
{
	log->trace("%v : 0xCB8D - RES 1,L", reg.pc);

	res(1, reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB8E() // RES 1,(HL)
{
	log->trace("%v : 0xCB8E - RES 1,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);

    res(1, value);

    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB8F() // RES 1,A
{
	log->trace("%v : 0xCB8F - RES 1,A", reg.pc);

	res(1, reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB90() // RES 2,B
{
	log->trace("%v : 0xCB90 - RES 2,B", reg.pc);

	res(2, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB91() // RES 2,C
{
	log->trace("%v : 0xCB91 - RES 2,C", reg.pc);

	res(2, reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB92() // RES 2,D
{
	log->trace("%v : 0xCB92 - RES 2,D", reg.pc);

	res(2, reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB93() // RES 2,E
{
	log->trace("%v : 0xCB93 - RES 2,E", reg.pc);

	res(2, reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB94() // RES 2,H
{
	log->trace("%v : 0xCB94 - RES 2,H", reg.pc);

	res(2, reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB95() // RES 2,L
{
	log->trace("%v : 0xCB95 - RES 2,L", reg.pc);

	res(2, reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB96() // RES 2,(HL)
{
	log->trace("%v : 0xCB96 - RES 2,(HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	res(2, value);

    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB97() // RES 2,A
{
	log->trace("%v : 0xCB97 - RES 2,A", reg.pc);

	res(2, reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB98() // RES 3,B
{
	log->trace("%v : 0xCB98 - RES 3,B", reg.pc);

	res(3, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB99() // RES 3,C
{
	log->trace("%v : 0xCB99 - RES 3,C", reg.pc);

	res(3, reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB9A() // RES 3,D
{
	log->trace("%v : 0xCB9A - RES 3,D", reg.pc);

	res(3, reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB9B() // RES 3,E
{
	log->trace("%v : 0xCB9B - RES 3,E", reg.pc);

	res(3, reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB9C() // RES 3,H
{
	log->trace("%v : 0xCB9C - RES 3,H", reg.pc);

	res(3, reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB9D() // RES 3,L
{
	log->trace("%v : 0xCB9D - RES 3,L", reg.pc);

	res(3, reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCB9E() // RES 3,(HL)
{
	log->trace("%v : 0xCB9E - RES 3,(HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	res(3, value);

    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCB9F() // RES 3,A
{
	log->trace("%v : 0xCB9F - RES 3,A", reg.pc);

	res(3, reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA0() // RES 4,B
{
	log->trace("%v : 0xCBA0 - RES 4,B", reg.pc);

	res(4, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA1() // RES 4,C
{
	log->trace("%v : 0xCBA1 - RES 4,C", reg.pc);

	res(4, reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA2() // RES 4,D
{
	log->trace("%v : 0xCBA2 - RES 4,D", reg.pc);

	res(4, reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA3() // RES 4,E
{
	log->trace("%v : 0xCBA3 - RES 4,E", reg.pc);

	res(4, reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA4() // RES 4,H
{
	log->trace("%v : 0xCBA4 - RES 4,H", reg.pc);

	res(4, reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA5() // RES 4,L
{
	log->trace("%v : 0xCBA5 - RES 4,L", reg.pc);

	res(4, reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA6() // RES 4,(HL)
{
	log->trace("%v : 0xCBA6 - RES 4,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);

    res(4, value);
    
    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBA7() // RES 4,A
{
	log->trace("%v : 0xCBA7 - RES 4,A", reg.pc);

	res(4, reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA8() // RES 5,B
{
	log->trace("%v : 0xCBA8 - RES 5,B", reg.pc);

	res(5, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBA9() // RES 5,C
{
	log->trace("%v : 0xCBA9 - RES 5,C", reg.pc);

	res(5, reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBAA() // RES 5,D
{
	log->trace("%v : 0xCBAA - RES 5,D", reg.pc);

	res(5, reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBAB() // RES 5,E
{
	log->trace("%v : 0xCBAB - RES 5,E", reg.pc);

	res(5, reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBAC() // RES 5,H
{
	log->trace("%v : 0xCBAC - RES 5,H", reg.pc);

	res(5, reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBAD() // RES 5,L
{
	log->trace("%v : 0xCBAD - RES 5,L", reg.pc);

	res(5, reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBAE() // RES 5,(HL)
{
	log->trace("%v : 0xCBAE - RES 5,(HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	res(5, value);

    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBAF() // RES 5,A
{
	log->trace("%v : 0xCBAF - RES 5,A", reg.pc);

	res(5, reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB0() // RES 6,B
{
	log->trace("%v : 0xCBB0 - RES 6,B", reg.pc);

	res(6, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB1() // RES 6,C
{
	log->trace("%v : 0xCBB1 - RES 6,C", reg.pc);

	res(6, reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB2() // RES 6,D
{
	log->trace("%v : 0xCBB2 - RES 6,D", reg.pc);

	res(6, reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB3() // RES 6,E
{
	log->trace("%v : 0xCBB3 - RES 6,E", reg.pc);

	res(6, reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB4() // RES 6,H
{
	log->trace("%v : 0xCBB4 - RES 6,H", reg.pc);

	res(6, reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB5() // RES 6,L
{
	log->trace("%v : 0xCBB5 - RES 6,L", reg.pc);

	res(6, reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB6() // RES 6,(HL)
{
	log->trace("%v : 0xCBB6 - RES 6,(HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	res(6, value);

    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBB7() // RES 6,A
{
	log->trace("%v : 0xCBB7 - RES 6,A", reg.pc);

	res(6, reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB8() // RES 7,B
{
	log->trace("%v : 0xCBB8 - RES 7,B", reg.pc);

	res(7, reg.b);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBB9() // RES 7,C
{
	log->trace("%v : 0xCBB9 - RES 7,C", reg.pc);

	res(7, reg.c);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBBA() // RES 7,D
{
	log->trace("%v : 0xCBBA - RES 7,D", reg.pc);

	res(7, reg.d);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBBB() // RES 7,E
{
	log->trace("%v : 0xCBBB - RES 7,E", reg.pc);

	res(7, reg.e);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBBC() // RES 7,H
{
	log->trace("%v : 0xCBBC - RES 7,H", reg.pc);

	res(7, reg.h);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBBD() // RES 7,L
{
	log->trace("%v : 0xCBBD - RES 7,L", reg.pc);

	res(7, reg.l);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBBE() // RES 7,(HL)
{
	log->trace("%v : 0xCBBE - RES 7,(HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	res(7, value);

    mcu->write_8bit(reg.hl, value);
    
    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBBF() // RES 7,A
{
	log->trace("%v : 0xCBBF - RES 7,A", reg.pc);

	res(7, reg.a);
    
    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC0() // SET 0,B
{
	log->trace("%v : 0xCBC0 - SET 0,B", reg.pc);

	set(0, reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC1() // SET 0,C
{
	log->trace("%v : 0xCBC1 - SET 0,C", reg.pc);

	set(0, reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC2() // SET 0,D
{
	log->trace("%v : 0xCBC2 - SET 0,D", reg.pc);

	set(0, reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC3() // SET 0,E
{
	log->trace("%v : 0xCBC3 - SET 0,E", reg.pc);

	set(0, reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC4() // SET 0,H
{
	log->trace("%v : 0xCBC4 - SET 0,H", reg.pc);

	set(0, reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC5() // SET 0,L
{
	log->trace("%v : 0xCBC5 - SET 0,L", reg.pc);

	set(0, reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC6() // SET 0,(HL)
{
	log->trace("%v : 0xCBC6 - SET 0,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);
    
    set(0, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBC7() // SET 0,A
{
	log->trace("%v : 0xCBC7 - SET 0,A", reg.pc);

	set(0, reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC8() // SET 1,B
{
	log->trace("%v : 0xCBC8 - SET 1,B", reg.pc);

	set(1, reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBC9() // SET 1,C
{
	log->trace("%v : 0xCBC9 - SET 1,C", reg.pc);

	set(1, reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBCA() // SET 1,D
{
	log->trace("%v : 0xCBCA - SET 1,D", reg.pc);

	set(1, reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBCB() // SET 1,E
{
	log->trace("%v : 0xCBCB - SET 1,E", reg.pc);

	set(1, reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBCC() // SET 1,H
{
	log->trace("%v : 0xCBCC - SET 1,H", reg.pc);

	set(1, reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBCD() // SET 1,L
{
	log->trace("%v : 0xCBCD - SET 1,L", reg.pc);

	set(1, reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBCE() // SET 1,(HL)
{
	log->trace("%v : 0xCBCE - SET 1,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);
    
    set(1, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBCF() // SET 1,A
{
	log->trace("%v : 0xCBCF - SET 1,A", reg.pc);

	set(1, reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD0() // SET 2,B
{
	log->trace("%v : 0xCBD0 - SET 2,B", reg.pc);

	set(2, reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD1() // SET 2,C
{
	log->trace("%v : 0xCBD1 - SET 2,C", reg.pc);

	set(2, reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD2() // SET 2,D
{
	log->trace("%v : 0xCBD2 - SET 2,D", reg.pc);

	set(2, reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD3() // SET 2,E
{
	log->trace("%v : 0xCBD3 - SET 2,E", reg.pc);

	set(2, reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD4() // SET 2,H
{
	log->trace("%v : 0xCBD4 - SET 2,H", reg.pc);

	set(2, reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD5() // SET 2,L
{
	log->trace("%v : 0xCBD5 - SET 2,L", reg.pc);

	set(2, reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD6() // SET 2,(HL)
{
	log->trace("%v : 0xCBD6 - SET 2,(HL)", reg.pc);

    uint8 value = mcu->read_8bit(reg.hl);

	set(2, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBD7() // SET 2,A
{
	log->trace("%v : 0xCBD7 - SET 2,A", reg.pc);

	set(2, reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD8() // SET 3,B
{
	log->trace("%v : 0xCBD8 - SET 3,B", reg.pc);

	set(3, reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBD9() // SET 3,C
{
	log->trace("%v : 0xCBD9 - SET 3,C", reg.pc);

	set(3, reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBDA() // SET 3,D
{
	log->trace("%v : 0xCBDA - SET 3,D", reg.pc);

	set(3, reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBDB() // SET 3,E
{
	log->trace("%v : 0xCBDB - SET 3,E", reg.pc);

	set(3, reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBDC() // SET 3,H
{
	log->trace("%v : 0xCBDC - SET 3,H", reg.pc);

	set(3, reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBDD() // SET 3,L
{
	log->trace("%v : 0xCBDD - SET 3,L", reg.pc);

	set(3, reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBDE() // SET 3,(HL)
{
	log->trace("%v : 0xCBDE - SET 3,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);
    
    set(3, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBDF() // SET 3,A
{
	log->trace("%v : 0xCBDF - SET 3,A", reg.pc);

	set(3, reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE0() // SET 4,B
{
	log->trace("%v : 0xCBE0 - SET 4,B", reg.pc);

	set(4, reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE1() // SET 4,C
{
	log->trace("%v : 0xCBE1 - SET 4,C", reg.pc);

	set(4, reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE2() // SET 4,D
{
	log->trace("%v : 0xCBE2 - SET 4,D", reg.pc);

	set(4, reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE3() // SET 4,E
{
	log->trace("%v : 0xCBE3 - SET 4,E", reg.pc);

	set(4, reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE4() // SET 4,H
{
	log->trace("%v : 0xCBE4 - SET 4,H", reg.pc);

	set(4, reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE5() // SET 4,L
{
	log->trace("%v : 0xCBE5 - SET 4,L", reg.pc);

	set(4, reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE6() // SET 4,(HL)
{
	log->trace("%v : 0xCBE6 - SET 4,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);
    
    set(4, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBE7() // SET 4,A
{
	log->trace("%v : 0xCBE7 - SET 4,A", reg.pc);

	set(4, reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE8() // SET 5,B
{
	log->trace("%v : 0xCBE8 - SET 5,B", reg.pc);

	set(5, reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBE9() // SET 5,C
{
	log->trace("%v : 0xCBE9 - SET 5,C", reg.pc);

	set(5, reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBEA() // SET 5,D
{
	log->trace("%v : 0xCBEA - SET 5,D", reg.pc);

	set(5, reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBEB() // SET 5,E
{
	log->trace("%v : 0xCBEB - SET 5,E", reg.pc);

	set(5, reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBEC() // SET 5,H
{
	log->trace("%v : 0xCBEC - SET 5,H", reg.pc);

	set(5, reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBED() // SET 5,L
{
	log->trace("%v : 0xCBED - SET 5,L", reg.pc);

	set(5, reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBEE() // SET 5,(HL)
{
	log->trace("%v : 0xCBEE - SET 5,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);
    
    set(5, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBEF() // SET 5,A
{
	log->trace("%v : 0xCBEF - SET 5,A", reg.pc);

	set(5, reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF0() // SET 6,B
{
	log->trace("%v : 0xCBF0 - SET 6,B", reg.pc);

	set(6, reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF1() // SET 6,C
{
	log->trace("%v : 0xCBF1 - SET 6,C", reg.pc);

	set(6, reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF2() // SET 6,D
{
	log->trace("%v : 0xCBF2 - SET 6,D", reg.pc);

	set(6, reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF3() // SET 6,E
{
	log->trace("%v : 0xCBF3 - SET 6,E", reg.pc);

	set(6, reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF4() // SET 6,H
{
	log->trace("%v : 0xCBF4 - SET 6,H", reg.pc);

	set(6, reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF5() // SET 6,L
{
	log->trace("%v : 0xCBF5 - SET 6,L", reg.pc);

	set(6, reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF6() // SET 6,(HL)
{
	log->trace("%v : 0xCBF6 - SET 6,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);
    
    set(6, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBF7() // SET 6,A
{
	log->trace("%v : 0xCBF7 - SET 6,A", reg.pc);

	set(6, reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF8() // SET 7,B
{
	log->trace("%v : 0xCBF8 - SET 7,B", reg.pc);

	set(7, reg.b);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBF9() // SET 7,C
{
	log->trace("%v : 0xCBF9 - SET 7,C", reg.pc);

	set(7, reg.c);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBFA() // SET 7,D
{
	log->trace("%v : 0xCBFA - SET 7,D", reg.pc);

	set(7, reg.d);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBFB() // SET 7,E
{
	log->trace("%v : 0xCBFB - SET 7,E", reg.pc);

	set(7, reg.e);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBFC() // SET 7,H
{
	log->trace("%v : 0xCBFC - SET 7,H", reg.pc);

	set(7, reg.h);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBFD() // SET 7,L
{
	log->trace("%v : 0xCBFD - SET 7,L", reg.pc);

	set(7, reg.l);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
void LR35902::instr_0xCBFE() // SET 7,(HL)
{
	log->trace("%v : 0xCBFE - SET 7,(HL)", reg.pc);

	uint8 value = mcu->read_8bit(reg.hl);
    
    set(7, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc   += 1;
	t_cycles += 10;
	m_cycles += 4;
}
void LR35902::instr_0xCBFF() // SET 7,A
{
	log->trace("%v : 0xCBFF - SET 7,A", reg.pc);

	set(7, reg.a);

    reg.pc   += 1;
	t_cycles += 8;
	m_cycles += 2;
}
