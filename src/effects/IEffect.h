#ifndef IEFFECT_H
#define IEFFECT_H

#include <string>

/**
 * Interface para interação com efeitos
 */
class IEffect
{
    public:
        virtual float process(float input) {}
        virtual string* getParamNames() {}
        virtual void setParam(int param_index, float val) {}
        virtual int getParamValue(int param_index) {}
};

#endif // IEFFECT_H