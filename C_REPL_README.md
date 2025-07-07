# C 语言交互式终端

这是一个类似于 Python 交互式终端的 C 语言 REPL（Read-Eval-Print Loop）系统。

## 功能特性

- **交互式执行**: 输入 C 代码并立即看到结果
- **全局变量和函数**: 支持定义全局变量和函数，可在后续代码中使用
- **表达式计算**: 自动计算数学表达式
- **彩色输出**: 不同类型的消息以不同颜色显示
- **历史记录**: 记录和显示之前输入的命令
- **跨平台**: 支持 Windows 和 Linux/Unix 系统

## 编译和运行

### Windows
```bash
gcc -o c_repl_advanced.exe c_repl_advanced.c
c_repl_advanced.exe
```

或者直接运行：
```bash
build_c_repl.bat
```

### Linux/Unix
```bash
gcc -o c_repl_advanced c_repl_advanced.c
./c_repl_advanced
```

## 使用示例

```
C>>> int x = 5;
已添加到全局代码

C>>> printf("x = %d\n", x);
x = 5

C>>> int add(int a, int b) { return a + b; }
已添加到全局代码

C>>> printf("5 + 3 = %d\n", add(5, 3));
5 + 3 = 8

C>>> 2 * 3 + 4
结果: 10

C>>> float pi = 3.14159;
已添加到全局代码

C>>> printf("圆周率: %.2f\n", pi);
圆周率: 3.14
```

## 命令说明

- `help` - 显示帮助信息
- `clear` - 清除所有全局定义
- `show` - 显示当前全局代码
- `history` - 查看历史命令
- `exit` 或 `quit` - 退出程序

## 支持的功能

1. **变量声明**: `int x = 10;`、`float y = 3.14;`
2. **函数定义**: `int func(int a) { return a * 2; }`
3. **表达式计算**: `1 + 2 * 3`
4. **函数调用**: `printf("Hello\n");`
5. **控制结构**: `if`、`for`、`while` 等

## 注意事项

- 需要安装 GCC 编译器
- 每次输入的代码都会被编译和执行
- 全局变量和函数在整个会话中保持可用
- 临时文件会在程序退出时自动清理

## 实现原理

1. **代码解析**: 区分声明、表达式和执行语句
2. **代码累积**: 保存全局变量和函数定义
3. **动态编译**: 将输入代码包装成完整的 C 程序
4. **即时执行**: 编译并运行生成的程序
5. **结果显示**: 捕获并显示程序输出

这个系统让你能够像使用 Python 解释器一样交互式地编写和测试 C 代码！
