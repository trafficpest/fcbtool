#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/stat.h>
#include "midi.h"
#include "fcb.h"
#include "ui_ncurses.h"
#include "fcb_io.h"

int main() {
    initialize_ui();

    create_fcb_home_dir();  // Ensure the ~/.fcb1010 directory is created

    while (1) {
        int user_choice = display_main_menu();  // UI function to display the main menu

        switch (user_choice) {
            case 'q':
                endwin();
                return 0;
            case '1':
                handle_sysex_receive();  // MIDI handling function
                break;
            case '2':
                handle_sysex_send();  // MIDI handling function
                break;
            case '3':
                handle_parse_and_inspect();  // Function to parse and inspect SysEx data
                break;
            case '4':
                handle_create_csv();  // Function to create CSV from SysEx data
                break;
            case '5':
                csv_to_sysex();  // FCB IO function
                break;
            case '6':
                backup_sysex_file();  // FCB IO function
                break;
            default:
                break;  // Invalid input, re-prompt
        }
    }

    endwin();
    return 0;
}

