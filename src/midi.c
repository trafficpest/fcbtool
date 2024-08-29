#include <alsa/asoundlib.h>
#include <ncurses.h>
#include <stdio.h>
#include "midi.h"

void list_midi_devices(char devices[MAX_DEVICES][128], int *count) {
    int card = -1;
    int dev;
    snd_rawmidi_info_t *info;
    snd_rawmidi_info_alloca(&info);

    *count = 0;

    if (snd_card_next(&card) < 0 || card < 0) return;

    while (*count < MAX_DEVICES && card >= 0) {
        snd_ctl_t *ctl;
        char name[32];

        sprintf(name, "hw:%d", card);
        if (snd_ctl_open(&ctl, name, 0) < 0) continue;

        dev = -1;
        while (snd_ctl_rawmidi_next_device(ctl, &dev) >= 0 && dev >= 0) {
            snd_rawmidi_info_set_device(info, dev);
            snd_rawmidi_info_set_subdevice(info, 0);
            snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);

            if (snd_ctl_rawmidi_info(ctl, info) >= 0) {
                snprintf(devices[*count], 128, "hw:%d,%d - %s", card, dev, snd_rawmidi_info_get_name(info));
                (*count)++;
            }
        }

        snd_ctl_close(ctl);
        snd_card_next(&card);
    }
}

void receive_sysex_dump(const char *device_name, const char *filename) {
    snd_rawmidi_t *input = NULL;
    FILE *output_file;
    unsigned char buffer[BUFFER_SIZE];
    int err;
    int in_sysex = 0;  // Flag to indicate if we are inside a SysEx message
    size_t total_bytes = 0;
    char actual_device_name[16];

    // Extract the actual device identifier ("hw:2,0") from the full device_name
    sscanf(device_name, "%15s", actual_device_name);

    printw("Attempting to open MIDI input on device %s...\n", actual_device_name);
    refresh();
    if ((err = snd_rawmidi_open(&input, NULL, actual_device_name, 0)) < 0) {
        printw("Error opening MIDI input: %s\n", snd_strerror(err));
        refresh();
        getch();  // Wait for user to press a key
        return;
    }

    output_file = fopen(filename, "wb");
    if (!output_file) {
        printw("Error opening file: %s\n", filename);
        refresh();
        snd_rawmidi_close(input);
        getch();  // Wait for user to press a key
        return;
    }

    printw("Ready to receive SysEx dump.\n");
    printw("Please activate the SysEx dump on your MIDI device...\n");
    refresh();

    printw("Receiving SysEx dump...\n");
    refresh();

    while (1) {
        err = snd_rawmidi_read(input, buffer, BUFFER_SIZE);
        if (err < 0) {
            printw("Error reading MIDI input: %s\n", snd_strerror(err));
            refresh();
            getch();  // Wait for user to press a key
            break;
        }

        for (int i = 0; i < err; i++) {
            if (buffer[i] == 0xF0) {
                in_sysex = 1;  // Start of SysEx message
                total_bytes = 0;
            }

            if (in_sysex) {
                fwrite(&buffer[i], 1, 1, output_file);
                total_bytes++;

                if (buffer[i] == 0xF7) {
                    in_sysex = 0;  // End of SysEx message
                    printw("SysEx dump received (%zu bytes) and saved to %s\n", total_bytes, filename);
                    refresh();
                    fclose(output_file);
                    snd_rawmidi_close(input);
                    return;
                }
            }
        }
    }

    fclose(output_file);
    snd_rawmidi_close(input);
    printw("SysEx dump reception was incomplete or failed.\n");
    refresh();
}

void send_sysex_dump(const char *device_name, const char *filename) {
    snd_rawmidi_t *output = NULL;
    FILE *input_file;
    unsigned char buffer[BUFFER_SIZE];
    int err;
    size_t bytes_read;
    char actual_device_name[16];

    // Extract the actual device identifier ("hw:2,0") from the full device_name
    sscanf(device_name, "%15s", actual_device_name);

    printw("Attempting to open MIDI output on device %s...\n", actual_device_name);
    printw("Set your device in receive mode then hit enter.");
    refresh();
    getch();  // Wait for user to press a key
    if ((err = snd_rawmidi_open(NULL, &output, actual_device_name, 0)) < 0) {
        printw("Error opening MIDI output: %s\n", snd_strerror(err));
        refresh();
        getch();  // Wait for user to press a key
        return;
    }

    input_file = fopen(filename, "rb");
    if (!input_file) {
        printw("Error opening file: %s\n", filename);
        refresh();
        snd_rawmidi_close(output);
        getch();  // Wait for user to press a key
        return;
    }

    printw("Sending SysEx dump...\n");
    refresh();
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input_file)) > 0) {
        if ((err = snd_rawmidi_write(output, buffer, bytes_read)) < 0) {
            printw("Error writing to MIDI output: %s\n", snd_strerror(err));
            refresh();
            getch();  // Wait for user to press a key
            break;
        }
    }

    fclose(input_file);
    snd_rawmidi_drain(output);
    snd_rawmidi_close(output);

    if (err < 0) {
        printw("Error during SysEx dump send: %s\n", snd_strerror(err));
    } else {
        printw("SysEx dump sent from %s\n", filename);
    }
    refresh();
    getch();  // Wait for user to press a key
}


