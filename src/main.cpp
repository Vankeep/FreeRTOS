#include <Arduino.h>
#include <STM32FreeRTOS.h>

#define LED PC13

HardwareSerial uartPort(PA10, PA9);

// Дескриптор (указатель) бинарного семафора - будет создан в setup()
SemaphoreHandle_t xBibarySemaphore;


// Две задачи, обменивающиеся данными через семафор
// Задача-отправитель (активируется по Serial)
void vSenderTask(void *pdParam){
  while (1){
    if(uartPort.available()){
      char c = uartPort.read();
      if(c == '1'){
        // Отправляем семафор - делаем его доступным
        xSemaphoreGive(xBibarySemaphore);
        uartPort.println("vSenderTask: semafore geived");
      }
    }
    // Задержка 100 мс для уменьшения нагрузки на процессор
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// Задача-получатель (реагирует на семафор)
void vReceiverTask(void *pdParam){
  while (1){
    // Пытаемся получить семафор в течение 1000 мс (макс. время ожидания)
    // xSemaphoreTake вернет pdTRUE, если семафор получен успешно
    if(xSemaphoreTake(xBibarySemaphore, pdMS_TO_TICKS(1000)) == pdTRUE){
      uartPort.println("vReceiverTask: semafore taked. Switch led.");
      digitalWrite(LED, !digitalRead(LED));
    } else {
      uartPort.println("vReceiverTask: timeout 1 sec");
    }
  }
  
}

void setup(){
  pinMode(LED, OUTPUT);
  
  uartPort.begin(1200);
  while (!uartPort){}

  xBibarySemaphore = xSemaphoreCreateBinary();
  if(xBibarySemaphore != NULL){
    xTaskCreate(vSenderTask, "sender", 128, NULL, 1, NULL);
    xTaskCreate(vReceiverTask, "receiver", 128, NULL, 2, NULL);

    vTaskStartScheduler();
  }

  while (1){
    uartPort.println("err init xBibarySemaphore");
    delay(1000);
  }
  
  
}

void loop() {}