#include "ThreeBandEQ.h"

// Taxa de amostragem do seu sistema (ex: 44.1kHz)
#define SAMPLE_RATE 44100

ThreeBandEQ eq(SAMPLE_RATE);

void setup() {
    // Configura ganhos (em dB)
    eq.set_low_gain(3.0);    // +3dB nos baixos
    eq.set_mid_gain(-1.0);   // -1dB nos médios
    eq.set_high_gain(2.0);   // +2dB nos agudos
    
    // Opcional: ajustar frequências de corte
    eq.set_low_cutoff(300);  // Baixos até 300Hz
    eq.set_high_cutoff(5000);// Agudos a partir de 5kHz
}

void process_audio(float* input, float* output, uint32_t block_size) {
    for (uint32_t i = 0; i < block_size; i++) {
        output[i] = eq.process(input[i]);
    }
}

// Exemplo de ajuste de parâmetros em tempo real
void set_eq_parameters(float low_gain, float mid_gain, float high_gain) {
    eq.set_low_gain(low_gain);
    eq.set_mid_gain(mid_gain);
    eq.set_high_gain(high_gain);
}