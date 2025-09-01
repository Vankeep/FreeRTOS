#include <Arduino.h>
#include <STM32FreeRTOS.h>

#define LED PC13

HardwareSerial uartPort(PA10, PA9);

// Функция задачи для мигания LED
void blinkTask(void *pvParameters) {
  
  pinMode(LED, OUTPUT);  // Настроить пин LED как выход

  for (;;) {
    digitalWrite(LED, HIGH);  // Включить LED
    uartPort.println("LED on");
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Задержка 1 секунда
    digitalWrite(LED, LOW);   // Выключить LED
    uartPort.println("LED off");
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Задержка 1 секунда
  }
}
void setup() {
  uartPort.begin(115200); // Инициализация UART
  uartPort.println("Starting FreeRTOS Blink Task");

  // Создать задачу FreeRTOS
  BaseType_t result = xTaskCreate(
    blinkTask,          // Функция задачи
    "Blink",            // Имя задачи (для отладки)
    256,                // Размер стека (в словах)
    NULL,               // Параметры (не используются)
    1,                  // Приоритет (1 - нормальный)
    NULL                // Хэндл задачи (не используется)
  );

  // Проверка успешности создания задачи
  // pdPASS (обычно 1) — успешное выполнение
  // pdFAIL (обычно 0) — ошибка
  // errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY — ошибка выделения памяти
  if (result != pdPASS) {
    // Если задача не создана, можно обработать ошибку (например, включить LED или вывести сообщение)
  }

  // Запустить планировщик FreeRTOS
  vTaskStartScheduler();
}
void loop() {
  // Пустой цикл: все действия в задачах FreeRTOS
}