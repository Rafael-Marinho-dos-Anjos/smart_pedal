/**
 * Código a ser embarcado no ESP32
 */

#include <Constants.h>
#include <Menu.h>
#include <i2s_audio.h>
#include <Conversion.h>

uint8_t audioBuffer[I2S_BUFFER_LEN];
float input[I2S_BUFFER_LEN / 4];
float output[I2S_BUFFER_LEN / 4];

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
        i2s_24bit_to_float(audioBuffer, input, bytesRead / 4);
        system.process_audio(input, output, sizeof(input));

        float_to_i2s_24bit(output, audioBuffer, sizeof(output));
        I2SAudio::writeAudio(audioBuffer, bytesRead);
    }
    
    system.read_knob(KNOB_PIN);
}