/**
 * Código a ser embarcado no ESP32
 */

#include <Constants.h>
#include <Menu.h>
#include <i2s_audio.h>

uint8_t audioBuffer[I2S_BUFFER_LEN];
float input[BLOCK_SIZE];
float output[BLOCK_SIZE];

Menu system = Menu();

// Funções de interrupção (navegação de menu)
void LEFT_PRESS() {
    system.navigate_left();
}

void RIGHT_PRESS() {
    system.navigate_right();
}

void OK_PRESS() {
    system.navigate_ok();
}

void setup() {
    // Ativando interrupções
    attachInterrupt(BNT_LEFT, LEFT_PRESS, INTERRUPT_MODE);
    attachInterrupt(BNT_RIGHT, RIGHT_PRESS, INTERRUPT_MODE);
    attachInterrupt(BNT_OK, OK_PRESS, INTERRUPT_MODE);

    I2SAudio::begin(I2S_SCK_PIN, I2S_WS_PIN, I2S_DIN_PIN, I2S_DOUT_PIN)
}

void loop() {
    size_t bytesRead = I2SAudio::readAudio(audioBuffer, sizeof(audioBuffer));

    if (bytesRead > 0) {
        system.process_audio(audioBuffer, output, BLOCK_SIZE);

        I2SAudio::writeAudio(output, bytesRead);
    }
    
    system.read_knob(KNOB_PIN);
}