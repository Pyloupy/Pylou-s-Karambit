#include <Arduino.h>
#include <mic.h>

mic_config_t mic_config{
  .channel_cnt = 1,
  .sampling_rate = 16000,
  .buf_size = 1600,
  .debug_pin = LED_BUILTIN
};

NRF52840_ADC_Class Mic(&mic_config);

#define AUDIO_BUF_SIZE 1600

// two temps buffer of 1600 size
uint16_t buffer_ping[AUDIO_BUF_SIZE];
uint16_t buffer_pong[AUDIO_BUF_SIZE];

volatile bool ping_ready = false;
volatile bool pong_ready = false;
volatile int current_buffer = 0;   // 0 = Fill Ping, 1 = Fill Pong
volatile uint32_t buffer_idx = 0;  // Index to fill everything and not loose data

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Mic.set_callback(audio_rec_callback);

  if (!Mic.begin()) {
    while (1);
  }
}

void loop() {
  // Loop is doing the job of sending data to the computer
  if (ping_ready) {
    Serial.write((uint8_t*)buffer_ping, AUDIO_BUF_SIZE * sizeof(uint16_t));
    ping_ready = false;
  }

  if (pong_ready) {
    Serial.write((uint8_t*)buffer_pong, AUDIO_BUF_SIZE * sizeof(uint16_t));
    pong_ready = false;
  }
}

// Callback is recording every packet of recording
static void audio_rec_callback(uint16_t *buf, uint32_t buf_len) {
  for (uint32_t i = 0; i < buf_len; i++) {
    
    if (current_buffer == 0) {
      // Filling the Ping buffer
      buffer_ping[buffer_idx++] = buf[i];
      if (buffer_idx >= AUDIO_BUF_SIZE) {
        ping_ready = true;     // Ping is full, we'll send it in the loop
        current_buffer = 1;    // Switch to Pong
        buffer_idx = 0;        // Restart from 0
      }
    } 
    else {
      // Filling the Pong buffer
      buffer_pong[buffer_idx++] = buf[i];
      if (buffer_idx >= AUDIO_BUF_SIZE) {
        pong_ready = true;     // Pong is full
        current_buffer = 0;    // Switch to Ping
        buffer_idx = 0;        // Restart from 0
      }
    }
    
  }
}