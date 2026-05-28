# lvgl 学习
运行硬件是 野火 iso 开发板
stm32cubemx 配置项目
make 构建
使用 openocd 和 vscode Cortex-Debug 插件去调试追踪代码

# build
make
# 烧录
make flash
# 智能提示
安装clangd和vscode clangd插件
make clean
bear -- make

# vscode tasks
.vscode/tasks.json 配置了 构建，烧录任务
contrl-shift +B 快捷键 选择对应的任务，执行任务命令

# 调试
* vscode
按 F5

* 命令行
启动 openocd 做 gdb server，默认监听在 3333 端口
```
openocd -f jlink_sw.cfg 
```
另开启一个终端启动gdb
```
arm-none-eabi-gdb build/lvgl-learn.elf
```
在 gdb 交互中输入命令连接 server
```
target  remote localhost:3333
```

gdb 常用命令

复位
```
monitor reset halt
```
烧录
```
load
```

打断点 break main 函数
```
b main
```
列出所有断点
```
info b
```

列出当前代码 list
```
l
```
打印变量值
'''
p defaultTaskHandle
'''
继续运行 continue
```
c
```

