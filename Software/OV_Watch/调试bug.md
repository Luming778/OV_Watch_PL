### 问题1：Bug Report: HardFault on vTaskDelete / Idle Task uxListRemove

###### 现象

程序执行到 `HardwareInitTask` 末尾的 `vTaskDelete(NULL)` 后：

1. 最初表现为程序**卡住不动**（停在 `configASSERT` 死循环）
2. 增大栈后变为**HardFault**，Call Stack 如下：
   ```
   HardFault_Handler
   uxListRemove
   prvCheckTasksWaitingTermination
   prvIdleTask
   ```

###### 直接原因

`HardwareInitTask` **栈溢出（Stack Overflow）**，溢出部分覆盖了该任务的 TCB（Task Control Block）中的链表指针。当 `vTaskDelete(NULL)` 被调用后，**Idle 任务**负责回收已删除任务的内存；在 `prvCheckTasksWaitingTermination` -> `uxListRemove` 时访问了被损坏的链表节点，触发 HardFault。

###### 为什么栈会溢出

- `HardwareInitTask` 原栈大小仅 `128 * 10 = 1280` 字节
- 该任务内部执行了大量深层调用：
  - `LCD_Init()` / `LCD_Fill()` / `LCD_ShowString()`
  - `lv_init()`
  - `lv_port_disp_init()`
  - `lv_port_indev_init()`
  - `ui_init()`
- 这些函数层层嵌套，加上局部变量，远超 1280 字节栈空间
- 同时 `LvHandlerTask` 栈也仅有 1280 字节，跑 `lv_task_handler()` 同样存在溢出风险

##### 修复措施

###### 1. 增大任务栈大小

**`user_TasksInit.c`**

```c
// HardwareInitTask: 1280 bytes -> 5120 bytes
const osThreadAttr_t HardwareInitTask_attributes = {
  .name = "HardwareInitTask",
  .stack_size = 128 * 40,
  .priority = (osPriority_t) osPriorityNormal+1,
};

// LvHandlerTask: 1280 bytes -> 3840 bytes
const osThreadAttr_t LvHandlerTask_attributes = {
  .name = "LvHandlerTask",
  .stack_size = 128 * 30,
  .priority = (osPriority_t) osPriorityNormal,
};
```

###### 2. 增大 FreeRTOS 堆大小

**`FreeRTOSConfig.h`**

```c
// 15KB -> 32KB（LVGL 需要大量动态内存）
#define configTOTAL_HEAP_SIZE  ((size_t)32768)
```

###### 3. 启用栈溢出检测

**`FreeRTOSConfig.h`**

```c
#define configCHECK_FOR_STACK_OVERFLOW  2
```

启用后，若发生栈溢出会进入 `vApplicationStackOverflowHook`，可在此处打断点快速定位问题任务。

###### 4. 建议：添加栈溢出 Hook（可选）

在 `freertos.c` 中添加：

```c
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for(;;); // 在此打断点，查看 pcTaskName 定位爆栈任务
}
```

##### 调试复盘

| 阶段   | 现象                          | 分析                                                                        |
| ------ | ----------------------------- | --------------------------------------------------------------------------- |
| 第一次 | `configASSERT` 死循环       | TCB 被溢出数据破坏，`ucStaticallyAllocated` 字段非法，断言失败            |
| 第二次 | HardFault 在 `uxListRemove` | 增大栈后不再断言，但之前溢出已导致链表节点损坏，Idle 任务清理时访问非法地址 |

##### 总结

FreeRTOS 任务栈溢出不会立刻报错，而是**静默破坏相邻内存**（通常是 TCB 或堆管理结构），症状可能在完全无关的地方（如 `vTaskDelete`、`Idle 任务`、`uxListRemove`）才暴露出来。因此：

- 涉及 LVGL、文件系统、网络等复杂初始化时，任务栈建议至少 **4KB ~ 8KB**
- 务必开启 `configCHECK_FOR_STACK_OVERFLOW`
- 利用 `uxTaskGetStackHighWaterMark()` 监控各任务实际栈使用峰值

### 问题2：
