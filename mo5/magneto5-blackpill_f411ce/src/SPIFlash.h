/* Arduino SPIMemory Library v.3.4.0
 * Copyright (C) 2019 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by @boseji <salearj@hotmail.com> - 02/03/2017
 * Modified by Prajwal Bhattaram - 03/06/2019
 *
 * This file is part of the Arduino SPIMemory Library. This library is for
 * Flash and FRAM memory modules. In its current form it enables reading,
 * writing and erasing data from and to various locations;
 * suspending and resuming programming/erase and powering down for low power operation.
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License v3.0
 * along with the Arduino SPIMemory Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef SPIFLASH_H
#define SPIFLASH_H

#include <Arduino.h>
#include <SPI.h>
#include "defines.h"
#include "diagnostics.h"

#if defined (ARDUINO_ARCH_SAM)
  #include <malloc.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include "DMASAM.h"
#endif

#if defined (ARDUINO_ARCH_SAMD)
  #if defined (ENABLEZERODMA)
    #include "DMASAMD.h"
  #endif
#endif

#if defined (BOARD_RTL8195A)
  #ifdef __cplusplus
    extern "C" {
  #endif

  #include "gpio_api.h"
  #include "PinNames.h"

  #ifdef __cplusplus
    }
  #endif
#endif

#ifndef ARCH_STM32
  #if defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_STM32L0) || defined(__STM32F1__) || defined(STM32F1) || defined(STM32F3) || defined(STM32F4) || defined(STM32F0xx)
    #define ARCH_STM32
  #endif
#endif

#if defined (ARDUINO_ARCH_SAM) || defined (ARDUINO_ARCH_SAMD) || defined (ARDUINO_ARCH_ESP8266) || defined (SIMBLEE) || defined (ARDUINO_ARCH_ESP32) || defined (BOARD_RTL8195A) || defined(ARCH_STM32) || defined(ESP32) || defined(NRF52)
// RTL8195A included - @boseji <salearj@hotmail.com> 02.03.17
  #define _delay_us(us) delayMicroseconds(us)
#else
  #include <util/delay.h>
#endif

#define SPIFLASH_LIBVER 3
#define SPIFLASH_LIBSUBVER 4
#define SPIFLASH_REVVER 0

#define SPIFRAM_LIBVER 0
#define SPIFRAM_LIBSUBVER 0
#define SPIFRAM_REVVER 1

class SPIMemory {
public:
  //------------------------------------ Constructor ------------------------------------//
  SPIMemory(void) {};
  ~SPIMemory(void) {};
  //------------------------------- Public functions -----------------------------------//
  //------------------------------- Public variables -----------------------------------//


};

extern SPIMemory SPIMem; //default SPIMemory instance;

class SPIFlash {
public:
  //------------------------------------ Constructor ------------------------------------//
  //New Constructor to Accept the PinNames as a Chip select Parameter - @boseji <salearj@hotmail.com> 02.03.17
  #if defined (ARDUINO_ARCH_SAMD) || defined(ARCH_STM32) || defined(ARDUINO_ARCH_ESP32)
  SPIFlash(uint8_t cs = CS, SPIClass *spiinterface=&SPI);
  #elif defined (BOARD_RTL8195A)
  SPIFlash(PinName cs = CS);
  #else
  SPIFlash(uint8_t cs = CS);
  SPIFlash(int8_t *SPIPinsArray);
  #endif
  //----------------------------- Initial / Chip Functions ------------------------------//
  bool     begin(uint32_t flashChipSize = 0);
  #ifdef SPI_HAS_TRANSACTION
  void     setClock(uint32_t clockSpeed);
  #else
  void     setClock(uint8_t clockdiv);
  #endif
  bool     libver(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  bool     sfdpPresent(void);
  uint8_t  error(bool verbosity = false);
  uint16_t getManID(void);
  uint32_t getJEDECID(void);
  uint64_t getUniqueID(void);
  uint32_t getAddress(uint16_t size);
  uint32_t getCapacity(void);
  uint32_t getMaxPage(void);
  float    functionRunTime(void);
  //-------------------------------- Write / Read Bytes ---------------------------------//
  bool     writeByte(uint32_t _addr, uint8_t data, bool errorCheck = true);
  bool     readByte(uint32_t _addr, uint8_t &value, bool fastRead = false);
  //----------------------------- Write / Read Byte Arrays ------------------------------//
  bool     writeByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool errorCheck = true);
  bool     readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false);
  //-------------------------------- Erase functions ------------------------------------//
  bool     eraseSection(uint32_t _addr, uint32_t _sz);
  bool     eraseSector(uint32_t _addr);
  bool     eraseBlock32K(uint32_t _addr);
  bool     eraseBlock64K(uint32_t _addr);
  bool     eraseChip(void);
  //-------------------------------- Power functions ------------------------------------//
  bool     suspendProg(void);
  bool     resumeProg(void);
  bool     powerDown(void);
  bool     powerUp(void);
  //------------------------------- Public variables ------------------------------------//
private:
  //------------------------------- Private functions -----------------------------------//
  void     _troubleshoot(uint8_t _code, bool printoverride = false);
  void     _endSPI(void);
  bool     _disableGlobalBlockProtect(void);
  bool     _isChipPoweredDown(void);
  bool     _prep(uint8_t opcode, uint32_t _addr, uint32_t size = 0);
  bool     _startSPIBus(void);
  bool     _beginSPI(uint8_t opcode);
  bool     _noSuspend(void);
  bool     _notBusy(uint32_t timeout = BUSY_TIMEOUT);
  bool     _notPrevWritten(uint32_t _addr, uint32_t size = 1);
  bool     _writeEnable(bool _troubleshootEnable = true);
  bool     _writeDisable(void);
  bool     _getJedecId(void);
  bool     _getManId(uint8_t *b1, uint8_t *b2);
  bool     _chipID(uint32_t flashChipSize = 0);
  bool     _transferAddress(void);
  bool     _addressCheck(uint32_t _addr, uint32_t size = 1);
  bool     _enable4ByteAddressing(void);
  bool     _disable4ByteAddressing(void);
  uint8_t  _nextByte(char IOType, uint8_t data = NULLBYTE);
  uint16_t _nextInt(uint16_t = NULLINT);
  void     _nextBuf(uint8_t opcode, uint8_t *data_buffer, uint32_t size);
  uint8_t  _readStat1(void);
  uint8_t  _readStat2(void);
  uint8_t  _readStat3(void);
  bool     _getSFDPTable(uint32_t _tableAddress, uint8_t *data_buffer, uint8_t numberOfDWords);
  bool     _getSFDPData(uint32_t _address, uint8_t *data_buffer, uint8_t numberOfBytes);
  uint32_t _getSFDPdword(uint32_t _tableAddress, uint8_t dWordNumber);
  uint16_t _getSFDPint(uint32_t _tableAddress, uint8_t dWordNumber, uint8_t startByte);
  uint8_t  _getSFDPbyte(uint32_t _tableAddress, uint8_t dWordNumber, uint8_t byteNumber);
  bool     _getSFDPbit(uint32_t _tableAddress, uint8_t dWordNumber, uint8_t bitNumber);
  uint32_t _getSFDPTableAddr(uint32_t paramHeaderNum);
  uint32_t _calcSFDPEraseTimeUnits(uint8_t _unitBits);
  bool     _checkForSFDP(void);
  void     _getSFDPEraseParam(void);
  void     _getSFDPProgramTimeParam(void);
  bool     _getSFDPFlashParam(void);
  //-------------------------------- Private variables ----------------------------------//
  #ifdef SPI_HAS_TRANSACTION
    SPISettings _settings;
    bool _SPISettingsSet = false;
  #else
    uint8_t _clockdiv;
  #endif

  //If multiple SPI ports are available this variable is used to choose between them (SPI, SPI1, SPI2 etc.)
  SPIClass *_spi;

  #if !defined (BOARD_RTL8195A)
  uint8_t     csPin;
  #else
  // Object declaration for the GPIO HAL type for csPin - @boseji <salearj@hotmail.com> 02.03.17
  gpio_t      csPin;
  #endif

  // Variables specific to using non-standard SPI (currently only tested with ESP32)
  struct _SPIPins {
    int8_t sck = -1;
    int8_t miso = -1;
    int8_t mosi = -1;
    int8_t ss = -1;
  };
  _SPIPins _nonStdSPI;
  //_SPIPins _stdSPI;
  uint8_t _SPIInUse;

  volatile uint8_t *cs_port;
  bool        pageOverflow;
  bool        SPIBusState = false;
  bool        chipPoweredDown = false;
  bool        address4ByteEnabled = false;
  bool        _loopedOver = false;
  uint8_t     cs_mask, errorcode, stat1, stat2, stat3, _SPCR, _SPSR, _a0, _a1, _a2;
  char READ = 'R';
  char WRITE = 'W';
  float _spifuncruntime = 0;
  struct      chipID {
                bool supported;
                bool supportedMan;
                bool sfdpAvailable;
                uint8_t manufacturerID;
                uint8_t memoryTypeID;
                uint8_t capacityID;
                uint32_t capacity;
                uint32_t eraseTime;
              };
              chipID _chip;
  struct      eraseParam{
              bool supported;
              uint8_t opcode;
              uint32_t time;
            } kb4Erase, kb32Erase, kb64Erase, kb256Erase, chipErase;
  uint8_t     _noOfParamHeaders, _noOfBasicParamDwords;
  uint16_t    _eraseTimeMultiplier, _prgmTimeMultiplier, _pageSize;
  uint32_t    currentAddress, _currentAddress = 0;
  uint32_t    _addressOverflow = false;
  uint32_t    _BasicParamTableAddr, _SectorMapParamTableAddr, _byteFirstPrgmTime, _byteAddnlPrgmTime, _pagePrgmTime;
  uint8_t     _uniqueID[8];
  const uint8_t _capID[18]   =
  {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x41, 0x42, 0x43, 0x4B, 0x00, 0x01, 0x13, 0x37};

  const uint32_t _memSize[18]  =
  {KB(64), KB(128), KB(256), KB(512), MB(1), MB(2), MB(4), MB(8), MB(16), MB(32), MB(2), MB(4), MB(8), MB(8), KB(256), KB(512), MB(4), KB(512)};
  // To understand the _memSize definitions check defines.h

  const uint8_t _supportedManID[9] = {WINBOND_MANID, MICROCHIP_MANID, CYPRESS_MANID, ADESTO_MANID, MICRON_MANID, ON_MANID, GIGA_MANID, AMIC_MANID, MACRONIX_MANID};

  const uint8_t _altChipEraseReq[3] = {A25L512, M25P40, SST26};
};

#endif // _SPIFLASH_H_
