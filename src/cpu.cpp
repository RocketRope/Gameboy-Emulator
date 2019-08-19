#include "cpu.h"

// Constructor/Destructor //

LR35902::LR35902(MCU *_mcu)
    : carry(reg.f),
      half_carry(reg.f),
      subtraction(reg.f),
      zero(reg.f),
      if_register(_mcu->get_memory_reference(MCU::ADDRESS::IF)),
      ie_register(_mcu->get_memory_reference(MCU::ADDRESS::IE)),
      mcu(_mcu)
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
    di();

    halted = false;

    reg.af = 0x1180; // 0x01B0;
    reg.bc = 0x0000; // 0x0013;
    reg.de = 0xFF56; // 0x00D8;
    reg.hl = 0x000D; // 0x014D;
    reg.sp = 0xFFFE;
    reg.pc = 0x0100;
}

uint64 LR35902::step()
{
    uint64 prev_clocks = clocks;

    // Interrupts (Skip Fetch-Decode-Execute if interrupt occurred)
    if (!handle_interrupts())
    {
        // Fetch
        uint8 opcode = mcu->read_8bit(reg.pc);

        // Decode/Execute
        if (opcode != 0xCB)
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

    return clocks - prev_clocks;
}

void LR35902::requests_interupt(INTERRUPT interrupt)
{
    set_bit(interrupt, if_register, true);
}

//

bool LR35902::handle_interrupts()
{
    if (interrupt_enable)
    {
        uint8 ie_if = if_register & ie_register;

        if (ie_if != 0)
        {
            clocks += 80;

            if (halted)
            {
                reg.pc++;
                clocks += 16;
                halted = false;
            }

            push(reg.pc);

            if (get_bit(INTERRUPT::V_BLANK, ie_if))
            {
                reg.pc = MCU::ADDRESS::INTERUPT_VBLANK;
                set_bit(INTERRUPT::V_BLANK, if_register, false);
            }
            else if (get_bit(INTERRUPT::LCD, ie_if))
            {
                reg.pc = MCU::ADDRESS::INTERUPT_LCD;
                set_bit(INTERRUPT::LCD, if_register, false);
            }
            else if (get_bit(INTERRUPT::TIMER, ie_if))
            {
                reg.pc = MCU::ADDRESS::INTERUPT_TIMER;
                set_bit(INTERRUPT::TIMER, if_register, false);
            }
            else if (get_bit(INTERRUPT::SERIAL, ie_if))
            {
                reg.pc = MCU::ADDRESS::INTERUPT_SERIAL;
                set_bit(INTERRUPT::SERIAL, if_register, false);
            }
            else if (get_bit(INTERRUPT::JOYPAD, ie_if))
            {
                reg.pc = MCU::ADDRESS::INTERUPT_JOYPAD;
                set_bit(INTERRUPT::JOYPAD, if_register, false);
            }

            di();

            mcu->write_8bit(MCU::ADDRESS::IF, if_register);

            return true;
        }
    }

    return false;
}

void LR35902::execute_opcode(uint8 opcode, bool CB_prefix)
{
    if (CB_prefix)
        (this->*instrCB_table[opcode])();
    else
        (this->*instr_table[opcode])();
}

// Misc functions //

void LR35902::daa()
{
    uint8 correction = 0;

    // least significant bcd nibble > 9
    if ((((reg.a & 0x0F) > 0x09) && !subtraction) || half_carry)
        correction |= 0x06;

    // Most significant bcd nibble > 9
    if (((reg.a > 0x99) && !subtraction) || carry)
    {
        correction |= 0x60;
        carry = true;
    }

    if (subtraction)
        reg.a -= correction;
    else
        reg.a += correction;

    half_carry = false;
    zero = (reg.a == 0);
}
void LR35902::cpl()
{
    reg.a ^= 0xFF;

    half_carry = true;
    subtraction = true;
}
void LR35902::swap(uint8 &source)
{
    uint8 low_nibble = source & 0x0F;

    source = (source >> 4) | (low_nibble << 4);

    carry = false;
    half_carry = false;
    subtraction = false;
    zero = (source == 0);
}

void LR35902::ccf()
{
    carry = !carry;

    half_carry = false;
    subtraction = false;
}
void LR35902::scf()
{
    carry = true;

    half_carry = false;
    subtraction = false;
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
    halted = true;
}

// Bit Opcodes //

void LR35902::bit(uint8 n, uint8 source)
{
    zero = !get_bit(n, source);

    half_carry = true;
    subtraction = false;
}
void LR35902::set(uint8 n, uint8 &source)
{
    set_bit(n, source, true);
}
void LR35902::res(uint8 n, uint8 &source)
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

    if (with_carry && carry)
        result++;

    // Update h flag
    half_carry = get_bit(4, result);

    // High nibbles
    result += (reg.a & 0xF0) + (source & 0xF0);

    // Update c flag
    carry = get_bit(8, result);

    reg.a = static_cast<uint8>(result);

    // Clear n flag
    subtraction = false;

    // Update z flag
    zero = (reg.a == 0);
}
void LR35902::sub_8bit(uint8 source, bool with_carry)
{
    // Sub lower nibbles (to check half carry)
    uint16 result = (reg.a & 0x0F) - (source & 0x0F);

    if (with_carry && carry)
        result--;

    // Update h flag
    half_carry = get_bit(4, result);

    // High nibbles
    result += (reg.a & 0xF0) - (source & 0xF0);

    // Update c flag
    carry = get_bit(8, result);

    reg.a = static_cast<uint8>(result);

    // Set n flag
    subtraction = true;

    // Update z flag
    zero = (reg.a == 0);
}

void LR35902::inc_8bit(uint8 &source)
{
    // Add lower nibbles (to check half carry)
    uint8 result_low = (source & 0x0F) + 1;

    // Update h flag
    half_carry = get_bit(4, result_low);

    //
    source++;

    // Clear n flag
    subtraction = false;

    // Update z flag
    zero = (source == 0);
}
void LR35902::dec_8bit(uint8 &source)
{
    uint8 result_low = (source & 0x0F) - 1;

    // Update h flag
    half_carry = get_bit(4, result_low);

    //
    source--;

    // Set n flag
    subtraction = true;

    // Update z flag
    zero = (source == 0);
}

