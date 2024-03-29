#ifndef __SPACELIGHT_GUI_H_
#define __SPACELIGHT_GUI_H_

#include "spacelight.h"

#include "u8g2.h"

typedef struct
{
    u8g2_uint_t left;
    u8g2_uint_t top;
    u8g2_uint_t right;
    u8g2_uint_t bottom;
} Rect;

extern void gui_text(u8g2_t *u8g2, Rect *pos, const char *str);

extern void gui_init();
extern void gui_clear();
extern void gui_update(uint16_t stage, GuiMsg msg);

extern void render_gui_main(u8g2_t *u8g2, GuiStage stage, Msg msg);

extern void init_gui_menu();
extern void render_gui_menu(u8g2_t *u8g2, GuiStage gui_stage, GuiStage last_gui_stage);

extern void render_gui_dmxaddr(u8g2_t *u8g2, GuiStage gui_stage, GuiStage last_gui_stage);
extern void render_gui_lampcount(u8g2_t *u8g2, GuiStage gui_stage, GuiStage last_gui_stage);
extern void render_gui_locktime(u8g2_t *u8g2, GuiStage gui_stage, GuiStage last_gui_stage);

extern void render_gui_msgbox(u8g2_t *u8g2, GuiStage gui_stage, MsgType msg_type);

#endif // __SPACELIGHT_GUI_H_
