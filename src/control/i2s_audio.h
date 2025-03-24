#ifndef I2S_AUDIO_H
#define I2S_AUDIO_H

#include "driver/i2s.h"
#include <Arduino.h>

// Configurações I2S
#define I2S_SAMPLE_RATE     (44100)
#define I2S_SAMPLE_BITS     (16)
#define I2S_CHANNEL_NUM     (2)
#define I2S_READ_LEN        (16 * 1024)
#define I2S_BUFFER_LEN      (I2S_READ_LEN / 8)

class I2SAudio {
public:
    // Inicializa as interfaces I2S
    static void begin(int sck_pin, int ws_pin, int din_pin, int dout_pin) {
        // Armazena os pinos
        _sck_pin = sck_pin;
        _ws_pin = ws_pin;
        _din_pin = din_pin;
        _dout_pin = dout_pin;

        // Configuração para recepção
        i2s_config_t i2s_read_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = I2S_SAMPLE_RATE,
            .bits_per_sample = (i2s_bits_per_sample_t)I2S_SAMPLE_BITS,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = I2S_BUFFER_LEN,
            .use_apll = false,
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0
        };

        i2s_pin_config_t read_pin_config = {
            .bck_io_num = _sck_pin,
            .ws_io_num = _ws_pin,
            .data_in_num = _din_pin,
            .data_out_num = I2S_PIN_NO_CHANGE
        };

        i2s_driver_install(I2S_NUM_0, &i2s_read_config, 0, NULL);
        i2s_set_pin(I2S_NUM_0, &read_pin_config);

        // Configuração para transmissão
        i2s_config_t i2s_write_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = I2S_SAMPLE_RATE,
            .bits_per_sample = (i2s_bits_per_sample_t)I2S_SAMPLE_BITS,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = I2S_BUFFER_LEN,
            .use_apll = false,
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0
        };

        i2s_pin_config_t write_pin_config = {
            .bck_io_num = _sck_pin,
            .ws_io_num = _ws_pin,
            .data_in_num = I2S_PIN_NO_CHANGE,
            .data_out_num = _dout_pin
        };

        i2s_driver_install(I2S_NUM_1, &i2s_write_config, 0, NULL);
        i2s_set_pin(I2S_NUM_1, &write_pin_config);
    }

    // Função para ler dados de áudio
    static size_t readAudio(uint8_t *buffer, size_t buffer_size) {
        size_t bytes_read = 0;
        i2s_read(I2S_NUM_0, (void*)buffer, buffer_size, &bytes_read, portMAX_DELAY);
        return bytes_read;
    }

    // Função para escrever dados de áudio
    static size_t writeAudio(uint8_t *buffer, size_t buffer_size) {
        size_t bytes_written = 0;
        i2s_write(I2S_NUM_1, (void*)buffer, buffer_size, &bytes_written, portMAX_DELAY);
        return bytes_written;
    }

private:
    static int _sck_pin;
    static int _ws_pin;
    static int _din_pin;
    static int _dout_pin;
};

// Inicialização das variáveis estáticas
int I2SAudio::_sck_pin = 0;
int I2SAudio::_ws_pin = 0;
int I2SAudio::_din_pin = 0;
int I2SAudio::_dout_pin = 0;

#endif