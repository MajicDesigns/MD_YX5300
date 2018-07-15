// Test program for the MD_YX5300 library
//
// Menu driven interface using the Serial Monitor to test individual functions.
//

#include <MD_YX5300.h>

// Connections for serial interface to the YX5300 module
#define ARDUINO_RX 5  // connect to TX of MP3 Player module
#define ARDUINO_TX 6  // connect to RX of MP3 Player module

// Define global variables
MD_YX5300 mp3(ARDUINO_RX, ARDUINO_TX);
bool bUseCallback = true;    // use callbacks or run synchronous mode

void cbResponse(const MD_YX5300::cbData *status)
// Used to process device responses either as a library callback function
// or called locally when not in callback mode.
{
  if (bUseCallback)
    Serial.print(F("\nCback status: "));
  else
    Serial.print(F("\nSync Status: "));

  switch (status->code)
  {
  case MD_YX5300::STS_OK:         Serial.print(F("STS_OK"));         break;
  case MD_YX5300::STS_TIMEOUT:    Serial.print(F("STS_TIMEOUT"));    break;
  case MD_YX5300::STS_TF_INSERT:  Serial.print(F("STS_TF_INSERT"));  break;
  case MD_YX5300::STS_TF_REMOVE:  Serial.print(F("STS_TF_REMOVE"));  break;
  case MD_YX5300::STS_ERR:        Serial.print(F("STS_ERR"));        break;
  case MD_YX5300::STS_ACK_OK:     Serial.print(F("STS_ACK_OK"));     break;
  case MD_YX5300::STS_FILE_END:   Serial.print(F("STS_FILE_END"));   break;
  case MD_YX5300::STS_STATUS:     Serial.print(F("STS_STATUS"));     break;
  case MD_YX5300::STS_VOLUME:     Serial.print(F("STS_VOLUME"));     break;
  case MD_YX5300::STS_TOT_FILES:  Serial.print(F("STS_TOT_FILES"));  break;
  case MD_YX5300::STS_PLAYING:    Serial.print(F("STS_PLAYING"));    break;
  case MD_YX5300::STS_FLDR_FILES: Serial.print(F("STS_FLDR_FILES")); break;
  case MD_YX5300::STS_TOT_FLDR:   Serial.print(F("STS_TOT_FLDR"));   break;
  default: Serial.print(F("STS_??? 0x")); Serial.print(status->code, HEX); break;
  }

  Serial.print(F(", "));
  Serial.print(status->data);
}

void setCallbackMode(bool b)
{
  bUseCallback = b;
  mp3.setCallback(b ? cbResponse : nullptr);
}

char getNextChar(bool block = false)
{
  char c = '\0';

  if (!block) return(Serial.available() ? Serial.read() : c);

  while (c == '\0')
  {
    if (Serial.available())
      c = Serial.read();
  }

  return(c);
}

uint8_t c2i(char c)
{
  if (c < '0' || c > '9') return(0);

  return(c - '0');
}

uint16_t getNum(char c, uint8_t dig)
{
  uint16_t v = c2i(c);

  if (c != '\0') dig--;    // first digit was passed in
  for (uint8_t i = 0; i < dig; i++)
    v = (v * 10) + c2i(getNextChar(true));

  return(v);
}

void help(void)
{
  Serial.print(F("\n[MD_YX5300 Test Menu]"));
  Serial.print(F("\nh,?\thelp"));
  Serial.print(F("\n\np!\tPlay"));
  Serial.print(F("\npxxx\tPlay track xxx (0-255)"));
  Serial.print(F("\npp\tPlay Pause"));
  Serial.print(F("\npz\tPlay Stop"));
  Serial.print(F("\np>\tPlay Next"));
  Serial.print(F("\np<\tPlay Previous"));
  Serial.print(F("\nptxxyyy\tPlay Track folder xx, file yyy"));
  Serial.print(F("\npfn\tPlay cycle folder n"));
  Serial.print(F("\npsx\tPlay Shuffle on (x=1), off (x=0)"));
  Serial.print(F("\nprx\tPlay Repeat on (x=1), off (x=0)"));
  Serial.print(F("\n\nv+\tVolume up"));
  Serial.print(F("\nv-\tVolume down"));
  Serial.print(F("\nvxx\tVolume set xx (max 32)"));
  Serial.print(F("\nvm\tVolume Mute on (x=1), off (x=0)"));
  Serial.print(F("\n\nqf\tQuery current file"));
  Serial.print(F("\nqs\tQuery status"));
  Serial.print(F("\nqv\tQuery volume"));
  Serial.print(F("\nqx\tQuery folder count"));
  Serial.print(F("\nqt\tQuery total file count"));
  Serial.print(F("\n\ns\tSleep"));
  Serial.print(F("\nw\tWake up"));
  Serial.print(F("\nr\tReset"));
  Serial.print(F("\nzx\tSynchronous mode on (x=1), off (x=0)"));
  Serial.print(F("\ncx\tCallback mode on (x=1), off (x=0)"));
  Serial.print(F("\n\n"));
}

