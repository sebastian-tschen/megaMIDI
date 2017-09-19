
#include <Encoder.h>

class Rotary {

uint8_t controlNo;

public: Rotary(Encoder encoder, uint8_t pControlNo){
        controlNo=pControlNo;

}

public: uint8_t getControlNo(void){
        return controlNo;
}
};
