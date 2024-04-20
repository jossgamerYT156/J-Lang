// this file contains the same functions and basics... just ported for Windows OS.
// again, this doesn't plan to replace or try to be a real programming language.
// project was made for pure entretainment only

// windows specific includes like windows.h and conio.h
// others are mostly the same
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>

// Mapping of keys in jlang to their keyboard module counterparts
#define MAX_KEY_MAP_SIZE 26
char KEY_MAP[MAX_KEY_MAP_SIZE][2] = {
    {'a', 'a'},
    {'b', 'b'},
    {'c', 'c'}
    // You can add more mappings as needed
};

// Function prototypes
void print_(const char *text);
void msg(const char *message);
void browser(const char *url);
bool keyCombo(char key);
char *getInput(void);
void printInput(const char *prompt, const char *value);
void execute_instructions(const char *filename);
char *execute_instruction(char *instruction);

// Function definitions
void print_(const char *text) {
    printf("%s\n", text);
}

void msg(const char *message) {
    // Treat message as a comment, simply ignore it
}

void browser(const char *url) {
    ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

bool keyCombo(char key) {
    // Simulated key combo detection
    if (_kbhit()) {
        char pressedKey = _getch();
        return (pressedKey == key);
    }
    return false;
}

char *getInput(void) {
    char *input = (char *)malloc(100 * sizeof(char)); // Adjust buffer size as needed
    printf("\n"); // Add newline for better user experience
    gets(input);
    return input;
}

void printInput(const char *prompt, const char *value) {
    printf("%s%s\n", prompt, value);
}

void execute_instructions(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("File not found: %s\n", filename);
        return;
    }
    char line[100]; // Adjust buffer size as needed
    while (fgets(line, sizeof(line), file)) {
        char *result = execute_instruction(line);
        if (result != NULL) {
            printf("%s", result);
            free(result); // Free dynamically allocated memory
        }
    }
    fclose(file);
}

void activityClass(const char *type) {
    if (strcmp(type, "inputO") == 0) {
        // Handle input only activity
        // For example, ask the user for input without echoing it back
        printf("Input only activity:\n");
        char *input = getInput();
        // Process input as needed
        free(input);
    } else if (strcmp(type, "outputO") == 0) {
        // Handle output only activity
        // For example, output some predefined data
        printf("Output only activity:\n");
        print_("Predefined output data.");
    } else if (strcmp(type, "InOut") == 0) {
        // Handle input and output activity
        // For example, ask for input and echo it back
        printf("Input and output activity:\n");
        char *input = getInput();
        printInput("You entered: ", input);
        free(input);
    } else {
        printf("Unknown activity type: %s\n", type);
    }
}

char *execute_instruction(char *instruction) {
    char *command = strtok(instruction, "(");
    char *args = strtok(NULL, ")");
    if (strcmp(command, "print") == 0) {
        print_(args + 1); // Skip the opening quote
    } else if (strcmp(command, "msg") == 0) {
        msg(args + 1); // Skip the opening quote
    } else if (strcmp(command, "activityClass") == 0) {
        // Parse the type from args (e.g., "inputO", "outputO", "InOut")
        char *type = strtok(args, ")");
        activityClass(type);
    } else if (strcmp(command, "browser") == 0) {
        browser(args + 1); // Skip the opening quote
    } else if (strcmp(command, "keyCombo") == 0) {
        char *key = strtok(args, "\"");
        for (int i = 0; i < MAX_KEY_MAP_SIZE; i++) {
            if (KEY_MAP[i][0] == key[0]) {
                bool key_combo_detected = keyCombo(KEY_MAP[i][1]);
                if (key_combo_detected) {
                    browser("https://www.google.com"); // Example URL
                }
                break;
            }
        }
    } else if (strcmp(command, "getInput") == 0) {
        return getInput();
    } else if (strcmp(command, "printInput") == 0) {
        char *prompt = strtok(args, "\"");
        char *value = strtok(NULL, "\"");
        // Dynamically allocate memory for the concatenated string
        char *result = (char *)malloc(strlen(prompt) + strlen(value) + 1);
        strcpy(result, prompt);
        strcat(result, value);
        return result;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename.jl>\n", argv[0]);
        return 1;
    }
    const char *filename = argv[1];
    if (strcmp(filename + strlen(filename) - 3, ".jl") != 0) {
        printf("Error: Input file must have a .jl extension\n");
        return 1;
    }
    execute_instructions(filename);
    return 0;
}
