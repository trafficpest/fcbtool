#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ncurses.h>
#include "fcb.h"

void create_fcb_home_dir() {
    const char *backup_dir = getenv("HOME");
    if (backup_dir) {
        char dir_path[256];
        snprintf(dir_path, sizeof(dir_path), "%s/.fcb1010/", backup_dir);
        mkdir(dir_path, 0755);  
        snprintf(dir_path, sizeof(dir_path), "%s/.fcb1010/backups", backup_dir);
        mkdir(dir_path, 0755);  
    }
}

void csv_to_sysex() {
    FCB1010 fcb;
    init_fcb1010(&fcb);

    // Get the home directory
    const char *home_dir = getenv("HOME");
    if (!home_dir) {
        printw("Failed to get home directory.\n");
        refresh();
        getch();
        return;
    }

    // Build the path for the CSV file
    char csv_filename[512];
    snprintf(csv_filename, sizeof(csv_filename), "%s/.fcb1010/fcb1010.csv", home_dir);

    // Load CSV into FCB1010 structure
    if (!load_csv(&fcb, csv_filename)) {
        printw("Failed to load CSV file: %s\n", csv_filename);
        refresh();
        getch();
        return;
    }

    // Build the path for the SysEx file
    char sysex_filename[512];
    snprintf(sysex_filename, sizeof(sysex_filename), "%s/.fcb1010/dump.syx", home_dir);

    FILE *sysex_file = fopen(sysex_filename, "wb");
    if (!sysex_file) {
        printw("Failed to open SysEx file for writing: %s\n", sysex_filename);
        refresh();
        getch();
        return;
    }

    uint8_t sysex_data[SYSEX_SIZE];
    memset(sysex_data, 0, SYSEX_SIZE);  // Clear the SysEx data array

    if (get_raw_sysex(&fcb, sysex_data)) {
        fwrite(sysex_data, 1, SYSEX_SIZE, sysex_file);
        printw("SysEx data generated and saved successfully.\n");
    } else {
        printw("Failed to generate SysEx data.\n");
    }

    fclose(sysex_file);
    refresh();
    getch();
}

void backup_sysex_file() {
    // Get the current time and format it as yymmdd_hhmm
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[16];
    strftime(timestamp, sizeof(timestamp), "%y%m%d_%H%M", t);

    // Get the home directory
    const char *home_dir = getenv("HOME");
    if (!home_dir) {
        printw("Failed to get home directory.\n");
        refresh();
        getch();
        return;
    }

    // Build the full path for the source and backup files
    char source_path[512];
    snprintf(source_path, sizeof(source_path), "%s/.fcb1010/dump.syx", home_dir);

    char backup_path[512];
    snprintf(backup_path, sizeof(backup_path), "%s/.fcb1010/backups/%s.syx", home_dir, timestamp);

    FILE *source_file = fopen(source_path, "rb");
    if (!source_file) {
        printw("Failed to open source SysEx file for reading: %s\n", source_path);
        refresh();
        getch();
        return;
    }

    FILE *destination_file = fopen(backup_path, "wb");
    if (!destination_file) {
        printw("Failed to open destination SysEx file for writing: %s\n", backup_path);
        fclose(source_file);
        refresh();
        getch();
        return;
    }

    char buffer[BUFSIZ];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
        if (fwrite(buffer, 1, bytes_read, destination_file) != bytes_read) {
            printw("Failed to write to destination SysEx file: %s\n", backup_path);
            fclose(source_file);
            fclose(destination_file);
            refresh();
            getch();
            return;
        }
    }

    if (ferror(source_file)) {
        printw("Error reading source SysEx file.\n");
    } else if (ferror(destination_file)) {
        printw("Error writing to destination SysEx file.\n");
    } else {
        printw("SysEx dump copied successfully to %s.\n", backup_path);
    }

    fclose(source_file);
    fclose(destination_file);
    refresh();
    getch();
}

