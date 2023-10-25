
// https://www.raylib.com/cheatsheet/cheatsheet.html
// https://kremlin.cc/k&r.pdf
// https://codepoints.net/search?blk=Cyrillic
// https://github.com/philj56/fuzzy-match
// https://github.com/anyascii/anyascii

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "fuzzy_match.h"
#include "anyascii.h"

#define FONT_SIZE 16
#define FONT_SPACING 1

#ifndef MAX_TEXT_BUFFER_LENGTH
#define MAX_TEXT_BUFFER_LENGTH 1024
#endif

char* ascii_string(char* input) {

    int input_size = strlen(input);
    
    char* buffer = (char*) calloc(MAX_TEXT_BUFFER_LENGTH, sizeof(char));
    size_t buffer_cursor = 0;

    for (int i = 0; i < input_size; ++i) {

        int current_char = input[i];

        if (current_char >= 0 && current_char <= 128) { // ascii
            buffer[buffer_cursor++] = current_char;
        } else { // unicode

            int codepoint_size;
            int codepoint = GetCodepoint(input + i, &codepoint_size);

            char* ascii_buffer;
            size_t ascii_size = anyascii(codepoint, (const char**) &ascii_buffer);

            for (size_t j = 0; j < ascii_size; ++j) {
                buffer[buffer_cursor++] = (char) ascii_buffer[j];
            }

            i++;

        }
        
    }

    return buffer;

}

char** read_lines_from_file(char* file_path, size_t* out_lines_count) {

    const size_t line_size = 1024;
    size_t lines_size = 1024;
    char** lines = (char**) malloc(lines_size * sizeof(char*));
    size_t line_index = 0;
    
    FILE* file = fopen(file_path, "r");

    char buffer[2048];

    while (fgets(buffer, sizeof(buffer), file)) {

        if (line_index + 1 == lines_size) {
            lines_size *= 2;
            lines = (char**) realloc(lines, lines_size * sizeof(char*));
        }

        char* current_line = (char*) malloc(line_size * sizeof(char));

        if (line_index == 0) strcpy(current_line, ((char*)buffer) + 3);
        else strcpy(current_line, buffer);
        
        lines[line_index++] = current_line;
        
    }

    fclose(file);

    *out_lines_count = line_index;

    return lines;
    
}

int main(void) {

    InitWindow(800, 600, "raylib");

    int codepoints[512] = { 0 };
    for (int i = 0; i < 95; i++) codepoints[i] = 32 + i;   // Basic ASCII characters
    for (int i = 0; i < 255; i++) codepoints[96 + i] = 0x400 + i;   // Cyrillic characters
    Font font = LoadFontEx("./CascadiaMono.ttf", FONT_SIZE, codepoints, 512);

    char* file_path = "ibases.v8i";

    size_t ibases_lines_count = 0;
    char** ibases_lines = read_lines_from_file(file_path, &ibases_lines_count);

    printf("read %lld lines from file %s\n", ibases_lines_count, file_path);

    char* input = (char*) calloc(1024, 1);
    size_t input_size = 0;

    while (!WindowShouldClose()) {

        int key = GetKeyPressed();
        while (key != 0) {

            printf("key pressed %d\n", key);

            if (key == 259) { // backspace
                if (input_size > 0) {
                    input[(input_size--) - 1] = '\0';
                }
            }

            key = GetKeyPressed();

        }

        int pressed_codepoint = GetCharPressed();
        while (pressed_codepoint != 0) {
            
            printf("codepoint %d\n", pressed_codepoint);

            char* ascii_buffer;
            size_t ascii_size = anyascii(pressed_codepoint, (const char**) &ascii_buffer);
            for (size_t j = 0; j < ascii_size; ++j) {
                input[input_size++] = (char) ascii_buffer[j];
            }

            pressed_codepoint = GetCharPressed();
            
        }

        BeginDrawing();
        ClearBackground(BLACK);

        int cursor_y = 100;

        for (size_t i = 0; i < ibases_lines_count; ++i) {
        
            const char* current_line = TextToLower(ascii_string(ibases_lines[i]));
            int score = fuzzy_match(input, current_line);

            if (score != INT32_MIN) {
                Vector2 text_size = MeasureTextEx(font, current_line, FONT_SIZE, FONT_SPACING);
                if (i > 0) cursor_y += text_size.y;
                DrawTextEx(font, current_line, (Vector2) {0, cursor_y}, FONT_SIZE, FONT_SPACING, WHITE);
            }
        
        }
        
        DrawTextEx(font, input, (Vector2) {10, 10}, FONT_SIZE, FONT_SPACING, YELLOW);
    
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

