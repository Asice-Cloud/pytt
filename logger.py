import inspect
import datetime
from typing import Any, Optional
from pathlib import Path

# 日志级别颜色映射
LOG_COLORS = {
    "DEBUG": "\033[36m",     # 青色
    "INFO": "\033[32m",      # 绿色
    "WARNING": "\033[33m",   # 黄色
    "ERROR": "\033[31m",     # 红色
    "CRITICAL": "\033[41;97m" # 白底红字
}
RESET_COLOR = "\033[0m"


class Logger:
    
    def __init__(self, name: str = "pytt"):
        self.name = name
    
    def _get_caller_info(self) -> tuple[str, str, int]:
        """获取调用者的文件名、函数名和行号"""
        frame = inspect.currentframe()
        try:
            # 获取调用日志方法的帧（跳过当前帧和_log方法的帧）
            caller_frame = frame.f_back.f_back.f_back  # 跳过三层：当前->_log->日志方法->实际调用者
            filename = Path(caller_frame.f_code.co_filename).name
            function_name = caller_frame.f_code.co_name
            line_number = caller_frame.f_lineno
            return filename, function_name, line_number
        finally:
            del frame
    
    def _log(self, level: str, message: Any) -> None:
        """内部日志方法"""
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        filename, function_name, line_number = self._get_caller_info()
        color = LOG_COLORS.get(level, "")
        reset = RESET_COLOR if color else ""
        
        # 格式化输出
        log_message = f"{color}[{timestamp}] [{level}] [{filename}:{function_name}:{line_number}] {message}{reset}"
        print(log_message)
    
    def info(self, message: Any) -> None:
        self._log("INFO", message)
    
    def debug(self, message: Any) -> None:
        self._log("DEBUG", message)
    
    def warning(self, message: Any) -> None:
        self._log("WARNING", message)
    
    def error(self, message: Any) -> None:
        self._log("ERROR", message)
    
    def critical(self, message: Any) -> None:
        self._log("CRITICAL", message)


# 创建全局日志实例
logger = Logger()

# 导出常用的日志函数，方便直接使用
info = logger.info
debug = logger.debug
warning = logger.warning
error = logger.error
critical = logger.critical
