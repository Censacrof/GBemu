#include "Cpu.h"

#include <iostream>
#include <cstring>

Cpu::Cpu(Mmu *mmu)
{
    _mmu = mmu;
}

void Cpu::dump()
{
    uint8_t &A = _state[R_A] , &F = _state[R_F]; uint16_t &AF = *((uint16_t *) &_state[R_F]);
    uint8_t &B = _state[R_B] , &C = _state[R_C]; uint16_t &BC = *((uint16_t *) &_state[R_C]);
    uint8_t &D = _state[R_D] , &E = _state[R_E]; uint16_t &DE = *((uint16_t *) &_state[R_E]);
    uint8_t &H = _state[R_H] , &L = _state[R_L]; uint16_t &HL = *((uint16_t *) &_state[R_L]);

    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);

    printf("\n-----------------\n");
    printf("AF = %04x\nBC = %04x\nDE = %04x\nHL = %04x\n\nSP = %04x\nPC = %04x\n\nLast opcode: %04x - %s\n",
        AF,
        BC,
        DE,
        HL,

        SP,
        PC,

        _opcode, _mnemonic.c_str()
    );
    printf("-----------------\n");    
}

void Cpu::reset()
{
    _isHalted = false;

    uint8_t &A = _state[R_A] , &F = _state[R_F]; uint16_t &AF = *((uint16_t *) &_state[R_F]);
    uint8_t &B = _state[R_B] , &C = _state[R_C]; uint16_t &BC = *((uint16_t *) &_state[R_C]);
    uint8_t &D = _state[R_D] , &E = _state[R_E]; uint16_t &DE = *((uint16_t *) &_state[R_E]);
    uint8_t &H = _state[R_H] , &L = _state[R_L]; uint16_t &HL = *((uint16_t *) &_state[R_L]);

    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);

    PC = 0x0000;
    SP = 0xfffe;
}

void Cpu::init()
{
    uint8_t &A = _state[R_A] , &F = _state[R_F]; uint16_t &AF = *((uint16_t *) &_state[R_F]);
    uint8_t &B = _state[R_B] , &C = _state[R_C]; uint16_t &BC = *((uint16_t *) &_state[R_C]);
    uint8_t &D = _state[R_D] , &E = _state[R_E]; uint16_t &DE = *((uint16_t *) &_state[R_E]);
    uint8_t &H = _state[R_H] , &L = _state[R_L]; uint16_t &HL = *((uint16_t *) &_state[R_L]);

    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);

    AF = 0x0001; // GB/SGB => 0x01; GBP => 0xff; GBC => 0x11;
    F = 0xb0;

    BC = 0x0013;
    DE = 0x00d8;
    HL = 0x014d;

    SP = 0xfffe;
    PC = 0x0100;

    _mmu->_memory[Mmu::TIMA] = 0x00;
    _mmu->_memory[Mmu::TMA]  = 0x00;
    _mmu->_memory[Mmu::TAC]  = 0x00;
    _mmu->_memory[Mmu::NR10] = 0x80;
    _mmu->_memory[Mmu::NR11] = 0xbf;
    _mmu->_memory[Mmu::NR12] = 0xf3;
    _mmu->_memory[Mmu::NR14] = 0xbf;
    _mmu->_memory[Mmu::NR21] = 0x3f;
    _mmu->_memory[Mmu::NR22] = 0x00;
    _mmu->_memory[Mmu::NR24] = 0xbf;
    _mmu->_memory[Mmu::NR30] = 0x7f;
    _mmu->_memory[Mmu::NR31] = 0xff;
    _mmu->_memory[Mmu::NR32] = 0x9f;
    _mmu->_memory[Mmu::NR33] = 0xbf;
    _mmu->_memory[Mmu::NR41] = 0xff;
    _mmu->_memory[Mmu::NR42] = 0x00;
    _mmu->_memory[Mmu::NR43] = 0x00;
    _mmu->_memory[Mmu::NR30] = 0xbf;
    _mmu->_memory[Mmu::NR50] = 0x77;
    _mmu->_memory[Mmu::NR51] = 0xf3;
    _mmu->_memory[Mmu::NR52] = 0xf1; // GB => 0xf1; sgb => 0xF0;
    _mmu->_memory[Mmu::LCDC] = 0x91;
    _mmu->_memory[Mmu::SCY]  = 0x00;
    _mmu->_memory[Mmu::SCX]  = 0x00;
    _mmu->_memory[Mmu::LYC]  = 0x00;
    _mmu->_memory[Mmu::BGP]  = 0xfc;
    _mmu->_memory[Mmu::OBP0] = 0xff;
    _mmu->_memory[Mmu::OBP1] = 0xff;
    _mmu->_memory[Mmu::WY]   = 0x00;
    _mmu->_memory[Mmu::WX]   = 0x00;
    _mmu->_memory[Mmu::IE]   = 0x00;

    PC = 0x0100;
    SP = 0xfffe;
}


