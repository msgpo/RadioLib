#ifndef _RADIOLIB_MODULE_H
#define _RADIOLIB_MODULE_H

#include "TypeDef.h"

#include <SPI.h>
//#include <Wire.h>

#ifndef RADIOLIB_HARDWARE_SERIAL_PORT
// If a hardware serial port is not specified, assume the platform doesn't have one
#define RADIOLIB_HARDWARE_SERIAL_PORT NULL
#endif

#ifndef RADIOLIB_SOFTWARE_SERIAL_UNSUPPORTED
#include <SoftwareSerial.h>
#endif


/*!
  \class Module

  \brief Implements all common low-level SPI/UART/I2C methods to control the wireless module.
  Every module class contains one private instance of this class.
*/
class Module {
  public:

    /*!
      \brief UART-based module constructor.

      \param tx Arduino pin to be used as Tx pin for SoftwareSerial communication.

      \param rx Arduino pin to be used as Rx pin for SoftwareSerial communication.

      \param serial HardwareSerial to be used on platforms that do not support SoftwareSerial. Defaults to Serial1.

      \param rst Arduino pin to be used as hardware reset for the module. Defaults to NC (unused).
    */
#ifdef RADIOLIB_SOFTWARE_SERIAL_UNSUPPORTED
    Module(RADIOLIB_PIN_TYPE tx, RADIOLIB_PIN_TYPE rx, HardwareSerial* serial = RADIOLIB_HARDWARE_SERIAL_PORT, RADIOLIB_PIN_TYPE rst = RADIOLIB_NC);
#else
    Module(RADIOLIB_PIN_TYPE tx, RADIOLIB_PIN_TYPE rx, HardwareSerial* serial = nullptr, RADIOLIB_PIN_TYPE rst = RADIOLIB_NC);
#endif

    /*!
      \brief SPI-based module constructor. Will use the default SPI interface automatically initialize it.

      \param cs Arduino pin to be used as chip select.

      \param irq Arduino pin to be used as interrupt/GPIO.

      \param rst Arduino pin to be used as hardware reset for the module.
    */
    Module(RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq, RADIOLIB_PIN_TYPE rst);

    /*!
      \brief Extended SPI-based module constructor. Will use the default SPI interface automatically initialize it.

      \param cs Arduino pin to be used as chip select.

      \param irq Arduino pin to be used as interrupt/GPIO.

      \param rst Arduino pin to be used as hardware reset for the module.

      \param gpio Arduino pin to be used as additional interrupt/GPIO.
    */
    Module(RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq, RADIOLIB_PIN_TYPE rst, RADIOLIB_PIN_TYPE gpio);

    /*!
      \brief SPI-based module constructor.

      \param cs Arduino pin to be used as chip select.

      \param irq Arduino pin to be used as interrupt/GPIO.

      \param rst Arduino pin to be used as hardware reset for the module.

      \param spi SPI interface to be used, can also use software SPI implementations.

      \param spiSettings SPI interface settings.
    */
    Module(RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq, RADIOLIB_PIN_TYPE rst, SPIClass& spi, SPISettings spiSettings);

    /*!
      \brief Extended SPI-based module constructor.

      \param cs Arduino pin to be used as chip select.

      \param irq Arduino pin to be used as interrupt/GPIO.

      \param rst Arduino pin to be used as hardware reset for the module.

      \param gpio Arduino pin to be used as additional interrupt/GPIO.

      \param spi SPI interface to be used, can also use software SPI implementations.

      \param spiSettings SPI interface settings.
    */
    Module(RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq, RADIOLIB_PIN_TYPE rst, RADIOLIB_PIN_TYPE gpio, SPIClass& spi, SPISettings spiSettings);

    /*!
      \brief Generic module constructor.

      \param cs Arduino pin to be used as chip select.

      \param irq Arduino pin to be used as interrupt/GPIO.

      \param rst Arduino pin to be used as hardware reset for the module.

      \param tx Arduino pin to be used as Tx pin for SoftwareSerial communication.

      \param rx Arduino pin to be used as Rx pin for SoftwareSerial communication.

      \param spi SPI interface to be used. Defaults to Arduino hardware SPI interface, can also use software SPI implementations.

      \param spiSettings SPI interface settings. Defaults to 2 MHz clock, MSB first, mode 0.

      \param serial HardwareSerial to be used on ESP32 and SAMD. Defaults to 1
    */
#ifdef RADIOLIB_SOFTWARE_SERIAL_UNSUPPORTED
    Module(RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq, RADIOLIB_PIN_TYPE rst, RADIOLIB_PIN_TYPE rx, RADIOLIB_PIN_TYPE tx, SPIClass& spi = SPI, SPISettings spiSettings = SPISettings(2000000, MSBFIRST, SPI_MODE0), HardwareSerial* serial = RADIOLIB_HARDWARE_SERIAL_PORT);
#else
    Module(RADIOLIB_PIN_TYPE cs, RADIOLIB_PIN_TYPE irq, RADIOLIB_PIN_TYPE rst, RADIOLIB_PIN_TYPE rx, RADIOLIB_PIN_TYPE tx, SPIClass& spi = SPI, SPISettings spiSettings = SPISettings(2000000, MSBFIRST, SPI_MODE0), HardwareSerial* serial = nullptr);
#endif


