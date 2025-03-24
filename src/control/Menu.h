#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <Constants.h>
#include <IEffect.h>
#include <Overdrive.h>
#include <Chorus.h>
#include <ThreeBandEQ.h>


class Menu{
    public:
        Menu(int ef = 0) {
            ef_index = ef;
            param_index = 0;
            menu_level = 0;

            load_parameters();
        }

        /**
         * Funções de navegação
         */
        void navigate_left() {}
        void navigate_right() {}
        void navigate_ok() {}

        void read_knob(int knob_pin) {
            if (menu_level == 2) {
                float knob;
                // knob = analogRead(knob_pin) / 4095;

                IEffect ef = get_effect(ef_index);
                ef.setParam(param_index, knob);
            }
        }

        void process_audio(float* input, float* output, uint32_t block_size) {
            IEffect ef = get_effect(ef_index);

            for (uint32_t i = 0; i < block_size; i++) {
                output[i] = ef.process(input[i]);
            }
        }

    private:
        int menu_level;
        int ef_index;
        int param_index;

        IEffect EFF_0;
        IEffect EFF_1;
        IEffect EFF_2;

        IEffect get_effect(int index) {
            if (index == 0) {
                return EFF_0;
            }
            if (index == 1) {
                return EFF_1;
            }
            if (index == 2) {
                return EFF_2;
            }
        }

        /**
         * Funções para persistir as configurações dos efeitos
         */
        void load_parameters() {
            // Recupera valores dos parâmetros de um arquivo e instancia os efeitos
            EFF_0 = Overdrive(SAMPLE_RATE, 3.0, 0.8, 0.5);
            EFF_1 = Chorus(SAMPLE_RATE, 0.5, 0.7, 15.0, 0.5);
            EFF_2 = ThreeBandEQ(SAMPLE_RATE);
        }
        void save_parameters() {}

        /**
         * Instancia os efeitos
         */
        void init_effects() {}
};

#endif // MENU_H