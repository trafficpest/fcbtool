#include <stddef.h>
#include <string.h>
#include <ncurses.h>
#include "fcb.h"
#include "midi.h"
#include "ui_ncurses.h"

void initialize_ui() {
    initscr();
    start_color();
    use_default_colors();
    cbreak();
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);

    // Define color pairs
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_WHITE, -1);
    init_pair(5, COLOR_CYAN, COLOR_BLUE);

    bkgd(COLOR_PAIR(5) | A_BOLD);
}

int display_main_menu() {
    int rows, cols;
    clear();
    getmaxyx(stdscr, rows, cols);  // Get the screen size

    const int WIN_WIDTH = 35;
    const int WIN_HEIGHT = 11;

    // Create a new window for the settings box
    WINDOW *menu_win = newwin(WIN_HEIGHT, WIN_WIDTH, (rows - WIN_HEIGHT) / 2, (cols - WIN_WIDTH) / 2);
    wbkgd(menu_win, COLOR_PAIR(3));  
    box(menu_win, 0, 0);  // Draw a box around the window

    // Display the title and instructions in the main window
    mvprintw(1, (cols - strlen("FCB1010 Edit Tool:")) / 2, "FCB1010 Edit Tool:");
    mvprintw(rows - 2, (cols - strlen("Make a selection.")) / 2, "Make a selection.");

    // Display menu options inside the settings window
    mvwprintw(menu_win, 1, 2, "Select an option:");
    mvwprintw(menu_win, 3, 2, "1: Receive Midi SysEx & Save");
    mvwprintw(menu_win, 4, 2, "2: Send SysEx file over Midi");
    mvwprintw(menu_win, 5, 2, "3: Parse and Inspect dump.syx");
    mvwprintw(menu_win, 6, 2, "4: Create CSV from dump.syx");
    mvwprintw(menu_win, 7, 2, "5: Create Sysex from CSV");
    mvwprintw(menu_win, 8, 2, "6: Backup SysEx Dump");
    mvwprintw(menu_win, 9, 2, "q: Quit");

    // Refresh the settings window and the main screen
    refresh();  
    wrefresh(menu_win);  

    // Delete the settings window
//    delwin(menu_win);  
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
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  // Get the screen size

    const int WIN_WIDTH = 70;
    const int WIN_HEIGHT = 20;

    // Create a new window for the settings box
    WINDOW *settings_win = newwin(WIN_HEIGHT, WIN_WIDTH, (rows - WIN_HEIGHT) / 2, (cols - WIN_WIDTH) / 2);
    if (settings_win == NULL) {
        endwin();
        fprintf(stderr, "Error creating window.\n");
        return;
    }

    wbkgd(settings_win, COLOR_PAIR(3));
    box(settings_win, 0, 0);  // Draw a box around the window

    while (1) {
        // Clear the content of the window
        werase(settings_win);

        // Draw a box around the window
        box(settings_win, 0, 0);

        mvwprintw(settings_win, 0, 2, "Preset %d", preset + 1);

        const FCB1010Preset *p = &fcb->preset[preset];
        mvwprintw(settings_win, 2, 2, "  PC1: %s %d  PC2: %s %d",
                 p->pc1_enabled ? "On" : "Off", p->pc1_program,
                 p->pc2_enabled ? "On" : "Off", p->pc2_program);
        mvwprintw(settings_win, 3, 2, "  PC3: %s %d  PC4: %s %d",
                 p->pc3_enabled ? "On" : "Off", p->pc3_program,
                 p->pc4_enabled ? "On" : "Off", p->pc4_program);
        mvwprintw(settings_win, 4, 2, "  PC5: %s %d",
                 p->pc5_enabled ? "On" : "Off", p->pc5_program);
        mvwprintw(settings_win, 5, 2, "  CC1: %s %d %d CC2: %s %d %d",
                 p->cc1_enabled ? "On" : "Off", p->cc1_controller, p->cc1_value,
                 p->cc2_enabled ? "On" : "Off", p->cc2_controller, p->cc2_value);
        mvwprintw(settings_win, 6, 2, "  EXP A: %s %d [%d, %d]",
                 p->expA_enabled ? "On" : "Off", p->expA_controller, p->expA_min, p->expA_max);
        mvwprintw(settings_win, 7, 2, "  EXP B: %s %d [%d, %d]",
                 p->expB_enabled ? "On" : "Off", p->expB_controller, p->expB_min, p->expB_max);
        mvwprintw(settings_win, 8, 2, "  NOTE: %s %d",
                 p->note_enabled ? "On" : "Off", p->note_value);
        mvwprintw(settings_win, 9, 2, "  Switch 1: %s  Switch 2: %s",
          p->switch1_enabled ? "On" : "Off", p->switch2_enabled ? "On" : "Off");

        mvwprintw(settings_win, 11, 2, "Global Settings:");
        mvwprintw(settings_win, 12, 2, "  MIDI Channels: PC1: %d  PC2: %d  PC3: %d  PC4: %d  PC5: %d",
                 fcb->pc1_midi_channel, fcb->pc2_midi_channel, fcb->pc3_midi_channel,
                 fcb->pc4_midi_channel, fcb->pc5_midi_channel);
        mvwprintw(settings_win, 13, 2, "                 CC1: %d  CC2: %d  EXP A: %d  EXP B: %d  NOTE: %d",
                 fcb->cc1_midi_channel, fcb->cc2_midi_channel,
                 fcb->expA_midi_channel, fcb->expB_midi_channel, fcb->note_midi_channel);
        mvwprintw(settings_win, 14, 2, "  Direct Select: %s  Running Status: %s  Merge: %s",
                 fcb->direct_select ? "Yes" : "No", fcb->running_status ? "Yes" : "No",
                 fcb->merge ? "Yes" : "No");
        mvwprintw(settings_win, 15, 2, "  Switch 1: %s  Switch 2: %s",
                 fcb->switch1 ? "Yes" : "No", fcb->switch2 ? "Yes" : "No");
        mvwprintw(settings_win, 16, 2, "  EXP A Calibration: [%d, %d]  EXP B Calibration: [%d, %d]",
                 fcb->expA_calibration_min, fcb->expA_calibration_max,
                 fcb->expB_calibration_min, fcb->expB_calibration_max);

        mvwprintw(settings_win, 18, 2, "Press 'n' for next preset, 'p' for previous preset, 'q' to exit.");

        // Refresh the window to show changes
        wrefresh(settings_win);

        // Get user input
        ch = getch();
        if (ch == 'q') {
            break;
        } else if (ch == 'n') {
            preset = (preset + 1) % NUM_PRESETS;
        } else if (ch == 'p') {
            preset = (preset - 1 + NUM_PRESETS) % NUM_PRESETS;
        }
    }

    // Delete the window to free resources
    delwin(settings_win);
}

