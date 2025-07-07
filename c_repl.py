#!/usr/bin/env python3
"""
Python 实现的 C 语言交互式终端
更容易理解和扩展的版本
"""

import subprocess
import os
import tempfile
import sys
from pathlib import Path

class CInteractiveShell:
    def __init__(self):
        self.global_code = ""
        self.includes = "#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <math.h>\n\n"
        self.history = []
        self.temp_dir = tempfile.mkdtemp()
        
    def print_welcome(self):
        print("=" * 50)
        print("     C 语言交互式终端 (Python 版本)")
        print("=" * 50)
        print("输入 'help' 查看帮助")
        print("输入 'exit' 或 'quit' 退出")
        print("输入 'clear' 清除全局代码")
        print("输入 'show' 显示当前全局代码")
        print("输入 'history' 查看历史命令")
        print()
    
    def print_help(self):
        help_text = """
帮助信息:
========================================
基本用法:
- 单行C语句: printf("Hello World\\n");
- 变量定义: int x = 10;
- 函数定义: int add(int a, int b) { return a + b; }
- 数学表达式: 1 + 2 * 3
- 调用函数: add(5, 3);

命令:
- help     显示此帮助
- clear    清除所有全局定义
- show     查看当前全局代码
- history  查看历史命令
- exit/quit 退出程序

示例:
C>>> int x = 5;
C>>> printf("x = %d\\n", x);
C>>> int factorial(int n) { return n <= 1 ? 1 : n * factorial(n-1); }
C>>> printf("5! = %d\\n", factorial(5));
========================================
"""
        print(help_text)
    
    def is_declaration(self, code):
        """检查是否是声明语句"""
        code = code.strip()
        type_keywords = ['int', 'float', 'double', 'char', 'void', 'long', 'short', 'unsigned']
        
        for keyword in type_keywords:
            if code.startswith(keyword + ' '):
                # 函数定义或变量声明
                if '{' in code or ';' in code:
                    return True
        return False
    
    def is_expression(self, code):
        """检查是否是数学表达式"""
        code = code.strip()
        operators = ['+', '-', '*', '/', '%']
        
        if any(op in code for op in operators):
            if ';' not in code and '(' not in code:
                return True
        return False
    
    def compile_and_run(self, code):
        """编译并运行C代码"""
        try:
            # 创建临时文件
            c_file = os.path.join(self.temp_dir, 'temp.c')
            exe_file = os.path.join(self.temp_dir, 'temp.exe')
            
            # 写入C代码
            with open(c_file, 'w', encoding='utf-8') as f:
                f.write(code)
            
            # 编译
            compile_cmd = ['gcc', '-o', exe_file, c_file]
            compile_result = subprocess.run(
                compile_cmd, 
                capture_output=True, 
                text=True
            )
            
            if compile_result.returncode != 0:
                print(f"编译错误: {compile_result.stderr}")
                return False
            
            # 运行
            run_result = subprocess.run(
                [exe_file], 
                capture_output=True, 
                text=True
            )
            
            if run_result.stdout:
                print(run_result.stdout, end='')
            if run_result.stderr:
                print(f"运行时错误: {run_result.stderr}")
            
            return True
            
        except Exception as e:
            print(f"执行错误: {e}")
            return False
    
    def handle_expression(self, expr):
        """处理数学表达式"""
        code = f"""
{self.includes}
{self.global_code}
int main() {{
    printf("结果: %g\\n", (double)({expr}));
    return 0;
}}
"""
        self.compile_and_run(code)
    
    def add_to_global(self, code):
        """添加到全局代码"""
        self.global_code += code + "\n"
        print("已添加到全局代码")
    
    def run(self):
        """运行交互式终端"""
        self.print_welcome()
        
        while True:
            try:
                user_input = input("C>>> ").strip()
                
                if not user_input:
                    continue
                
                # 处理命令
                if user_input in ['exit', 'quit']:
                    print("再见！")
                    break
                elif user_input == 'help':
                    self.print_help()
                    continue
                elif user_input == 'clear':
                    self.global_code = ""
                    print("全局代码已清除")
                    continue
                elif user_input == 'show':
                    print("当前全局代码:")
                    print(self.global_code)
                    continue
                elif user_input == 'history':
                    print("历史命令:")
                    for i, cmd in enumerate(self.history, 1):
                        print(f"{i}: {cmd}")
                    continue
                
                # 添加到历史
                self.history.append(user_input)
                
                # 处理代码
                if self.is_expression(user_input):
                    self.handle_expression(user_input)
                elif self.is_declaration(user_input):
                    self.add_to_global(user_input)
                else:
                    # 普通语句
                    code = f"""
{self.includes}
{self.global_code}
int main() {{
    {user_input}
    return 0;
}}
"""
                    self.compile_and_run(code)
                    
            except KeyboardInterrupt:
                print("\n再见！")
                break
            except EOFError:
                print("\n再见！")
                break
    
    def cleanup(self):
        """清理临时文件"""
        import shutil
        try:
            shutil.rmtree(self.temp_dir)
        except:
            pass

def main():
    shell = CInteractiveShell()
    try:
        shell.run()
    finally:
        shell.cleanup()

if __name__ == "__main__":
    main()
