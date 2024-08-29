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

#define MAX_DEVICES 10  // Adjust as needed

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);  // Enable reading special keys

    create_fcb_home_dir();  // Ensure the ~/.fcb1010 directory is created

    char devices[MAX_DEVICES][128];
    int device_count = 0;
    int selected_device = 0;
    int user_choice = 0;

    while (1) {
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

        int ch = getch();
        switch (ch) {
            case 'q':
                endwin();
                return 0;
            case '1':
                user_choice = 1;  // User selected Receive SysEx
                break;
            case '2':
                user_choice = 2;  // User selected Send SysEx
                break;
            case '3':
                user_choice = 3;  // User selected Parse and Inspect
                break;
            case '4':
                user_choice = 4;  // User selected Create CSV
                break;
            case '5':
                csv_to_sysex();
                user_choice = 0;  // Reset the choice to return to the main menu
                continue;
            case '6':
                backup_sysex_file();  // Backup SysEx dump
                user_choice = 0;  // Reset the choice to return to the main menu
                continue;
            default:
                continue;  // Invalid input, re-prompt
        }

        if (user_choice == 1 || user_choice == 2) {
            list_midi_devices(devices, &device_count);

            if (device_count == 0) {
                printw("No MIDI devices found.\n");
                getch();
                endwin();
                return 1;
            }

            selected_device = 0;  // Reset the selected device index

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

                ch = getch();
                switch (ch) {
                    case 'q':
                        user_choice = 0;  // Reset the choice to return to the main menu
                        endwin();
                        return 0;  // Exit the program
                    case KEY_UP:
                        if (selected_device > 0) selected_device--;
                        break;
                    case KEY_DOWN:
                        if (selected_device < device_count - 1) selected_device++;
                        break;
                    case '\n':
                        clear();
                        printw("Selected device: %s\n", devices[selected_device]);
                        refresh();

                        // Use the correct path for dump.syx
                        char sysex_filename[512];
                        snprintf(sysex_filename, sizeof(sysex_filename), "%s/.fcb1010/dump.syx", getenv("HOME"));

                        if (user_choice == 1) {
                            printw("Attempting to receive SysEx...\n");
                            refresh();
                            receive_sysex_dump(devices[selected_device], sysex_filename);
                        } else if (user_choice == 2) {
                            printw("Attempting to send SysEx...\n");
                            refresh();
                            send_sysex_dump(devices[selected_device], sysex_filename);
                        }

                        printw("Operation completed. Press any key to return to the main menu.\n");
                        refresh();
                        getch();
                        user_choice = 0;  // Reset the choice to return to the main menu
                        break;
                }

                if (user_choice == 0) {
                    break;  // Break out to the main menu
                }
            }
        } else if (user_choice == 3) {
            // Option 3: Parse and Inspect SysEx file
            char sysex_filename[512];
            snprintf(sysex_filename, sizeof(sysex_filename), "%s/.fcb1010/dump.syx", getenv("HOME"));

            FILE *sysex_file = fopen(sysex_filename, "rb");
            if (!sysex_file) {
                printw("Failed to open SysEx file: %s\n", sysex_filename);
                refresh();
                getch();
                endwin();
                return 1;
            }

            uint8_t sysex_data[SYSEX_SIZE];
            size_t read_size = fread(sysex_data, 1, SYSEX_SIZE, sysex_file);
            fclose(sysex_file);

            if (read_size != SYSEX_SIZE) {
                printw("Error: SysEx file size does not match expected size\n");
                refresh();
                getch();
                endwin();
                return 1;
            }

            FCB1010 fcb;
            init_fcb1010(&fcb);

            if (!parse_sysex(&fcb, sysex_data, SYSEX_SIZE)) {
                printw("Failed to parse SysEx data\n");
                refresh();
                getch();
                endwin();
                return 1;
            }

            // Allow user to inspect the parsed SysEx data using the print_fcb1010 function
            print_fcb1010(&fcb);
            user_choice = 0;  // Reset the choice to return to the main menu
        } else if (user_choice == 4) {
            // Option 4: Create CSV from parsed SysEx file
            char sysex_filename[512];
            snprintf(sysex_filename, sizeof(sysex_filename), "%s/.fcb1010/dump.syx", getenv("HOME"));

            FILE *sysex_file = fopen(sysex_filename, "rb");
            if (!sysex_file) {
                printw("Failed to open SysEx file: %s\n", sysex_filename);
                refresh();
                getch();
                endwin();
                return 1;
            }

            uint8_t sysex_data[SYSEX_SIZE];
            size_t read_size = fread(sysex_data, 1, SYSEX_SIZE, sysex_file);
            fclose(sysex_file);

            if (read_size != SYSEX_SIZE) {
                printw("Error: SysEx file size does not match expected size\n");
                refresh();
                getch();
                endwin();
                return 1;
            }

            FCB1010 fcb;
            init_fcb1010(&fcb);

            if (!parse_sysex(&fcb, sysex_data, SYSEX_SIZE)) {
                printw("Failed to parse SysEx data\n");
                refresh();
                getch();
                endwin();
                return 1;
            }

            char csv_filename[512];
            snprintf(csv_filename, sizeof(csv_filename), "%s/.fcb1010/fcb1010.csv", getenv("HOME"));

            if (!write_csv(&fcb, csv_filename)) {
                printw("Failed to write CSV file: %s\n", csv_filename);
                refresh();
                getch();
                endwin();
                return 1;
            }

            printw("CSV file written successfully. Press any key to return to the main menu.\n");
            refresh();
            getch();
            user_choice = 0;  // Reset the choice to return to the main menu
        }
    }

    endwin();
    return 0;
}

