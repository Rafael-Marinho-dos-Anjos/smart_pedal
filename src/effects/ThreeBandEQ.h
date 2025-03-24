#ifndef THREE_BAND_EQ_H
#define THREE_BAND_EQ_H

#include <Arduino.h>
#include <math.h>

class ThreeBandEQ {
public:
    /**
     * @brief Construtor da classe ThreeBandEQ
     * @param sample_rate Taxa de amostragem do áudio (em Hz)
     */
    ThreeBandEQ(uint32_t sample_rate = 44100) : sample_rate_(sample_rate) {
        // Inicializa todos os filtros
        reset_filters();
        
        // Configura frequências de corte padrão
        low_cutoff_ = 250.0f;    // 250Hz divisão entre baixos e médios
        high_cutoff_ = 4000.0f;  // 4kHz divisão entre médios e agudos
        
        // Ganhos iniciais (0dB)
        low_gain_ = 1.0f;
        mid_gain_ = 1.0f;
        high_gain_ = 1.0f;
        
        // Atualiza coeficientes dos filtros
        update_filters();
    }

    const String EFFECT_NAME = "EQUALIZER";

    /**
     * @brief Processa uma amostra de áudio com o equalizador
     * @param input Amostra de entrada (-1.0 a 1.0)
     * @return Amostra processada (-1.0 a 1.0)
     */
    float process(float input) {
        // Filtro passa-baixas (baixos)
        float low = biquad_process(&low_filter_, input);
        
        // Filtro passa-altas (agudos)
        float high = biquad_process(&high_filter_, input);
        
        // Médias = sinal original - baixos - agudos
        float mid = input - low - high;
        
        // Aplica ganhos e soma as bandas
        return (low * low_gain_) + (mid * mid_gain_) + (high * high_gain_);
    }

    string* getParamNames() {
        string * params = ["LOW", "MID", "HIGH", "LOW CUT", "HIGH CUT"];

        return params;
    }

    /**
     * @brief Atualiza o valor de um parâmetro através do index
     * @param param_index Número do parâmetro
     * @param val Valor a ser atribuído ao parâmetro
     */
    void setParam(int param_index, float val) {
        if (param_index == 0) {
            val = -12.0 + val * (12.0 + 12.0);
            set_low_gain(val);
        }
        else if (param_index == 1) {
            val = -12.0 + val * (12.0 + 12.0);
            set_mid_gain(val);
        }
        else if (param_index == 2) {
            val = -12.0 + val * (12.0 + 12.0);
            set_high_gain(val);
        }
        else if (param_index == 3) {
            val = 50.0 + val * (500.0 - 50.0);
            set_low_cutoff(val);
        }
        else if (param_index == 4) {
            val = 2000.0 + val * (8000.0 - 2000.0);
            set_low_cutoff(val);
        }

        update_filters();
    }

    /**
     * @brief Retorna o valor de um parâmetro através do index
     * @param param_index Número do parâmetro
     */
    int getParamValue(int param_index) {
        int val = 100;
        if (param_index == 0) {
            return val * (low_gain_ + 12.0) / (12.0 + 12.0);
        }
        else if (param_index == 1) {
            return val * (mid_gain_ + 12.0) / (12.0 + 12.0);
        }
        else if (param_index == 2) {
            return val * (high_gain_ + 12.0) / (12.0 + 12.0);
        }
        else if (param_index == 3) {
            return val * (low_cutoff_ - 50.0) / (500.0 - 50.0);
        }
        else if (param_index == 4) {
            return val * (high_cutoff_ - 2000.0) / (8000.0 - 2000.0);
        }
    }

    /**
     * @brief Define o ganho para baixos
     * @param gain Ganho em dB (-12 a +12 dB)
     */
    void set_low_gain(float gain_db) {
        low_gain_ = db_to_linear(constrain(gain_db, -12.0f, 12.0f));
    }

    /**
     * @brief Define o ganho para médios
     * @param gain Ganho em dB (-12 a +12 dB)
     */
    void set_mid_gain(float gain_db) {
        mid_gain_ = db_to_linear(constrain(gain_db, -12.0f, 12.0f));
    }

