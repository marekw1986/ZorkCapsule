#ifndef _Z_MEM_LOCATIONS_H_
#define _Z_MEM_LOCATIONS_H_

#define GLOBALS 		0x2271
#define SCORE   		((uint16_t)dynamic_memory[GLOBALS + 2] << 8) | dynamic_memory[GLOBALS + 3]
#define MOVES   		((uint16_t)dynamic_memory[GLOBALS + 4] << 8) | dynamic_memory[GLOBALS + 5]

#define SWORD_GLOW 		dynamic_memory[0x15c2]   // 0 = off, 1 = faint blue, 2 = bright
//#define SWORD_TVALUE 	((uint16_t)dynamic_memory[0x15c1] << 8) | dynamic_memory[0x15c2]

#define LANTERN_ONBIT_ADDR   0x08ab   /* 0x08a9 + 2 (attribute 20 lives in byte 2) */
#define LANTERN_ONBIT_MASK   0x08     /* bit 4 within that byte: 16 + 4 = attribute 20 */
#define LANTERN_STATE        ((dynamic_memory[LANTERN_ONBIT_ADDR] & LANTERN_ONBIT_MASK) ? 1 : 0)

#define SWORD_ADDR      0x06c3
#define SWORD_PARENT    dynamic_memory[SWORD_ADDR + 4]

#define LANTERN_ADDR    0x08a9
#define LANTERN_PARENT  dynamic_memory[LANTERN_ADDR + 4]

#define PLAYER_OBJ      4        /* confirmed independently via mojozork source comments ("ZORK1_PLAYER_OBJID 4") */
#define SWORD_HELD      (SWORD_PARENT == PLAYER_OBJ)
#define LANTERN_HELD    (LANTERN_PARENT == PLAYER_OBJ)

#define PLAYER_ADDR   0x0309
#define PLAYER_LOC    dynamic_memory[PLAYER_ADDR + 4]

#define STONE_BARROW_ROOM   178


extern uint8_t dynamic_memory[];

#endif // _Z_MEM_LOCATIONS_H_
