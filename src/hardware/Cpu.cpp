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
    printf("AF = %04x\nBC = %04x\nDE = %04x\nHL = %04x\n\nSP = %04x\nPC = %04x\n\nLast opcode: %04x\n",
        AF,
        BC,
        DE,
        HL,

        SP,
        PC,

        _opcode
    );
    printf("-----------------\n");    
}

// TODO: current values are not correct, and most are missing
void Cpu::reset()
{
    _isHalted = false;

    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);

    PC = 0x0000;    
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

void Cpu::fetch()
{
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);

    _opcode = (uint16_t) _mmu->readb(PC);
    if (_opcode == 0xcb) // if this is true the instruction opcode is 2 bites long
    {
        _opcode = _mmu->readw(PC);
        PC += 2;
    } else PC += 1;
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


void Cpu::execute()
{
    uint8_t &A = _state[R_A] , &F = _state[R_F]; uint16_t &AF = *((uint16_t *) &_state[R_F]);
    uint8_t &B = _state[R_B] , &C = _state[R_C]; uint16_t &BC = *((uint16_t *) &_state[R_C]);
    uint8_t &D = _state[R_D] , &E = _state[R_E]; uint16_t &DE = *((uint16_t *) &_state[R_E]);
    uint8_t &H = _state[R_H] , &L = _state[R_L]; uint16_t &HL = *((uint16_t *) &_state[R_L]);

    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);


    switch (_opcode)
    {
        // JP nn: Unconditional jump to the absolute address specified by the operand nn
        case 0xc3:
        {
            PC = _mmu->readw(PC);
            PC += 2;
            break;
        }
        
        // JP HL: Unconditional jump to the absolute address specified by the register HL
        case 0xe9:
        {
            PC = HL;
            break;
        }        
        
        // JP cc, nn: Conditional jump to the absolute address specified by the operand nn, 
        // depending on the condition cc. Note that the operand (absolute address) is read 
        // even when the condition is false!
        case 0xc2: // JP NZ, nn
        case 0xd2: // JP NC, nn
        case 0xca: // JP  Z, nn
        case 0xda: // JP  C, nn
        {
            uint16_t op = _readw_PC();

            if (_checkCondition())
                PC = op;
            break;
        }

        // JR r: Unconditional jump to the relative address specified by the signed operand r
        case 0x18:
        {
            int8_t op = uAsInt8(_readb_PC());
            PC += op;
            break;
        }
        
        // JR cc, r: Conditional jump to the relative address specified by the signed operand r, depending 
        // on the condition cc. Note that the operand (relative address offset) is read even when 
        // the condition is false!
        case 0x20: // JR NZ, r 
        case 0x30: // JR NC, r 
        case 0x28: // JR  Z, r 
        case 0x38: // JR  C, r 
        {
            int8_t op = uAsInt8(_readb_PC());

            if (_checkCondition())
                PC += op;            
            break;
        }

        // CALL nn: Unconditional function call to the absolute address specified by the operand nn
        case 0xcd:
        {
            uint16_t op = _readw_PC();

            _pushw(PC);
            PC = op;
            break;
        }

        // CALL cc, nn: Conditional function call to the absolute address specified by the operand nn, 
        // depending on the condition cc. Note that the operand (absolute address) is read even when 
        // the condition is false!
        case 0xc4: 
        case 0xd4:
        case 0xcc:
        case 0xdc:
        {
            uint16_t op = _readw_PC();

            if (_checkCondition())
            {
                _pushw(PC);
                PC = op;
            }
            break;
        }

        // RET: Unconditional return from function
        case 0xc9:
        {
            PC = _popw();
            break;
        }

        // RET cc: Conditional return from function, depending on the condition cc
        case 0xc0:
        case 0xd0: 
        case 0xc8:
        case 0xd8:
        {
            if (_checkCondition())
                PC = _popw();
            break;
        }

        // RETI: Unconditional return from function. Also enables interrupts by setting IE = 0xff
        case 0xd9:
        {
            PC = _popw();
            _mmu->writeb(Mmu::IE, 0xff);            
            break;
        }

        // RST n: Unconditional function call to the absolute fixed address 
        // defined by the opcode in the bits 3, 4 and 5
        case 0xc7:
        case 0xd7:
        case 0xe7:
        case 0xf7:
        case 0xcf:
        case 0xdf:
        case 0xef:
        case 0xff:
        {
            uint16_t op;
            switch (_opcode)
            {
                case 0xc7:
                {
                    op = 0x00;
                    break;
                }
                
                case 0xcf:
                {
                    op = 0x08;
                    break;
                }
                
                case 0xd7:
                {
                    op = 0x10;
                    break;
                }

                case 0xdf:
                {
                    op = 0x18;
                    break;
                }

                case 0xe7:
                {
                    op = 0x20;
                    break;
                }

                case 0xef:
                {
                    op = 0x28;
                    break;
                }

                case 0xf7:
                {
                    op = 0x30;
                    break;
                }

                case 0xff:
                {
                    op = 0x38;
                    break;
                }
            }

            _pushw(PC);
            PC = op;
            break;
        }

        // HALT: Disables interrupt handling by setting IME=0 and cancelling any 
        // scheduled effects of the EI instruction if any
        case 0xf3:
        {
            _mmu->writeb(Mmu::IE, 0x00);
            _isHalted = true;
        }

        // TODO: EI and so on

        // NOP and unused opcodes
        case 0x00:
        default:
        {
            break;
        }        
    }
}