#ifndef KEYBORD_H_
#define KEYBORD_H_

#define KEY_MASK        (0x0f)

#define KEY_1           (0x01)
#define KEY_2           (0x02)
#define KEY_3           (0x04)
#define KEY_4           (0x08)

unsigned char read_keybord(void);


#endif