bool Cpu::cycle()
{
    if (_isHalted)
    {
        printf("Cpu is halted\n");
        return false;
    }

    fetch();
    execute();

    return true;
}


bool Cpu::_checkCondition()
{
    uint8_t F = _state[R_F];

    switch (_opcode & 0x18) // we need to check bit 3 and 4 of the _opcode
    {
        case 0x00: // NZ
            return !(F & F_Z);

        case 0x10: // NC
            return !(F & F_C);

        case 0x01: //  Z
            return F & F_Z;

        case 0x11: //  C
            return F & F_C;
    
        default:
            return false;
    }
}


// reintepret (mantain bit pattern but not value)
int8_t uAsInt8(uint8_t u) { int8_t tmp; std::memcpy(&tmp, &u, sizeof(tmp)); return tmp; }
int16_t uAsInt16(uint16_t u) { int16_t tmp; std::memcpy(&tmp, &u, sizeof(tmp)); return tmp; }

int8_t intAsU8(uint8_t i) { uint8_t tmp; std::memcpy(&tmp, &i, sizeof(tmp)); return tmp; }
int16_t intAsU16(uint16_t i) { uint16_t tmp; std::memcpy(&tmp, &i, sizeof(tmp)); return tmp; }



inline uint8_t Cpu::_readb_PC() 
{ 
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);
    PC += 1;
    return _mmu->readb(PC);
}

inline uint16_t Cpu::_readw_PC() 
{ 
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);
    PC += 2;
    return _mmu->readb(PC);
}


inline void Cpu::_pushb(uint8_t b)
{
    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    SP -= 1,
    _mmu->writeb(SP, b);
}

inline void Cpu::_pushw(uint16_t w)
{
    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    SP -= 2;
    _mmu->writew(SP, w);
}

inline uint8_t Cpu::_popb()
{
    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    uint8_t b = _mmu->readb(SP);
    SP += 1;
    return b;
}

inline uint16_t Cpu::_popw()
{
    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    uint16_t w = _mmu->readb(SP);
    SP += 2;
    return w;
}


// void Cpu::execute()
// {
//     uint8_t &A = _state[R_A] , &F = _state[R_F]; uint16_t &AF = *((uint16_t *) &_state[R_F]);
//     uint8_t &B = _state[R_B] , &C = _state[R_C]; uint16_t &BC = *((uint16_t *) &_state[R_C]);
//     uint8_t &D = _state[R_D] , &E = _state[R_E]; uint16_t &DE = *((uint16_t *) &_state[R_E]);
//     uint8_t &H = _state[R_H] , &L = _state[R_L]; uint16_t &HL = *((uint16_t *) &_state[R_L]);

//     uint16_t &SP = *((uint16_t *) &_state[R_SP]);
//     uint16_t &PC = *((uint16_t *) &_state[R_PC]);


//     switch (_opcode)
//     {
//         // JP nn: Unconditional jump to the absolute address specified by the operand nn
//         case 0xc3:
//         {
//             PC = _mmu->readw(PC);
//             PC += 2;
//             break;
//         }
        
//         // JP HL: Unconditional jump to the absolute address specified by the register HL
//         case 0xe9:
//         {
//             PC = HL;
//             break;
//         }        
        
//         // JP cc, nn: Conditional jump to the absolute address specified by the operand nn, 
//         // depending on the condition cc. Note that the operand (absolute address) is read 
//         // even when the condition is false!
//         case 0xc2: // JP NZ, nn
//         case 0xd2: // JP NC, nn
//         case 0xca: // JP  Z, nn
//         case 0xda: // JP  C, nn
//         {
//             uint16_t op = _readw_PC();

//             if (_checkCondition())
//                 PC = op;
//             break;
//         }

//         // JR r: Unconditional jump to the relative address specified by the signed operand r
//         case 0x18:
//         {
//             int8_t op = uAsInt8(_readb_PC());
//             PC += op;
//             break;
//         }
        
//         // JR cc, r: Conditional jump to the relative address specified by the signed operand r, depending 
//         // on the condition cc. Note that the operand (relative address offset) is read even when 
//         // the condition is false!
//         case 0x20: // JR NZ, r 
//         case 0x30: // JR NC, r 
//         case 0x28: // JR  Z, r 
//         case 0x38: // JR  C, r 
//         {
//             int8_t op = uAsInt8(_readb_PC());

