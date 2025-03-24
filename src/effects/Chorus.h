#ifndef CHORUS_H
#define CHORUS_H

#include <Arduino.h>
#include <math.h>
#include <IEffect.h>

class Chorus: public IEffect {
public:
    /**
     * @brief Construtor da classe Chorus
     * @param sample_rate Taxa de amostragem do áudio (em Hz)
     * @param rate Frequência do LFO (0.1 a 2.0 Hz)
     * @param depth Intensidade do efeito (0.0 a 1.0)
     * @param delay_time Atraso base em ms (5 a 30 ms)
     * @param mix Mixagem do efeito (0.0 a 1.0)
     */
    Chorus(uint32_t sample_rate = 44100, float rate = 0.5, float depth = 0.5, 
           float delay_time = 15.0, float mix = 0.5)
        : sample_rate_(sample_rate), rate_(rate), depth_(depth), 
          delay_time_(delay_time), mix_(mix), lfo_phase_(0.0) {
        
        // Calcula tamanho do buffer baseado no máximo delay necessário
        max_delay_samples_ = static_cast<uint32_t>((30.0 /ms/ * sample_rate_) / 1000.0);
        delay_buffer_ = new float[max_delay_samples_]();
        write_index_ = 0;
        
        // Inicializa parâmetros
        update_parameters();
    }

    ~Chorus() {
        delete[] delay_buffer_;
    }

    const string EFFECT_NAME = "CHORUS";

    /**
     * @brief Processa uma amostra de áudio com efeito chorus
     * @param input Amostra de entrada (-1.0 a 1.0)
     * @return Amostra processada (-1.0 a 1.0)
     */
    float process(float input) {
        // Escreve no buffer de delay
        delay_buffer_[write_index_] = input;
        
        // Atualiza fase do LFO
        lfo_phase_ += lfo_inc_;
        if (lfo_phase_ >= 1.0) {
            lfo_phase_ -= 1.0;
        }
        
        // Calcula o deslocamento atual do LFO
        float lfo = sin(2.0 * PI * lfo_phase_);
        float modulated_delay = base_delay_samples_ + (lfo * depth_samples_);
        
        // Calcula posição de leitura
        float read_index = write_index_ - modulated_delay;
        while (read_index < 0) {
            read_index += max_delay_samples_;
        }
        
        // Interpolação linear para leitura suave
        uint32_t read_index_int = static_cast<uint32_t>(read_index);
        float frac = read_index - read_index_int;
        uint32_t next_index = (read_index_int + 1) % max_delay_samples_;
        
        float delayed_sample = delay_buffer_[read_index_int] * (1.0 - frac) + 
                              delay_buffer_[next_index] * frac;
        
        // Mixa sinal original com sinal processado
        float output = (input * (1.0 - mix_)) + (delayed_sample * mix_);
        
        // Atualiza índice de escrita
        write_index_ = (write_index_ + 1) % max_delay_samples_;
        
        return output;
    }

    string* getParamNames() {
        string * params = ["RATE", "DEPTH", "DELAY", "MIX"];

        return params;
    }

    /**
     * @brief Atualiza o valor de um parâmetro através do index
     * @param param_index Número do parâmetro
     * @param val Valor a ser atribuído ao parâmetro
     */
    void setParam(int param_index, float val) {
        if (param_index == 0) {
            val = 0.1 + val * (2.0 - 0.1);
            set_rate(val);
        }
        else if (param_index == 1) {
            set_depth(val);
        }
        else if (param_index == 2) {
            val = 5.0 + val * (30.0 - 5.0);
            set_delay_time(val);
        }
        else if (param_index == 3) {
            set_mix(val);
        }

        update_parameters();
    }

    /**
     * @brief Retorna o valor de um parâmetro através do index
     * @param param_index Número do parâmetro
     */
    int getParamValue(int param_index) {
        int val = 100;
        if (param_index == 0) {
            return val * (rate_ - 0.1) / (2.0 - 0.1);
        }
        else if (param_index == 1) {
            return val * depth_;
        }
        else if (param_index == 2) {
            return val * (delay_time_ - 5.0) / (30.0 - 5.0);
        }
        else if (param_index == 3) {
            return val * mix_;
        }
    }

    /**
     * @brief Atualiza o parâmetro de rate
     * @param rate Novo valor de rate (0.1 a 2.0 Hz)
     */
    void set_rate(float rate) {
        rate_ = constrain(rate, 0.1, 2.0);
        update_parameters();
    }

    /**
     * @brief Atualiza o parâmetro de depth
     * @param depth Novo valor de depth (0.0 a 1.0)
     */
    void set_depth(float depth) {
        depth_ = constrain(depth, 0.0, 1.0);
        update_parameters();
    }

    /**
     * @brief Atualiza o parâmetro de delay_time
     * @param delay_time Novo valor de delay_time (5 a 30 ms)
     */
    void set_delay_time(float delay_time) {
        delay_time_ = constrain(delay_time, 5.0, 30.0);
        update_parameters();
    }

    /**
     * @brief Atualiza o parâmetro de mix
     * @param mix Novo valor de mix (0.0 a 1.0)
     */
    void set_mix(float mix) {
        mix_ = constrain(mix, 0.0, 1.0);
    }

private:
    uint32_t sample_rate_;
    float rate_;
    float depth_;
    float delay_time_;
    float mix_;
    
    // Variáveis de estado
    float lfo_phase_;
    float lfo_inc_;
    float base_delay_samples_;
    float depth_samples_;
    
    // Buffer de delay
    float* delay_buffer_;
    uint32_t max_delay_samples_;
    uint32_t write_index_;
    
    /**
     * @brief Atualiza parâmetros internos quando configurações mudam
     */
    void update_parameters() {
        // Calcula incremento do LFO
        lfo_inc_ = rate_ / sample_rate_;
        
        // Converte tempo de delay para samples
        base_delay_samples_ = (delay_time_ * sample_rate_) / 1000.0;
        
        // Calcula depth em samples (5ms como máximo)
        depth_samples_ = (5.0 * depth_ * sample_rate_) / 1000.0;
    }
};

#endif // CHORUS_H