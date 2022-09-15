#include "json_parser.h"
#include "access_points_data.h"

#include <trace.h>

#define JSMN_STATIC
#include <jsmn/jsmn.h>

#include <stb/stb_ds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECTED_TOKENS 1024

typedef struct file_buffer
{
    const char* data;
    long size;
} file_buffer;

static file_buffer read_file(const char* file_path)
{
    FILE* file = fopen(file_path, "rb");

    if (!file)
    {
        TRACE_ERROR("Couldn't open file: %s", file_path);
        return (file_buffer){.data = NULL, .size = 0};
    }

    fseek(file, 0, SEEK_END);
    const long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* file_data = malloc(file_size + 1);
    if (!file_data)
    {
        TRACE_ERROR("Couldn't allocate to open file: %s", file_path);
        return (file_buffer){.data = NULL, .size = 0};
    }

    int number_of_bytes = fread(file_data, 1, file_size, file);
    if (number_of_bytes != file_size)
    {
        TRACE_ERROR("Couldn't read file: %s", file_path);
        return (file_buffer){.data = NULL, .size = 0};
    }

    // force null terminate
    file_data[file_size] = 0;

    if (fclose(file) != 0)
    {
        TRACE_ERROR("Failed to close file: %s", file_path);
    }

    return (file_buffer){.data = file_data, .size = file_size};
}

static int json_token_eq_str(const char* file_data, jsmntok_t* tok, const char* str)
{
    if (tok->type == JSMN_STRING &&
        (int)strlen(str) == tok->end - tok->start &&
        strncmp(file_data + tok->start, str, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}

access_point_map* parse_json(const char* file_path)
{

    const file_buffer file_buf = read_file(file_path);
    if (!file_buf.data)
    {
        return NULL;
    }

    jsmntok_t tokens[EXPECTED_TOKENS];

    jsmn_parser parser;
    jsmn_init(&parser);
    int read_tokens =
        jsmn_parse(&parser, file_buf.data, file_buf.size, tokens, EXPECTED_TOKENS);

    if (read_tokens < 3)
    {
        TRACE_ERROR("JSON parsing error: %d", read_tokens);
        return NULL;
    }

    if (tokens[0].type != JSMN_OBJECT)
    {
        TRACE_ERROR("Top JSON object expected.");
        return NULL;
    }

    if (json_token_eq_str(file_buf.data, &tokens[1], "access_points") != 0 ||
        tokens[2].type != JSMN_ARRAY)
    {
        TRACE_ERROR("JSON access_points array expected.");
        return NULL;
    }

    const int access_points_array_token = 2;

    access_point_map* ap_map = create_access_point_map();

    int current_token = access_points_array_token;
    int number_of_aps = tokens[access_points_array_token].size;
    for (int ap_n = 0; ap_n < number_of_aps; ++ap_n)

    {
        if (tokens[current_token + 1].type == JSMN_OBJECT)
        {
            // to next object
            ++current_token;
            int number_of_attributes = tokens[current_token].size;

            access_point ap;

            for (int attribute = 0; attribute < number_of_attributes; ++attribute)
            {
                // to attribute
                ++current_token;

                if (json_token_eq_str(file_buf.data, &tokens[current_token], "ssid") == 0)
                {
                    // to value
                    ++current_token;
                    int ssid_len =
                        tokens[current_token].end - tokens[current_token].start;

                    ssid_len = (ssid_len > (SSID_MAX_LEN - 1)) ? SSID_MAX_LEN - 1 : ssid_len;

                    strncpy(ap.ssid,
                            file_buf.data + tokens[current_token].start,
                            ssid_len);

                    ap.ssid[ssid_len] = 0;
                }
                else if (json_token_eq_str(file_buf.data, &tokens[current_token], "snr") == 0)
                {
                    // to value
                    ++current_token;
                    ap.SNR = strtol(file_buf.data + tokens[current_token].start, NULL, 10);
                }
                else if (json_token_eq_str(file_buf.data, &tokens[current_token], "channel") == 0)
                {
                    // to value
                    ++current_token;
                    ap.channel = strtol(file_buf.data + tokens[current_token].start, NULL, 10);
                }
            }

            // insert ap into hash map
            shput(ap_map, ap.ssid, ap);
        }
    }
    return ap_map;
}