//             if (_checkCondition())
//                 PC += op;            
//             break;
//         }

//         // CALL nn: Unconditional function call to the absolute address specified by the operand nn
//         case 0xcd:
//         {
//             uint16_t op = _readw_PC();

//             _pushw(PC);
//             PC = op;
//             break;
//         }

//         // CALL cc, nn: Conditional function call to the absolute address specified by the operand nn, 
//         // depending on the condition cc. Note that the operand (absolute address) is read even when 
//         // the condition is false!
//         case 0xc4: 
//         case 0xd4:
//         case 0xcc:
//         case 0xdc:
//         {
//             uint16_t op = _readw_PC();

//             if (_checkCondition())
//             {
//                 _pushw(PC);
//                 PC = op;
//             }
//             break;
//         }

//         // RET: Unconditional return from function
//         case 0xc9:
//         {
//             PC = _popw();
//             break;
//         }

//         // RET cc: Conditional return from function, depending on the condition cc
//         case 0xc0:
//         case 0xd0: 
//         case 0xc8:
//         case 0xd8:
//         {
//             if (_checkCondition())
//                 PC = _popw();
//             break;
//         }

//         // RETI: Unconditional return from function. Also enables interrupts by setting IE = 0xff
//         case 0xd9:
//         {
//             PC = _popw();
//             _mmu->writeb(Mmu::IE, 0xff);            
//             break;
//         }

//         // RST n: Unconditional function call to the absolute fixed address 
//         // defined by the opcode in the bits 3, 4 and 5
//         case 0xc7:
//         case 0xd7:
//         case 0xe7:
//         case 0xf7:
//         case 0xcf:
//         case 0xdf:
//         case 0xef:
//         case 0xff:
//         {
//             uint16_t op;
//             switch (_opcode)
//             {
//                 case 0xc7:
//                 {
//                     op = 0x00;
//                     break;
//                 }
                
//                 case 0xcf:
//                 {
//                     op = 0x08;
//                     break;
//                 }
                
//                 case 0xd7:
//                 {
//                     op = 0x10;
//                     break;
//                 }

//                 case 0xdf:
//                 {
//                     op = 0x18;
//                     break;
//                 }

//                 case 0xe7:
//                 {
//                     op = 0x20;
//                     break;
//                 }

//                 case 0xef:
//                 {
//                     op = 0x28;
//                     break;
//                 }

//                 case 0xf7:
//                 {
//                     op = 0x30;
//                     break;
//                 }

//                 case 0xff:
//                 {
//                     op = 0x38;
//                     break;
//                 }
//             }

//             _pushw(PC);
//             PC = op;
//             break;
//         }

//         // HALT: Disables interrupt handling by setting IME=0 and cancelling any 
//         // scheduled effects of the EI instruction if any
//         case 0xf3:
//         {
//             _mmu->writeb(Mmu::IE, 0x00);
//             _isHalted = true;
//             break;
//         }

//         // EI: Schedules interrupt handling to be enabled after the next machine cycle
//         case 0xfb:
//         {
//             printf("WRN: EI instruction not implemented yet\n");
//             break;
//         }


//         // CCF: Flips the carry flag, and clears the N and H flags
//         case 0x3f:
//         {
//             F = F ^ F_C;  //  flip C
//             F = F & ~F_N; // reset N
//             F = F & ~F_H; // reset H
//             break;
//         }

//         // SCF: Sets the carry flag, and clears the N and H flags
//         case 0x37:
//         {
//             F = F | F_C;  //   set F
//             F = F & ~F_N; // reset N
//             F = F & ~F_H; // reset H
//             break;
//         }

//         // DAA: adjusts register A so that the correct representation
//         // of Binary Coded Decimal (BCD) is obtained.
//         // Flags affected:
//         //      Z - Set if register A is zero.
//         //      H - Reset.
//         //      C - Set or reset according to operation.
//         case 0x27:
//         {
//             printf("WRN: DAA instruction not implemented yet\n");
//             break;
//         }

//         // CPL: Flips all the bits in the A register, and sets the N and H flags
//         case 0x2f:
//         {
//             A = ~A;
//             F = F | F_N;
//             F = F | F_H;
//             break;
//         }


//         // NOP and unused opcodes
//         case 0x00:
//         default:
//         {
//             break;
//         }

        
//     }
// }