    // public member variables

    /*!
      \brief Internal SoftwareSerial instance.
    */
#ifdef RADIOLIB_SOFTWARE_SERIAL_UNSUPPORTED
    HardwareSerial* ModuleSerial;
#else
    SoftwareSerial* ModuleSerial;
#endif

    /*!
      \brief Baud rate of SoftwareSerial UART communication. Defaults to 9600 baud.
    */
    uint32_t baudrate = 9600;

    /*!
      \brief Line feed to be used when sending AT commands. Defaults to CR+LF.
    */
    const char* AtLineFeed = "\r\n";

    /*!
      \brief Basic SPI read command. Defaults to 0x00.
    */
    uint8_t SPIreadCommand = 0b00000000;

    /*!
      \brief Basic SPI write command. Defaults to 0x80.
    */
    uint8_t SPIwriteCommand = 0b10000000;

    // basic methods

    /*!
      \brief Initialize low-level module control.

      \param interface Interface to be used on the module. See \ref shield_config for details.
    */
    void init(uint8_t interface);

    /*!
      \brief Terminate low-level module control.
    */
    void term();

    // AT methods

    /*!
      \brief Empty internal AT buffer.
    */
    void ATemptyBuffer();

    /*!
      \brief Get response after sending AT command.

      \returns True if AT response contains the string "OK", false otherwise.
    */
    bool ATgetResponse();

    /*!
      \brief Send AT command. Will also call ATgetResponse.

      \param cmd AT command to be sent. Line feed characters are added automatically.

      \returns True if AT response contains the string "OK", false otherwise.
    */
    bool ATsendCommand(const char* cmd);

    /*!
      \brief Send raw AT data. Will also call ATgetResponse.

      \param data Data to be sent.

      \param len Number of bytes to send.

      \returns True if AT response contains the string "OK", false otherwise.
    */
    bool ATsendData(uint8_t* data, uint32_t len);

    // SPI methods

    /*!
      \brief SPI read method that automatically masks unused bits. This method is the preferred SPI read mechanism.

      \param reg Address of SPI register to read.

      \param msb Most significant bit of the register variable. Bits above this one will be masked out.

      \param lsb Least significant bit of the register variable. Bits below this one will be masked out.

      \returns Masked register value or status code.
    */
    int16_t SPIgetRegValue(uint8_t reg, uint8_t msb = 7, uint8_t lsb = 0);

    /*!
      \brief Overwrite-safe SPI write method with verification. This method is the preferred SPI write mechanism.

      \param reg Address of SPI register to write.

      \param value Single byte value that will be written to the SPI register.

      \param msb Most significant bit of the register variable. Bits above this one will not be affected by the write operation.

      \param lsb Least significant bit of the register variable. Bits below this one will not be affected by the write operation.

      \param checkInterval Number of milliseconds between register writing and verification reading. Some registers need up to 10ms to process the change.

      \returns \ref status_codes
    */
    int16_t SPIsetRegValue(uint8_t reg, uint8_t value, uint8_t msb = 7, uint8_t lsb = 0, uint8_t checkInterval = 2);

    /*!
      \brief SPI burst read method.

      \param reg Address of SPI register to read.

      \param numBytes Number of bytes that will be read.

      \param inBytes Pointer to array that will hold the read data.
    */
    void SPIreadRegisterBurst(uint8_t reg, uint8_t numBytes, uint8_t* inBytes);

    /*!
      \brief SPI basic read method. Use of this method is reserved for special cases, SPIgetRegValue should be used instead.

      \param reg Address of SPI register to read.

      \returns Value that was read from register.
    */
    uint8_t SPIreadRegister(uint8_t reg);

    /*!
      \brief SPI burst write method.

      \param reg Address of SPI register to write.

      \param data Pointer to array that holds the data that will be written.

      \param numBytes Number of bytes that will be written.
    */
    void SPIwriteRegisterBurst(uint8_t reg, uint8_t* data, uint8_t numBytes);

