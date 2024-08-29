#include <stddef.h>
#include <ncurses.h>
#include "fcb.h"
#include "midi.h"
#include "ui_ncurses.h"

int display_main_menu() {
    clear();
    printw("Select an option:\n");
    printw("1: Receive Midi SysEx & Save\n");
    printw("2: Send SysEx file over Midi\n");
    printw("3: Parse and Inspect dump.syx\n");
    printw("4: Create CSV from dump.syx\n");
    printw("5: Create Sysex from CSV\n");
    printw("6: Backup SysEx Dump\n");
    printw("q: Quit\n");
    refresh();

    return getch();
}

int select_midi_device(char devices[MAX_DEVICES][128], int device_count) {
    int selected_device = 0;

    while (1) {
        clear();
        printw("Select a MIDI device:\n");
        for (int i = 0; i < device_count; i++) {
            if (i == selected_device) {
                attron(A_REVERSE);
            }
            printw("%d: %s\n", i, devices[i]);
            if (i == selected_device) {
                attroff(A_REVERSE);
            }
        }

        int ch = getch();
        switch (ch) {
            case 'q':
                return -1;  // Return -1 to indicate quitting
            case KEY_UP:
                if (selected_device > 0) selected_device--;
                break;
            case KEY_DOWN:
                if (selected_device < device_count - 1) selected_device++;
                break;
            case '\n':
                return selected_device;  // Return the selected device index
            default:
                break;
        }
    }
}


void print_fcb1010(const FCB1010 *fcb) {
    int ch;
    int preset = 0;

    while (1) {
        clear();
        mvprintw(0, 0, "Preset %d", preset + 1);

        const FCB1010Preset *p = &fcb->preset[preset];
        mvprintw(2, 0, "  PC1: %s %d  PC2: %s %d",
                 p->pc1_enabled ? "On" : "Off", p->pc1_program,
                 p->pc2_enabled ? "On" : "Off", p->pc2_program);
        mvprintw(3, 0, "  PC3: %s %d  PC4: %s %d",
                 p->pc3_enabled ? "On" : "Off", p->pc3_program,
                 p->pc4_enabled ? "On" : "Off", p->pc4_program);
        mvprintw(4, 0, "  PC5: %s %d",
                 p->pc5_enabled ? "On" : "Off", p->pc5_program);
        mvprintw(5, 0, "  CC1: %s %d %d CC2: %s %d %d",
                 p->cc1_enabled ? "On" : "Off", p->cc1_controller, p->cc1_value,
                 p->cc2_enabled ? "On" : "Off", p->cc2_controller, p->cc2_value);
        mvprintw(6, 0, "  EXP A: %s %d [%d, %d]",
                 p->expA_enabled ? "On" : "Off", p->expA_controller, p->expA_min, p->expA_max);
        mvprintw(7, 0, "  EXP B: %s %d [%d, %d]",
                 p->expB_enabled ? "On" : "Off", p->expB_controller, p->expB_min, p->expB_max);
        mvprintw(8, 0, "  NOTE: %s %d",
                 p->note_enabled ? "On" : "Off", p->note_value);
        mvprintw(9, 0, "  Switch 1: %s  Switch 2: %s", 
          p->switch1_enabled ? "On" : "Off", p->switch2_enabled ? "On" : "Off");

        mvprintw(11, 0, "Global Settings:");
        mvprintw(12, 0, "  MIDI Channels: PC1: %d  PC2: %d  PC3: %d  PC4: %d  PC5: %d",
                 fcb->pc1_midi_channel, fcb->pc2_midi_channel, fcb->pc3_midi_channel,
                 fcb->pc4_midi_channel, fcb->pc5_midi_channel);
        mvprintw(13, 0, "                 CC1: %d  CC2: %d  EXP A: %d  EXP B: %d  NOTE: %d",
                 fcb->cc1_midi_channel, fcb->cc2_midi_channel,
                 fcb->expA_midi_channel, fcb->expB_midi_channel, fcb->note_midi_channel);
        mvprintw(14, 0, "  Direct Select: %s  Running Status: %s  Merge: %s",
                 fcb->direct_select ? "Yes" : "No", fcb->running_status ? "Yes" : "No",
                 fcb->merge ? "Yes" : "No");
        mvprintw(15, 0, "  Switch 1: %s  Switch 2: %s",
                 fcb->switch1 ? "Yes" : "No", fcb->switch2 ? "Yes" : "No");
        mvprintw(16, 0, "  EXP A Calibration: [%d, %d]  EXP B Calibration: [%d, %d]",
                 fcb->expA_calibration_min, fcb->expA_calibration_max,
                 fcb->expB_calibration_min, fcb->expB_calibration_max);

        mvprintw(18, 0, "Press 'n' for next preset, 'p' for previous preset, 'q' to exit.");

        ch = getch();
        if (ch == 'q') {
            break;
        } else if (ch == 'n') {
            preset = (preset + 1) % NUM_PRESETS;
        } else if (ch == 'p') {
            preset = (preset - 1 + NUM_PRESETS) % NUM_PRESETS;
        }
    }
}

