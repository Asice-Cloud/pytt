#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_INPUT_SIZE 1024
#define MAX_CODE_SIZE 4096
#define TEMP_FILE_NAME "temp_c_code.c"
#define TEMP_EXEC_NAME "temp_c_exec"

// 全局变量和函数声明的累积缓存
char global_code[MAX_CODE_SIZE] = "";
char includes[MAX_CODE_SIZE] = "#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <math.h>\n\n";

// 函数声明
void print_welcome();
void print_prompt();
int is_complete_statement(const char* code);
int compile_and_run(const char* code);
void add_to_global_code(const char* code);
int is_declaration(const char* code);
void cleanup_files();
void print_help();

int main() {
    char input[MAX_INPUT_SIZE];
    char current_code[MAX_CODE_SIZE];
    
    print_welcome();
    
    while (1) {
        print_prompt();
        
        // 读取用户输入
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // 去除换行符
        input[strcspn(input, "\n")] = 0;
        
        // 检查退出命令
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("再见！\n");
            break;
        }
        
        // 检查帮助命令
        if (strcmp(input, "help") == 0) {
            print_help();
            continue;
        }
        
        // 检查清除命令
        if (strcmp(input, "clear") == 0) {
            memset(global_code, 0, sizeof(global_code));
            printf("全局代码已清除\n");
            continue;
        }
        
        // 检查显示全局代码命令
        if (strcmp(input, "show") == 0) {
            printf("当前全局代码:\n%s\n", global_code);
            continue;
        }
        
        // 跳过空行
        if (strlen(input) == 0) {
            continue;
        }
        
        // 检查是否是声明（变量、函数等）
        if (is_declaration(input)) {
            add_to_global_code(input);
            printf("已添加到全局代码\n");
            continue;
        }
        
        // 构建完整的代码
        snprintf(current_code, sizeof(current_code), 
                "%s%s\nint main() {\n%s\nreturn 0;\n}\n", 
                includes, global_code, input);
        
        // 编译并执行
        if (compile_and_run(current_code) != 0) {
            printf("执行失败\n");
        }
    }
    
    cleanup_files();
    return 0;
}

void print_welcome() {
    printf("========================================\n");
    printf("     C 语言交互式终端 v1.0\n");
    printf("========================================\n");
    printf("输入 'help' 查看帮助\n");
    printf("输入 'exit' 或 'quit' 退出\n");
    printf("输入 'clear' 清除全局代码\n");
    printf("输入 'show' 显示当前全局代码\n\n");
}

void print_prompt() {
    printf("C>>> ");
    fflush(stdout);
}

void print_help() {
    printf("\n帮助信息:\n");
    printf("- 可以输入单行C语句，如: printf(\"Hello\\n\");\n");
    printf("- 可以定义变量，如: int x = 10;\n");
    printf("- 可以定义函数，如: int add(int a, int b) { return a + b; }\n");
    printf("- 表达式会自动用printf输出，如: 1 + 2\n");
    printf("- 使用 'clear' 清除所有全局定义\n");
    printf("- 使用 'show' 查看当前全局代码\n");
    printf("- 使用 'exit' 或 'quit' 退出\n\n");
}

int is_declaration(const char* code) {
    // 简单检查是否是声明语句
    char* trimmed = strdup(code);
    
    // 去除前导空格
    while (*trimmed == ' ' || *trimmed == '\t') {
        trimmed++;
    }
    
    // 检查是否是类型声明
    if (strncmp(trimmed, "int ", 4) == 0 ||
        strncmp(trimmed, "float ", 6) == 0 ||
        strncmp(trimmed, "double ", 7) == 0 ||
        strncmp(trimmed, "char ", 5) == 0 ||
        strncmp(trimmed, "void ", 5) == 0) {
        
        // 检查是否是函数定义（包含花括号）
        if (strchr(trimmed, '{') != NULL) {
            return 1; // 函数定义
        }
        
        // 检查是否是变量声明（以分号结尾）
        if (strchr(trimmed, ';') != NULL) {
            return 1; // 变量声明
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
    
    // 写入临时文件
    temp_file = fopen(TEMP_FILE_NAME, "w");
    if (temp_file == NULL) {
        perror("无法创建临时文件");
        return -1;
    }
    
    fprintf(temp_file, "%s", code);
    fclose(temp_file);
    
    // 编译代码
    char compile_cmd[256];
    snprintf(compile_cmd, sizeof(compile_cmd), "gcc -o %s %s 2>&1", TEMP_EXEC_NAME, TEMP_FILE_NAME);
    
    compile_result = system(compile_cmd);
    
    if (compile_result != 0) {
        printf("编译错误\n");
        return -1;
    }
    
    // 运行程序
    char run_cmd[256];
    snprintf(run_cmd, sizeof(run_cmd), "./%s", TEMP_EXEC_NAME);
    
    run_result = system(run_cmd);
    
    return run_result;
}

void cleanup_files() {
    remove(TEMP_FILE_NAME);
    remove(TEMP_EXEC_NAME);
}
