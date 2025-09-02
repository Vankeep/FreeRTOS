#include <Arduino.h>
#include <STM32FreeRTOS.h>

// Дескрипторы задач для отслеживания
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

HardwareSerial uartPort(PA10, PA9);

// Задача 1 - с большим стеком
void task1(void *pvParameters) {
  while (1) {
    // Имитация работы с использованием стека
    volatile uint32_t temp[20];
    for (int i = 0; i < 20; i++) {
      temp[i] = i * 100;
    }
    
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// Задача 2 - с маленьким стеком
void task2(void *pvParameters) {
  while (1) {
    // Имитация работы с минимальным использованием стека
    volatile uint32_t temp = 0;
    temp++;
    
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// Задача мониторинга памяти
void monitorTask(void *pvParameters) {
  while (1) {
    uartPort.println("\n--- Memory Usage Report ---");
    
    // Информация о задаче 1
    if (task1Handle != NULL) {
      UBaseType_t stack1 = uxTaskGetStackHighWaterMark(task1Handle);
      uartPort.print("Task 1 Stack: ");
      uartPort.print(stack1 * sizeof(StackType_t));
      uartPort.print(" bytes free (");
      uartPort.print((float)stack1 / (1024 / sizeof(StackType_t)), 2);
      uartPort.println("KB)");
    }
    
    // Информация о задаче 2
    if (task2Handle != NULL) {
      UBaseType_t stack2 = uxTaskGetStackHighWaterMark(task2Handle);
      uartPort.print("Task 2 Stack: ");
      uartPort.print(stack2 * sizeof(StackType_t));
      uartPort.print(" bytes free (");
      uartPort.print((float)stack2 / (1024 / sizeof(StackType_t)), 2);
      uartPort.println("KB)");
    }
    
    // Информация о куче FreeRTOS
    size_t heapFree = xPortGetFreeHeapSize();
    uartPort.print("FreeRTOS Heap: ");
    uartPort.print(heapFree);
    uartPort.println(" bytes free");
    
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void setup() {
  uartPort.begin(115200);
  while (!uartPort);
  
  uartPort.println("FreeRTOS Memory Monitor Demo");
  uartPort.println("----------------------------");
  
  // Создаем задачи с разными размерами стека
  xTaskCreate(
    task1,             // Функция задачи
    "Task1",           // Имя задачи
    1024,              // Размер стека (в словах)
    NULL,              // Параметры
    1,                 // Приоритет
    &task1Handle       // Дескриптор задачи
  );
  
  xTaskCreate(
    task2,             // Функция задачи
    "Task2",           // Имя задачи
    128,               // Размер стека (в словах)
    NULL,              // Параметры
    1,                 // Приоритет
    &task2Handle       // Дескриптор задачи
  );
  
  // Задача мониторинга
  xTaskCreate(
    monitorTask,
    "Monitor",
    512,
    NULL,
    2,                 // Более высокий приоритет
    NULL
  );
  
  vTaskStartScheduler();
}

void loop() {
  // Пустой цикл - управление передано FreeRTOS
}