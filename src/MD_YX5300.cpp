/*
MD_YX5300 - Library for YX5300 MP3 IC using serial interface

See header file for comments

This file contains class and hardware related methods.

Copyright (C) 2018 Marco Colli. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
#include "MD_YX5300.h"

/**
 * \file
 * \brief Implements class methods
 */

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))  ///< Generic array element count macro

#define LIBDEBUG  0   ///< Set to 1 to enable Debug statement in the library

#if LIBDEBUG
#define PRINT(s, v)   { Serial.print(F(s)); Serial.print(v); }      ///< Print a string followed by a value (decimal)
#define PRINTX(s, v)  { Serial.print(F(s)); Serial.print(v, HEX); } ///< Print a string followed by a value (hex)
#define PRINTS(s)     { Serial.print(F(s)); }                       ///< Print a string
#else
#define PRINT(s, v)   ///< Print a string followed by a value (decimal)
#define PRINTX(s, v)  ///< Print a string followed by a value (hex)
#define PRINTS(s)     ///< Print a string
#endif

void MD_YX5300::begin(void)
{
  _S.begin(9600);
  reset();
  device(CMD_OPT_DEV_TF);
}

bool MD_YX5300::check(void)
// returns true when received full message or timeout
{
  uint8_t c = '\0';

  // check for timeout if waiting response
  if (_waitResponse && (millis() - _timeSent >= _timeout))
  {
    processResponse(true);
    return(true);
  }

  // check if any characters available
  if (!_S.available())
    return(false);

  // process all the characters waiting
  do
  {
    c = _S.read();
    if (c == PKT_SOM) _bufIdx = 0;
    _bufRx[_bufIdx++] = c;
  } while (_S.available() && c != PKT_EOM);

  // check if we have a whole message to 
  // process and do something with it here!
  if (c == PKT_EOM)
  {
    _waitResponse = false;
    processResponse();
  }

  return(c == PKT_EOM);   // we have just processed a response
}

bool MD_YX5300::sendRqst(cmdSet_t cmd, uint8_t data1, uint8_t data2)
// returns true if the response status is ready for processing
{
  static uint8_t msg[] = { PKT_SOM, PKT_VER, PKT_LEN, CMD_NUL, PKT_FB_ON, PKT_DATA_NUL, PKT_DATA_NUL, PKT_EOM };

  msg[3] = cmd;
  // msg[4] = _synch ? PKT_FB_ON : PKT_FB_OFF;
  msg[5] = data1;
  msg[6] = data2;

  _S.write(msg, ARRAY_SIZE(msg));
  _timeSent = millis();
  _status.code = STS_OK;
  _waitResponse = true;
#if LIBDEBUG
  dumpMessage(msg, ARRAY_SIZE(msg), "S");
#endif

  // if synchronous mode enabled, wait for a 
  // response message to be processed
  if (_synch)
  {
    do {} while (!check());
    PRINTS("\nSynch wait over");
    return(true);
  }
  
  return(false);
}

void MD_YX5300::processResponse(bool bTimeout = false)
{
#if LIBDEBUG
  dumpMessage(_bufRx, _bufIdx, "R");
#endif

  // set the status memory up with current return codes
  _status.code = (bTimeout ? STS_TIMEOUT : _bufRx[3]);
  _status.data = 0;

  PRINTX(" -> ", _status.code);
  PRINTS(" : ");

  // allocate the return code & print debug message
  switch (_status.code)
  {
  case STS_OK:        PRINTS("OK");          break;
  case STS_TIMEOUT:   PRINTS("Timeout");     break;
  case STS_TF_INSERT: PRINTS("TF inserted"); break;
  case STS_TF_REMOVE: PRINTS("TF removed");  break;
  case STS_ERR:       PRINTS("Error");       break;
  case STS_ACK_OK:    PRINTS("Ack OK");      break;
  case STS_FILE_END:   _status.data = _bufRx[6]; PRINT("Ended track ", _status.data);    break;
  case STS_STATUS:     _status.data = (_bufRx[5] << 8) | _bufRx[6]; PRINTX("Status 0x", _status.data);      break;
  case STS_VOLUME:     _status.data = _bufRx[6]; PRINT("Vol ", _status.data);            break;
  case STS_TOT_FILES:  _status.data = _bufRx[6]; PRINT("Tot files ", _status.data);    break;
  case STS_PLAYING:    _status.data = _bufRx[6]; PRINT("Playing File ", _status.data);      break;
  case STS_FLDR_FILES: _status.data = _bufRx[6]; PRINT("Folder files ", _status.data); break;
  case STS_TOT_FLDR:   _status.data = _bufRx[6]; PRINT("Tot folder: ", _status.data);      break;
  default:             _status.data = _bufRx[6]; PRINTS("Unknown Status Code");          break;
  }

  // finally, call the callback if there is one
  if (_cbStatus != nullptr)
    _cbStatus(&_status);
}

char MD_YX5300::itoh(uint8_t i)
{
  if (i < 10) return(i + '0');
  else return(i - 10 + 'A');
}

void MD_YX5300::dumpMessage(uint8_t *msg, uint8_t len, char *psz)
{
  char sz[3] = "00";

  PRINT("\n", psz); PRINTS(": ");
  for (uint8_t i=0; i<len; i++, msg++)
  {
    sz[0] = itoh((*msg & 0xf0) >> 4);
    sz[1] = itoh(*msg & 0x0f);
    PRINT(" ", sz);
  }
}

