#ifndef CONVERSION_H
#define CONVERSION_H

#include <Arduino.h>


/**
 * Converte um buffer I2S (uint8_t) com amostras de 24 bits para float (range [-1.0, 1.0]).
 * @param input Buffer de entrada (4 bytes por amostra, 24 bits válidos alinhados à esquerda).
 * @param output Buffer de saída em float.
 * @param samples Número de amostras a converter.
 */
void i2s_24bit_to_float(const uint8_t* input, float* output, size_t samples) {
    for (size_t i = 0; i < samples; i++) {
        // Combina 4 bytes em um int32_t (24 bits válidos + 8 bits de padding)
        int32_t raw_sample = (int32_t)(
            (input[4*i] << 24) | 
            (input[4*i + 1] << 16) | 
            (input[4*i + 2] << 8)
        ) >> 8; // Remove os 8 bits baixos (alinhamento à esquerda)

        // Normaliza para float entre [-1.0, 1.0]
        output[i] = (float)raw_sample / 8388608.0f; // 2^23 = 8388608
    }
}


/**
 * Converte um buffer float (range [-1.0, 1.0]) para I2S 24-bit (uint8_t).
 * @param input Buffer de entrada em float.
 * @param output Buffer de saída (4 bytes por amostra, 24 bits válidos alinhados à esquerda).
 * @param samples Número de amostras a converter.
 */
void float_to_i2s_24bit(const float* input, uint8_t* output, size_t samples) {
    for (size_t i = 0; i < samples; i++) {
        // Denormaliza para int32_t (24 bits com sinal)
        int32_t raw_sample = (int32_t)(input[i] * 8388608.0f);

        // Alinha os 24 bits à esquerda em 32 bits (8 bits baixos = 0)
        raw_sample = raw_sample << 8;

        // Escreve os 4 bytes no buffer de saída
        output[4*i] = (raw_sample >> 24) & 0xFF;
        output[4*i + 1] = (raw_sample >> 16) & 0xFF;
        output[4*i + 2] = (raw_sample >> 8) & 0xFF;
        output[4*i + 3] = 0x00; // Padding
    }
}

#endif