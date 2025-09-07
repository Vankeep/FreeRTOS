```cpp
#include <Arduino.h>
#include <STM32FreeRTOS.h>

#define LED_PIN    PC13
#define BUTTON_PIN PA1

// Объявляем семафор глобально, чтобы был виден и задаче, и прерыванию
SemaphoreHandle_t xButtonClickSemaphore;

// Обработчик прерывания
void buttonISR() {
    // 1. Объявляем "сигнальную лампочку"
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;            
    // 2. Кричим системе: "Посылка на складе!" (даем семафор)
    xSemaphoreGiveFromISR(xButtonClickSemaphore, &xHigherPriorityTaskWoken); 
    // 3. После того как "прокричали", проверяем, нужно ли
    //    немедленно будить задачу-курьера?
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Задача обработки нажатия
void buttonTask(void* pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  while(1) {
    if(xSemaphoreTake(xButtonClickSemaphore, portMAX_DELAY) == pdTRUE) {
      // Обработка нажатия с защитой от дребезга
      vTaskDelay(pdMS_TO_TICKS(50)); // Задержка для антидребезга
      
      if(digitalRead(BUTTON_PIN) == LOW) { // Проверяем состояние после задержки
        digitalToggle(LED_PIN); // Инвертируем состояние светодиода
      }
    }
  }
}

void setup() {
    // Создание бинарного семафора
    xButtonClickSemaphore = xSemaphoreCreateBinary();
    // Настройка прерывания на falling edge (кнопка нажата -> LOW)
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
    // Создание задачи с приоритетом выше обычного
    xTaskCreate(buttonTask, "Button", 128, NULL, 2, NULL);
    // Запуск планировщика
    vTaskStartScheduler();
}

void loop() {}
```