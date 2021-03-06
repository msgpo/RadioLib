#include "AFSK.h"

AFSKClient::AFSKClient(PhysicalLayer* phy, RADIOLIB_PIN_TYPE pin) {
  _phy = phy;
  _pin = pin;
}

int16_t AFSKClient::tone(uint16_t freq, bool autoStart) {
  if(autoStart) {
    int16_t state = _phy->transmitDirect();
    RADIOLIB_ASSERT(state);
  }

  Module::tone(_pin, freq);
  return(ERR_NONE);
}

int16_t AFSKClient::noTone() {
  Module::noTone(_pin);
  return(_phy->standby());
}
