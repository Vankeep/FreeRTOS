#include <Arduino.h>
#include <STM32FreeRTOS.h>

#define LED PC13

HardwareSerial uartPort(PA10, PA9);

SemaphoreHandle_t xMutex;

// Задача 1: отправка каждые 300 msec
void xTask1(void *pvParam){
  while (1){
    // Захватываем мьютекс xSemaphoreTake. portMAX_DELAY (ждем бесконечно)
    // portMAX_DELAY это - Я готов ждать столько, сколько потребуется. Не возвращай управление моей задаче, пока я не получу то, что жду. Тем временем ты можешь выполнять другие задачи.
    // Используя portMAX_DELAY, вы должны быть уверены, что рано или поздно событие произойдет 
    //    (мьютекс освободится, данные поступят в очередь). 
    //    Иначе задача заблокируется навсегда, что может привести к "зависанию" всей системы.
    if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE){
      uartPort.println("Task 1: Hello from Task 1!");
      xSemaphoreGive(xMutex); // Освобождаем мьютекс
    }
    vTaskDelay(pdMS_TO_TICKS(300)); // Ждем 300 мсек
  }
}

// Задача 2: отправка каждые 700 msec
void xTask2(void *pvParam){
  while (1){
    if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE){
      uartPort.println("Task 2: Message from Task 2!");
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(700)); // Ждем 700 мсек
  }
}

void setup(){
  uartPort.begin(1200);

  xMutex = xSemaphoreCreateMutex();
  
  if(xMutex != NULL){
    xTaskCreate(xTask1, "t1", 128, NULL, 1, NULL);
    xTaskCreate(xTask2, "t2", 128, NULL, 1, NULL);

    vTaskStartScheduler();
  } 

  // Если планировщик не запустился, мигаем светодиодом быстро
  pinMode(PC13, OUTPUT);
  while(1) {
    digitalToggle(PC13);
    delay(100);
  }
  
}

void loop() {}