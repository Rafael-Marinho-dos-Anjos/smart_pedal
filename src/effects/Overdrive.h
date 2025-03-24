#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include <Arduino.h>
#include <math.h>
#include <IEffect.h>

class Overdrive: IEffect {
public:
    /**
     * @brief Construtor da classe Overdrive
     * @param sample_rate Taxa de amostragem do áudio (em Hz)
     * @param drive Ganho de entrada (1.0 a 10.0)
     * @param mix Mixagem do efeito (0.0 a 1.0)
     * @param tone Controle de tonalidade (0.0 a 1.0)
     */
    Overdrive(uint32_t sample_rate = 44100, float drive = 3.0, float mix = 0.8, float tone = 0.5)
        : sample_rate_(sample_rate), drive_(drive), mix_(mix), tone_(tone) {
        // Inicializa filtros
        init_filters();
    }

    const string EFFECT_NAME = "OVERDRIVE";

    /**
     * @brief Processa uma amostra de áudio com efeito overdrive
     * @param input Amostra de entrada (-1.0 a 1.0)
     * @return Amostra processada (-1.0 a 1.0)
     */
    float process(float input) {
        // Aplica ganho de entrada
        float boosted = input * drive_;
        
        // Aplica função de distorção não-linear (soft clipping)
        float distorted = soft_clip(boosted);
        
        // Aplica filtro de tonalidade
        float filtered = apply_tone_filter(distorted);
        
        // Mixa sinal original com sinal distorcido
        return (input * (1.0 - mix_)) + (filtered * mix_);
    }

    string* getParamNames() {
        string * params = ["GAIN", "TONE", "MIX"];

        return params;
    }

    /**
     * @brief Atualiza o valor de um parâmetro através do index
     * @param param_index Número do parâmetro
     * @param val Valor a ser atribuído ao parâmetro
     */
    void setParam(int param_index, float val) {
        if (param_index == 0) {
            val = 1.0 + val * (10.0 - 1.0);
            set_drive(val);
        }
        else if (param_index == 1) {
            set_tone(val);
        }
        else if (param_index == 2) {
            set_mix(val);
        }
    }

    /**
     * @brief Retorna o valor de um parâmetro através do index
     * @param param_index Número do parâmetro
     */
    int getParamValue(int param_index) {
        int val = 100;
        if (param_index == 0) {
            return val * (drive_ - 1.0) / (10.0 - 1.0);
        }
        else if (param_index == 1) {
            return val * tone_;
        }
        else if (param_index == 2) {
            return val * mix_;
        }
    }

    /**
     * @brief Atualiza o parâmetro de drive
     * @param drive Novo valor de drive (1.0 a 10.0)
     */
    void set_drive(float drive) {
        drive_ = constrain(drive, 1.0, 10.0);
    }

    /**
     * @brief Atualiza o parâmetro de mix
     * @param mix Novo valor de mix (0.0 a 1.0)
     */
    void set_mix(float mix) {
        mix_ = constrain(mix, 0.0, 1.0);
    }

    /**
     * @brief Atualiza o parâmetro de tone
     * @param tone Novo valor de tone (0.0 a 1.0)
     */
    void set_tone(float tone) {
        tone_ = constrain(tone, 0.0, 1.0);
        update_tone_filter();
    }

private:
    uint32_t sample_rate_;
    float drive_;
    float mix_;
    float tone_;
    
    // Coeficientes para o filtro de tonalidade (filtro passa-baixas)
    float lpf_b0_, lpf_b1_, lpf_a1_;
    float lpf_z1_ = 0.0;
    
    /**
     * @brief Inicializa os filtros
     */
    void init_filters() {
        update_tone_filter();
    }
    
    /**
     * @brief Atualiza os coeficientes do filtro de tonalidade
     */
    void update_tone_filter() {
        // Mapeia tone_ para frequência de corte (200Hz a 5000Hz)
        float cutoff = 200.0 + (tone_ * 4800.0);
        
        // Calcula coeficientes para filtro passa-baixas simples (1º ordem)
        float w0 = 2 * PI * cutoff / sample_rate_;
        float alpha = sin(w0) / (2.0 * 0.707); // Q = 0.707
        
        lpf_b0 = (1.0 - cos(w0)) / 2.0;
        lpf_b1 = 1.0 - cos(w0);
        lpf_a1 = (-2.0 * alpha * cos(w0)) / (1.0 + alpha);
        
        // Normaliza os coeficientes
        float norm = lpf_b0 + lpf_b1 + (1.0 + lpf_a1);
        lpf_b0 /= norm;
        lpf_b1 /= norm;
        lpf_a1 /= norm;
    }
    
    /**
     * @brief Aplica filtro de tonalidade
     * @param input Amostra de entrada
     * @return Amostra filtrada
     */
    float apply_tone_filter(float input) {
        // Implementa filtro IIR de 1ª ordem
        float output = input * lpf_b0 + lpf_z1_;
        lpf_z1_ = input * lpf_b1 - output * lpf_a1;
        return output;
    }
    
    /**
     * @brief Aplica soft clipping não-linear
     * @param input Amostra de entrada
     * @return Amostra com distorção
     */
    float soft_clip(float input) {
        // Limita o sinal entre -1.0 e 1.0 com curva suave
        return (2.0 / PI) * atan(input);
        
        // Alternativa mais eficiente (aproximação):
        // float x = constrain(input, -3.0, 3.0);
        // return x - (x*x*x)/9.0;
    }
};

#endif // OVERDRIVE_H