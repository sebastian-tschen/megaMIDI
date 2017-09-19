
#include <MIDI.h>
#include <Button.h>
#include "Arduino.h"
#include <Encoder.h>


#define FUNCTION_BUTTON_COUNT 20
#define ENCODER_COUNT 14
#define CHANNEL_BUTTON_COUNT 3

#define FUNCTION_BUTTON_CONTROLL_NO_OFFSET 64
#define ENCODER_CONTROLL_NO_OFFSET 0

MIDI_CREATE_DEFAULT_INSTANCE();

Encoder encoders[ENCODER_COUNT] = {
        Encoder(22,23),
        Encoder(24,25),
        Encoder(26,27),
        Encoder(28,29),
        Encoder(38,39),
        Encoder(40,41),
        Encoder(42,43),
        Encoder(44,45),
        Encoder(46,47),
        Encoder(48,49),
        Encoder(50,51),
        Encoder(52,53),
        Encoder(A11,A12),
        Encoder(A14,A15)

//skip 13 weil LED
        // Encoder(14,15),
        // Encoder(16,17),
        // Encoder(18,19),
        // Encoder(20,21)
};
int32_t encoderPositions[ENCODER_COUNT];

uint8_t channelButtons[CHANNEL_BUTTON_COUNT] = {2,3,4};

Button functionButtons[FUNCTION_BUTTON_COUNT] {
        Button(30),
        Button(31),
        Button(32),
        Button(33),
        Button(34),
        Button(35),
        Button(36),
        Button(37),
        Button(A0),
        Button(A1),
        Button(A2),
        Button(A3),
        Button(A4),
        Button(A5),
        Button(A6),
        Button(A7),
        Button(A8),
        Button(A9),
        Button(A10),
        Button(A13),
};


uint8_t functionButtonToControlNo[FUNCTION_BUTTON_COUNT];

uint8_t currentChannel=1;

byte channel_control_values[16][128];
boolean controlValuesChangedExternally=false;

void handleControlChange(byte channel, byte number, byte value)
{

        byte oldValue = channel_control_values[channel][number];
        if (oldValue!=value) {
                channel_control_values[channel][number] = value;
        }
}
void setup()
{

        pinMode(A0,INPUT_PULLUP);
        MIDI.setHandleControlChange(handleControlChange);
        MIDI.begin(MIDI_CHANNEL_OMNI);
        for (int i=0; i<CHANNEL_BUTTON_COUNT; i++) {

                pinMode(channelButtons[i], INPUT_PULLUP);

        }
        pinMode(LED_BUILTIN, OUTPUT);

//initialize controls to controlNo. maps
        for (int i = 0; i<FUNCTION_BUTTON_COUNT; i++) {
                functionButtons[i].begin();
                functionButtonToControlNo[i]=i+FUNCTION_BUTTON_CONTROLL_NO_OFFSET;
        }
        MIDI.turnThruOff();

}

int counter=0;
boolean current=false;

void changeChannelControlValue(int32_t valueDiff,uint8_t i){

        int32_t newValue=channel_control_values[currentChannel][i]+valueDiff;
        if (newValue<0) {
                channel_control_values[currentChannel][i]=0;
        }else if (newValue>127) {
                channel_control_values[currentChannel][i]=127;
        }else{
                channel_control_values[currentChannel][i]=newValue;
        }

        MIDI.sendControlChange(i,channel_control_values[currentChannel][i], currentChannel);

        digitalWrite(LED_BUILTIN, current);
        counter++;
        if (counter>1) {
                counter=0;
                current=!current;
        }
}

boolean blink=true;
long n=0;
void loop()
{

//check channel buttons
//check function buttons
//check rotaryEncoders which have no interrupts attached
//read midi
//update leds if change happened


        for (int i = 0; i<FUNCTION_BUTTON_COUNT; i++) {
                functionButtons[i].read();
                if (functionButtons[i].pressed()) {
                        MIDI.sendControlChange(functionButtonToControlNo[i], 127, currentChannel);
                }
        }
        for (int i = 0; i<ENCODER_COUNT; i++) {
                int32_t newDiff;
                int32_t newValue = encoders[i].read();
                int32_t oldValue = encoderPositions[i];
                if (newValue>oldValue) {
                        int32_t diff= newValue-oldValue;
                        newDiff = diff >> 2;
                }else {
                        int32_t diff= oldValue-newValue;
                        newDiff = -(diff >> 2);
                }
                if (newDiff!=0) {
                        changeChannelControlValue(newDiff,i);
                        encoderPositions[i]=newValue;
                }

        }
        for (int i = 0; i<CHANNEL_BUTTON_COUNT; i++) {
                if (!digitalRead(channelButtons[i])) {
                        currentChannel=i+1;
                }
        }

        MIDI.read();

        if (n>10000) {
                n=0;
                if (blink) {
                        blink=false;
                }else{
                        blink = true;
                }
                digitalWrite(LED_BUILTIN,blink);

        }else{
          n++;
        }
}
