#include <Arduino.h>
#include <STM32FreeRTOS.h>

#define LED PC13

// Функция задачи для мигания LED
void blinkTask(void *pvParameters) {
  for (;;) {
    digitalWrite(LED, HIGH);  // Включить LED
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Задержка 1 секунда
    digitalWrite(LED, LOW);   // Выключить LED
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Задержка 1 секунда
  }
}

void setup() {
  pinMode(LED, OUTPUT);  // Настроить пин LED как выход
  
  // Создать задачу FreeRTOS
  xTaskCreate(
    blinkTask,          // Функция задачи
    "Blink",            // Имя задачи (для отладки)
    128,                // Размер стека (в словах)
    NULL,               // Параметры (не используются)
    1,                  // Приоритет (1 - нормальный)
    NULL                // Хэндл задачи (не используется)
  );
  
  // Запустить планировщик FreeRTOS
  vTaskStartScheduler();
}

void loop() {
  // Пустой цикл: все действия в задачах FreeRTOS
}