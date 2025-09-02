#include <Arduino.h>
#include <STM32FreeRTOS.h>

// Пины светодиодов
const uint8_t LED1_PIN = PA6;
const uint8_t LED2_PIN = PA4;
const uint8_t LED3_PIN = PA5;

// ==================== ТИПЫ ДАННЫХ ====================
enum LedCommand {
  LED1_ON,    // Включить светодиод 1
  LED2_ON,    // Включить светодиод 2
  LED3_ON,    // Включить светодиод 3
  ALL_OFF,    // Выключить все светодиоды
  INVALID_CMD // Неверная команда
};

HardwareSerial uartPort(PA10, PA9);
QueueHandle_t ledCommandQueue;

// ==================== ЗАДАЧИ FREE_RTOS ====================
void uartSenderTask(void *pvParameters) {
  const int MAX_BUFFER_SIZE = 32; // Максимальный размер буфера для принимаемой строки
  char buffer[MAX_BUFFER_SIZE];   // Буфер для принимаемой строки
  int bufferIndex = 0;            // Индекс в буфере
  
  while (1) {
    if (uartPort.available()) {
      char input = uartPort.read();
      
      // Если получен символ новой строки или возврата каретки, обрабатываем команду
      if (input == '\n' || input == '\r') {
        // Завершаем строку
        buffer[bufferIndex] = '\0';
        
        LedCommand command = LedCommand::INVALID_CMD;
        if(strcmp(buffer, "один") == 0) command = LedCommand::LED1_ON;
        else if(strcmp(buffer, "два") == 0) command = LedCommand::LED2_ON;
        else if(strcmp(buffer, "три") == 0) command = LedCommand::LED3_ON;
        else if(strcmp(buffer, "выкл") == 0) command = LedCommand::ALL_OFF;
        
        BaseType_t queueStatus = xQueueSend(ledCommandQueue, &command, portMAX_DELAY);
        
        // Сбрасываем индекс буфера для следующей команды
        bufferIndex = 0;
      } 
      // Если буфер не заполнен и получен обычный символ, добавляем его в буфер
      else if (bufferIndex < MAX_BUFFER_SIZE - 1) {
        buffer[bufferIndex++] = input;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // Уменьшим задержку для более быстрой реакции
  }
}

void ledControllerTask(void *pvParameters) {
  LedCommand receivedCommand;
  
  while (1) {
    if (xQueueReceive(ledCommandQueue, &receivedCommand, portMAX_DELAY) == pdPASS) {
      // Выключаем все светодиоды перед выполнением команды
      digitalWrite(LED1_PIN, HIGH);
      digitalWrite(LED2_PIN, HIGH);
      digitalWrite(LED3_PIN, HIGH);
  
      switch (receivedCommand) {
        case LED1_ON:
          digitalWrite(LED1_PIN, LOW);
          uartPort.println("LED1 activated");
          break;
        case LED2_ON:
          digitalWrite(LED2_PIN, LOW);
          uartPort.println("LED2 activated");
          break;
        case LED3_ON:
          digitalWrite(LED3_PIN, LOW);
          uartPort.println("LED3 activated");
          break;
        case ALL_OFF:
          uartPort.println("All LEDs deactivated");
          break;
        case INVALID_CMD:
          uartPort.println("Error: Invalid command received");
          // Индикация ошибки - быстрое мигание всеми светодиодами
          for (uint8_t i = 0; i < 3; i++){
            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, HIGH);
            digitalWrite(LED3_PIN, HIGH);
            vTaskDelay(pdMS_TO_TICKS(50));
            digitalWrite(LED1_PIN, HIGH);
            digitalWrite(LED2_PIN, LOW);
            digitalWrite(LED3_PIN, HIGH);
            vTaskDelay(pdMS_TO_TICKS(50));
            digitalWrite(LED1_PIN, HIGH);
            digitalWrite(LED2_PIN, HIGH);
            digitalWrite(LED3_PIN, LOW);
            vTaskDelay(pdMS_TO_TICKS(50));
          }
          digitalWrite(LED1_PIN, HIGH);
          digitalWrite(LED2_PIN, HIGH);
          digitalWrite(LED3_PIN, HIGH);
          break;
      }
    }
  }
}

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  
  // Выключаем все светодиоды (HIGH = выключено для данной схемы)
  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, HIGH);
  digitalWrite(LED3_PIN, HIGH);
  
  uartPort.begin(9600);
  uartPort.println("LED Control System Started");
  uartPort.println("Send '1', '2' or '3' to control LEDs");
  uartPort.println("Invalid commands will trigger error indication");
  
  ledCommandQueue = xQueueCreate(5, sizeof(LedCommand));
  
  if (ledCommandQueue != NULL) {
    xTaskCreate(uartSenderTask, "UART Sender", 128, NULL, 1, NULL);
    xTaskCreate(ledControllerTask, "LED Controller", 128, NULL, 1, NULL);
    
    vTaskStartScheduler();
  } else {
    
    // Индикация ошибки - ledCommandQueue не инициализировалась
    while (1) {
      uartPort.println("Error: Failed to create command queue!");
      delay(1000);
    }
  }
}

void loop() {}