void Cpu::execute()
{
    switch(_opcode)
    {
        /*##################### NOP #####################*/
        case 0x0: // NOP 
        {
            break;
        }

        /*##################### LD #####################*/
        case 0x1: // LD BC d16
        case 0x2: // LD (BC) A
        case 0x6: // LD B d8
        case 0x8: // LD (a16) SP
        case 0xa: // LD A (BC)
        case 0xe: // LD C d8
        case 0x11: // LD DE d16
        case 0x12: // LD (DE) A
        case 0x16: // LD D d8
        case 0x1a: // LD A (DE)
        case 0x1e: // LD E d8
        case 0x21: // LD HL d16
        case 0x22: // LD (HL+) A
        case 0x26: // LD H d8
        case 0x2a: // LD A (HL+)
        case 0x2e: // LD L d8
        case 0x31: // LD SP d16
        case 0x32: // LD (HL-) A
        case 0x36: // LD (HL) d8
        case 0x3a: // LD A (HL-)
        case 0x3e: // LD A d8
        case 0x40: // LD B B
        case 0x41: // LD B C
        case 0x42: // LD B D
        case 0x43: // LD B E
        case 0x44: // LD B H
        case 0x45: // LD B L
        case 0x46: // LD B (HL)
        case 0x47: // LD B A
        case 0x48: // LD C B
        case 0x49: // LD C C
        case 0x4a: // LD C D
        case 0x4b: // LD C E
        case 0x4c: // LD C H
        case 0x4d: // LD C L
        case 0x4e: // LD C (HL)
        case 0x4f: // LD C A
        case 0x50: // LD D B
        case 0x51: // LD D C
        case 0x52: // LD D D
        case 0x53: // LD D E
        case 0x54: // LD D H
        case 0x55: // LD D L
        case 0x56: // LD D (HL)
        case 0x57: // LD D A
        case 0x58: // LD E B
        case 0x59: // LD E C
        case 0x5a: // LD E D
        case 0x5b: // LD E E
        case 0x5c: // LD E H
        case 0x5d: // LD E L
        case 0x5e: // LD E (HL)
        case 0x5f: // LD E A
        case 0x60: // LD H B
        case 0x61: // LD H C
        case 0x62: // LD H D
        case 0x63: // LD H E
        case 0x64: // LD H H
        case 0x65: // LD H L
        case 0x66: // LD H (HL)
        case 0x67: // LD H A
        case 0x68: // LD L B
        case 0x69: // LD L C
        case 0x6a: // LD L D
        case 0x6b: // LD L E
        case 0x6c: // LD L H
        case 0x6d: // LD L L
        case 0x6e: // LD L (HL)
        case 0x6f: // LD L A
        case 0x70: // LD (HL) B
        case 0x71: // LD (HL) C
        case 0x72: // LD (HL) D
        case 0x73: // LD (HL) E
        case 0x74: // LD (HL) H
        case 0x75: // LD (HL) L
        case 0x77: // LD (HL) A
        case 0x78: // LD A B
        case 0x79: // LD A C
        case 0x7a: // LD A D
        case 0x7b: // LD A E
        case 0x7c: // LD A H
        case 0x7d: // LD A L
        case 0x7e: // LD A (HL)
        case 0x7f: // LD A A
        case 0xe2: // LD (C) A
        case 0xea: // LD (a16) A
        case 0xf2: // LD A (C)
        case 0xf8: // LD HL SP+r8
        case 0xf9: // LD SP HL
        case 0xfa: // LD A (a16)
        {
            break;
        }

        /*##################### INC #####################*/
        case 0x3: // INC BC
        case 0x4: // INC B
        case 0xc: // INC C
        case 0x13: // INC DE
        case 0x14: // INC D
        case 0x1c: // INC E
        case 0x23: // INC HL
        case 0x24: // INC H
        case 0x2c: // INC L
        case 0x33: // INC SP
        case 0x34: // INC (HL)
        case 0x3c: // INC A
        {
            break;
        }

        /*##################### DEC #####################*/
        case 0x5: // DEC B
        case 0xb: // DEC BC
        case 0xd: // DEC C
        case 0x15: // DEC D
        case 0x1b: // DEC DE
        case 0x1d: // DEC E
        case 0x25: // DEC H
        case 0x2b: // DEC HL
        case 0x2d: // DEC L
        case 0x35: // DEC (HL)
        case 0x3b: // DEC SP
        case 0x3d: // DEC A
        {
            break;
        }

        /*##################### RLCA #####################*/
        case 0x7: // RLCA 
        {
            break;
        }

        /*##################### ADD #####################*/
        case 0x9: // ADD HL BC
        case 0x19: // ADD HL DE
        case 0x29: // ADD HL HL
        case 0x39: // ADD HL SP
        case 0x80: // ADD A B
        case 0x81: // ADD A C
        case 0x82: // ADD A D
        case 0x83: // ADD A E
        case 0x84: // ADD A H
        case 0x85: // ADD A L
        case 0x86: // ADD A (HL)
        case 0x87: // ADD A A
        case 0xc6: // ADD A d8
        case 0xe8: // ADD SP r8
        {
            break;
        }

        /*##################### RRCA #####################*/
        case 0xf: // RRCA 
        {
            break;
        }

        /*##################### STOP #####################*/
        case 0x10: // STOP 0
        {
            break;
        }

        /*##################### RLA #####################*/
        case 0x17: // RLA 
        {
            break;
        }

        /*##################### JR #####################*/
        case 0x18: // JR r8
        case 0x20: // JR fNZ r8
        case 0x28: // JR fZ r8
        case 0x30: // JR fNC r8
        case 0x38: // JR fC r8
        {
            break;
        }

        /*##################### RRA #####################*/
        case 0x1f: // RRA 
        {
            break;
        }

        /*##################### DAA #####################*/
        case 0x27: // DAA 
        {
            break;
        }

        /*##################### CPL #####################*/
        case 0x2f: // CPL 
        {
            break;
        }

        /*##################### SCF #####################*/
        case 0x37: // SCF 
        {
            break;
        }

        /*##################### CCF #####################*/
        case 0x3f: // CCF 
        {
            break;
        }

        /*##################### HALT #####################*/
        case 0x76: // HALT 
        {
            break;
        }

        /*##################### ADC #####################*/
        case 0x88: // ADC A B
        case 0x89: // ADC A C
        case 0x8a: // ADC A D
        case 0x8b: // ADC A E
        case 0x8c: // ADC A H
        case 0x8d: // ADC A L
        case 0x8e: // ADC A (HL)
        case 0x8f: // ADC A A
        case 0xce: // ADC A d8
        {
            break;
        }

        /*##################### SUB #####################*/
        case 0x90: // SUB B
        case 0x91: // SUB C
        case 0x92: // SUB D
        case 0x93: // SUB E
        case 0x94: // SUB H
        case 0x95: // SUB L
        case 0x96: // SUB (HL)
        case 0x97: // SUB A
        case 0xd6: // SUB d8
        {
            break;
        }

        /*##################### SBC #####################*/
        case 0x98: // SBC A B
        case 0x99: // SBC A C
        case 0x9a: // SBC A D
        case 0x9b: // SBC A E
        case 0x9c: // SBC A H
        case 0x9d: // SBC A L
        case 0x9e: // SBC A (HL)
        case 0x9f: // SBC A A
        case 0xde: // SBC A d8
        {
            break;
        }

        /*##################### AND #####################*/
        case 0xa0: // AND B
        case 0xa1: // AND C
        case 0xa2: // AND D
        case 0xa3: // AND E
        case 0xa4: // AND H
        case 0xa5: // AND L
        case 0xa6: // AND (HL)
        case 0xa7: // AND A
        case 0xe6: // AND d8
        {
            break;
        }

        /*##################### XOR #####################*/
        case 0xa8: // XOR B
        case 0xa9: // XOR C
        case 0xaa: // XOR D
        case 0xab: // XOR E
        case 0xac: // XOR H
        case 0xad: // XOR L
        case 0xae: // XOR (HL)
        case 0xaf: // XOR A
        case 0xee: // XOR d8
        {
            break;
        }

        /*##################### OR #####################*/
        case 0xb0: // OR B
        case 0xb1: // OR C
        case 0xb2: // OR D
        case 0xb3: // OR E
        case 0xb4: // OR H
        case 0xb5: // OR L
        case 0xb6: // OR (HL)
        case 0xb7: // OR A
        case 0xf6: // OR d8
        {
            break;
        }

        /*##################### CP #####################*/
        case 0xb8: // CP B
        case 0xb9: // CP C
        case 0xba: // CP D
        case 0xbb: // CP E
        case 0xbc: // CP H
        case 0xbd: // CP L
        case 0xbe: // CP (HL)
        case 0xbf: // CP A
        case 0xfe: // CP d8
        {
            break;
        }

        /*##################### RET #####################*/
        case 0xc0: // RET fNZ
        case 0xc8: // RET fZ
        case 0xc9: // RET 
        case 0xd0: // RET fNC
        case 0xd8: // RET fC
        {
            break;
        }

        /*##################### POP #####################*/
        case 0xc1: // POP BC
        case 0xd1: // POP DE
        case 0xe1: // POP HL
        case 0xf1: // POP AF
        {
            break;
        }

        /*##################### JP #####################*/
        case 0xc2: // JP fNZ a16
        case 0xc3: // JP a16
        case 0xca: // JP fZ a16
        case 0xd2: // JP fNC a16
        case 0xda: // JP fC a16
        case 0xe9: // JP (HL)
        {
            break;
        }

        /*##################### CALL #####################*/
        case 0xc4: // CALL fNZ a16
        case 0xcc: // CALL fZ a16
        case 0xcd: // CALL a16
        case 0xd4: // CALL fNC a16
        case 0xdc: // CALL fC a16
        {
            break;
        }

        /*##################### PUSH #####################*/
        case 0xc5: // PUSH BC
        case 0xd5: // PUSH DE
        case 0xe5: // PUSH HL
        case 0xf5: // PUSH AF
        {
            break;
        }

        /*##################### RST #####################*/
        case 0xc7: // RST 00H
        case 0xcf: // RST 08H
        case 0xd7: // RST 10H
        case 0xdf: // RST 18H
        case 0xe7: // RST 20H
        case 0xef: // RST 28H
        case 0xf7: // RST 30H
        case 0xff: // RST 38H
        {
            break;
        }

        /*##################### RETI #####################*/
        case 0xd9: // RETI 
        {
            break;
        }

        /*##################### LDH #####################*/
        case 0xe0: // LDH (a8) A
        case 0xf0: // LDH A (a8)
        {
            break;
        }

        /*##################### DI #####################*/
        case 0xf3: // DI 
        {
            break;
        }

        /*##################### EI #####################*/
        case 0xfb: // EI 
        {
            break;
        }

        /*##################### RLC #####################*/
        case 0x0cb: // RLC B
        case 0x1cb: // RLC C
        case 0x2cb: // RLC D
        case 0x3cb: // RLC E
        case 0x4cb: // RLC H
        case 0x5cb: // RLC L
        case 0x6cb: // RLC (HL)
        case 0x7cb: // RLC A
        {
            break;
        }

        /*##################### RRC #####################*/
        case 0x8cb: // RRC B
        case 0x9cb: // RRC C
        case 0xacb: // RRC D
        case 0xbcb: // RRC E
        case 0xccb: // RRC H
        case 0xdcb: // RRC L
        case 0xecb: // RRC (HL)
        case 0xfcb: // RRC A
        {
            break;
        }

        /*##################### RL #####################*/
        case 0x10cb: // RL B
        case 0x11cb: // RL C
        case 0x12cb: // RL D
        case 0x13cb: // RL E
        case 0x14cb: // RL H
        case 0x15cb: // RL L
        case 0x16cb: // RL (HL)
        case 0x17cb: // RL A
        {
            break;
        }

        /*##################### RR #####################*/
        case 0x18cb: // RR B
        case 0x19cb: // RR C
        case 0x1acb: // RR D
        case 0x1bcb: // RR E
        case 0x1ccb: // RR H
        case 0x1dcb: // RR L
        case 0x1ecb: // RR (HL)
        case 0x1fcb: // RR A
        {
            break;
        }

        /*##################### SLA #####################*/
        case 0x20cb: // SLA B
        case 0x21cb: // SLA C
        case 0x22cb: // SLA D
        case 0x23cb: // SLA E
        case 0x24cb: // SLA H
        case 0x25cb: // SLA L
        case 0x26cb: // SLA (HL)
        case 0x27cb: // SLA A
        {
            break;
        }

        /*##################### SRA #####################*/
        case 0x28cb: // SRA B
        case 0x29cb: // SRA C
        case 0x2acb: // SRA D
        case 0x2bcb: // SRA E
        case 0x2ccb: // SRA H
        case 0x2dcb: // SRA L
        case 0x2ecb: // SRA (HL)
        case 0x2fcb: // SRA A
        {
            break;
        }

        /*##################### SWAP #####################*/
        case 0x30cb: // SWAP B
        case 0x31cb: // SWAP C
        case 0x32cb: // SWAP D
        case 0x33cb: // SWAP E
        case 0x34cb: // SWAP H
        case 0x35cb: // SWAP L
        case 0x36cb: // SWAP (HL)
        case 0x37cb: // SWAP A
        {
            break;
        }

        /*##################### SRL #####################*/
        case 0x38cb: // SRL B
        case 0x39cb: // SRL C
        case 0x3acb: // SRL D
        case 0x3bcb: // SRL E
        case 0x3ccb: // SRL H
        case 0x3dcb: // SRL L
        case 0x3ecb: // SRL (HL)
        case 0x3fcb: // SRL A
        {
            break;
        }

        /*##################### BIT #####################*/
        case 0x40cb: // BIT 0 B
        case 0x41cb: // BIT 0 C
        case 0x42cb: // BIT 0 D
        case 0x43cb: // BIT 0 E
        case 0x44cb: // BIT 0 H
        case 0x45cb: // BIT 0 L
        case 0x46cb: // BIT 0 (HL)
        case 0x47cb: // BIT 0 A
        case 0x48cb: // BIT 1 B
        case 0x49cb: // BIT 1 C
        case 0x4acb: // BIT 1 D
        case 0x4bcb: // BIT 1 E
        case 0x4ccb: // BIT 1 H
        case 0x4dcb: // BIT 1 L
        case 0x4ecb: // BIT 1 (HL)
        case 0x4fcb: // BIT 1 A
        case 0x50cb: // BIT 2 B
        case 0x51cb: // BIT 2 C
        case 0x52cb: // BIT 2 D
        case 0x53cb: // BIT 2 E
        case 0x54cb: // BIT 2 H
        case 0x55cb: // BIT 2 L
        case 0x56cb: // BIT 2 (HL)
        case 0x57cb: // BIT 2 A
        case 0x58cb: // BIT 3 B
        case 0x59cb: // BIT 3 C
        case 0x5acb: // BIT 3 D
        case 0x5bcb: // BIT 3 E
        case 0x5ccb: // BIT 3 H
        case 0x5dcb: // BIT 3 L
        case 0x5ecb: // BIT 3 (HL)
        case 0x5fcb: // BIT 3 A
        case 0x60cb: // BIT 4 B
        case 0x61cb: // BIT 4 C
        case 0x62cb: // BIT 4 D
        case 0x63cb: // BIT 4 E
        case 0x64cb: // BIT 4 H
        case 0x65cb: // BIT 4 L
        case 0x66cb: // BIT 4 (HL)
        case 0x67cb: // BIT 4 A
        case 0x68cb: // BIT 5 B
        case 0x69cb: // BIT 5 C
        case 0x6acb: // BIT 5 D
        case 0x6bcb: // BIT 5 E
        case 0x6ccb: // BIT 5 H
        case 0x6dcb: // BIT 5 L
        case 0x6ecb: // BIT 5 (HL)
        case 0x6fcb: // BIT 5 A
        case 0x70cb: // BIT 6 B
        case 0x71cb: // BIT 6 C
        case 0x72cb: // BIT 6 D
        case 0x73cb: // BIT 6 E
        case 0x74cb: // BIT 6 H
        case 0x75cb: // BIT 6 L
        case 0x76cb: // BIT 6 (HL)
        case 0x77cb: // BIT 6 A
        case 0x78cb: // BIT 7 B
        case 0x79cb: // BIT 7 C
        case 0x7acb: // BIT 7 D
        case 0x7bcb: // BIT 7 E
        case 0x7ccb: // BIT 7 H
        case 0x7dcb: // BIT 7 L
        case 0x7ecb: // BIT 7 (HL)
        case 0x7fcb: // BIT 7 A
        {
            break;
        }

        /*##################### RES #####################*/
        case 0x80cb: // RES 0 B
        case 0x81cb: // RES 0 C
        case 0x82cb: // RES 0 D
        case 0x83cb: // RES 0 E
        case 0x84cb: // RES 0 H
        case 0x85cb: // RES 0 L
        case 0x86cb: // RES 0 (HL)
        case 0x87cb: // RES 0 A
        case 0x88cb: // RES 1 B
        case 0x89cb: // RES 1 C
        case 0x8acb: // RES 1 D
        case 0x8bcb: // RES 1 E
        case 0x8ccb: // RES 1 H
        case 0x8dcb: // RES 1 L
        case 0x8ecb: // RES 1 (HL)
        case 0x8fcb: // RES 1 A
        case 0x90cb: // RES 2 B
        case 0x91cb: // RES 2 C
        case 0x92cb: // RES 2 D
        case 0x93cb: // RES 2 E
        case 0x94cb: // RES 2 H
        case 0x95cb: // RES 2 L
        case 0x96cb: // RES 2 (HL)
        case 0x97cb: // RES 2 A
        case 0x98cb: // RES 3 B
        case 0x99cb: // RES 3 C
        case 0x9acb: // RES 3 D
        case 0x9bcb: // RES 3 E
        case 0x9ccb: // RES 3 H
        case 0x9dcb: // RES 3 L
        case 0x9ecb: // RES 3 (HL)
        case 0x9fcb: // RES 3 A
        case 0xa0cb: // RES 4 B
        case 0xa1cb: // RES 4 C
        case 0xa2cb: // RES 4 D
        case 0xa3cb: // RES 4 E
        case 0xa4cb: // RES 4 H
        case 0xa5cb: // RES 4 L
        case 0xa6cb: // RES 4 (HL)
        case 0xa7cb: // RES 4 A
        case 0xa8cb: // RES 5 B
        case 0xa9cb: // RES 5 C
        case 0xaacb: // RES 5 D
        case 0xabcb: // RES 5 E
        case 0xaccb: // RES 5 H
        case 0xadcb: // RES 5 L
        case 0xaecb: // RES 5 (HL)
        case 0xafcb: // RES 5 A
        case 0xb0cb: // RES 6 B
        case 0xb1cb: // RES 6 C
        case 0xb2cb: // RES 6 D
        case 0xb3cb: // RES 6 E
        case 0xb4cb: // RES 6 H
        case 0xb5cb: // RES 6 L
        case 0xb6cb: // RES 6 (HL)
        case 0xb7cb: // RES 6 A
        case 0xb8cb: // RES 7 B
        case 0xb9cb: // RES 7 C
        case 0xbacb: // RES 7 D
        case 0xbbcb: // RES 7 E
        case 0xbccb: // RES 7 H
        case 0xbdcb: // RES 7 L
        case 0xbecb: // RES 7 (HL)
        case 0xbfcb: // RES 7 A
        {
            break;
        }

        /*##################### SET #####################*/
        case 0xc0cb: // SET 0 B
        case 0xc1cb: // SET 0 C
        case 0xc2cb: // SET 0 D
        case 0xc3cb: // SET 0 E
        case 0xc4cb: // SET 0 H
        case 0xc5cb: // SET 0 L
        case 0xc6cb: // SET 0 (HL)
        case 0xc7cb: // SET 0 A
        case 0xc8cb: // SET 1 B
        case 0xc9cb: // SET 1 C
        case 0xcacb: // SET 1 D
        case 0xcbcb: // SET 1 E
        case 0xcccb: // SET 1 H
        case 0xcdcb: // SET 1 L
        case 0xcecb: // SET 1 (HL)
        case 0xcfcb: // SET 1 A
        case 0xd0cb: // SET 2 B
        case 0xd1cb: // SET 2 C
        case 0xd2cb: // SET 2 D
        case 0xd3cb: // SET 2 E
        case 0xd4cb: // SET 2 H
        case 0xd5cb: // SET 2 L
        case 0xd6cb: // SET 2 (HL)
        case 0xd7cb: // SET 2 A
        case 0xd8cb: // SET 3 B
        case 0xd9cb: // SET 3 C
        case 0xdacb: // SET 3 D
        case 0xdbcb: // SET 3 E
        case 0xdccb: // SET 3 H
        case 0xddcb: // SET 3 L
        case 0xdecb: // SET 3 (HL)
        case 0xdfcb: // SET 3 A
        case 0xe0cb: // SET 4 B
        case 0xe1cb: // SET 4 C
        case 0xe2cb: // SET 4 D
        case 0xe3cb: // SET 4 E
        case 0xe4cb: // SET 4 H
        case 0xe5cb: // SET 4 L
        case 0xe6cb: // SET 4 (HL)
        case 0xe7cb: // SET 4 A
        case 0xe8cb: // SET 5 B
        case 0xe9cb: // SET 5 C
        case 0xeacb: // SET 5 D
        case 0xebcb: // SET 5 E
        case 0xeccb: // SET 5 H
        case 0xedcb: // SET 5 L
        case 0xeecb: // SET 5 (HL)
        case 0xefcb: // SET 5 A
        case 0xf0cb: // SET 6 B
        case 0xf1cb: // SET 6 C
        case 0xf2cb: // SET 6 D
        case 0xf3cb: // SET 6 E
        case 0xf4cb: // SET 6 H
        case 0xf5cb: // SET 6 L
        case 0xf6cb: // SET 6 (HL)
        case 0xf7cb: // SET 6 A
        case 0xf8cb: // SET 7 B
        case 0xf9cb: // SET 7 C
        case 0xfacb: // SET 7 D
        case 0xfbcb: // SET 7 E
        case 0xfccb: // SET 7 H
        case 0xfdcb: // SET 7 L
        case 0xfecb: // SET 7 (HL)
        case 0xffcb: // SET 7 A
        {
            break;
        }

    }
}