    /**
     * @brief Define o ganho para agudos
     * @param gain Ganho em dB (-12 a +12 dB)
     */
    void set_high_gain(float gain_db) {
        high_gain_ = db_to_linear(constrain(gain_db, -12.0f, 12.0f));
    }

    /**
     * @brief Define a frequência de corte para baixos
     * @param cutoff Frequência em Hz (50 a 500 Hz)
     */
    void set_low_cutoff(float cutoff) {
        low_cutoff_ = constrain(cutoff, 50.0f, 500.0f);
        update_filters();
    }

    /**
     * @brief Define a frequência de corte para agudos
     * @param cutoff Frequência em Hz (2000 a 8000 Hz)
     */
    void set_high_cutoff(float cutoff) {
        high_cutoff_ = constrain(cutoff, 2000.0f, 8000.0f);
        update_filters();
    }

private:
    uint32_t sample_rate_;
    float low_cutoff_;
    float high_cutoff_;
    float low_gain_;
    float mid_gain_;
    float high_gain_;

    // Estrutura para filtros biquad
    struct BiquadFilter {
        float b0, b1, b2, a1, a2;
        float x1, x2, y1, y2;
    };
    
    BiquadFilter low_filter_;  // Filtro passa-baixas para baixos
    BiquadFilter high_filter_; // Filtro passa-altas para agudos

    /**
     * @brief Processa um filtro biquad
     */
    float biquad_process(BiquadFilter* filter, float input) {
        float output = filter->b0 * input + filter->b1 * filter->x1 + filter->b2 * filter->x2
                      - filter->a1 * filter->y1 - filter->a2 * filter->y2;
        
        // Atualiza estados anteriores
        filter->x2 = filter->x1;
        filter->x1 = input;
        filter->y2 = filter->y1;
        filter->y1 = output;
        
        return output;
    }

    /**
     * @brief Reseta todos os filtros
     */
    void reset_filters() {
        memset(&low_filter_, 0, sizeof(BiquadFilter));
        memset(&high_filter_, 0, sizeof(BiquadFilter));
    }

    /**
     * @brief Atualiza os coeficientes dos filtros
     */
    void update_filters() {
        // Calcula coeficientes para o filtro passa-baixas (baixos)
        calculate_lowpass_coeffs(&low_filter_, low_cutoff_, 0.707f);
        
        // Calcula coeficientes para o filtro passa-altas (agudos)
        calculate_highpass_coeffs(&high_filter_, high_cutoff_, 0.707f);
    }

    /**
     * @brief Calcula coeficientes para filtro passa-baixas
     */
    void calculate_lowpass_coeffs(BiquadFilter* filter, float cutoff, float q) {
        float w0 = 2 * PI * cutoff / sample_rate_;
        float alpha = sin(w0) / (2 * q);
        float cos_w0 = cos(w0);
        
        filter->b0 = (1 - cos_w0) / 2;
        filter->b1 = 1 - cos_w0;
        filter->b2 = filter->b0;
        filter->a1 = -2 * cos_w0;
        filter->a2 = 1 - alpha;
        
        // Normaliza coeficientes
        float norm = 1 + alpha;
        filter->b0 /= norm;
        filter->b1 /= norm;
        filter->b2 /= norm;
        filter->a1 /= norm;
        filter->a2 /= norm;
    }

    /**
     * @brief Calcula coeficientes para filtro passa-altas
     */
    void calculate_highpass_coeffs(BiquadFilter* filter, float cutoff, float q) {
        float w0 = 2 * PI * cutoff / sample_rate_;
        float alpha = sin(w0) / (2 * q);
        float cos_w0 = cos(w0);
        
        filter->b0 = (1 + cos_w0) / 2;
        filter->b1 = -(1 + cos_w0);
        filter->b2 = filter->b0;
        filter->a1 = -2 * cos_w0;
        filter->a2 = 1 - alpha;
        
        // Normaliza coeficientes
        float norm = 1 + alpha;
        filter->b0 /= norm;
        filter->b1 /= norm;
        filter->b2 /= norm;
        filter->a1 /= norm;
        filter->a2 /= norm;
    }

    /**
     * @brief Converte dB para valor linear
     */
    float db_to_linear(float db) {
        return powf(10.0f, db / 20.0f);
    }
};

#endif // THREE_BAND_EQ_H