    /*!
      \brief SPI basic write method. Use of this method is reserved for special cases, SPIsetRegValue should be used instead.

      \param reg Address of SPI register to write.

      \param data Value that will be written to the register.
    */
    void SPIwriteRegister(uint8_t reg, uint8_t data);

    /*!
      \brief SPI single transfer method.

      \param cmd SPI access command (read/write/burst/...).

      \param reg Address of SPI register to transfer to/from.

      \param dataOut Data that will be transfered from master to slave.

      \param dataIn Data that was transfered from slave to master.

      \param numBytes Number of bytes to transfer.
    */
    void SPItransfer(uint8_t cmd, uint8_t reg, uint8_t* dataOut, uint8_t* dataIn, uint8_t numBytes);

    // pin number access methods

    /*!
      \brief Access method to get the pin number of SPI chip select.

      \returns Pin number of SPI chip select configured in the constructor.
    */
    RADIOLIB_PIN_TYPE getCs() const { return(_cs); }

    /*!
      \brief Access method to get the pin number of interrupt/GPIO.

      \returns Pin number of interrupt/GPIO configured in the constructor.
    */
    RADIOLIB_PIN_TYPE getIrq() const { return(_irq); }

    /*!
      \brief Access method to get the pin number of hardware reset pin.

      \returns Pin number of hardware reset pin configured in the constructor.
    */
    RADIOLIB_PIN_TYPE getRst() const { return(_rst); }

    /*!
      \brief Access method to get the pin number of second interrupt/GPIO.

      \returns Pin number of second interrupt/GPIO configured in the constructor.
    */
    RADIOLIB_PIN_TYPE getGpio() const { return(_rx); }

    /*!
      \brief Access method to get the pin number of UART Rx.

      \returns Pin number of UART Rx configured in the constructor.
    */
    RADIOLIB_PIN_TYPE getRx() const { return(_rx); }

    /*!
      \brief Access method to get the pin number of UART Rx.

      \returns Pin number of UART Rx configured in the constructor.
    */
    RADIOLIB_PIN_TYPE getTx() const { return(_tx); }

    /*!
      \brief Access method to get the SPI interface.

      \returns SPI interface configured in the constructor.
    */
    SPIClass* getSpi() const { return(_spi); }

    /*!
      \brief Access method to get the SPI interface settings.

      \returns SPI interface settings configured in the constructor.
    */
    SPISettings getSpiSettings() const { return(_spiSettings); }

    /*!
      \brief Arduino core pinMode override that checks RADIOLIB_NC as alias for unused pin.

      \param pin Pin to change the mode of.

      \param mode Which mode to set.
    */
    static void pinMode(RADIOLIB_PIN_TYPE pin, RADIOLIB_PIN_MODE mode);

    /*!
      \brief Arduino core digitalWrite override that checks RADIOLIB_NC as alias for unused pin.

      \param pin Pin to write to.

      \param value Whether to set the pin high or low.
    */
    static void digitalWrite(RADIOLIB_PIN_TYPE pin, RADIOLIB_PIN_STATUS value);

    /*!
      \brief Arduino core digitalWrite override that checks RADIOLIB_NC as alias for unused pin.

      \param pin Pin to read from.

      \returns Pin value.
    */
    static RADIOLIB_PIN_STATUS digitalRead(RADIOLIB_PIN_TYPE pin);

    /*!
      \brief Arduino core tone override that checks RADIOLIB_NC as alias for unused pin and RADIOLIB_TONE_UNSUPPORTED to make sure the platform does support tone.

      \param pin Pin to write to.

      \param value Frequency to output.
    */
    static void tone(RADIOLIB_PIN_TYPE pin, uint16_t value);

    /*!
      \brief Arduino core noTone override that checks RADIOLIB_NC as alias for unused pin and RADIOLIB_TONE_UNSUPPORTED to make sure the platform does support tone.

      \param pin Pin to write to.
    */
    static void noTone(RADIOLIB_PIN_TYPE pin);

#ifndef RADIOLIB_GODMODE
  private:
#endif
    RADIOLIB_PIN_TYPE _cs;
    RADIOLIB_PIN_TYPE _tx;
    RADIOLIB_PIN_TYPE _rx;
    RADIOLIB_PIN_TYPE _irq;
    RADIOLIB_PIN_TYPE _rst;

    bool _initInterface;
    SPIClass* _spi;
    SPISettings _spiSettings;

    uint32_t _ATtimeout = 15000;
};

#endif
