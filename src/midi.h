#ifndef MIDI_H
#define MIDI_H

#define MAX_DEVICES 10
#define BUFFER_SIZE 1024

void list_midi_devices(char devices[MAX_DEVICES][128], int *count);
void receive_sysex_dump(const char *device_name, const char *filename);
void send_sysex_dump(const char *device_name, const char *filename);

#endif

