#include "Chorus.h"

// Taxa de amostragem do seu sistema (ex: 44.1kHz)
#define SAMPLE_RATE 44100

Chorus chorus(SAMPLE_RATE, 0.5, 0.7, 15.0, 0.5);

void process_audio(float* input, float* output, uint32_t block_size) {
    for (uint32_t i = 0; i < block_size; i++) {
        output[i] = chorus.process(input[i]);
    }
}

// Exemplo de ajuste de parâmetros em tempo real
void set_effect_parameters(float rate, float depth, float delay_time, float mix) {
    chorus.set_rate(rate);
    chorus.set_depth(depth);
    chorus.set_delay_time(delay_time);
    chorus.set_mix(mix);
}