#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #define TEMP_FILE_NAME "temp_c_code.c"
    #define TEMP_EXEC_NAME "temp_c_exec.exe"
    #define PATH_SEPARATOR "\\"
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #define TEMP_FILE_NAME "temp_c_code.c"
    #define TEMP_EXEC_NAME "temp_c_exec"
    #define PATH_SEPARATOR "/"
#endif

#define MAX_INPUT_SIZE 2048
#define MAX_CODE_SIZE 8192
#define MAX_HISTORY 50

// Global variables
char global_code[MAX_CODE_SIZE] = "";
char includes[MAX_CODE_SIZE] = "#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <math.h>\n\n";
char history[MAX_HISTORY][MAX_INPUT_SIZE];
int history_count = 0;

// Function declarations
void print_welcome();
void print_prompt();
int compile_and_run(const char* code);
void add_to_global_code(const char* code);
int is_declaration(const char* code);
void cleanup_files();
void print_help();
void add_to_history(const char* input);
void show_history();
int is_expression(const char* code);
void handle_expression(const char* expr);
void set_console_color(int color);
void reset_console_color();

// Console color codes
#define COLOR_RED 12
#define COLOR_GREEN 10
#define COLOR_YELLOW 14
#define COLOR_BLUE 9
#define COLOR_WHITE 15
#define COLOR_GRAY 8

int main() {
    char input[MAX_INPUT_SIZE];
    char current_code[MAX_CODE_SIZE];
    
    print_welcome();
    
    while (1) {
        print_prompt();
        
        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // 去除换行符
        input[strcspn(input, "\n")] = 0;
        
        // Check exit commands
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            set_console_color(COLOR_GREEN);
            printf("Goodbye! Thank you for using C Interactive Terminal\n");
            reset_console_color();
            break;
        }
        
        // 检查帮助命令
        if (strcmp(input, "help") == 0) {
            print_help();
            continue;
        }
        
        // Check clear command
        if (strcmp(input, "clear") == 0) {
            memset(global_code, 0, sizeof(global_code));
            set_console_color(COLOR_GREEN);
            printf("Global code cleared\n");
            reset_console_color();
            continue;
        }
        
        // Check show global code command
        if (strcmp(input, "show") == 0) {
            set_console_color(COLOR_BLUE);
            printf("Current global code:\n%s\n", global_code);
            reset_console_color();
            continue;
        }
        
        // 检查历史命令
        if (strcmp(input, "history") == 0) {
            show_history();
            continue;
        }
        
        // 跳过空行
        if (strlen(input) == 0) {
            continue;
        }
        
        // 添加到历史
        add_to_history(input);
        
        // 检查是否是表达式（用于计算）
        if (is_expression(input)) {
            handle_expression(input);
            continue;
        }
        
        // Check if it's a declaration (variables, functions, etc.)
        if (is_declaration(input)) {
            add_to_global_code(input);
            set_console_color(COLOR_GREEN);
            printf("Added to global code\n");
            reset_console_color();
            continue;
        }
        
        // 构建完整的代码
        snprintf(current_code, sizeof(current_code), 
                "%s%s\nint main() {\n%s\nreturn 0;\n}\n", 
                includes, global_code, input);
        
        // Compile and execute
        if (compile_and_run(current_code) != 0) {
            set_console_color(COLOR_RED);
            printf("Execution failed\n");
            reset_console_color();
        }
    }
    
    cleanup_files();
    return 0;
}

void set_console_color(int color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#else
    // Linux/Unix color codes
    switch(color) {
        case COLOR_RED: printf("\033[31m"); break;
        case COLOR_GREEN: printf("\033[32m"); break;
        case COLOR_YELLOW: printf("\033[33m"); break;
        case COLOR_BLUE: printf("\033[34m"); break;
        case COLOR_WHITE: printf("\033[37m"); break;
        case COLOR_GRAY: printf("\033[90m"); break;
    }
#endif
}

void reset_console_color() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, COLOR_WHITE);
#else
    printf("\033[0m");
#endif
}

void print_welcome() {
    set_console_color(COLOR_YELLOW);
    printf("========================================\n");
    printf("     C Interactive Terminal v2.0\n");
    printf("========================================\n");
    reset_console_color();
    set_console_color(COLOR_BLUE);
    printf("Type 'help' for help\n");
    printf("Type 'exit' or 'quit' to exit\n");
    printf("Type 'clear' to clear global code\n");
    printf("Type 'show' to display current global code\n");
    printf("Type 'history' to view command history\n\n");
    reset_console_color();
}

void print_prompt() {
    set_console_color(COLOR_GREEN);
    printf("C>>> ");
    reset_console_color();
    fflush(stdout);
}

