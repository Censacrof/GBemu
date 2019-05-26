#include "Cpu.h"

void Cpu::fetch()
{
    uint8_t &A = _state[R_A] , &F = _state[R_F]; uint16_t &AF = *((uint16_t *) &_state[R_F]);
    uint8_t &B = _state[R_B] , &C = _state[R_C]; uint16_t &BC = *((uint16_t *) &_state[R_C]);
    uint8_t &D = _state[R_D] , &E = _state[R_E]; uint16_t &DE = *((uint16_t *) &_state[R_E]);
    uint8_t &H = _state[R_H] , &L = _state[R_L]; uint16_t &HL = *((uint16_t *) &_state[R_L]);

    uint16_t &SP = *((uint16_t *) &_state[R_SP]);
    uint16_t &PC = *((uint16_t *) &_state[R_PC]);

    _opcode = (uint16_t) _mmu->readb(PC);
    if (_opcode == 0xcb) // if this is true the instruction opcode is 2 bites long
    {
        _opcode = _mmu->readw(PC);
        PC += 2;
    } else PC += 1;

    switch(_opcode)
    {
        case 0x0: // NOP  
        case 0x7: // RLCA  
        case 0xf: // RRCA  
        case 0x17: // RLA  
        case 0x1f: // RRA  
        case 0x27: // DAA  
        case 0x2f: // CPL  
        case 0x37: // SCF  
        case 0x3f: // CCF  
        case 0x76: // HALT  
        case 0xc9: // RET  
        case 0xd9: // RETI  
        case 0xf3: // DI  
        case 0xfb: // EI  
        {
            _mnemonic = "EI\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x1: // LD BC d16
        {
            _mnemonic = "LD BC d16\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &BC;
            _sourceType = OPT_IMM16;
            _source = (uint16_t *) _readw_PC();
            break;
        }

        case 0x2: // LD (BC) A
        {
            _mnemonic = "LD (BC) A\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) BC;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x3: // INC BC 
        case 0xb: // DEC BC 
        case 0xc1: // POP BC 
        case 0xc5: // PUSH BC 
        {
            _mnemonic = "PUSH BC\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &BC;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x4: // INC B 
        case 0x5: // DEC B 
        case 0x90: // SUB B 
        case 0xa0: // AND B 
        case 0xa8: // XOR B 
        case 0xb0: // OR B 
        case 0xb8: // CP B 
        case 0x0cb: // RLC B 
        case 0x8cb: // RRC B 
        case 0x10cb: // RL B 
        case 0x18cb: // RR B 
        case 0x20cb: // SLA B 
        case 0x28cb: // SRA B 
        case 0x30cb: // SWAP B 
        case 0x38cb: // SRL B 
        {
            _mnemonic = "SRL B\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x6: // LD B d8
        {
            _mnemonic = "LD B d8\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x8: // LD (a16) SP
        {
            _mnemonic = "LD (a16) SP\0";
            _destinationType = OPT_INDADDR8;
            _destination = (uint16_t *) _readb_PC();
            _sourceType = OPT_DIR16;
            _source = (uint16_t *) &SP;
            break;
        }

        case 0x9: // ADD HL BC
        {
            _mnemonic = "ADD HL BC\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &HL;
            _sourceType = OPT_DIR16;
            _source = (uint16_t *) &BC;
            break;
        }

        case 0xa: // LD A (BC)
        {
            _mnemonic = "LD A (BC)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) BC;
            break;
        }

        case 0xc: // INC C 
        case 0xd: // DEC C 
        case 0x91: // SUB C 
        case 0xa1: // AND C 
        case 0xa9: // XOR C 
        case 0xb1: // OR C 
        case 0xb9: // CP C 
        case 0x1cb: // RLC C 
        case 0x9cb: // RRC C 
        case 0x11cb: // RL C 
        case 0x19cb: // RR C 
        case 0x21cb: // SLA C 
        case 0x29cb: // SRA C 
        case 0x31cb: // SWAP C 
        case 0x39cb: // SRL C 
        {
            _mnemonic = "SRL C\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xe: // LD C d8
        {
            _mnemonic = "LD C d8\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x10: // STOP 0 
        {
            _mnemonic = "STOP 0\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x11: // LD DE d16
        {
            _mnemonic = "LD DE d16\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &DE;
            _sourceType = OPT_IMM16;
            _source = (uint16_t *) _readw_PC();
            break;
        }

        case 0x12: // LD (DE) A
        {
            _mnemonic = "LD (DE) A\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) DE;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x13: // INC DE 
        case 0x1b: // DEC DE 
        case 0xd1: // POP DE 
        case 0xd5: // PUSH DE 
        {
            _mnemonic = "PUSH DE\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &DE;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x14: // INC D 
        case 0x15: // DEC D 
        case 0x92: // SUB D 
        case 0xa2: // AND D 
        case 0xaa: // XOR D 
        case 0xb2: // OR D 
        case 0xba: // CP D 
        case 0x2cb: // RLC D 
        case 0xacb: // RRC D 
        case 0x12cb: // RL D 
        case 0x1acb: // RR D 
        case 0x22cb: // SLA D 
        case 0x2acb: // SRA D 
        case 0x32cb: // SWAP D 
        case 0x3acb: // SRL D 
        {
            _mnemonic = "SRL D\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x16: // LD D d8
        {
            _mnemonic = "LD D d8\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x18: // JR r8 
        {
            _mnemonic = "JR r8\0";
            _destinationType = OPT_IMM8;
            _destination = (uint16_t *) _readb_PC();
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x19: // ADD HL DE
        {
            _mnemonic = "ADD HL DE\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &HL;
            _sourceType = OPT_DIR16;
            _source = (uint16_t *) &DE;
            break;
        }

        case 0x1a: // LD A (DE)
        {
            _mnemonic = "LD A (DE)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) DE;
            break;
        }

        case 0x1c: // INC E 
        case 0x1d: // DEC E 
        case 0x93: // SUB E 
        case 0xa3: // AND E 
        case 0xab: // XOR E 
        case 0xb3: // OR E 
        case 0xbb: // CP E 
        case 0x3cb: // RLC E 
        case 0xbcb: // RRC E 
        case 0x13cb: // RL E 
        case 0x1bcb: // RR E 
        case 0x23cb: // SLA E 
        case 0x2bcb: // SRA E 
        case 0x33cb: // SWAP E 
        case 0x3bcb: // SRL E 
        {
            _mnemonic = "SRL E\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x1e: // LD E d8
        {
            _mnemonic = "LD E d8\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x20: // JR fNZ r8
        {
            _mnemonic = "JR fNZ r8\0";
            _destinationType = OPT_NFLAG;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x21: // LD HL d16
        {
            _mnemonic = "LD HL d16\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &HL;
            _sourceType = OPT_IMM16;
            _source = (uint16_t *) _readw_PC();
            break;
        }

        case 0x22: // LD (HL+) A
        {
            _mnemonic = "LD (HL+) A\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x23: // INC HL 
        case 0x2b: // DEC HL 
        case 0xe1: // POP HL 
        case 0xe5: // PUSH HL 
        {
            _mnemonic = "PUSH HL\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &HL;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x24: // INC H 
        case 0x25: // DEC H 
        case 0x94: // SUB H 
        case 0xa4: // AND H 
        case 0xac: // XOR H 
        case 0xb4: // OR H 
        case 0xbc: // CP H 
        case 0x4cb: // RLC H 
        case 0xccb: // RRC H 
        case 0x14cb: // RL H 
        case 0x1ccb: // RR H 
        case 0x24cb: // SLA H 
        case 0x2ccb: // SRA H 
        case 0x34cb: // SWAP H 
        case 0x3ccb: // SRL H 
        {
            _mnemonic = "SRL H\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x26: // LD H d8
        {
            _mnemonic = "LD H d8\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x28: // JR fZ r8
        {
            _mnemonic = "JR fZ r8\0";
            _destinationType = OPT_FLAG;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x29: // ADD HL HL
        {
            _mnemonic = "ADD HL HL\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &HL;
            _sourceType = OPT_DIR16;
            _source = (uint16_t *) &HL;
            break;
        }

        case 0x2a: // LD A (HL+)
        {
            _mnemonic = "LD A (HL+)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x2c: // INC L 
        case 0x2d: // DEC L 
        case 0x95: // SUB L 
        case 0xa5: // AND L 
        case 0xad: // XOR L 
        case 0xb5: // OR L 
        case 0xbd: // CP L 
        case 0x5cb: // RLC L 
        case 0xdcb: // RRC L 
        case 0x15cb: // RL L 
        case 0x1dcb: // RR L 
        case 0x25cb: // SLA L 
        case 0x2dcb: // SRA L 
        case 0x35cb: // SWAP L 
        case 0x3dcb: // SRL L 
        {
            _mnemonic = "SRL L\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x2e: // LD L d8
        {
            _mnemonic = "LD L d8\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x30: // JR fNC r8
        {
            _mnemonic = "JR fNC r8\0";
            _destinationType = OPT_NFLAG;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x31: // LD SP d16
        {
            _mnemonic = "LD SP d16\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &SP;
            _sourceType = OPT_IMM16;
            _source = (uint16_t *) _readw_PC();
            break;
        }

        case 0x32: // LD (HL-) A
        {
            _mnemonic = "LD (HL-) A\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x33: // INC SP 
        case 0x3b: // DEC SP 
        {
            _mnemonic = "DEC SP\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &SP;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x34: // INC (HL) 
        case 0x35: // DEC (HL) 
        case 0x96: // SUB (HL) 
        case 0xa6: // AND (HL) 
        case 0xae: // XOR (HL) 
        case 0xb6: // OR (HL) 
        case 0xbe: // CP (HL) 
        case 0xe9: // JP (HL) 
        case 0x6cb: // RLC (HL) 
        case 0xecb: // RRC (HL) 
        case 0x16cb: // RL (HL) 
        case 0x1ecb: // RR (HL) 
        case 0x26cb: // SLA (HL) 
        case 0x2ecb: // SRA (HL) 
        case 0x36cb: // SWAP (HL) 
        case 0x3ecb: // SRL (HL) 
        {
            _mnemonic = "SRL (HL)\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x36: // LD (HL) d8
        {
            _mnemonic = "LD (HL) d8\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x38: // JR fC r8
        {
            _mnemonic = "JR fC r8\0";
            _destinationType = OPT_FLAG;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x39: // ADD HL SP
        {
            _mnemonic = "ADD HL SP\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &HL;
            _sourceType = OPT_DIR16;
            _source = (uint16_t *) &SP;
            break;
        }

        case 0x3a: // LD A (HL-)
        {
            _mnemonic = "LD A (HL-)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x3c: // INC A 
        case 0x3d: // DEC A 
        case 0x97: // SUB A 
        case 0xa7: // AND A 
        case 0xaf: // XOR A 
        case 0xb7: // OR A 
        case 0xbf: // CP A 
        case 0x7cb: // RLC A 
        case 0xfcb: // RRC A 
        case 0x17cb: // RL A 
        case 0x1fcb: // RR A 
        case 0x27cb: // SLA A 
        case 0x2fcb: // SRA A 
        case 0x37cb: // SWAP A 
        case 0x3fcb: // SRL A 
        {
            _mnemonic = "SRL A\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x3e: // LD A d8
        case 0xc6: // ADD A d8
        case 0xce: // ADC A d8
        case 0xde: // SBC A d8
        {
            _mnemonic = "SBC A d8\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0x40: // LD B B
        {
            _mnemonic = "LD B B\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x41: // LD B C
        {
            _mnemonic = "LD B C\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x42: // LD B D
        {
            _mnemonic = "LD B D\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x43: // LD B E
        {
            _mnemonic = "LD B E\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x44: // LD B H
        {
            _mnemonic = "LD B H\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x45: // LD B L
        {
            _mnemonic = "LD B L\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x46: // LD B (HL)
        {
            _mnemonic = "LD B (HL)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x47: // LD B A
        {
            _mnemonic = "LD B A\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &B;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x48: // LD C B
        {
            _mnemonic = "LD C B\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x49: // LD C C
        {
            _mnemonic = "LD C C\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x4a: // LD C D
        {
            _mnemonic = "LD C D\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x4b: // LD C E
        {
            _mnemonic = "LD C E\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x4c: // LD C H
        {
            _mnemonic = "LD C H\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x4d: // LD C L
        {
            _mnemonic = "LD C L\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x4e: // LD C (HL)
        {
            _mnemonic = "LD C (HL)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x4f: // LD C A
        {
            _mnemonic = "LD C A\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &C;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x50: // LD D B
        {
            _mnemonic = "LD D B\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x51: // LD D C
        {
            _mnemonic = "LD D C\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x52: // LD D D
        {
            _mnemonic = "LD D D\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x53: // LD D E
        {
            _mnemonic = "LD D E\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x54: // LD D H
        {
            _mnemonic = "LD D H\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x55: // LD D L
        {
            _mnemonic = "LD D L\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x56: // LD D (HL)
        {
            _mnemonic = "LD D (HL)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x57: // LD D A
        {
            _mnemonic = "LD D A\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &D;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x58: // LD E B
        {
            _mnemonic = "LD E B\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x59: // LD E C
        {
            _mnemonic = "LD E C\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x5a: // LD E D
        {
            _mnemonic = "LD E D\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x5b: // LD E E
        {
            _mnemonic = "LD E E\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x5c: // LD E H
        {
            _mnemonic = "LD E H\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x5d: // LD E L
        {
            _mnemonic = "LD E L\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x5e: // LD E (HL)
        {
            _mnemonic = "LD E (HL)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x5f: // LD E A
        {
            _mnemonic = "LD E A\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &E;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x60: // LD H B
        {
            _mnemonic = "LD H B\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x61: // LD H C
        {
            _mnemonic = "LD H C\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x62: // LD H D
        {
            _mnemonic = "LD H D\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x63: // LD H E
        {
            _mnemonic = "LD H E\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x64: // LD H H
        {
            _mnemonic = "LD H H\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x65: // LD H L
        {
            _mnemonic = "LD H L\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x66: // LD H (HL)
        {
            _mnemonic = "LD H (HL)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x67: // LD H A
        {
            _mnemonic = "LD H A\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &H;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x68: // LD L B
        {
            _mnemonic = "LD L B\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x69: // LD L C
        {
            _mnemonic = "LD L C\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x6a: // LD L D
        {
            _mnemonic = "LD L D\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x6b: // LD L E
        {
            _mnemonic = "LD L E\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x6c: // LD L H
        {
            _mnemonic = "LD L H\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x6d: // LD L L
        {
            _mnemonic = "LD L L\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x6e: // LD L (HL)
        {
            _mnemonic = "LD L (HL)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x6f: // LD L A
        {
            _mnemonic = "LD L A\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &L;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x70: // LD (HL) B
        {
            _mnemonic = "LD (HL) B\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x71: // LD (HL) C
        {
            _mnemonic = "LD (HL) C\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x72: // LD (HL) D
        {
            _mnemonic = "LD (HL) D\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x73: // LD (HL) E
        {
            _mnemonic = "LD (HL) E\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x74: // LD (HL) H
        {
            _mnemonic = "LD (HL) H\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x75: // LD (HL) L
        {
            _mnemonic = "LD (HL) L\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x77: // LD (HL) A
        {
            _mnemonic = "LD (HL) A\0";
            _destinationType = OPT_IND16;
            _destination = (uint16_t *) HL;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x78: // LD A B
        case 0x80: // ADD A B
        case 0x88: // ADC A B
        case 0x98: // SBC A B
        {
            _mnemonic = "SBC A B\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x79: // LD A C
        case 0x81: // ADD A C
        case 0x89: // ADC A C
        case 0x99: // SBC A C
        {
            _mnemonic = "SBC A C\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x7a: // LD A D
        case 0x82: // ADD A D
        case 0x8a: // ADC A D
        case 0x9a: // SBC A D
        {
            _mnemonic = "SBC A D\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x7b: // LD A E
        case 0x83: // ADD A E
        case 0x8b: // ADC A E
        case 0x9b: // SBC A E
        {
            _mnemonic = "SBC A E\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x7c: // LD A H
        case 0x84: // ADD A H
        case 0x8c: // ADC A H
        case 0x9c: // SBC A H
        {
            _mnemonic = "SBC A H\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x7d: // LD A L
        case 0x85: // ADD A L
        case 0x8d: // ADC A L
        case 0x9d: // SBC A L
        {
            _mnemonic = "SBC A L\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x7e: // LD A (HL)
        case 0x86: // ADD A (HL)
        case 0x8e: // ADC A (HL)
        case 0x9e: // SBC A (HL)
        {
            _mnemonic = "SBC A (HL)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x7f: // LD A A
        case 0x87: // ADD A A
        case 0x8f: // ADC A A
        case 0x9f: // SBC A A
        {
            _mnemonic = "SBC A A\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0xc0: // RET fNZ 
        {
            _mnemonic = "RET fNZ\0";
            _destinationType = OPT_NFLAG;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xc2: // JP fNZ a16
        case 0xc4: // CALL fNZ a16
        {
            _mnemonic = "CALL fNZ a16\0";
            _destinationType = OPT_NFLAG;
            _sourceType = OPT_IMM16;
            _source = (uint16_t *) _readw_PC();
            break;
        }

        case 0xc3: // JP a16 
        case 0xcd: // CALL a16 
        {
            _mnemonic = "CALL a16\0";
            _destinationType = OPT_IMM16;
            _destination = (uint16_t *) _readw_PC();
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xc7: // RST 00H 
        {
            _mnemonic = "RST 00H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xc8: // RET fZ 
        {
            _mnemonic = "RET fZ\0";
            _destinationType = OPT_FLAG;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xca: // JP fZ a16
        case 0xcc: // CALL fZ a16
        {
            _mnemonic = "CALL fZ a16\0";
            _destinationType = OPT_FLAG;
            _sourceType = OPT_IMM16;
            _source = (uint16_t *) _readw_PC();
            break;
        }

        case 0xcf: // RST 08H 
        {
            _mnemonic = "RST 08H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xd0: // RET fNC 
        {
            _mnemonic = "RET fNC\0";
            _destinationType = OPT_NFLAG;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xd2: // JP fNC a16
        case 0xd4: // CALL fNC a16
        {
            _mnemonic = "CALL fNC a16\0";
            _destinationType = OPT_NFLAG;
            _sourceType = OPT_IMM16;
            _source = (uint16_t *) _readw_PC();
            break;
        }

        case 0xd6: // SUB d8 
        case 0xe6: // AND d8 
        case 0xee: // XOR d8 
        case 0xf6: // OR d8 
        case 0xfe: // CP d8 
        {
            _mnemonic = "CP d8\0";
            _destinationType = OPT_IMM8;
            _destination = (uint16_t *) _readb_PC();
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xd7: // RST 10H 
        {
            _mnemonic = "RST 10H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xd8: // RET fC 
        {
            _mnemonic = "RET fC\0";
            _destinationType = OPT_FLAG;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xda: // JP fC a16
        case 0xdc: // CALL fC a16
        {
            _mnemonic = "CALL fC a16\0";
            _destinationType = OPT_FLAG;
            _sourceType = OPT_IMM16;
            _source = (uint16_t *) _readw_PC();
            break;
        }

        case 0xdf: // RST 18H 
        {
            _mnemonic = "RST 18H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xe0: // LDH (a8) A
        {
            _mnemonic = "LDH (a8) A\0";
            _destinationType = OPT_INDADDR8;
            _destination = (uint16_t *) _readb_PC();
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0xe2: // LD (C) A
        {
            _mnemonic = "LD (C) A\0";
            _destinationType = OPT_IND8;
            _destination = (uint16_t *) C;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0xe7: // RST 20H 
        {
            _mnemonic = "RST 20H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xe8: // ADD SP r8
        {
            _mnemonic = "ADD SP r8\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &SP;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0xea: // LD (a16) A
        {
            _mnemonic = "LD (a16) A\0";
            _destinationType = OPT_INDADDR8;
            _destination = (uint16_t *) _readb_PC();
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0xef: // RST 28H 
        {
            _mnemonic = "RST 28H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xf0: // LDH A (a8)
        {
            _mnemonic = "LDH A (a8)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_INDADDR8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0xf1: // POP AF 
        case 0xf5: // PUSH AF 
        {
            _mnemonic = "PUSH AF\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &AF;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xf2: // LD A (C)
        {
            _mnemonic = "LD A (C)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_IND8;
            _source = (uint16_t *) C;
            break;
        }

        case 0xf7: // RST 30H 
        {
            _mnemonic = "RST 30H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0xf8: // LD HL SP+r8
        {
            _mnemonic = "LD HL SP+r8\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &HL;
            _sourceType = OPT_IMM8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0xf9: // LD SP HL
        {
            _mnemonic = "LD SP HL\0";
            _destinationType = OPT_DIR16;
            _destination = (uint16_t *) &SP;
            _sourceType = OPT_DIR16;
            _source = (uint16_t *) &HL;
            break;
        }

        case 0xfa: // LD A (a16)
        {
            _mnemonic = "LD A (a16)\0";
            _destinationType = OPT_DIR8;
            _destination = (uint16_t *) &A;
            _sourceType = OPT_INDADDR8;
            _source = (uint16_t *) _readb_PC();
            break;
        }

        case 0xff: // RST 38H 
        {
            _mnemonic = "RST 38H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IMPLICIT;
            break;
        }

        case 0x40cb: // BIT 0 B
        case 0x80cb: // RES 0 B
        case 0xc0cb: // SET 0 B
        {
            _mnemonic = "SET 0 B\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x41cb: // BIT 0 C
        case 0x81cb: // RES 0 C
        case 0xc1cb: // SET 0 C
        {
            _mnemonic = "SET 0 C\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x42cb: // BIT 0 D
        case 0x82cb: // RES 0 D
        case 0xc2cb: // SET 0 D
        {
            _mnemonic = "SET 0 D\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x43cb: // BIT 0 E
        case 0x83cb: // RES 0 E
        case 0xc3cb: // SET 0 E
        {
            _mnemonic = "SET 0 E\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x44cb: // BIT 0 H
        case 0x84cb: // RES 0 H
        case 0xc4cb: // SET 0 H
        {
            _mnemonic = "SET 0 H\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x45cb: // BIT 0 L
        case 0x85cb: // RES 0 L
        case 0xc5cb: // SET 0 L
        {
            _mnemonic = "SET 0 L\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x46cb: // BIT 0 (HL)
        case 0x86cb: // RES 0 (HL)
        case 0xc6cb: // SET 0 (HL)
        {
            _mnemonic = "SET 0 (HL)\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x47cb: // BIT 0 A
        case 0x87cb: // RES 0 A
        case 0xc7cb: // SET 0 A
        {
            _mnemonic = "SET 0 A\0";
            _destinationType = OPT_IMPLICIT, 0x00;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x48cb: // BIT 1 B
        case 0x88cb: // RES 1 B
        case 0xc8cb: // SET 1 B
        {
            _mnemonic = "SET 1 B\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x49cb: // BIT 1 C
        case 0x89cb: // RES 1 C
        case 0xc9cb: // SET 1 C
        {
            _mnemonic = "SET 1 C\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x4acb: // BIT 1 D
        case 0x8acb: // RES 1 D
        case 0xcacb: // SET 1 D
        {
            _mnemonic = "SET 1 D\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x4bcb: // BIT 1 E
        case 0x8bcb: // RES 1 E
        case 0xcbcb: // SET 1 E
        {
            _mnemonic = "SET 1 E\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x4ccb: // BIT 1 H
        case 0x8ccb: // RES 1 H
        case 0xcccb: // SET 1 H
        {
            _mnemonic = "SET 1 H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x4dcb: // BIT 1 L
        case 0x8dcb: // RES 1 L
        case 0xcdcb: // SET 1 L
        {
            _mnemonic = "SET 1 L\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x4ecb: // BIT 1 (HL)
        case 0x8ecb: // RES 1 (HL)
        case 0xcecb: // SET 1 (HL)
        {
            _mnemonic = "SET 1 (HL)\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x4fcb: // BIT 1 A
        case 0x8fcb: // RES 1 A
        case 0xcfcb: // SET 1 A
        {
            _mnemonic = "SET 1 A\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x50cb: // BIT 2 B
        case 0x90cb: // RES 2 B
        case 0xd0cb: // SET 2 B
        {
            _mnemonic = "SET 2 B\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x51cb: // BIT 2 C
        case 0x91cb: // RES 2 C
        case 0xd1cb: // SET 2 C
        {
            _mnemonic = "SET 2 C\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x52cb: // BIT 2 D
        case 0x92cb: // RES 2 D
        case 0xd2cb: // SET 2 D
        {
            _mnemonic = "SET 2 D\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x53cb: // BIT 2 E
        case 0x93cb: // RES 2 E
        case 0xd3cb: // SET 2 E
        {
            _mnemonic = "SET 2 E\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x54cb: // BIT 2 H
        case 0x94cb: // RES 2 H
        case 0xd4cb: // SET 2 H
        {
            _mnemonic = "SET 2 H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x55cb: // BIT 2 L
        case 0x95cb: // RES 2 L
        case 0xd5cb: // SET 2 L
        {
            _mnemonic = "SET 2 L\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x56cb: // BIT 2 (HL)
        case 0x96cb: // RES 2 (HL)
        case 0xd6cb: // SET 2 (HL)
        {
            _mnemonic = "SET 2 (HL)\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x57cb: // BIT 2 A
        case 0x97cb: // RES 2 A
        case 0xd7cb: // SET 2 A
        {
            _mnemonic = "SET 2 A\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x58cb: // BIT 3 B
        case 0x98cb: // RES 3 B
        case 0xd8cb: // SET 3 B
        {
            _mnemonic = "SET 3 B\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x59cb: // BIT 3 C
        case 0x99cb: // RES 3 C
        case 0xd9cb: // SET 3 C
        {
            _mnemonic = "SET 3 C\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x5acb: // BIT 3 D
        case 0x9acb: // RES 3 D
        case 0xdacb: // SET 3 D
        {
            _mnemonic = "SET 3 D\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x5bcb: // BIT 3 E
        case 0x9bcb: // RES 3 E
        case 0xdbcb: // SET 3 E
        {
            _mnemonic = "SET 3 E\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x5ccb: // BIT 3 H
        case 0x9ccb: // RES 3 H
        case 0xdccb: // SET 3 H
        {
            _mnemonic = "SET 3 H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x5dcb: // BIT 3 L
        case 0x9dcb: // RES 3 L
        case 0xddcb: // SET 3 L
        {
            _mnemonic = "SET 3 L\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x5ecb: // BIT 3 (HL)
        case 0x9ecb: // RES 3 (HL)
        case 0xdecb: // SET 3 (HL)
        {
            _mnemonic = "SET 3 (HL)\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x5fcb: // BIT 3 A
        case 0x9fcb: // RES 3 A
        case 0xdfcb: // SET 3 A
        {
            _mnemonic = "SET 3 A\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x60cb: // BIT 4 B
        case 0xa0cb: // RES 4 B
        case 0xe0cb: // SET 4 B
        {
            _mnemonic = "SET 4 B\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x61cb: // BIT 4 C
        case 0xa1cb: // RES 4 C
        case 0xe1cb: // SET 4 C
        {
            _mnemonic = "SET 4 C\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x62cb: // BIT 4 D
        case 0xa2cb: // RES 4 D
        case 0xe2cb: // SET 4 D
        {
            _mnemonic = "SET 4 D\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x63cb: // BIT 4 E
        case 0xa3cb: // RES 4 E
        case 0xe3cb: // SET 4 E
        {
            _mnemonic = "SET 4 E\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x64cb: // BIT 4 H
        case 0xa4cb: // RES 4 H
        case 0xe4cb: // SET 4 H
        {
            _mnemonic = "SET 4 H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x65cb: // BIT 4 L
        case 0xa5cb: // RES 4 L
        case 0xe5cb: // SET 4 L
        {
            _mnemonic = "SET 4 L\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x66cb: // BIT 4 (HL)
        case 0xa6cb: // RES 4 (HL)
        case 0xe6cb: // SET 4 (HL)
        {
            _mnemonic = "SET 4 (HL)\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x67cb: // BIT 4 A
        case 0xa7cb: // RES 4 A
        case 0xe7cb: // SET 4 A
        {
            _mnemonic = "SET 4 A\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x68cb: // BIT 5 B
        case 0xa8cb: // RES 5 B
        case 0xe8cb: // SET 5 B
        {
            _mnemonic = "SET 5 B\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x69cb: // BIT 5 C
        case 0xa9cb: // RES 5 C
        case 0xe9cb: // SET 5 C
        {
            _mnemonic = "SET 5 C\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x6acb: // BIT 5 D
        case 0xaacb: // RES 5 D
        case 0xeacb: // SET 5 D
        {
            _mnemonic = "SET 5 D\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x6bcb: // BIT 5 E
        case 0xabcb: // RES 5 E
        case 0xebcb: // SET 5 E
        {
            _mnemonic = "SET 5 E\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x6ccb: // BIT 5 H
        case 0xaccb: // RES 5 H
        case 0xeccb: // SET 5 H
        {
            _mnemonic = "SET 5 H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x6dcb: // BIT 5 L
        case 0xadcb: // RES 5 L
        case 0xedcb: // SET 5 L
        {
            _mnemonic = "SET 5 L\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x6ecb: // BIT 5 (HL)
        case 0xaecb: // RES 5 (HL)
        case 0xeecb: // SET 5 (HL)
        {
            _mnemonic = "SET 5 (HL)\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x6fcb: // BIT 5 A
        case 0xafcb: // RES 5 A
        case 0xefcb: // SET 5 A
        {
            _mnemonic = "SET 5 A\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x70cb: // BIT 6 B
        case 0xb0cb: // RES 6 B
        case 0xf0cb: // SET 6 B
        {
            _mnemonic = "SET 6 B\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x71cb: // BIT 6 C
        case 0xb1cb: // RES 6 C
        case 0xf1cb: // SET 6 C
        {
            _mnemonic = "SET 6 C\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x72cb: // BIT 6 D
        case 0xb2cb: // RES 6 D
        case 0xf2cb: // SET 6 D
        {
            _mnemonic = "SET 6 D\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x73cb: // BIT 6 E
        case 0xb3cb: // RES 6 E
        case 0xf3cb: // SET 6 E
        {
            _mnemonic = "SET 6 E\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x74cb: // BIT 6 H
        case 0xb4cb: // RES 6 H
        case 0xf4cb: // SET 6 H
        {
            _mnemonic = "SET 6 H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x75cb: // BIT 6 L
        case 0xb5cb: // RES 6 L
        case 0xf5cb: // SET 6 L
        {
            _mnemonic = "SET 6 L\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x76cb: // BIT 6 (HL)
        case 0xb6cb: // RES 6 (HL)
        case 0xf6cb: // SET 6 (HL)
        {
            _mnemonic = "SET 6 (HL)\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x77cb: // BIT 6 A
        case 0xb7cb: // RES 6 A
        case 0xf7cb: // SET 6 A
        {
            _mnemonic = "SET 6 A\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

        case 0x78cb: // BIT 7 B
        case 0xb8cb: // RES 7 B
        case 0xf8cb: // SET 7 B
        {
            _mnemonic = "SET 7 B\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &B;
            break;
        }

        case 0x79cb: // BIT 7 C
        case 0xb9cb: // RES 7 C
        case 0xf9cb: // SET 7 C
        {
            _mnemonic = "SET 7 C\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &C;
            break;
        }

        case 0x7acb: // BIT 7 D
        case 0xbacb: // RES 7 D
        case 0xfacb: // SET 7 D
        {
            _mnemonic = "SET 7 D\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &D;
            break;
        }

        case 0x7bcb: // BIT 7 E
        case 0xbbcb: // RES 7 E
        case 0xfbcb: // SET 7 E
        {
            _mnemonic = "SET 7 E\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &E;
            break;
        }

        case 0x7ccb: // BIT 7 H
        case 0xbccb: // RES 7 H
        case 0xfccb: // SET 7 H
        {
            _mnemonic = "SET 7 H\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &H;
            break;
        }

        case 0x7dcb: // BIT 7 L
        case 0xbdcb: // RES 7 L
        case 0xfdcb: // SET 7 L
        {
            _mnemonic = "SET 7 L\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &L;
            break;
        }

        case 0x7ecb: // BIT 7 (HL)
        case 0xbecb: // RES 7 (HL)
        case 0xfecb: // SET 7 (HL)
        {
            _mnemonic = "SET 7 (HL)\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_IND16;
            _source = (uint16_t *) HL;
            break;
        }

        case 0x7fcb: // BIT 7 A
        case 0xbfcb: // RES 7 A
        case 0xffcb: // SET 7 A
        {
            _mnemonic = "SET 7 A\0";
            _destinationType = OPT_IMPLICIT;
            _sourceType = OPT_DIR8;
            _source = (uint16_t *) &A;
            break;
        }

    }
}