void LR35902::add_16bit(uint16 source)
{
    // Add lower byte (to check half carry)
    uint32 result = (reg.hl & 0x0FFF) + (source & 0x0FFF);

    // Update h flag
    half_carry = get_bit(12, result);

    //
    result = reg.hl + source;

    // Update c flag
    carry = get_bit(16, result);

    reg.hl = static_cast<uint16>(result);

    // Clear n flag
    subtraction = false;
}
void LR35902::inc_16bit(uint16 &source)
{
    source++;
}
void LR35902::dec_16bit(uint16 &source)
{
    source--;
}

void LR35902::and_8bit(uint8 source)
{
    reg.a = reg.a & source;

    carry = false;
    half_carry = true;
    subtraction = false;
    zero = (reg.a == 0);
}
void LR35902::or_8bit(uint8 source)
{
    reg.a = reg.a | source;

    carry = false;
    half_carry = false;
    subtraction = false;
    zero = (reg.a == 0);
}
void LR35902::xor_8bit(uint8 source)
{
    reg.a = reg.a ^ source;

    carry = false;
    half_carry = false;
    subtraction = false;
    zero = (reg.a == 0);
}
void LR35902::cp_8bit(uint8 source)
{
    uint8 temp_a = reg.a;

    sub_8bit(source, false);

    reg.a = temp_a;
}

// Rotates & Shifts functions //

void LR35902::rlc_8bit(uint8 &source)
{
    // Bit 7 to carry
    carry = get_bit(7, source);

    // Shift
    source <<= 1;

    // Bit 0 same as old bit 7
    if (carry)
        set_bit(0, source, true);

    // Clear h and n flag
    half_carry = false;
    subtraction = false;

    // Update z flag
    zero = (source == 0);
}
void LR35902::rl_8bit(uint8 &source)
{
    bool old_carry = carry;

    // Bit 7 to carry
    carry = get_bit(7, source);

    // Shift
    source <<= 1;

    // Bit 0 same as old carry
    if (old_carry)
        set_bit(0, source, true);

    // Clear h and n flag
    half_carry = false;
    subtraction = false;

    // Update z flag
    zero = (source == 0);
}
void LR35902::rrc_8bit(uint8 &source)
{
    // Bit 0 to carry
    carry = get_bit(0, source);

    // Shift
    source >>= 1;

    // Bit 7 same as old bit 0
    if (carry)
        set_bit(7, source, true);

    // Clear h and n flag
    half_carry = false;
    subtraction = false;

    // Update z flag
    zero = (source == 0);
}
void LR35902::rr_8bit(uint8 &source)
{
    bool old_carry = carry;

    // Bit 0 to carry
    carry = get_bit(0, source);

    // Shift
    source >>= 1;

    // Bit 7 same as old carry
    if (old_carry)
        set_bit(7, source, true);

    // Clear h and n flag
    half_carry = false;
    subtraction = false;

    // Update z flag
    zero = (source == 0);
}

void LR35902::sla_8bit(uint8 &source)
{
    // Bit 7 to carry
    carry = get_bit(7, source);

    // Shift
    source <<= 1;

    // Clear h and n flag
    half_carry = false;
    subtraction = false;

    // Update z flag
    zero = (source == 0);
}
void LR35902::sra_8bit(uint8 &source)
{
    // Save bit 7
    bool old_bit7 = get_bit(7, source);

    // Bit 0 to carry
    carry = get_bit(0, source);

    // Shift
    source >>= 1;

    // Set bit 7 to old bit 7
    if (old_bit7)
        set_bit(7, source, true);

    // Clear h and n flag
    half_carry = false;
    subtraction = false;

    // Update z flag
    zero = (source == 0);
}
void LR35902::srl_8bit(uint8 &source)
{
    // Bit 0 to carry
    carry = get_bit(0, source);

    // Shift
    source >>= 1;

    // Clear h and n flag
    half_carry = false;
    subtraction = false;

    // Update z flag
    zero = (source == 0);
}

// Instructions functions //

