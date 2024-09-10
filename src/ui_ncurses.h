#ifndef UI_NCURSES_H
#define UI_NCURSES_H

void initialize_ui();
int display_main_menu();
int select_midi_device(char devices[MAX_DEVICES][128], int device_count);
void print_fcb1010(const FCB1010 *fcb);

#endif
