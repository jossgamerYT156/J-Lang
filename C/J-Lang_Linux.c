//attention, this code is just the one that handles the execution of .jl files, this is NOT a Compilator or Builder like GCC or VSDCMD
//the whole idea of this existing it's because Lilly was bored as hell and decided to write a pseudo programming language.


//includes for needed libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <sys/wait.h>

// Mapping of keys in J-Lang to their keyboard module counterparts
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
void activityClass(const char *type);

// Function definitions
void print_(const char *text) {
    printf("%s\n", text);
}

void msg(const char *message) {
    // Treat message as a comment, simply ignore it
   // we didn't needed to add this... but we did because why tf not.
}

void browser(const char *url) {
    // Launch the default browser with the given URL
    // ATTENTION! this might not work in all distros!
    if (fork() == 0) {
        execlp("xdg-open", "xdg-open", url, NULL);
        // If exec fails, print error message
        perror("Failed to launch browser");
        exit(EXIT_FAILURE);
    }
    wait(NULL);  // Wait for the child process to terminate
}

bool keyCombo(char key) {
    // Simulated key combo detection using Linux-specific terminal handling
   // for now it is always detecting FALSE... because idk how to properly code this part.
    struct termios oldt, newt;
    int ch;
    bool key_detected = false;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // Disable canonical mode and echoing
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Check if a key is pressed
    if (read(STDIN_FILENO, &ch, 1) > 0) {
        key_detected = (ch == key);
    }

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return key_detected;
}

char *getInput(void) {
    char *input = (char *)malloc(100 * sizeof(char)); // Adjust buffer size as needed
    printf("\n"); // Add newline for better user experience
    fgets(input, 100, stdin);
    // Remove the trailing newline character
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
    return input;
}

// this function is just to print inputs... obviously... not actual reason to why is here... i just added it on an impulse.
void printInput(const char *prompt, const char *value) {
    printf("%s%s\n", prompt, value);
}

// this part is what actually executes the J-Lang code.
// J-Lang is a pseudo programming language... and this its interpreter...
// if anything in this part is changed, the whole working of J-Lang could be affected.
void execute_instructions(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("File not found: %s\n", filename);
        return;
    }
    char line[100]; // Command Buffer size.... we don't need a lot of buffer since J-Lang is very basic at this point
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

    // Check the command and handle the appropriate action
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

// and the main function... this is the one that detects if the user specified a file and so to interpret it and execute the functions.
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


// PD: this was made for pure entretainment purposes only... is not at all a programming language as complex as C or Python.
// i will keep working on it and adding more things as i see need to add.
// if you think i should add something... please let me know in the issues box :D
