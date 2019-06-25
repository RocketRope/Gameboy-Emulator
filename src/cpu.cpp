#include "cpu.h"

// Constructor/Destructor // 

LR35902::LR35902(MCU& _mcu)
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
    reg.hl = 0x000D; //0x014D;
    reg.sp = 0xFFFE;
    reg.pc = 0x0100;
}

void LR35902::step()
{
    uint8 opcode = mcu.read_8bit(reg.pc);

    execute_opcode(opcode);
}

void LR35902::run(uint16 break_pc)
{
    for ( int i = 0 ; i < 100000 ; i++ )
    {
        step();

        if ( reg.pc == break_pc )
            return;
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

void LR35902::execute_opcode(uint8 opcode)
{
    (this->*instructions[opcode])();
}

// Misc functions //

void LR35902::daa()
{
    uint8 correction = 0;

    // least significant bcd nibble > 9
    if ( (reg.a & 0x0F) > 0x09 || get_flag(Flag::half_carry) )
        correction |= 0x06;

    // Most significant bcd nibble > 9
    if ( (reg.a & 0xF0) > 0x90 || get_flag(Flag::carry) )
    {
        correction |= 0x60;
        set_flag(Flag::carry, true);
    }

    if ( get_flag(Flag::sub) )
        reg.a -= correction;
    else
        reg.a += correction;

    set_flag(Flag::zero, reg.a == 0);

    set_flag(Flag::half_carry, false);
}
void LR35902::cpl()
{
    reg.a ^= 0xFF;

    set_flag(Flag::sub, true);
    set_flag(Flag::half_carry, true);
}
void LR35902::ccf()
{
    set_flag(Flag::carry, !get_flag(Flag::carry));
    
    set_flag(Flag::sub, false);
    set_flag(Flag::half_carry, false);

}
void LR35902::scf()
{
    set_flag(Flag::carry, true);
    
    set_flag(Flag::sub, false);
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
    push(reg.pc);
    reg.pc = address;
}
void LR35902::jr(bool condition)
{
    if ( condition )
    {
        reg.pc += (int8) mcu.read_8bit(reg.pc + 1);
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

// Load functions //

uint16 LR35902::pop()
{
    uint16 value = mcu.read_16bit(reg.sp);
    reg.sp += 2;
    
    return value;
}
void LR35902::push(uint16 source)
{
    reg.sp -= 2;
    mcu.write_16bit(reg.sp, source);
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

    // Higher nibbles
    result += (reg.a & 0xF0) + (source & 0xF0);

    // Update c flag
    set_flag(Flag::carry, get_bit(8, result));

    reg.a = (uint8) result;

    // Update z flag
    set_flag(Flag::zero, reg.a == 0);

    // Update n flag
    set_flag(Flag::sub, false);
}

void LR35902::sub_8bit(uint8 source, bool with_carry)
{
    // Add Two's Complement
    add_8bit((source ^ 0xFF ) + 1, with_carry);

    // Update h flag
    set_flag(Flag::half_carry, !get_flag(Flag::half_carry));

    // Update c flag
    set_flag(Flag::carry, !get_flag(Flag::carry));

    // Update n flag
    set_flag(Flag::sub, true);
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
    set_flag(Flag::sub, false);
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
    set_flag(Flag::sub, true);
}

void LR35902::add_16bit(uint16 source)
{
    // Add lower byte (to check half carry)
    uint32 result = (reg.hl & 0x00FF) + (source & 0x00FF);

    // Update h flag
    set_flag(Flag::half_carry, get_bit(12, result));

    // Higher byte
    result += (reg.a & 0xFF00) + (source & 0xFF00);

    // Update c flag
    set_flag(Flag::carry,get_bit(16, result));

    reg.a = (uint16) result;

    // Update n flag
    set_flag(Flag::sub, false);
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

    set_flag(Flag::sub, false);
    set_flag(Flag::carry, false);
    set_flag(Flag::half_carry, true);
}
void LR35902::or_8bit(uint8 source)
{
    reg.a = reg.a | source;

    set_flag(Flag::zero, reg.a == 0);

    set_flag(Flag::sub, false);
    set_flag(Flag::carry, false);
    set_flag(Flag::half_carry, false);
}
void LR35902::xor_8bit(uint8 source)
{
    reg.a = reg.a ^ source;

    set_flag(Flag::zero, reg.a == 0);

    set_flag(Flag::sub, false);
    set_flag(Flag::carry, false);
    set_flag(Flag::half_carry, false);
}
void LR35902::cp_8bit(uint8 source)
{
    uint8 temp_a = reg.a;

    sub_8bit(source);

    reg.a = temp_a;
}

void LR35902::rotate_left_8bit(uint8& source, bool through_carry)
{
    bool carry = get_flag(Flag::carry);

    // Update c flag
    set_flag(Flag::carry, get_bit(7, source));

    if ( !through_carry )
        carry = get_flag(Flag::carry);

    source = source << 1;

    if ( carry )
        source |= 0x01;

    // Update z flag
    set_flag(Flag::zero, source == 0);
}
void LR35902::rotate_right_8bit(uint8& source, bool through_carry)
{
    bool carry = get_flag(Flag::carry);

    // Update c flag
    set_flag(Flag::carry, get_bit(7, source));

    if ( !through_carry )
        carry = get_flag(Flag::carry);

    source = source << 1;

    if ( carry )
        source |= 0x01;

    // Update z flag
    set_flag(Flag::zero, source == 0);
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
    log->trace("%v : 0x01 - LD BC,%v", reg.pc, mcu.read_16bit(reg.pc + 1));

    reg.bc = mcu.read_16bit(reg.pc + 1);
    
    reg.pc   += 3;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x02() // LD (BC),A
{
    log->trace("%v : 0x02 - LD (BC),A", reg.pc);

    mcu.write_8bit(reg.bc, reg.a);

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

    reg.b = mcu.read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x07() // RLCA
{
    log->trace("%v : 0x07 - RLCA", reg.pc);

    rotate_left_8bit(reg.a, false);

    set_flag(Flag::zero, false);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x08() // LD (a16),SP
{
    log->trace("%v : 0x08 - LD (a16),SP", reg.pc);

    mcu.write_16bit(mcu.read_16bit(reg.pc + 1), reg.sp);

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

    reg.a = mcu.read_8bit(reg.bc);

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

    reg.c = mcu.read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x0F() // RRCA
{
    log->trace("%v : 0x0F - RRCA", reg.pc);

    rotate_right_8bit(reg.a, false);

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

    reg.de = mcu.read_16bit(reg.pc + 1);

    reg.pc   += 3;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x12() // LD (DE),A
{
    log->trace("%v : 0x12 - LD (DE),A", reg.pc);

    mcu.write_8bit(reg.de, reg.a);

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

    reg.d = mcu.read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x17() // RLA
{
    log->trace("%v : 0x17 - RLA", reg.pc);

    rotate_left_8bit(reg.a, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x18() // JR r8
{
    log->trace("%v : 0x18 - JR r8", reg.pc);

    jr( true );
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

    reg.a = mcu.read_8bit(reg.de);

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

    reg.e = mcu.read_8bit(reg.pc + 1);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x1F() // RRA
{
    log->trace("%v : 0x1F - RRA", reg.pc);

    rotate_right_8bit(reg.a, true);

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0x20() // JR NZ,r8
{
    log->trace("%v : 0x20 - JR NZ,r8", reg.pc);

    jr( get_flag(Flag::zero) == false );   
}
void LR35902::instr_0x21() // LD HL,d16
{
    log->trace("%v : 0x21 - LD HL,d16", reg.pc);

    reg.hl = mcu.read_16bit(reg.pc + 1);

    reg.pc   += 3;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x22() // LD (HL+),A
{
    log->trace("%v : 0x22 - LD (HL+),A", reg.pc);

    mcu.write_16bit(reg.hl, reg.a);
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

    reg.h = mcu.read_8bit(reg.pc + 1);

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

    jr( get_flag(Flag::zero) == true );
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

    reg.a = mcu.read_8bit(reg.hl);
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

    reg.l = mcu.read_8bit(reg.pc + 1);

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

    jr( get_flag(Flag::carry) == false );
}
void LR35902::instr_0x31() // LD SP,d16
{
    log->trace("%v : 0x31 - LD SP,d16", reg.pc);

    reg.sp = mcu.read_16bit(reg.pc + 1);

    reg.pc   += 3;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x32() // LD (HL-),A
{
    log->trace("%v : 0x32 - LD (HL-),A", reg.pc);

    mcu.write_8bit(reg.hl, reg.a);
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

    inc_8bit(mcu.read_8bit(reg.hl));

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x35() // DEC (HL)
{
    log->trace("%v : 0x35 - DEC (HL)", reg.pc);

    uint8 value = mcu.read_8bit(reg.hl);

    dec_8bit(value);

    mcu.write_8bit(reg.hl, value);

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0x36() // LD (HL),d8
{
    log->trace("%v : 0x36 - LD (HL),d8", reg.pc);

    mcu.write_8bit(reg.hl, mcu.read_8bit(reg.pc + 1));

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

    jr( get_flag(Flag::carry) == true );
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

    reg.a = mcu.read_8bit(reg.hl);
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

    reg.a = mcu.read_8bit(reg.pc + 1);

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

    reg.b = mcu.read_8bit(reg.hl);

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

    reg.c = mcu.read_8bit(reg.hl);

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

    reg.d = mcu.read_8bit(reg.hl);

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

    reg.e = mcu.read_8bit(reg.hl);

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

    reg.h = mcu.read_8bit(reg.hl);

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

    reg.l = mcu.read_8bit(reg.hl);

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

    mcu.write_8bit(reg.hl, reg.b);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x71() // LD (HL),C
{
    log->trace("%v : 0x71 - LD (HL),C", reg.pc);

    mcu.write_8bit(reg.hl, reg.c);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x72() // LD (HL),D
{
    log->trace("%v : 0x72 - LD (HL),D", reg.pc);

    mcu.write_8bit(reg.hl, reg.d);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x73() // LD (HL),E
{
    log->trace("%v : 0x73 - LD (HL),E", reg.pc);

    mcu.write_8bit(reg.hl, reg.e);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x74() // LD (HL),H
{
    log->trace("%v : 0x74 - LD (HL),H", reg.pc);

    mcu.write_8bit(reg.hl, reg.h);

    reg.pc   += 1;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0x75() // LD (HL),L
{
    log->trace("%v : 0x75 - LD (HL),L", reg.pc);

    mcu.write_8bit(reg.hl, reg.l);

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

    mcu.write_8bit(reg.hl, reg.a);

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

    reg.a = mcu.read_8bit(reg.hl);

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

    add_8bit(mcu.read_8bit(reg.hl), false);

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

    add_8bit(mcu.read_8bit(reg.hl), true);

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

    sub_8bit(mcu.read_8bit(reg.hl), false);

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

    sub_8bit(mcu.read_8bit(reg.hl), true);

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

    and_8bit(mcu.read_8bit(reg.hl));

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

    xor_8bit(mcu.read_8bit(reg.hl));

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

    or_8bit(mcu.read_8bit(reg.hl));

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

    cp_8bit(mcu.read_8bit(reg.hl));

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
        reg.pc = mcu.read_16bit(reg.pc + 1);
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

    reg.pc = mcu.read_16bit(reg.pc + 1);

    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xC4() // CALL NZ,a16
{
    log->trace("%v : 0xC4 - CALL NZ,a16", reg.pc);

    if ( get_flag(Flag::zero) == false )
    {
        call(mcu.read_16bit(reg.pc + 1));
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

    add_8bit(mcu.read_16bit(reg.pc + 1), false);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xC7() // RST 0x0000
{
    log->trace("%v : 0xC7 - RST 0x0000", reg.pc);

    rst(MCU::Addr::rst_0x00);

    reg.pc   += 1;
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
        reg.pc = mcu.read_16bit(reg.pc + 1);
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
    log->error("%v : 0xCB - PREFIX CB m_cycles", reg.pc);

    uint8 opcode = mcu.read_8bit(reg.pc + 1); 

    reg.pc   += instruction_size[opcode];
    t_cycles += instruction_cycles[opcode];
    m_cycles += instruction_cycles[opcode] / 4;
}
void LR35902::instr_0xCC() // CALL Z,a16
{
    log->trace("%v : 0xCC - CALL Z,a16", reg.pc);

    if ( get_flag(Flag::zero) == true )
    {
        call(mcu.read_16bit(reg.pc + 1));
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

    call(mcu.read_16bit(reg.pc + 1));

    t_cycles += 24;
    m_cycles += 6;
}
void LR35902::instr_0xCE() // ADC A,d8
{
    log->trace("%v : 0xCE - ADC A,d8", reg.pc);

    add_8bit(mcu.read_8bit(reg.pc + 1), true);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xCF() // RST 08H
{
    log->trace("%v : 0xCF - RST 08H", reg.pc);

    rst(MCU::Addr::rst_0x08);

    reg.pc   += 1;
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
        reg.pc = mcu.read_16bit(reg.pc + 1);
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
        call(mcu.read_16bit(reg.pc + 1));
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

    sub_8bit(mcu.read_8bit(mcu.read_8bit(reg.pc + 1)));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xD7() // RST 10H
{
    log->trace("%v : 0xD7 - RST 10H", reg.pc);

    rst(MCU::Addr::rst_0x10);

    reg.pc   += 1;
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

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xDA() // JP C,a16
{
    log->trace("%v : 0xDA - JP C,a16", reg.pc);

    if ( get_flag(Flag::carry) == true )
    {
        reg.pc = mcu.read_16bit(reg.pc + 1);
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
        call(mcu.read_16bit(reg.pc + 1));
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

    sub_8bit(mcu.read_8bit(reg.pc + 1), true);

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xDF() // RST 18H
{
    log->trace("%v : 0xDF - RST 18H", reg.pc);

    rst(MCU::Addr::rst_0x18);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xE0() // LDH (a8),A
{
    log->trace("%v : 0xE0 - LDH (a8),A", reg.pc);

    mcu.write_8bit(0xFF00 + mcu.read_8bit(reg.pc + 1), reg.a);

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

    mcu.write_8bit(0xFF00 + reg.c, reg.a);

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

    and_8bit(mcu.read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xE7() // RST 20H
{
    log->trace("%v : 0xE7 - RST 20H", reg.pc);

    rst(MCU::Addr::rst_0x20);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xE8() // ADD SP,r8
{
    log->trace("%v : 0xE8 - ADD SP,r8", reg.pc);

    uint16 temp_hl = reg.hl;

    reg.hl = reg.sp;
    add_16bit( (int8) mcu.read_8bit(reg.pc + 1) );
    reg.sp = reg.hl;

    reg.hl = temp_hl;

    set_flag(Flag::zero, false);

    reg.pc   += 2;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xE9() // JP (HL)
{
    log->trace("%v : 0xE9 - JP (HL)", reg.pc);

    reg.pc = reg.hl;

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xEA() // LD (a16),A
{
    log->trace("%v : 0xEA - LD (a16),A", reg.pc);

    mcu.write_8bit(mcu.read_16bit(reg.pc + 1), reg.a);

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

    // ****************************** //

    reg.pc   += 1;
    t_cycles += 4;
    m_cycles += 1;
}
void LR35902::instr_0xEE() // XOR d8
{
    log->trace("%v : 0xEE - XOR d8", reg.pc);

    xor_8bit(mcu.read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xEF() // RST 28H
{
    log->trace("%v : 0xEF - RST 28H", reg.pc);

    rst(MCU::Addr::rst_0x28);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xF0() // LDH A,(a8)
{
    log->trace("%v : 0xF0 - LDH A,(a8)", reg.pc);

    reg.a = mcu.read_8bit(0xFF00 + mcu.read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xF1() // POP AF
{
    log->trace("%v : 0xF1 - POP AF", reg.pc);

    reg.af = pop();

    reg.pc   += 1;
    t_cycles += 12;
    m_cycles += 3;
}
void LR35902::instr_0xF2() // LD A,(C)
{
    log->trace("%v : 0xF2 - LD A,(C)", reg.pc);

    reg.a = mcu.read_8bit(0xFF00 + reg.c);

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

    or_8bit(mcu.read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xF7() // RST 30H
{
    log->trace("%v : 0xF7 - RST 30H", reg.pc);

    rst(MCU::Addr::rst_0x30);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
void LR35902::instr_0xF8() // LD HL,SP+r8
{
    log->trace("%v : 0xF8 - LD HL,SP+r8", reg.pc);

    reg.hl = reg.sp + mcu.read_8bit(reg.pc + 1);

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

    reg.a = mcu.read_8bit(mcu.read_16bit(reg.pc + 1));

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

    cp_8bit(mcu.read_8bit(reg.pc + 1));

    reg.pc   += 2;
    t_cycles += 8;
    m_cycles += 2;
}
void LR35902::instr_0xFF() // RST 38H
{
    log->trace("%v : 0xFF - RST 38H", reg.pc);

    rst(MCU::Addr::rst_0x38);

    reg.pc   += 1;
    t_cycles += 16;
    m_cycles += 4;
}
