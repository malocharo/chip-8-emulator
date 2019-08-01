#ifndef CPU_H
#define CPU_H

#define MEM_SIZE 4096
#define REG_NUMBER 16
#define SCREEN_SIZE 64*32
#define NB_KEY 16
#define STACK_SIZE 16
#define FONTSET_SIZE 80
#define PROG_START 0x200


unsigned short opcode;
unsigned char mem[MEM_SIZE];
unsigned char V[REG_NUMBER];

unsigned short Idx;
unsigned short pc;

unsigned char graph[SCREEN_SIZE];
unsigned int draw_flag;

unsigned char delay_timer;
unsigned char sound_timer;

unsigned char key[NB_KEY];


void init();
void load_game(char *);
void one_cycle();

void dump_mem(char *);
static inline void inc_pc(int n) { pc += n;}






#endif //CPU_H