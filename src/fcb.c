/*  Functions for MIDI System Exclusive messages used by Behringer FCB1010
*   Allows loading and saving data using comma separated value file
*   and manipuating the data.
*   This is a C Implementation of Brian Walton's (brian@riban.co.uk) 
*   with slight mods to some errors noticed in sysex files. (might be
*   version differences?)
*   There is no affiliation with Behringer. This is all reverse engineered
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "fcb.h"

void init_fcb1010(FCB1010 *fcb) {
    memset(fcb, 0, sizeof(FCB1010));
    for (int i = 0; i < NUM_PRESETS; ++i) {
        fcb->preset[i].expA_controller = 27;
        fcb->preset[i].expB_controller = 7;
        fcb->preset[i].expA_min = 0;
        fcb->preset[i].expA_max = 127;
        fcb->preset[i].expB_min = 0;
        fcb->preset[i].expB_max = 127;
        fcb->preset[i].note_value = 60;
    }
}

// Function to get the enable state of a preset's parameters
void get_param_enable_states(const uint8_t *data, size_t *offset, size_t *flag, bool invert, bool *result) {
    *result = (data[*offset] & (1 << *flag)) != 0;
    if (invert) {
        *result = !(*result);
    }
    
    (*flag)++;
    if (*flag > 6) {
        *flag = 0;
        (*offset) += 8;
    }
}

// Function to set the enable state of a preset's parameters in raw sysex
void set_param_enable_states(uint8_t *data, size_t *offset, size_t *flag, bool value) {
    data[*offset] |= (value << *flag);

    (*flag)++;
    
    if (*flag > 6) {
        *flag = 0;
        (*offset) += 8;
    }
}

// Function to get preset parameter values 
size_t get_params(const uint8_t *data, size_t offset, uint8_t *result) {
    *result = data[offset];
    offset++;

    if ((offset - 6) % 8 == 0) {
        offset++;
    }

    return offset;
}

// Function to set preset parameter values within raw SysEx data
size_t set_params(uint8_t *data, size_t offset, uint8_t value) {
    data[offset] = value;
    
    offset += 1;

    if ((offset - 6) % 8 == 0) {
        offset += 1;
    }

    return offset;
}


bool parse_sysex(FCB1010 *fcb, uint8_t *data, size_t size) {
    if (size != SYSEX_SIZE || data[0] != 0xF0 || data[size - 1] != 0xF7 ||
        data[1] != 0 || data[2] != 32 || data[3] != 50 || data[4] != 1 ||
        data[5] != 12 || data[6] != 15) {
        return false;
    }

    size_t offset = 14;  // Start offset after the header
    size_t flag = 0;

    for (int preset = 0; preset < NUM_PRESETS; ++preset) {
        FCB1010Preset *p = &fcb->preset[preset];

        bool value;

        get_param_enable_states(data, &offset, &flag, true, &value);
        p->pc1_enabled = value;

        get_param_enable_states(data, &offset, &flag, true, &value);
        p->pc2_enabled = value;

        get_param_enable_states(data, &offset, &flag, true, &value);
        p->pc3_enabled = value;

        get_param_enable_states(data, &offset, &flag, true, &value);
        p->pc4_enabled = value;

        get_param_enable_states(data, &offset, &flag, true, &value);
        p->pc5_enabled = value;

        get_param_enable_states(data, &offset, &flag, true, &value);
        p->cc1_enabled = value;

        get_param_enable_states(data, &offset, &flag, false, &value);
        p->switch1_enabled = value;

        get_param_enable_states(data, &offset, &flag, true, &value);
        p->cc2_enabled = value;

        get_param_enable_states(data, &offset, &flag, false, &value);
        p->switch2_enabled = value;

        get_param_enable_states(data, &offset, &flag, true, &value);
        p->expA_enabled = value;

        // Skip two parameters (not used)
        get_param_enable_states(data, &offset, &flag, false, &value);
        get_param_enable_states(data, &offset, &flag, false, &value);

        // expB_enabled
        get_param_enable_states(data, &offset, &flag, true, &value);
        p->expB_enabled = value;

        // Skip two parameters (not used)
        get_param_enable_states(data, &offset, &flag, false, &value);
        get_param_enable_states(data, &offset, &flag, false, &value);

        // note_enabled
        get_param_enable_states(data, &offset, &flag, true, &value);
        p->note_enabled = value;
    }

    offset = 7;
    for (int preset = 0; preset < 100; ++preset) {
        offset = get_params(data, offset, &fcb->preset[preset].pc1_program);
        offset = get_params(data, offset, &fcb->preset[preset].pc2_program);
        offset = get_params(data, offset, &fcb->preset[preset].pc3_program);
        offset = get_params(data, offset, &fcb->preset[preset].pc4_program);
        offset = get_params(data, offset, &fcb->preset[preset].pc5_program);
        offset = get_params(data, offset, &fcb->preset[preset].cc1_controller);
        offset = get_params(data, offset, &fcb->preset[preset].cc1_value);
        offset = get_params(data, offset, &fcb->preset[preset].cc2_controller);
        offset = get_params(data, offset, &fcb->preset[preset].cc2_value);
        offset = get_params(data, offset, &fcb->preset[preset].expA_controller);
        offset = get_params(data, offset, &fcb->preset[preset].expA_min);
        offset = get_params(data, offset, &fcb->preset[preset].expA_max);
        offset = get_params(data, offset, &fcb->preset[preset].expB_controller);
        offset = get_params(data, offset, &fcb->preset[preset].expB_min);
        offset = get_params(data, offset, &fcb->preset[preset].expB_max);
        offset = get_params(data, offset, &fcb->preset[preset].note_value);
    }

    fcb->pc1_midi_channel = data[2311];
    fcb->pc2_midi_channel = data[2312];
    fcb->pc3_midi_channel = data[2313];
    fcb->pc4_midi_channel = data[2314];
    fcb->pc5_midi_channel = data[2315];
    fcb->cc1_midi_channel = data[2316];
    fcb->cc2_midi_channel = data[2317];
    fcb->expA_midi_channel = data[2319];
    fcb->expB_midi_channel = data[2320];
    fcb->note_midi_channel = data[2321];
    fcb->direct_select = (data[2330] & 2) != 0;
    fcb->running_status = (data[2330] & 4) != 0;
    fcb->merge = (data[2330] & 16) != 0;
    fcb->switch1 = (data[2334] & 4) != 0;
    fcb->switch2 = (data[2329] & 64) != 0;
    fcb->expA_calibration_min = data[2343];
    fcb->expA_calibration_max = data[2344];
    fcb->expB_calibration_min = data[2345];
    fcb->expB_calibration_max = data[2346];

    return true;
}

bool get_raw_sysex(const FCB1010 *fcb, uint8_t *data) {
    memset(data, 0, SYSEX_SIZE);

    data[0] = 0xF0;  // SysEx start byte
    data[1] = 0;     // Manufacturer ID (Non-Real-Time)
    data[2] = 32;    // Behringer (assumed for FCB1010)
    data[3] = 50;    // Behringer device (assumed)
    data[4] = 1;     // Device ID (default 1)
    data[5] = 12;    // Model ID (12 for FCB1010)
    data[6] = 15;    // Command (assumed SysEx read/write)
    data[SYSEX_SIZE - 1] = 0xF7;  // SysEx end byte
    for (size_t i = 1835; i < 2311; ++i) {
        data[i] = 127;
    }
    size_t offset = 14;
    size_t flag = 0;

    for (int preset = 0; preset < NUM_PRESETS; ++preset) {
        const FCB1010Preset *p = &fcb->preset[preset];

        set_param_enable_states(data, &offset, &flag, !p->pc1_enabled);
        set_param_enable_states(data, &offset, &flag, !p->pc2_enabled);
        set_param_enable_states(data, &offset, &flag, !p->pc3_enabled);
        set_param_enable_states(data, &offset, &flag, !p->pc4_enabled);
        set_param_enable_states(data, &offset, &flag, !p->pc5_enabled);
        set_param_enable_states(data, &offset, &flag, !p->cc1_enabled);
        set_param_enable_states(data, &offset, &flag, p->switch1_enabled);
        set_param_enable_states(data, &offset, &flag, !p->cc2_enabled);
        set_param_enable_states(data, &offset, &flag, p->switch2_enabled);
        set_param_enable_states(data, &offset, &flag, !p->expA_enabled);
        set_param_enable_states(data, &offset, &flag, false);
        set_param_enable_states(data, &offset, &flag, false);
        set_param_enable_states(data, &offset, &flag, !p->expB_enabled);
        set_param_enable_states(data, &offset, &flag, false);
        set_param_enable_states(data, &offset, &flag, false);
        set_param_enable_states(data, &offset, &flag, !p->note_enabled);
    }

    offset = 7;
    for (int preset = 0; preset < NUM_PRESETS; ++preset) {
        offset = set_params(data, offset, fcb->preset[preset].pc1_program);
        offset = set_params(data, offset, fcb->preset[preset].pc2_program);
        offset = set_params(data, offset, fcb->preset[preset].pc3_program);
        offset = set_params(data, offset, fcb->preset[preset].pc4_program);
        offset = set_params(data, offset, fcb->preset[preset].pc5_program);
        offset = set_params(data, offset, fcb->preset[preset].cc1_controller);
        offset = set_params(data, offset, fcb->preset[preset].cc1_value);
        offset = set_params(data, offset, fcb->preset[preset].cc2_controller);
        offset = set_params(data, offset, fcb->preset[preset].cc2_value);
        offset = set_params(data, offset, fcb->preset[preset].expA_controller);
        offset = set_params(data, offset, fcb->preset[preset].expA_min);
        offset = set_params(data, offset, fcb->preset[preset].expA_max);
        offset = set_params(data, offset, fcb->preset[preset].expB_controller);
        offset = set_params(data, offset, fcb->preset[preset].expB_min);
        offset = set_params(data, offset, fcb->preset[preset].expB_max);
        offset = set_params(data, offset, fcb->preset[preset].note_value);
    }

    data[1838] = 120;
    data[2311] = fcb->pc1_midi_channel;
    data[2312] = fcb->pc2_midi_channel;
    data[2313] = fcb->pc3_midi_channel;
    data[2314] = fcb->pc4_midi_channel;
    data[2315] = fcb->pc5_midi_channel;
    data[2316] = fcb->cc1_midi_channel;
    data[2317] = fcb->cc2_midi_channel;
    data[2319] = fcb->expA_midi_channel;
    data[2320] = fcb->expB_midi_channel;
    data[2321] = fcb->note_midi_channel;
    data[2331] = fcb->pc1_midi_channel;
    data[2332] = fcb->pc2_midi_channel;
    data[2333] = fcb->pc3_midi_channel;
    data[2335] = fcb->pc4_midi_channel;
    data[2336] = fcb->pc5_midi_channel;
    data[2337] = fcb->cc1_midi_channel;
    data[2338] = fcb->cc2_midi_channel;
    data[2339] = fcb->expA_midi_channel;
    data[2340] = fcb->expB_midi_channel;
    data[2341] = fcb->note_midi_channel;
    data[2322] = 127; //fcb->pc1_midi_channel;
    data[2323] = 127; //fcb->pc2_midi_channel;
    data[2324] = 127; //fcb->pc3_midi_channel;
    data[2325] = 127; //fcb->pc4_midi_channel;
    data[2326] = 120; //fcb->pc5_midi_channel;
    data[2327] = 127; //fcb->cc1_midi_channel;
    data[2328] = 127; //fcb->cc2_midi_channel;
    data[2330] = 0;
    data[2329] = 0;
    data[2334] = 3;
    if (fcb->direct_select) data[2330] |= 2;
    if (fcb->running_status) data[2330] |= 4;
    if (fcb->merge) data[2330] |= 16;
    if (fcb->switch1) data[2334] |= 2;
    if (fcb->switch2) data[2329] |= 64;
    data[2343] = 15; //fcb->expA_calibration_min;
    data[2344] = 108; //fcb->expA_calibration_max;
    data[2345] = 14; //fcb->expB_calibration_min;
    data[2346] = 102; //fcb->expB_calibration_max;

    data[2350] = 10;
    data[2351] = 247;

    return true;
}

bool write_csv(const FCB1010 *fcb, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return false;

    // Write the CSV headers (matching the Python version)
    fprintf(file, "Global,,Program Change 1,,Program Change 2,,Program Change 3,,Program Change 4,,Program Change 5,,Continuous Controller 1,,,Continuous Controller 2,,,Switch 1,Switch 2,Expression Pedal A,,,,Expression Pedal B,,,,Note,\n");
    
    // Write the global MIDI channel settings (matching the Python version)
    fprintf(file, "MIDI Channel,,%d,,%d,,%d,,%d,,%d,,%d,,,%d,,,N/A,N/A,%d,,,,%d,,,,%d,\n",
            fcb->pc1_midi_channel, fcb->pc2_midi_channel, fcb->pc3_midi_channel,
            fcb->pc4_midi_channel, fcb->pc5_midi_channel, fcb->cc1_midi_channel,
            fcb->cc2_midi_channel, fcb->expA_midi_channel, fcb->expB_midi_channel,
            fcb->note_midi_channel);

    // Write the detailed headers for each preset (matching the Python version)
    fprintf(file, "Bank,Preset,Enabled,Program,Enabled,Program,Enabled,Program,Enabled,Program,Enabled,Program,"
                  "Enabled,Controller,Value,Enabled,Controller,Value,Enabled,Enabled,Enabled,Controller,Minimum,Maximum,"
                  "Enabled,Controller,Minimum,Maximum,Enabled,Value\n");

    // Write each preset (matching the Python version)
    for (int bank = 1; bank <= 10; ++bank) {
        for (int offset = 1; offset <= 10; ++offset) {
            int preset = (bank - 1) * 10 + (offset - 1);

            const FCB1010Preset *p = &fcb->preset[preset];
            fprintf(file, "%d,%d,%d,%hhu,%d,%hhu,%d,%hhu,%d,%hhu,%d,%hhu,%d,%hhu,%hhu,%d,%hhu,%hhu,%d,%d,%d,%hhu,%hhu,%hhu,"
                          "%d,%hhu,%hhu,%hhu,%d,%hhu\n",
                    bank, offset,
                    p->pc1_enabled, p->pc1_program,
                    p->pc2_enabled, p->pc2_program,
                    p->pc3_enabled, p->pc3_program,
                    p->pc4_enabled, p->pc4_program,
                    p->pc5_enabled, p->pc5_program,
                    p->cc1_enabled, p->cc1_controller, p->cc1_value,
                    p->cc2_enabled, p->cc2_controller, p->cc2_value,
                    p->switch1_enabled, p->switch2_enabled,
                    p->expA_enabled, p->expA_controller, p->expA_min, p->expA_max,
                    p->expB_enabled, p->expB_controller, p->expB_min, p->expB_max,
                    p->note_enabled, p->note_value);
        }
    }

    fclose(file);
    return true;
}

bool parse_csv_line(char *line, char *parsed_data[], int expected_columns) {
    char *start = line;
    char *line_ptr = line;
    int i = 0;

    while (*line_ptr && i < expected_columns) {
        if (*line_ptr == ',') {
            *line_ptr = '\0';
            if (start == line_ptr) {
                parsed_data[i++] = NULL;
            } else {
                parsed_data[i++] = start;
            }
            start = line_ptr + 1;
        }
        line_ptr++;
    }

    if (start != line_ptr) {
        parsed_data[i++] = start;
    } else if (*line_ptr == '\0') {
        parsed_data[i++] = NULL;
    }

    return (i == expected_columns);
}

bool load_csv(FCB1010 *fcb, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to read file %s\n", filename);
        return false;
    }

    char line[1024];
    char *lines[NUM_PRESETS + 3];
    int line_count = 0;

    while (fgets(line, sizeof(line), file) && line_count < NUM_PRESETS + 3) {
        lines[line_count] = strdup(line);
        line_count++;
    }

    fclose(file);

    if (line_count < 3) {
        printf("CSV has insufficient lines. Should be at least 3\n");
        return false;
    }

    if (strcmp(lines[0], "Global,,Program Change 1,,Program Change 2,,Program Change 3,,Program Change 4,,Program Change 5,,Continuous Controller 1,,,Continuous Controller 2,,,Switch 1,Switch 2,Expression Pedal A,,,,Expression Pedal B,,,,Note,\n") != 0) {
        printf("First line of CSV should contain headers:\n");
        printf("Global,,Program Change 1,,Program Change 2,,Program Change 3,,Program Change 4,,Program Change 5,,Continuous Controller 1,,,Continuous Controller 2,,,Switch 1,Switch 2,Expression Pedal A,,,,Expression Pedal B,,,,Note,\n");
        return false;
    }

    if (strcmp(lines[2], "Bank,Preset,Enabled,Program,Enabled,Program,Enabled,Program,Enabled,Program,Enabled,Program,Enabled,Controller,Value,Enabled,Controller,Value,Enabled,Enabled,Enabled,Controller,Minimum,Maximum,Enabled,Controller,Minimum,Maximum,Enabled,Value\n") != 0) {
        printf("Third line of CSV should contain headers:\n");
        printf("Bank,Preset,Enabled,Program,Enabled,Program,Enabled,Program,Enabled,Program,Enabled,Program,Enabled,Controller,Value,Enabled,Controller,Value,Enabled,Enabled,Enabled,Controller,Minimum,Maximum,Enabled,Controller,Minimum,Maximum,Enabled,Value\n");
        return false;
    }

    char *midi_channels[30];
    if (!parse_csv_line(lines[1], midi_channels, 30)) {
        printf("Insufficient MIDI Channel parameters.\n");
        return false;
    }

    fcb->pc1_midi_channel = atoi(midi_channels[2]);
    fcb->pc2_midi_channel = atoi(midi_channels[4]);
    fcb->pc3_midi_channel = atoi(midi_channels[6]);
    fcb->pc4_midi_channel = atoi(midi_channels[8]);
    fcb->pc5_midi_channel = atoi(midi_channels[10]);
    fcb->cc1_midi_channel = atoi(midi_channels[12]);
    fcb->cc2_midi_channel = atoi(midi_channels[15]);
    fcb->expA_midi_channel = atoi(midi_channels[20]);
    fcb->expB_midi_channel = atoi(midi_channels[24]);
    fcb->note_midi_channel = atoi(midi_channels[28]);

    for (int i = 3; i < line_count; i++) {
        char *data[30];
        if (!parse_csv_line(lines[i], data, 30)) continue;

        int bank = atoi(data[0]);
        if (bank < 1 || bank > 10) continue;

        int preset_num = atoi(data[1]);
        if (preset_num < 1 || preset_num > 10) continue;

        int preset_index = (bank - 1) * 10 + (preset_num - 1);
        FCB1010Preset *preset = &fcb->preset[preset_index];

        preset->pc1_enabled = atoi(data[2]) == 1;
        preset->pc1_program = atoi(data[3]);
        preset->pc2_enabled = atoi(data[4]) == 1;
        preset->pc2_program = atoi(data[5]);
        preset->pc3_enabled = atoi(data[6]) == 1;
        preset->pc3_program = atoi(data[7]);
        preset->pc4_enabled = atoi(data[8]) == 1;
        preset->pc4_program = atoi(data[9]);
        preset->pc5_enabled = atoi(data[10]) == 1;
        preset->pc5_program = atoi(data[11]);
        preset->cc1_enabled = atoi(data[12]) == 1;
        preset->cc1_controller = atoi(data[13]);
        preset->cc1_value = atoi(data[14]);
        preset->cc2_enabled = atoi(data[15]) == 1;
        preset->cc2_controller = atoi(data[16]);
        preset->cc2_value = atoi(data[17]);
        preset->switch1_enabled = atoi(data[18]) == 1;
        preset->switch2_enabled = atoi(data[19]) == 1;
        preset->expA_enabled = atoi(data[20]) == 1;
        preset->expA_controller = atoi(data[21]);
        preset->expA_min = atoi(data[22]);
        preset->expA_max = atoi(data[23]);
        preset->expB_enabled = atoi(data[24]) == 1;
        preset->expB_controller = atoi(data[25]);
        preset->expB_min = atoi(data[26]);
        preset->expB_max = atoi(data[27]);
        preset->note_enabled = atoi(data[28]) == 1;
        preset->note_value = atoi(data[29]);
    }

    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }

    return true;
}