void LR35902::instr_0x00() // NOP
{
    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x01() // LD BC,d16
{
    reg.bc = mcu->read_16bit(reg.pc + 1);

    reg.pc += 3;
    clocks += 12;
}
void LR35902::instr_0x02() // LD (BC),A
{
    mcu->write_8bit(reg.bc, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x03() // INC BC
{
    inc_16bit(reg.bc);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x04() // INC B
{
    inc_8bit(reg.b);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x05() // DEC B
{
    dec_8bit(reg.b);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x06() // LD B,d8
{
    reg.b = mcu->read_8bit(reg.pc + 1);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0x07() // RLCA
{
    rlc_8bit(reg.a);

    zero = false;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x08() // LD (a16),SP
{
    mcu->write_16bit(mcu->read_16bit(reg.pc + 1), reg.sp);

    reg.pc += 3;
    clocks += 20;
}
void LR35902::instr_0x09() // ADD HL,BC
{
    add_16bit(reg.bc);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x0A() // LD A,(BC)
{
    reg.a = mcu->read_8bit(reg.bc);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x0B() // DEC BC
{
    dec_16bit(reg.bc);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x0C() // INC C
{
    inc_8bit(reg.c);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x0D() // DEC C
{
    dec_8bit(reg.c);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x0E() // LD C,d8
{
    reg.c = mcu->read_8bit(reg.pc + 1);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0x0F() // RRCA
{
    rrc_8bit(reg.a);

    zero = false;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x10() // STOP 0
{
    stop();

    reg.pc += 2;
    clocks += 4;
}
void LR35902::instr_0x11() // LD DE,d16
{
    reg.de = mcu->read_16bit(reg.pc + 1);

    reg.pc += 3;
    clocks += 12;
}
void LR35902::instr_0x12() // LD (DE),A
{
    mcu->write_8bit(reg.de, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x13() // INC DE
{
    inc_16bit(reg.de);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x14() // INC D
{
    inc_8bit(reg.d);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x15() // DEC D
{
    dec_8bit(reg.d);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x16() // LD D,d8
{
    reg.d = mcu->read_8bit(reg.pc + 1);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0x17() // RLA
{
    rl_8bit(reg.a);

    zero = false;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x18() // JR r8
{
    reg.pc += static_cast<int8>(mcu->read_8bit(reg.pc + 1));

    reg.pc += 2;
    clocks += 12;
}
void LR35902::instr_0x19() // ADD HL,DE
{
    add_16bit(reg.de);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x1A() // LD A,(DE)
{
    reg.a = mcu->read_8bit(reg.de);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x1B() // DEC DE
{
    dec_16bit(reg.de);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x1C() // INC E
{
    inc_8bit(reg.e);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x1D() // DEC E
{
    dec_8bit(reg.e);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x1E() // LD E,d8
{
    reg.e = mcu->read_8bit(reg.pc + 1);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0x1F() // RRA
{
    rr_8bit(reg.a);

    zero = false;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x20() // JR NZ,r8
{
    if (zero == false)
    {
        reg.pc += static_cast<int8>(mcu->read_8bit(reg.pc + 1));
        clocks += 12;
    }
    else
    {
        clocks += 8;
    }

    reg.pc += 2;
}
void LR35902::instr_0x21() // LD HL,d16
{
    reg.hl = mcu->read_16bit(reg.pc + 1);

    reg.pc += 3;
    clocks += 12;
}
void LR35902::instr_0x22() // LD (HL+),A
{
    mcu->write_8bit(reg.hl, reg.a);
    reg.hl++;

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x23() // INC HL
{
    inc_16bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x24() // INC H
{
    inc_8bit(reg.h);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x25() // DEC H
{
    dec_8bit(reg.h);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x26() // LD H,d8
{
    reg.h = mcu->read_8bit(reg.pc + 1);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0x27() // DAA
{
    daa();

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x28() // JR Z,r8
{
    if (zero == true)
    {
        reg.pc += static_cast<int8>(mcu->read_8bit(reg.pc + 1));
        clocks += 12;
    }
    else
    {
        clocks += 8;
    }

    reg.pc += 2;
}
void LR35902::instr_0x29() // ADD HL,HL
{
    add_16bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x2A() // LD A,(HL+)
{
    reg.a = mcu->read_8bit(reg.hl);
    reg.hl++;

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x2B() // DEC HL
{
    dec_16bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x2C() // INC L
{
    inc_8bit(reg.l);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x2D() // DEC L
{
    dec_8bit(reg.l);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x2E() // LD L,d8
{
    reg.l = mcu->read_8bit(reg.pc + 1);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0x2F() // CPL
{
    cpl();

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x30() // JR NC,r8
{
    if (carry == false)
    {
        reg.pc += static_cast<int8>(mcu->read_8bit(reg.pc + 1));
        clocks += 12;
    }
    else
    {
        clocks += 8;
    }

    reg.pc += 2;
}
void LR35902::instr_0x31() // LD SP,d16
{
    reg.sp = mcu->read_16bit(reg.pc + 1);

    reg.pc += 3;
    clocks += 12;
}
void LR35902::instr_0x32() // LD (HL-),A
{
    mcu->write_8bit(reg.hl, reg.a);
    reg.hl--;

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x33() // INC SP
{
    inc_16bit(reg.sp);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x34() // INC (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    inc_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 12;
}
void LR35902::instr_0x35() // DEC (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    dec_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 12;
}
void LR35902::instr_0x36() // LD (HL),d8
{
    mcu->write_8bit(reg.hl, mcu->read_8bit(reg.pc + 1));

    reg.pc += 2;
    clocks += 12;
}
void LR35902::instr_0x37() // SCF
{
    scf();

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x38() // JR C,r8
{
    if (carry == true)
    {
        reg.pc += static_cast<int8>(mcu->read_8bit(reg.pc + 1));
        clocks += 12;
    }
    else
    {
        clocks += 8;
    }

    reg.pc += 2;
}
void LR35902::instr_0x39() // ADD HL,SP
{
    add_16bit(reg.sp);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x3A() // LD A,(HL-)
{
    reg.a = mcu->read_8bit(reg.hl);
    reg.hl--;

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x3B() // DEC SP
{
    dec_16bit(reg.sp);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x3C() // INC A
{
    inc_8bit(reg.a);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x3D() // DEC A
{
    dec_8bit(reg.a);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x3E() // LD A,d8
{
    reg.a = mcu->read_8bit(reg.pc + 1);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0x3F() // CCF
{
    ccf();

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x40() // LD B,B
{
    reg.b = reg.b;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x41() // LD B,C
{
    reg.b = reg.c;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x42() // LD B,D
{
    reg.b = reg.d;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x43() // LD B,E
{
    reg.b = reg.e;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x44() // LD B,H
{
    reg.b = reg.h;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x45() // LD B,L
{
    reg.b = reg.l;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x46() // LD B,(HL)
{
    reg.b = mcu->read_8bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x47() // LD B,A
{
    reg.b = reg.a;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x48() // LD C,B
{
    reg.c = reg.b;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x49() // LD C,C
{
    reg.c = reg.c;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x4A() // LD C,D
{
    reg.c = reg.d;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x4B() // LD C,E
{
    reg.c = reg.e;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x4C() // LD C,H
{
    reg.c = reg.h;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x4D() // LD C,L
{
    reg.c = reg.l;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x4E() // LD C,(HL)
{
    reg.c = mcu->read_8bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x4F() // LD C,A
{
    reg.c = reg.a;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x50() // LD D,B
{
    reg.d = reg.b;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x51() // LD D,C
{
    reg.d = reg.c;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x52() // LD D,D
{
    reg.d = reg.d;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x53() // LD D,E
{
    reg.d = reg.e;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x54() // LD D,H
{
    reg.d = reg.h;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x55() // LD D,L
{
    reg.d = reg.l;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x56() // LD D,(HL)
{
    reg.d = mcu->read_8bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x57() // LD D,A
{
    reg.d = reg.a;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x58() // LD E,B
{
    reg.e = reg.b;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x59() // LD E,C
{
    reg.e = reg.c;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x5A() // LD E,D
{
    reg.e = reg.d;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x5B() // LD E,E
{
    reg.e = reg.e;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x5C() // LD E,H
{
    reg.e = reg.h;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x5D() // LD E,L
{
    reg.e = reg.l;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x5E() // LD E,(HL)
{
    reg.e = mcu->read_8bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x5F() // LD E,A
{
    reg.e = reg.a;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x60() // LD H,B
{
    reg.h = reg.b;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x61() // LD H,C
{
    reg.h = reg.c;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x62() // LD H,D
{
    reg.h = reg.d;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x63() // LD H,E
{
    reg.h = reg.e;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x64() // LD H,H
{
    reg.h = reg.h;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x65() // LD H,L
{
    reg.h = reg.l;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x66() // LD H,(HL)
{
    reg.h = mcu->read_8bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x67() // LD H,A
{
    reg.h = reg.a;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x68() // LD L,B
{
    reg.l = reg.b;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x69() // LD L,C
{
    reg.l = reg.c;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x6A() // LD L,D
{
    reg.l = reg.d;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x6B() // LD L,E
{
    reg.l = reg.e;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x6C() // LD L,H
{
    reg.l = reg.h;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x6D() // LD L,L
{
    reg.l = reg.l;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x6E() // LD L,(HL)
{
    reg.l = mcu->read_8bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x6F() // LD L,A
{
    reg.l = reg.a;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x70() // LD (HL),B
{
    mcu->write_8bit(reg.hl, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x71() // LD (HL),C
{
    mcu->write_8bit(reg.hl, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x72() // LD (HL),D
{
    mcu->write_8bit(reg.hl, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x73() // LD (HL),E
{
    mcu->write_8bit(reg.hl, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x74() // LD (HL),H
{
    mcu->write_8bit(reg.hl, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x75() // LD (HL),L
{
    mcu->write_8bit(reg.hl, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x76() // HALT
{
    halt();

    // Exit halt
    if ((if_register & if_register) != 0)
        reg.pc += 1;

    clocks += 4;
}
void LR35902::instr_0x77() // LD (HL),A
{
    mcu->write_8bit(reg.hl, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x78() // LD A,B
{
    reg.a = reg.b;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x79() // LD A,C
{
    reg.a = reg.c;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x7A() // LD A,D
{
    reg.a = reg.d;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x7B() // LD A,E
{
    reg.a = reg.e;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x7C() // LD A,H
{
    reg.a = reg.h;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x7D() // LD A,L
{
    reg.a = reg.l;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x7E() // LD A,(HL)
{
    reg.a = mcu->read_8bit(reg.hl);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x7F() // LD A,A
{
    reg.a = reg.a;

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x80() // ADD A,B
{
    add_8bit(reg.b, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x81() // ADD A,C
{
    add_8bit(reg.c, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x82() // ADD A,D
{
    add_8bit(reg.d, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x83() // ADD A,E
{
    add_8bit(reg.e, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x84() // ADD A,H
{
    add_8bit(reg.h, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x85() // ADD A,L
{
    add_8bit(reg.l, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x86() // ADD A,(HL)
{
    add_8bit(mcu->read_8bit(reg.hl), false);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x87() // ADD A,A
{
    add_8bit(reg.a, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x88() // ADC A,B
{
    add_8bit(reg.b, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x89() // ADC A,C
{
    add_8bit(reg.c, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x8A() // ADC A,D
{
    add_8bit(reg.d, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x8B() // ADC A,E
{
    add_8bit(reg.e, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x8C() // ADC A,H
{
    add_8bit(reg.h, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x8D() // ADC A,L
{
    add_8bit(reg.l, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x8E() // ADC A,(HL)
{
    add_8bit(mcu->read_8bit(reg.hl), true);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x8F() // ADC A,A
{
    add_8bit(reg.a, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x90() // SUB B
{
    sub_8bit(reg.b, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x91() // SUB C
{
    sub_8bit(reg.c, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x92() // SUB D
{
    sub_8bit(reg.d, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x93() // SUB E
{
    sub_8bit(reg.e, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x94() // SUB H
{
    sub_8bit(reg.h, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x95() // SUB L
{
    sub_8bit(reg.l, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x96() // SUB (HL)
{
    sub_8bit(mcu->read_8bit(reg.hl), false);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x97() // SUB A
{
    sub_8bit(reg.a, false);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x98() // SBC A,B
{
    sub_8bit(reg.b, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x99() // SBC A,C
{
    sub_8bit(reg.c, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x9A() // SBC A,D
{
    sub_8bit(reg.d, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x9B() // SBC A,E
{
    sub_8bit(reg.e, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x9C() // SBC A,H
{
    sub_8bit(reg.h, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x9D() // SBC A,L
{
    sub_8bit(reg.l, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0x9E() // SBC A,(HL)
{
    sub_8bit(mcu->read_8bit(reg.hl), true);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0x9F() // SBC A,A
{
    sub_8bit(reg.a, true);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA0() // AND B
{
    and_8bit(reg.b);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA1() // AND C
{
    and_8bit(reg.c);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA2() // AND D
{
    and_8bit(reg.d);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA3() // AND E
{
    and_8bit(reg.e);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA4() // AND H
{
    and_8bit(reg.h);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA5() // AND L
{
    and_8bit(reg.l);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA6() // AND (HL)
{
    and_8bit(mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xA7() // AND A
{
    and_8bit(reg.a);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA8() // XOR B
{
    xor_8bit(reg.b);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xA9() // XOR C
{
    xor_8bit(reg.c);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xAA() // XOR D
{
    xor_8bit(reg.d);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xAB() // XOR E
{
    xor_8bit(reg.e);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xAC() // XOR H
{
    xor_8bit(reg.h);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xAD() // XOR L
{
    xor_8bit(reg.l);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xAE() // XOR (HL)
{
    xor_8bit(mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xAF() // XOR A
{
    xor_8bit(reg.a);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB0() // OR B
{
    or_8bit(reg.b);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB1() // OR C
{
    or_8bit(reg.c);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB2() // OR D
{
    or_8bit(reg.d);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB3() // OR E
{
    or_8bit(reg.e);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB4() // OR H
{
    or_8bit(reg.h);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB5() // OR L
{
    or_8bit(reg.l);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB6() // OR (HL)
{
    or_8bit(mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xB7() // OR A
{
    or_8bit(reg.a);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB8() // CP B
{
    cp_8bit(reg.b);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xB9() // CP C
{
    cp_8bit(reg.c);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xBA() // CP D
{
    cp_8bit(reg.d);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xBB() // CP E
{
    cp_8bit(reg.e);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xBC() // CP H
{
    cp_8bit(reg.h);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xBD() // CP L
{
    cp_8bit(reg.l);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xBE() // CP (HL)
{
    cp_8bit(mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xBF() // CP A
{
    cp_8bit(reg.a);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xC0() // RET NZ
{
    if (zero == false)
    {
        ret();
        clocks += 20;
    }
    else
    {
        reg.pc += 1;
        clocks += 8;
    }
}
void LR35902::instr_0xC1() // POP BC
{
    reg.bc = pop();

    reg.pc += 1;
    clocks += 12;
}
void LR35902::instr_0xC2() // JP NZ,a16
{
    if (zero == false)
    {
        reg.pc = mcu->read_16bit(reg.pc + 1);
        clocks += 16;
    }
    else
    {
        reg.pc += 3;
        clocks += 12;
    }
}
void LR35902::instr_0xC3() // JP a16
{
    reg.pc = mcu->read_16bit(reg.pc + 1);

    clocks += 16;
}
void LR35902::instr_0xC4() // CALL NZ,a16
{
    if (zero == false)
    {
        call(mcu->read_16bit(reg.pc + 1));
        clocks += 24;
    }
    else
    {
        reg.pc += 3;
        clocks += 12;
    }
}
void LR35902::instr_0xC5() // PUSH BC
{
    push(reg.bc);

    reg.pc += 1;
    clocks += 16;
}
void LR35902::instr_0xC6() // ADD A,d8
{
    add_8bit(mcu->read_8bit(reg.pc + 1), false);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xC7() // RST 0x0000
{
    rst(MCU::ADDRESS::RST_00);

    clocks += 16;
}
void LR35902::instr_0xC8() // RET Z
{
    if (zero == true)
    {
        ret();

        clocks += 20;
    }
    else
    {
        reg.pc += 1;
        clocks += 8;
    }
}
void LR35902::instr_0xC9() // RET
{
    ret();

    clocks += 16;
}
void LR35902::instr_0xCA() // JP Z,a16
{
    if (zero == true)
    {
        reg.pc = mcu->read_16bit(reg.pc + 1);
        clocks += 16;
    }
    else
    {
        reg.pc += 3;
        clocks += 12;
    }
}
void LR35902::instr_0xCB() // PREFIX CB
{
    log->error("%v : 0xCB - PREFIX CB", reg.pc);
}
void LR35902::instr_0xCC() // CALL Z,a16
{
    if (zero == true)
    {
        call(mcu->read_16bit(reg.pc + 1));
        clocks += 24;
    }
    else
    {
        reg.pc += 3;
        clocks += 12;
    }
}
void LR35902::instr_0xCD() // CALL a16
{
    call(mcu->read_16bit(reg.pc + 1));

    clocks += 24;
}
void LR35902::instr_0xCE() // ADC A,d8
{
    add_8bit(mcu->read_8bit(reg.pc + 1), true);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xCF() // RST 08H
{
    rst(MCU::ADDRESS::RST_08);

    clocks += 16;
}
void LR35902::instr_0xD0() // RET NC
{
    if (carry == false)
    {
        ret();
        clocks += 20;
    }
    else
    {
        reg.pc += 1;
        clocks += 8;
    }
}
void LR35902::instr_0xD1() // POP DE
{
    reg.de = pop();

    reg.pc += 1;
    clocks += 12;
}
void LR35902::instr_0xD2() // JP NC,a16
{
    if (carry == false)
    {
        reg.pc = mcu->read_16bit(reg.pc + 1);
        clocks += 16;
    }
    else
    {
        reg.pc += 3;
        clocks += 12;
    }
}
void LR35902::instr_0xD3() // Invalid instruction
{
    log->error("%v : 0xD3 - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xD4() // CALL NC,a16
{
    if (carry == false)
    {
        call(mcu->read_16bit(reg.pc + 1));
        clocks += 24;
    }
    else
    {
        reg.pc += 3;
        clocks += 12;
    }
}
void LR35902::instr_0xD5() // PUSH DE
{
    push(reg.de);

    reg.pc += 1;
    clocks += 16;
}
void LR35902::instr_0xD6() // SUB d8
{
    sub_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xD7() // RST 10H
{
    rst(MCU::ADDRESS::RST_10);

    clocks += 16;
}
void LR35902::instr_0xD8() // RET C
{
    if (carry == true)
    {
        ret();
        clocks += 20;
    }
    else
    {
        reg.pc += 1;
        clocks += 8;
    }
}
void LR35902::instr_0xD9() // RETI
{
    ret();
    ei();

    clocks += 16;
}
void LR35902::instr_0xDA() // JP C,a16
{
    if (carry == true)
    {
        reg.pc = mcu->read_16bit(reg.pc + 1);
        clocks += 16;
    }
    else
    {
        reg.pc += 3;
        clocks += 12;
    }
}
void LR35902::instr_0xDB() // Invalid instruction
{
    log->error("%v : 0xDB - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xDC() // CALL C,a16
{
    if (carry == true)
    {
        call(mcu->read_16bit(reg.pc + 1));
        clocks += 24;
    }
    else
    {
        reg.pc += 3;
        clocks += 12;
    }
}
void LR35902::instr_0xDD() // Invalid instruction
{
    log->error("%v : 0xDD - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xDE() // SBC A,d8
{
    sub_8bit(mcu->read_8bit(reg.pc + 1), true);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xDF() // RST 18H
{
    rst(MCU::ADDRESS::RST_18);

    clocks += 16;
}
void LR35902::instr_0xE0() // LDH (a8),A
{
    mcu->write_8bit(0xFF00 + mcu->read_8bit(reg.pc + 1), reg.a);

    reg.pc += 2;
    clocks += 12;
}
void LR35902::instr_0xE1() // POP HL
{
    reg.hl = pop();

    reg.pc += 1;
    clocks += 12;
}
void LR35902::instr_0xE2() // LD (C),A
{
    mcu->write_8bit(0xFF00 + reg.c, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xE3() // Invalid instruction
{
    log->error("%v : 0xE3 - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xE4() // Invalid instruction
{
    log->error("%v : 0xE4 - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xE5() // PUSH HL
{
    push(reg.hl);

    reg.pc += 1;
    clocks += 16;
}
void LR35902::instr_0xE6() // AND d8
{
    and_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xE7() // RST 20H
{
    rst(MCU::ADDRESS::RST_20);

    clocks += 16;
}
void LR35902::instr_0xE8() // ADD SP,r8
{
    int8 e = static_cast<int8>(mcu->read_8bit(reg.pc + 1));

    carry = get_bit(8, (reg.sp & 0x00FF) + e);
    half_carry = get_bit(4, (reg.sp & 0x000F) + e);

    // Invert h and c flags if e < 0
    if (e < 0)
    {
        carry = !carry;
        half_carry = !half_carry;
    }

    reg.sp = reg.sp + e;

    subtraction = false;
    zero = false;

    reg.pc += 2;
    clocks += 16;
}
void LR35902::instr_0xE9() // JP (HL)
{
    reg.pc = reg.hl;

    clocks += 4;
}
void LR35902::instr_0xEA() // LD (a16),A
{
    mcu->write_8bit(mcu->read_16bit(reg.pc + 1), reg.a);

    reg.pc += 3;
    clocks += 16;
}
void LR35902::instr_0xEB() // Invalid instruction
{
    log->error("%v : 0xEB - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xEC() // Invalid instruction
{
    log->error("%v : 0xEC - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xED() // Invalid instruction
{
    log->error("%v : 0xED - Invalid instruction", reg.pc);

    // // ***************// *************** //

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xEE() // XOR d8
{
    xor_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xEF() // RST 28H
{
    rst(MCU::ADDRESS::RST_28);

    clocks += 16;
}
void LR35902::instr_0xF0() // LDH A,(a8)
{
    reg.a = mcu->read_8bit(0xFF00 + mcu->read_8bit(reg.pc + 1));

    reg.pc += 2;
    clocks += 12;
}
void LR35902::instr_0xF1() // POP AF
{
    reg.af = pop();

    // Clear lower nibble
    reg.f = reg.f & 0xF0;

    reg.pc += 1;
    clocks += 12;
}
void LR35902::instr_0xF2() // LD A,(C)
{
    reg.a = mcu->read_8bit(0xFF00 + reg.c);

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xF3() // DI
{
    di();

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xF4() // Invalid instruction
{
    log->error("%v : 0xF4 - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xF5() // PUSH AF
{
    push(reg.af);

    reg.pc += 1;
    clocks += 16;
}
void LR35902::instr_0xF6() // OR d8
{
    or_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xF7() // RST 30H
{
    rst(MCU::ADDRESS::RST_30);

    clocks += 16;
}
void LR35902::instr_0xF8() // LD HL,SP+r8
{
    int8 e = static_cast<int8>(mcu->read_8bit(reg.pc + 1));

    carry = get_bit(8, (reg.sp & 0x00FF) + e);
    half_carry = get_bit(4, (reg.sp & 0x000F) + e);

    // Invert h and c flags if e < 0
    if (e < 0)
    {
        carry = !carry;
        half_carry = !half_carry;
    }

    reg.hl = reg.sp + e;

    subtraction = false;
    zero = false;

    reg.pc += 2;
    clocks += 12;
}
void LR35902::instr_0xF9() // LD SP,HL
{
    reg.sp = reg.hl;

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xFA() // LD A,(a16)
{
    reg.a = mcu->read_8bit(mcu->read_16bit(reg.pc + 1));

    reg.pc += 3;
    clocks += 16;
}
void LR35902::instr_0xFB() // EI
{
    ei();

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xFC() // Invalid instruction
{
    log->error("%v : 0xFC - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xFD() // Invalid instruction
{
    log->error("%v : 0xFD - Invalid instruction", reg.pc);

    reg.pc += 1;
    clocks += 4;
}
void LR35902::instr_0xFE() // CP d8
{
    cp_8bit(mcu->read_8bit(reg.pc + 1));

    reg.pc += 2;
    clocks += 8;
}
void LR35902::instr_0xFF() // RST 38H
{
    rst(MCU::ADDRESS::RST_38);

    clocks += 16;
}

void LR35902::instr_0xCB00() // RLC B
{
    rlc_8bit(reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB01() // RLC C
{
    rlc_8bit(reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB02() // RLC D
{
    rlc_8bit(reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB03() // RLC E
{
    rlc_8bit(reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB04() // RLC H
{
    rlc_8bit(reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB05() // RLC L
{
    rlc_8bit(reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB06() // RLC (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    rlc_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB07() // RLC A
{
    rlc_8bit(reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB08() // RRC B
{
    rrc_8bit(reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB09() // RRC C
{
    rrc_8bit(reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB0A() // RRC D
{
    rrc_8bit(reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB0B() // RRC E
{
    rrc_8bit(reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB0C() // RRC H
{
    rrc_8bit(reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB0D() // RRC L
{
    rrc_8bit(reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB0E() // RRC (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    rrc_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB0F() // RRC A
{
    rrc_8bit(reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB10() // RL B
{
    rl_8bit(reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB11() // RL C
{
    rl_8bit(reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB12() // RL D
{
    rl_8bit(reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB13() // RL E
{
    rl_8bit(reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB14() // RL H
{
    rl_8bit(reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB15() // RL L
{
    rl_8bit(reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB16() // RL (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    rl_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB17() // RL A
{
    rl_8bit(reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB18() // RR B
{
    rr_8bit(reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB19() // RR C
{
    rr_8bit(reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB1A() // RR D
{
    rr_8bit(reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB1B() // RR E
{
    rr_8bit(reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB1C() // RR H
{
    rr_8bit(reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB1D() // RR L
{
    rr_8bit(reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB1E() // RR (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    rr_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB1F() // RR A
{
    rr_8bit(reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB20() // SLA B
{
    sla_8bit(reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB21() // SLA C
{
    sla_8bit(reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB22() // SLA D
{
    sla_8bit(reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB23() // SLA E
{
    sla_8bit(reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB24() // SLA H
{
    sla_8bit(reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB25() // SLA L
{
    sla_8bit(reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB26() // SLA (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    sla_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB27() // SLA A
{
    sla_8bit(reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB28() // SRA B
{
    sra_8bit(reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB29() // SRA C
{
    sra_8bit(reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB2A() // SRA D
{
    sra_8bit(reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB2B() // SRA E
{
    sra_8bit(reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB2C() // SRA H
{
    sra_8bit(reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB2D() // SRA L
{
    sra_8bit(reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB2E() // SRA (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    sra_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB2F() // SRA A
{
    sra_8bit(reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB30() // SWAP B
{
    swap(reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB31() // SWAP C
{
    swap(reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB32() // SWAP D
{
    swap(reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB33() // SWAP E
{
    swap(reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB34() // SWAP H
{
    swap(reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB35() // SWAP L
{
    swap(reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB36() // SWAP (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    swap(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB37() // SWAP A
{
    swap(reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB38() // SRL B
{
    srl_8bit(reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB39() // SRL C
{
    srl_8bit(reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB3A() // SRL D
{
    srl_8bit(reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB3B() // SRL E
{
    srl_8bit(reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB3C() // SRL H
{
    srl_8bit(reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB3D() // SRL L
{
    srl_8bit(reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB3E() // SRL (HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    srl_8bit(value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB3F() // SRL A
{
    srl_8bit(reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB40() // BIT 0,B
{
    bit(0, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB41() // BIT 0,C
{
    bit(0, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB42() // BIT 0,D
{
    bit(0, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB43() // BIT 0,E
{
    bit(0, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB44() // BIT 0,H
{
    bit(0, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB45() // BIT 0,L
{
    bit(0, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB46() // BIT 0,(HL)
{
    bit(0, mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB47() // BIT 0,A
{
    bit(0, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB48() // BIT 1,B
{
    bit(1, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB49() // BIT 1,C
{
    bit(1, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB4A() // BIT 1,D
{
    bit(1, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB4B() // BIT 1,E
{
    bit(1, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB4C() // BIT 1,H
{
    bit(1, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB4D() // BIT 1,L
{
    bit(1, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB4E() // BIT 1,(HL)
{
    bit(1, mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB4F() // BIT 1,A
{
    bit(1, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB50() // BIT 2,B
{
    bit(2, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB51() // BIT 2,C
{
    bit(2, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB52() // BIT 2,D
{
    bit(2, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB53() // BIT 2,E
{
    bit(2, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB54() // BIT 2,H
{
    bit(2, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB55() // BIT 2,L
{
    bit(2, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB56() // BIT 2,(HL)
{
    bit(2, mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB57() // BIT 2,A
{
    bit(2, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB58() // BIT 3,B
{
    bit(3, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB59() // BIT 3,C
{
    bit(3, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB5A() // BIT 3,D
{
    bit(3, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB5B() // BIT 3,E
{
    bit(3, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB5C() // BIT 3,H
{
    bit(3, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB5D() // BIT 3,L
{
    bit(3, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB5E() // BIT 3,(HL)
{
    bit(3, mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB5F() // BIT 3,A
{
    bit(3, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB60() // BIT 4,B
{
    bit(4, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB61() // BIT 4,C
{
    bit(4, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB62() // BIT 4,D
{
    bit(4, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB63() // BIT 4,E
{
    bit(4, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB64() // BIT 4,H
{
    bit(4, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB65() // BIT 4,L
{
    bit(4, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB66() // BIT 4,(HL)
{
    bit(4, mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB67() // BIT 4,A
{
    bit(4, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB68() // BIT 5,B
{
    bit(5, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB69() // BIT 5,C
{
    bit(5, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB6A() // BIT 5,D
{
    bit(5, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB6B() // BIT 5,E
{
    bit(5, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB6C() // BIT 5,H
{
    bit(5, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB6D() // BIT 5,L
{
    bit(5, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB6E() // BIT 5,(HL)
{
    bit(5, mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB6F() // BIT 5,A
{
    bit(5, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB70() // BIT 6,B
{
    bit(6, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB71() // BIT 6,C
{
    bit(6, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB72() // BIT 6,D
{
    bit(6, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB73() // BIT 6,E
{
    bit(6, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB74() // BIT 6,H
{
    bit(6, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB75() // BIT 6,L
{
    bit(6, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB76() // BIT 6,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    bit(6, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB77() // BIT 6,A
{
    bit(6, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB78() // BIT 7,B
{
    bit(7, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB79() // BIT 7,C
{
    bit(7, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB7A() // BIT 7,D
{
    bit(7, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB7B() // BIT 7,E
{
    bit(7, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB7C() // BIT 7,H
{
    bit(7, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB7D() // BIT 7,L
{
    bit(7, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB7E() // BIT 7,(HL)
{
    bit(7, mcu->read_8bit(reg.hl));

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB7F() // BIT 7,A
{
    bit(7, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB80() // RES 0,B
{
    res(0, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB81() // RES 0,C
{
    res(0, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB82() // RES 0,D
{
    res(0, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB83() // RES 0,E
{
    res(0, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB84() // RES 0,H
{
    res(0, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB85() // RES 0,L
{
    res(0, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB86() // RES 0,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    res(0, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB87() // RES 0,A
{
    res(0, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB88() // RES 1,B
{
    res(1, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB89() // RES 1,C
{
    res(1, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB8A() // RES 1,D
{
    res(1, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB8B() // RES 1,E
{
    res(1, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB8C() // RES 1,H
{
    res(1, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB8D() // RES 1,L
{
    res(1, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB8E() // RES 1,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    res(1, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB8F() // RES 1,A
{
    res(1, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB90() // RES 2,B
{
    res(2, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB91() // RES 2,C
{
    res(2, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB92() // RES 2,D
{
    res(2, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB93() // RES 2,E
{
    res(2, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB94() // RES 2,H
{
    res(2, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB95() // RES 2,L
{
    res(2, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB96() // RES 2,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    res(2, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB97() // RES 2,A
{
    res(2, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB98() // RES 3,B
{
    res(3, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB99() // RES 3,C
{
    res(3, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB9A() // RES 3,D
{
    res(3, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB9B() // RES 3,E
{
    res(3, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB9C() // RES 3,H
{
    res(3, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB9D() // RES 3,L
{
    res(3, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCB9E() // RES 3,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    res(3, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCB9F() // RES 3,A
{
    res(3, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA0() // RES 4,B
{
    res(4, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA1() // RES 4,C
{
    res(4, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA2() // RES 4,D
{
    res(4, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA3() // RES 4,E
{
    res(4, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA4() // RES 4,H
{
    res(4, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA5() // RES 4,L
{
    res(4, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA6() // RES 4,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    res(4, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBA7() // RES 4,A
{
    res(4, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA8() // RES 5,B
{
    res(5, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBA9() // RES 5,C
{
    res(5, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBAA() // RES 5,D
{
    res(5, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBAB() // RES 5,E
{
    res(5, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBAC() // RES 5,H
{
    res(5, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBAD() // RES 5,L
{
    res(5, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBAE() // RES 5,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    res(5, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBAF() // RES 5,A
{
    res(5, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB0() // RES 6,B
{
    res(6, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB1() // RES 6,C
{
    res(6, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB2() // RES 6,D
{
    res(6, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB3() // RES 6,E
{
    res(6, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB4() // RES 6,H
{
    res(6, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB5() // RES 6,L
{
    res(6, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB6() // RES 6,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    res(6, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBB7() // RES 6,A
{
    res(6, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB8() // RES 7,B
{
    res(7, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBB9() // RES 7,C
{
    res(7, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBBA() // RES 7,D
{
    res(7, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBBB() // RES 7,E
{
    res(7, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBBC() // RES 7,H
{
    res(7, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBBD() // RES 7,L
{
    res(7, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBBE() // RES 7,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    res(7, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBBF() // RES 7,A
{
    res(7, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC0() // SET 0,B
{
    set(0, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC1() // SET 0,C
{
    set(0, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC2() // SET 0,D
{
    set(0, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC3() // SET 0,E
{
    set(0, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC4() // SET 0,H
{
    set(0, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC5() // SET 0,L
{
    set(0, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC6() // SET 0,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    set(0, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBC7() // SET 0,A
{
    set(0, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC8() // SET 1,B
{
    set(1, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBC9() // SET 1,C
{
    set(1, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBCA() // SET 1,D
{
    set(1, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBCB() // SET 1,E
{
    set(1, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBCC() // SET 1,H
{
    set(1, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBCD() // SET 1,L
{
    set(1, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBCE() // SET 1,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    set(1, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBCF() // SET 1,A
{
    set(1, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD0() // SET 2,B
{
    set(2, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD1() // SET 2,C
{
    set(2, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD2() // SET 2,D
{
    set(2, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD3() // SET 2,E
{
    set(2, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD4() // SET 2,H
{
    set(2, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD5() // SET 2,L
{
    set(2, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD6() // SET 2,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    set(2, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBD7() // SET 2,A
{
    set(2, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD8() // SET 3,B
{
    set(3, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBD9() // SET 3,C
{
    set(3, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBDA() // SET 3,D
{
    set(3, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBDB() // SET 3,E
{
    set(3, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBDC() // SET 3,H
{
    set(3, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBDD() // SET 3,L
{
    set(3, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBDE() // SET 3,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    set(3, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBDF() // SET 3,A
{
    set(3, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE0() // SET 4,B
{
    set(4, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE1() // SET 4,C
{
    set(4, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE2() // SET 4,D
{
    set(4, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE3() // SET 4,E
{
    set(4, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE4() // SET 4,H
{
    set(4, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE5() // SET 4,L
{
    set(4, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE6() // SET 4,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    set(4, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBE7() // SET 4,A
{
    set(4, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE8() // SET 5,B
{
    set(5, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBE9() // SET 5,C
{
    set(5, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBEA() // SET 5,D
{
    set(5, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBEB() // SET 5,E
{
    set(5, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBEC() // SET 5,H
{
    set(5, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBED() // SET 5,L
{
    set(5, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBEE() // SET 5,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    set(5, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBEF() // SET 5,A
{
    set(5, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF0() // SET 6,B
{
    set(6, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF1() // SET 6,C
{
    set(6, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF2() // SET 6,D
{
    set(6, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF3() // SET 6,E
{
    set(6, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF4() // SET 6,H
{
    set(6, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF5() // SET 6,L
{
    set(6, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF6() // SET 6,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    set(6, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBF7() // SET 6,A
{
    set(6, reg.a);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF8() // SET 7,B
{
    set(7, reg.b);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBF9() // SET 7,C
{
    set(7, reg.c);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBFA() // SET 7,D
{
    set(7, reg.d);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBFB() // SET 7,E
{
    set(7, reg.e);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBFC() // SET 7,H
{
    set(7, reg.h);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBFD() // SET 7,L
{
    set(7, reg.l);

    reg.pc += 1;
    clocks += 8;
}
void LR35902::instr_0xCBFE() // SET 7,(HL)
{
    uint8 value = mcu->read_8bit(reg.hl);

    set(7, value);

    mcu->write_8bit(reg.hl, value);

    reg.pc += 1;
    clocks += 10;
}
void LR35902::instr_0xCBFF() // SET 7,A
{
    set(7, reg.a);

    reg.pc += 1;
    clocks += 8;
}
