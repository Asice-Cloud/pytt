from collections.abc import Callable
from typing import Any
from logger import info, debug, warning, error, critical

def get_place(func: Callable[[], Any]) -> Any:
    info("正在执行函数调用")
    result = func()
    info(f"函数执行完成，返回值: {result}")
    return result

def example_function() -> str:
    debug("这是一个示例函数")
    return "示例结果"

def main() -> None:
    info("程序开始执行")
    
    # 演示不同级别的日志
    info("这是一条信息日志")
    debug("这是一条调试日志")
    warning("这是一条警告日志")
    error("这是一条错误日志")
    critical("这是一条严重错误日志")
    
    # 演示在函数调用中使用日志
    result = get_place(example_function)
    info(f"最终结果: {result}")
    
    info("程序执行完成")


if __name__ == "__main__":
    main()
