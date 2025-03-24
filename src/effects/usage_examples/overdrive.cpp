#include "Overdrive.h"

// Taxa de amostragem do seu sistema (ex: 44.1kHz)
#define SAMPLE_RATE 44100

Overdrive overdrive(SAMPLE_RATE, 3.0, 0.8, 0.5);

void process_audio(float* input, float* output, uint32_t block_size) {
    for (uint32_t i = 0; i < block_size; i++) {
        output[i] = overdrive.process(input[i]);
    }
}

// Exemplo de ajuste de parâmetros em tempo real
void set_effect_parameters(float drive, float mix, float tone) {
    overdrive.set_drive(drive);
    overdrive.set_mix(mix);
    overdrive.set_tone(tone);
}