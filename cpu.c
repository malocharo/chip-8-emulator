#include "cpu.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


unsigned char chip8_fontset[FONTSET_SIZE] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void init(){
    opcode = 0;
    Idx = 0;
    pc = PROG_START;
    delay_timer = 0;
    sound_timer = 0;

    for(int i = 0;i<MEM_SIZE;i++)
        mem[i] = 0;
    for(int i = 0;i<SCREEN_HEIGHT * SCREEN_WIDTH;i++)
        graph[i]  = 0;
    draw_flag = 0; 
    for(int i = 0;i<FONTSET_SIZE;i++)
        mem[i] = chip8_fontset[i];
    for(int i = 0;i<REG_NUMBER;i++)
        V[i] = 0;
    for(int i = 0;i<NB_KEY;i++)
        key[i] = 0;
    
    init_stack(STACK_SIZE);

    srand(time(NULL));
}

void load_game(char *filename)
{
    FILE *f = fopen(filename,"rb");

    if(f == NULL) 
    {
        fprintf(stderr,"could not find %s \n",filename);
        exit(-1);
    }
    
    fseek(f,0,SEEK_END);
    unsigned int size_file = ftell(f);
    rewind(f);

    if(size_file > MEM_SIZE - PROG_START) 
    {
        fprintf(stderr,"Mem too short: %i\n",size_file);
        exit(-1);
    }

    unsigned char* buff = malloc(sizeof(char)*size_file);
    if(buff == NULL)
    {
        fprintf(stderr,"error while allocating\n");
        exit(-1);
    }

    fread(buff,1,size_file,f);

    for(int i = 0;i < size_file; i++)
        mem[i+PROG_START] = buff[i];
    
    free(buff);
    fclose(f);

}

void dump_mem(char * filename)
{
    FILE *f = fopen(filename,"wb");
    if(f == NULL)
    {
        fprintf(stderr,"Error while dumping mem in %s\n",filename);
        return;
    }
    fwrite(mem,1,MEM_SIZE,f);
    fclose(f);
}