bool processPlay(void)
// Process the second level character(s) for the Play options
{
  char c = getNextChar(true);

  switch (toupper(c))
  {
  case '!': Serial.print(F("\nPlay Start")); return(mp3.playStart());
  case 'P': Serial.print(F("\nPlay Pause")); return(mp3.playPause());
  case 'Z': Serial.print(F("\nPlay Stop")); return(mp3.playStop());
  case '>': Serial.print(F("\nPlay Next")); return(mp3.playNext());
  case '<': Serial.print(F("\nPlay Prev")); return(mp3.playPrev());
  case '0'...'9':
    {
      uint8_t t = getNum(c, 3);
      Serial.print(F("\nPlay Track "));
      Serial.print(t);
      return(mp3.playTrack(t));
    }

  case 'T':
    {
      uint8_t fldr = getNum('\0', 2);
      uint8_t file = getNum('\0', 3);
      Serial.print(F("\nPlay Specific Fldr "));
      Serial.print(fldr);
      Serial.print(F(", "));
      Serial.print(file);
      return(mp3.playSpecific(fldr, file));
    }

  case 'F':
    {
      uint8_t fldr = getNum('\0', 1);
      Serial.print(F("\nPlay Folder "));
      Serial.print(fldr);
      return(mp3.playFolderRepeat(fldr));
    }

  case 'S':
  {
    uint8_t cmd = getNum('\0', 1);
    Serial.print(F("\nPlay Shuffle "));
    Serial.print(cmd);
    return(mp3.playShuffle(cmd != 0));
  }

  case 'R':
  {
    uint8_t cmd = getNum('\0', 1);
    Serial.print(F("\nPlay Repeat "));
    Serial.print(cmd);
    return(mp3.playRepeat(cmd != 0));
  }

  default: Serial.print(F("\nPlay ?")); Serial.print(c); break;
  }

  return(false);
}

bool processVolume(void)
// Process the second level character(s) for the Volume options
{
  char c = getNextChar(true);

  switch (toupper(c))
  {
  case '+': Serial.print(F("\nVolume Up"));  return(mp3.volumeInc());
  case '-': Serial.print(F("\nVolume Down"));  return(mp3.volumeDec());
  case 'M':
    {
      uint8_t cmd = getNum('\0', 1);
      Serial.print(F("\nVolume Enable "));
      Serial.print(cmd);
      return(mp3.volumeMute(cmd != 0));
    }

  default:
    {
      uint16_t v = getNum(c, 2);
      Serial.print(F("\nVolume ")); 
      Serial.print(v);
      return(mp3.volume(v)); 
    }
  }

  return(false);
}

bool processQuery(void)
// Process the second level character(s) for the Query options
{
  char c = getNextChar(true);

  switch (toupper(c))
  {
  case 'F': Serial.print(F("\nQuery Track"));  return(mp3.queryTrack());
  case 'S': Serial.print(F("\nQuery Status"));  return(mp3.queryStatus());
  case 'V': Serial.print(F("\nQuery Volume"));  return(mp3.queryVolume());
  case 'X': Serial.print(F("\nQuery Folder Count"));  return(mp3.queryFolderCount());
  case 'T': Serial.print(F("\nQuery Tracks Count"));  return(mp3.queryTracksCount());
  default: Serial.print(F("\nQuery ?")); Serial.print(c);
  }

  return(false);
}

bool processCmd(void)
// Process the top level character(s) for the main menu
{
  bool bRet = false;
  char c = getNextChar();

  if (c != '\0')
  {
    switch (toupper(c))
    {
    case '?':
    case 'H': help(); break;
    case 'P': bRet = processPlay(); break;
    case 'V': bRet = processVolume(); break;
    case 'Q': bRet = processQuery(); break;

    case 'S': Serial.print(F("\nSleep"));   bRet = mp3.sleep();  break;
    case 'W': Serial.print(F("\nWake up")); bRet = mp3.wakeUp(); break;
    case 'R': Serial.print(F("\nReset"));   bRet = mp3.reset();  break;
    case 'Z':
    {
      uint8_t cmd = getNum('\0', 1);
      Serial.print(F("\nSynchronous "));
      Serial.print(cmd);
      mp3.setSynchronous(cmd != 0);
    }
    break;
    case 'C':
    {
      uint8_t cmd = getNum('\0', 1);
      Serial.print(F("\nCallback "));
      Serial.print(cmd);
      setCallbackMode(cmd != 0);
    }
    break;

    default: Serial.print(F("\nCommand ?")); Serial.print(c); break;
    }
  }

  return(bRet);
}

void setup()
{
  Serial.begin(57600);
  mp3.begin();
  setCallbackMode(bUseCallback);

  help();
}

void loop()
{
  if (processCmd() && !bUseCallback)
    cbResponse(mp3.getStatus());

  mp3.check();
}
