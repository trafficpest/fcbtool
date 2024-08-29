#ifndef FCB2CSV_H
#define FCB2CSV_H

#include <stdbool.h>
#include <stdint.h>

#define SYSEX_SIZE 2352
#define NUM_PRESETS 100

typedef struct {
    bool pc1_enabled;
    bool pc2_enabled;
    bool pc3_enabled;
    bool pc4_enabled;
    bool pc5_enabled;
    bool cc1_enabled;
    bool cc2_enabled;
    bool switch1_enabled;
    bool switch2_enabled;
    bool expA_enabled;
    bool expB_enabled;
    bool note_enabled;
    uint8_t pc1_program;
    uint8_t pc2_program;
    uint8_t pc3_program;
    uint8_t pc4_program;
    uint8_t pc5_program;
    uint8_t cc1_controller;
    uint8_t cc1_value;
    uint8_t cc2_controller;
    uint8_t cc2_value;
    uint8_t expA_controller;
    uint8_t expA_min;
    uint8_t expA_max;
    uint8_t expB_controller;
    uint8_t expB_min;
    uint8_t expB_max;
    uint8_t note_value;
} FCB1010Preset;

typedef struct {
    FCB1010Preset preset[NUM_PRESETS];
    uint8_t pc1_midi_channel;
    uint8_t pc2_midi_channel;
    uint8_t pc3_midi_channel;
    uint8_t pc4_midi_channel;
    uint8_t pc5_midi_channel;
    uint8_t cc1_midi_channel;
    uint8_t cc2_midi_channel;
    uint8_t expA_midi_channel;
    uint8_t expB_midi_channel;
    uint8_t note_midi_channel;
    bool direct_select;
    bool running_status;
    bool merge;
    bool switch1;
    bool switch2;
    uint8_t expA_calibration_min;
    uint8_t expA_calibration_max;
    uint8_t expB_calibration_min;
    uint8_t expB_calibration_max;
} FCB1010;

void init_fcb1010(FCB1010 *fcb);

void get_param_enable_states(const uint8_t *data, size_t *offset, size_t *flag, bool invert, bool *result);

void set_param_enable_states(uint8_t *data, size_t *offset, size_t *flag, bool value);

size_t get_params(const uint8_t *data, size_t offset, uint8_t *result);

size_t set_params(uint8_t *data, size_t offset, uint8_t value);

bool parse_sysex(FCB1010 *fcb, uint8_t *data, size_t size);

bool get_raw_sysex(const FCB1010 *fcb, uint8_t *data);

void print_fcb1010(const FCB1010 *fcb);

bool write_csv(const FCB1010 *fcb, const char *filename);

bool parse_csv_line(char *line, char *parsed_data[], int expected_columns);

bool load_csv(FCB1010 *fcb, const char *filename);

#endif // FCB2CSV_H