void one_cycle()
{
    opcode = mem[pc] << 8 | mem[pc+1];
    fprintf(stderr,"opcode %0X at addr %X\n",opcode,(pc<<8) | (pc+1));
    switch(opcode & 0xF000)
    {
        case 0x0000: //0XXX
            switch (opcode & 0x000F)
            {
                case 0x0000: //00E0 : Clears the screen
                    for(int i = 0;i<SCREEN_HEIGHT * SCREEN_WIDTH;i++)
                        graph[i] = 0;
                    break;
                case 0x000E: //00EE returns from a subroutine
                    pc = pop()+2;
                    break;

                default:
                    fprintf(stderr,"wrong opcode %X\n",opcode);
                    break;
            }
            break;

        case 0x1000: //1NNN jump at addr NNN
            pc = opcode & 0x0FFF;
            break;
        
        case 0x2000: //2NNN call subroutine NNN
            push(pc);
            pc = opcode & 0xFFF;
            break;

        case 0x3000: //3XNN skips the next instr if Vx == NN
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                inc_pc(4);
            else
                inc_pc(2);    
            break;
        
        case 0x4000: //4XNN skips the next instr if Vx != NN
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                inc_pc(4);
            else
                inc_pc(2);
            break;
        
        case 0x5000: //5XY0 skips the next instr if Vx == Vy
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                inc_pc(4);
            else 
                inc_pc(2);
            break;
        
        case 0x6000: //6XNN sets Vx to NN
            V[(opcode & 0x0F00)>>8] = (opcode & 0x00FF);
            inc_pc(2);
            break;
        
        case 0x7000: //7XNN add NN to Vx (carry flag not changed)
            V[(opcode & 0x0F00)>>8] += (opcode & 0x00FF);
            inc_pc(2);
            break;
        
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000: //8XY0 Sets Vx to the value of Vy
                    V[(opcode & 0x0F00)>>8] = V[(opcode & 0x00F0) >> 4];
                    inc_pc(2);
                    break;
                
                case 0x0001: //8XY1 Sets Vx to the value of Vx | Vy (bitwise OR)
                    V[(opcode & 0x0F00)>>8] = V[(opcode & 0x0F00)>>8] | V[(opcode & 0x00F0) >> 4];
                    inc_pc(2);
                    break;
                
                case 0x0002: //8XY2 Sets Vx to the value of Vx & Vy (bitwise AND)
                    V[(opcode & 0x0F00)>>8] = V[(opcode & 0x0F00)>>8] & V[(opcode & 0x00F0) >> 4];
                    inc_pc(2);
                    break;

                case 0x0003: //8XY3 Sets Vx to the value of Vx ^ Vy (bitwise XOR)
                    V[(opcode & 0x0F00)>>8] = V[(opcode & 0x0F00)>>8] ^ V[(opcode & 0x00F0) >> 4];
                    inc_pc(2);
                    break;
                
                case 0x0004: //8XY4 Adds Vy to Vx. Vf is set to 0 when there is a borrow, 1 when there isn't
                    if((int)V[(opcode & 0x0F00)>>8] + V[(opcode & 0x00F0) >> 4] < 256)
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00)>>8] = V[(opcode & 0x0F00)>>8] + V[(opcode & 0x00F0) >> 4];
                    inc_pc(2);
                    break;
                
                case 0x0005: //8XY5 Vy is sub from Vx. Vf is set to 0 when there is a borrow, 1  when there isn't
                    if((int)V[(opcode & 0x0F00)>>8] - V[(opcode & 0x00F0) >> 4] >= 0)
                        V[0xF] = 1;
                    else 
                        V[0xF] = 0;
                    V[(opcode & 0x0F00)>>8] = V[(opcode & 0x0F00)>>8] - V[(opcode & 0x00F0) >> 4];
                    inc_pc(2);
                    break;
                
                case 0x0006: //8XY6 Store the least significant bit of Vx in VF and then shifts Vx to the right by 1.
                    V[0xF] = V[(opcode & 0x0F00)>>8] & 0x1;
                    V[(opcode & 0x0F00)>>8] >>= 1; 
                    inc_pc(2);
                    break;
                
                case 0x0007: //8XY7 Sets Vx to Vy minus Vx. Vf is set to 0 if there's a borrow, 1 if there isn't.
                    if((int)V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8] >=0 )
                        V[0xF] = 1;
                    else 
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    inc_pc(2);
                    break;
                
                case 0x000E: //8XYE Store the most significant bit of Vx in Vf and then shifts Vx to the left by 1.
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00)>>8] <<= 1; 
                    inc_pc(2);
                    break;
            }
            break;
        
        case 0x9000: //9XY0 skips the next instr if Vx != Vf
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                inc_pc(4);
            else 
                inc_pc(2);
            break;    

        case 0xA000: //ANNN: Set I to addr NNN
            Idx = opcode & 0x0FFF;
            inc_pc(2);
            break;
        
        case 0xB000: //BNNN Jump to V0+NNN
            pc = V[0x0]+ (opcode & 0x0FFF);
            break;
        
        case 0xC000: //CXNN Sets Vx to the res of a bitwise AND operation on a rand number (0 - 255) and NN
            V[(opcode & 0x0F00) >> 8] = (rand()%255) & (opcode & 0x00FF);
            break;
        

        //DXYN Draws a sprite at coordinate (Vx,Vy) that has a width of 8 pixels and height of N pixels. Each row of 8 pixels is read as bit-coded starting from mem Idx;
        // Vf is set to 1 if any screen pixels are flipped from set to unset, 0 if not
        case 0xD000:; // ; to avoid "label can only be follow by a statement, declaration is not a statement" https://tinyurl.com/y6n3nswe
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short h = opcode & 0x000F;
            unsigned short p;
            V[0xF] = 0;

            for(int i = 0;i < h;i++)
            {
                p = mem[Idx + i];
                for(int j = 0; j < 8;j++)
                {
                    if((p & (0x80>>j)) != 0)
                    {
                        if(graph[(x + i +((y+j)*64))] == 1)
                            V[0xF] = 1;
                        graph[x + i +((y+j)*64)] ^=1;
                    }
                }
            }
            draw_flag = 1;
            inc_pc(2);
            break;
        
        case 0xE000:
            switch (opcode & 0x000F)
            {
                case 0x000E: // EX9E skips the next instr if key stored in Vx is pressed
                    if(key[V[(opcode & 0x0F00)>>8]])
                        inc_pc(4);
                    else 
                        inc_pc(2);
                    break;

                case 0x0001: // EXA1 skips the next instr if key stored in Vx is not pressed
                    if(key[V[(opcode & 0x0F00)>>8]])
                        inc_pc(2);
                    else 
                        inc_pc(4);
                    break;

                default:
                    fprintf(stderr,"Wrong opcode %X\n",opcode);
                    break;
            }
        break;

        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x0007: // FX07 Sets Vx to the value of the delay timer
                    V[(opcode & 0x0F00)>>8] = delay_timer ;
                    inc_pc(2);
                    break;
            
                case 0x000A: // FX0A  A key pressed is awaited and then stored in Vx, blocking operation. All operation halted until next key event
                    key_pressed = 0;
                    for(int i = 0;i < NB_KEY;i++)
                        if(key[i]) 
                        {
                            V[(opcode & 0x0F00)>>8] = i;
                            key_pressed = 1;
                            inc_pc(2);
                        }
                    
                    break;

                case 0x0015: // FX15 Sets the delay timer to Vx    
                    delay_timer = V[(opcode & 0x0F00)>>8];
                    inc_pc(2);
                    break;
                
                case 0x0018: // FX18 Set the sound time to Vx
                    sound_timer = V[(opcode & 0x0F00)>>8];
                    inc_pc(2);
                    break;
                
                case 0x001E: // FX1E Adds Vx to Idx >> undocumented but Vf is set to 1 if range overflow (I+Vx>0xFFF) and 0 when there isn't
                    if(Idx + V[(opcode & 0x0F00)>>8] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    Idx += V[(opcode & 0x0F00)>>8];
                    inc_pc(2);
                    break;
                
                case 0x0029: // FX229 Sets Idx to the location of the sprite for the char in Vx. Char 0-F are rep by 4x5 font
                    Idx =  V[(opcode & 0x0F00)>>8] * 5;
                    inc_pc(2);
                    break;
                
                case 0x00033: // FX33 store the binary-coded decimal representation of VX, with the most significant of three digits at addr Idx, middle digit Idx+1 and least Idx +2 
                    mem[Idx]      =  V[(opcode & 0x0F00)>>8] / 100; 
                    mem[Idx + 1]  = (V[(opcode & 0x0F00)>>8] / 10) % 10;
                    mem[Idx + 2]  = (V[(opcode & 0x0F00)>>8] / 100) % 10;
                    inc_pc(2);
                    break;
                
                case 0x0055: // FX33 stores V0 up to Vx(included) in mem starting at addr Idx, offset is inc by 1, Idx shall remain the same
                    for(int i = 0;i <= V[(opcode & 0x0F00)>>8];i++)
                        mem[Idx + i] = V[i];
                    inc_pc(2);
                    break;
                
                case 0x0065 : //FX65 fills V0 up to Vx(included) with mem starting at Idx offset is inc by 1, Idx shall remain the same
                    for(int i = 0;i <= V[(opcode & 0x0F00)>>8];i++)
                        V[i] = mem[Idx + i]; 
                    inc_pc(2);
                    break;
                default:
                    fprintf(stderr,"Wrong opcode %X\n",opcode);
                    break;
            }
            break;
            default : 
                fprintf(stderr,"Wrong opcode %X\n",opcode); 
                break;
    }
}