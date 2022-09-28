#include "json_parser.h"
#include "access_points_data.h"

#include <trace.h>
#include <utils.h>

// make all JSMN symbols static to avoid collisions
#define JSMN_STATIC
#include <jsmn/jsmn.h>

#include <stdio.h>
#include <string.h>

#define EXPECTED_TOKENS 1024

static json_data_buffer read_file(const char* file_path)
{
    FILE* file = fopen(file_path, "rb");

    if (!file)
    {
        TRACE_ERROR("Couldn't open file: %s", file_path);
        return (json_data_buffer){.data = NULL, .size = 0};
    }

    fseek(file, 0, SEEK_END);
    const long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* file_data = SAFE_MALLOC(file_size + 1);

    const int number_of_bytes = fread(file_data, 1, file_size, file);
    if (number_of_bytes != file_size)
    {
        TRACE_ERROR("Couldn't read file: %s", file_path);
        SAFE_FREE(file_data);
        fclose(file);
        return (json_data_buffer){.data = NULL, .size = 0};
    }

    // force null terminate
    file_data[file_size] = 0;

    if (fclose(file) != 0)
    {
        TRACE_ERROR("Failed to close file: %s", file_path);
    }

    return (json_data_buffer){.data = file_data, .size = file_size};
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

// parses single access_point, returns last token in parsed access_point
static int parse_access_point(jsmntok_t* tokens, int current_token, const char* file_data, access_point_map** ap_map)
{
    int number_of_attributes = tokens[current_token].size;

    access_point ap = {0};

    for (int attribute = 0; attribute < number_of_attributes; ++attribute)
    {
        // to attribute
        ++current_token;

        if (json_token_eq_str(file_data, &tokens[current_token], "ssid") == 0)
        {
            // to value
            ++current_token;
            int ssid_len =
                tokens[current_token].end - tokens[current_token].start;

            ssid_len = (ssid_len > (SSID_MAX_LEN - 1)) ? SSID_MAX_LEN - 1 : ssid_len;

            strncpy(ap.ssid,
                    file_data + tokens[current_token].start,
                    ssid_len);

            ap.ssid[ssid_len] = 0;
        }
        else if (json_token_eq_str(file_data, &tokens[current_token], "snr") == 0)
        {
            // to value
            ++current_token;
            ap.SNR = strtol(file_data + tokens[current_token].start, NULL, 10);
        }
        else if (json_token_eq_str(file_data, &tokens[current_token], "channel") == 0)
        {
            // to value
            ++current_token;
            ap.channel = strtol(file_data + tokens[current_token].start, NULL, 10);
        }
    }

    // insert ap into hash map
    *ap_map = add_to_map(*ap_map, &ap);

    return current_token;
}

access_point_map* parse_json_from_file(const char* file_path)
{
    json_data_buffer file_buf = read_file(file_path);
    if (!file_buf.data)
    {
        return NULL;
    }

    access_point_map* map = parse_json_from_buffer(file_buf);

    SAFE_FREE(file_buf.data);

    return map;
}

access_point_map* parse_json_from_buffer(json_data_buffer json_buffer)
{
    if (json_buffer.data == NULL || json_buffer.size == 0)
    {
        TRACE_ERROR("Empty JSON buffer");
        return NULL;
    }

    jsmntok_t tokens[EXPECTED_TOKENS];

    jsmn_parser parser;
    jsmn_init(&parser);
    int read_tokens =
        jsmn_parse(&parser, json_buffer.data, json_buffer.size, tokens, EXPECTED_TOKENS);

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

    if (json_token_eq_str(json_buffer.data, &tokens[1], "access_points") != 0 ||
        tokens[2].type != JSMN_ARRAY)
    {
        TRACE_ERROR("JSON access_points array expected.");
        return NULL;
    }

    const int access_points_array_token = 2;

    int current_token = access_points_array_token;
    int number_of_aps = tokens[access_points_array_token].size;

    access_point_map* ap_map = create_access_point_map();
    for (int ap_n = 0; ap_n < number_of_aps; ++ap_n)

    {
        if (tokens[current_token + 1].type == JSMN_OBJECT)
        {
            // to next access point object
            ++current_token;
            current_token = parse_access_point(tokens, current_token, json_buffer.data, &ap_map);
        }
        else
        {
            TRACE_ERROR("Improper JSON format, expected access_point object.");
            delete_access_point_map(ap_map);
            return NULL;
        }
    }

    return ap_map;
}