void print_help() {
    set_console_color(COLOR_BLUE);
    printf("\nHelp Information:\n");
    printf("========================================\n");
    printf("Basic Usage:\n");
    printf("- Single C statement: printf(\"Hello World\\n\");\n");
    printf("- Variable definition: int x = 10;\n");
    printf("- Function definition: int add(int a, int b) { return a + b; }\n");
    printf("- Math expression: 1 + 2 * 3\n");
    printf("- Function call: add(5, 3);\n\n");
    printf("Commands:\n");
    printf("- help     Show this help\n");
    printf("- clear    Clear all global definitions\n");
    printf("- show     View current global code\n");
    printf("- history  View command history\n");
    printf("- exit/quit Exit program\n\n");
    printf("Examples:\n");
    printf("C>>> int x = 5;\n");
    printf("C>>> printf(\"x = %%d\\n\", x);\n");
    printf("C>>> int factorial(int n) { return n <= 1 ? 1 : n * factorial(n-1); }\n");
    printf("C>>> printf(\"5! = %%d\\n\", factorial(5));\n");
    printf("========================================\n\n");
    reset_console_color();
}

void add_to_history(const char* input) {
    if (history_count < MAX_HISTORY) {
        strcpy(history[history_count], input);
        history_count++;
    } else {
        // Move history records
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(history[i], history[i + 1]);
        }
        strcpy(history[MAX_HISTORY - 1], input);
    }
}

void show_history() {
    set_console_color(COLOR_GRAY);
    printf("Command History:\n");
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
    printf("\n");
    reset_console_color();
}

int is_expression(const char* code) {
    // Simple check for math expressions
    char* trimmed = strdup(code);
    
    // 去除前导空格
    while (*trimmed == ' ' || *trimmed == '\t') {
        trimmed++;
    }
    
    // Check if contains operators and doesn't end with semicolon
    if ((strchr(trimmed, '+') || strchr(trimmed, '-') || strchr(trimmed, '*') || strchr(trimmed, '/') || strchr(trimmed, '%')) 
        && strchr(trimmed, ';') == NULL && strchr(trimmed, '(') == NULL) {
        return 1;
    }
    
    return 0;
}

void handle_expression(const char* expr) {
    char current_code[MAX_CODE_SIZE];
    
    // Build expression calculation code
    snprintf(current_code, sizeof(current_code), 
            "%s%s\nint main() {\nprintf(\"Result: %%g\\n\", (double)(%s));\nreturn 0;\n}\n", 
            includes, global_code, expr);
    
    // Compile and execute
    if (compile_and_run(current_code) != 0) {
        set_console_color(COLOR_RED);
        printf("Expression calculation failed\n");
        reset_console_color();
    }
}

int is_declaration(const char* code) {
    char* trimmed = strdup(code);
    
    // Remove leading spaces
    while (*trimmed == ' ' || *trimmed == '\t') {
        trimmed++;
    }
    
    // Check if it's a type declaration
    if (strncmp(trimmed, "int ", 4) == 0 ||
        strncmp(trimmed, "float ", 6) == 0 ||
        strncmp(trimmed, "double ", 7) == 0 ||
        strncmp(trimmed, "char ", 5) == 0 ||
        strncmp(trimmed, "void ", 5) == 0 ||
        strncmp(trimmed, "long ", 5) == 0 ||
        strncmp(trimmed, "short ", 6) == 0 ||
        strncmp(trimmed, "unsigned ", 9) == 0) {
        
        // Check if it's a function definition (contains curly braces)
        if (strchr(trimmed, '{') != NULL) {
            return 1; // Function definition
        }
        
        // Check if it's a variable declaration (ends with semicolon)
        if (strchr(trimmed, ';') != NULL) {
            return 1; // Variable declaration
        }
    }
    
    return 0;
}

void add_to_global_code(const char* code) {
    strncat(global_code, code, MAX_CODE_SIZE - strlen(global_code) - 1);
    strncat(global_code, "\n", MAX_CODE_SIZE - strlen(global_code) - 1);
}

int compile_and_run(const char* code) {
    FILE* temp_file;
    int compile_result, run_result;
    
    // Write to temporary file
    temp_file = fopen(TEMP_FILE_NAME, "w");
    if (temp_file == NULL) {
        set_console_color(COLOR_RED);
        printf("Cannot create temporary file\n");
        reset_console_color();
        return -1;
    }
    
    fprintf(temp_file, "%s", code);
    fclose(temp_file);
    
    // Compile code
    char compile_cmd[512];
    snprintf(compile_cmd, sizeof(compile_cmd), "gcc -o %s %s 2>nul", TEMP_EXEC_NAME, TEMP_FILE_NAME);
    
    compile_result = system(compile_cmd);
    
    if (compile_result != 0) {
        set_console_color(COLOR_RED);
        printf("Compilation error\n");
        reset_console_color();
        return -1;
    }
    
    // Run program
    char run_cmd[256];
    snprintf(run_cmd, sizeof(run_cmd), "%s", TEMP_EXEC_NAME);
    
    run_result = system(run_cmd);
    
    return run_result;
}

void cleanup_files() {
    remove(TEMP_FILE_NAME);
    remove(TEMP_EXEC_NAME);
}
