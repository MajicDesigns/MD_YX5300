// Test program for the MD_YX5300 library
//
// Menu driven interface using the Serial Monitor to test individual functions.
//

#include <MD_YX5300.h>

// Connections for serial interface to the YX5300 module
const uint8_t ARDUINO_RX = 4;    // connect to TX of MP3 Player module
const uint8_t ARDUINO_TX = 5;    // connect to RX of MP3 Player module

// Define global variables
MD_YX5300 mp3(ARDUINO_RX, ARDUINO_TX);
bool bUseCallback = true; // use callbacks?
bool bUseSynch = false;   // use synchronous? 

void cbResponse(const MD_YX5300::cbData *status)
// Used to process device responses either as a library callback function
// or called locally when not in callback mode.
{
  if (bUseSynch)
    Serial.print(F("\nSync Status: "));
  else
    Serial.print(F("\nCback status: "));

  switch (status->code)
  {
  case MD_YX5300::STS_OK:         Serial.print(F("STS_OK"));         break;
  case MD_YX5300::STS_TIMEOUT:    Serial.print(F("STS_TIMEOUT"));    break;
  case MD_YX5300::STS_VERSION:    Serial.print(F("STS_VERSION"));    break;
  case MD_YX5300::STS_CHECKSUM:   Serial.print(F("STS_CHECKSUM"));    break;
  case MD_YX5300::STS_TF_INSERT:  Serial.print(F("STS_TF_INSERT"));  break;
  case MD_YX5300::STS_TF_REMOVE:  Serial.print(F("STS_TF_REMOVE"));  break;
  case MD_YX5300::STS_ERR_FILE:   Serial.print(F("STS_ERR_FILE"));   break;
  case MD_YX5300::STS_ACK_OK:     Serial.print(F("STS_ACK_OK"));     break;
  case MD_YX5300::STS_FILE_END:   Serial.print(F("STS_FILE_END"));   break;
  case MD_YX5300::STS_INIT:       Serial.print(F("STS_INIT"));       break;
  case MD_YX5300::STS_STATUS:     Serial.print(F("STS_STATUS"));     break;
  case MD_YX5300::STS_EQUALIZER:  Serial.print(F("STS_EQUALIZER"));  break;
  case MD_YX5300::STS_VOLUME:     Serial.print(F("STS_VOLUME"));     break;
  case MD_YX5300::STS_TOT_FILES:  Serial.print(F("STS_TOT_FILES"));  break;
  case MD_YX5300::STS_PLAYING:    Serial.print(F("STS_PLAYING"));    break;
  case MD_YX5300::STS_FLDR_FILES: Serial.print(F("STS_FLDR_FILES")); break;
  case MD_YX5300::STS_TOT_FLDR:   Serial.print(F("STS_TOT_FLDR"));   break;
  default: Serial.print(F("STS_??? 0x")); Serial.print(status->code, HEX); break;
  }

  Serial.print(F(", 0x"));
  Serial.print(status->data, HEX);
}

void setCallbackMode(bool b)
{
  bUseCallback = b;
  Serial.print(F("\n>Callback "));
  Serial.print(b ? F("ON") : F("OFF"));
  mp3.setCallback(b ? cbResponse : nullptr);
}

void setSynchMode(bool b)
{
  bUseSynch = b;
  Serial.print(F("\n>Synchronous "));
  Serial.print(b ? F("ON") : F("OFF"));
  mp3.setSynchronous(b);
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
  Serial.print(F("\npyyy\tPlay file index yyy (0-255)"));
  Serial.print(F("\npp\tPlay Pause"));
  Serial.print(F("\npz\tPlay Stop"));
  Serial.print(F("\np>\tPlay Next"));
  Serial.print(F("\np<\tPlay Previous"));
  Serial.print(F("\nptxxyyy\tPlay Track folder xx, file yyy"));
  Serial.print(F("\npfxx\tPlay loop folder xx"));
  Serial.print(F("\npxaa\tPlay shuffle folder aa"));
  Serial.print(F("\npryyy\tPlay loop file index yyy"));
  Serial.print(F("\n\nv+\tVolume up"));
  Serial.print(F("\nv-\tVolume down"));
  Serial.print(F("\nvxx\tVolume set xx (max 30)"));
  Serial.print(F("\nvb\tVolume Mute on (b=1), off (b=0)"));
  Serial.print(F("\n\nqe\tQuery equalizer"));
  Serial.print(F("\nqf\tQuery current file"));
  Serial.print(F("\nqs\tQuery status"));
  Serial.print(F("\nqv\tQuery volume"));
  Serial.print(F("\nqx\tQuery folder count"));
  Serial.print(F("\nqy\tQuery total file count"));
  Serial.print(F("\nqzxx\tQuery files count in folder xx"));
  Serial.print(F("\n\ns\tSleep"));
  Serial.print(F("\nw\tWake up"));
  Serial.print(F("\nen\tEqualizer type n"));
  Serial.print(F("\nxb\tPlay Shuffle on (b=1), off (b=0)"));
  Serial.print(F("\nrb\tPlay Repeat on (b=1), off (b=0)"));
  Serial.print(F("\nz\tReset"));
  Serial.print(F("\nyb\tSynchronous mode on (b=1), off (b=0)"));
  Serial.print(F("\ncb\tCallback mode on (b=1), off (b=0)"));
  Serial.print(F("\n\n"));
}

bool processPlay(void)
// Process the second level character(s) for the Play options
{
  char c = getNextChar(true);

  switch (toupper(c))
  {
    case '!': Serial.print(F("\n> Play Start")); return(mp3.playStart());
    case 'P': Serial.print(F("\n>Play Pause")); return(mp3.playPause());
    case 'Z': Serial.print(F("\n>Play Stop")); return(mp3.playStop());
    case '>': Serial.print(F("\n>Play Next")); return(mp3.playNext());
    case '<': Serial.print(F("\n>Play Prev")); return(mp3.playPrev());
    case '0'...'9':
      {
        uint8_t t = getNum(c, 3);
        Serial.print(F("\n>Play Track "));
        Serial.print(t);
        return(mp3.playTrack(t));
      }

    case 'T':
      {
        uint8_t fldr = getNum('\0', 2);
        uint8_t file = getNum('\0', 3);
        Serial.print(F("\n>Play Specific Fldr "));
        Serial.print(fldr);
        Serial.print(F(", "));
        Serial.print(file);
        return(mp3.playSpecific(fldr, file));
      }

    case 'F':
      {
        uint8_t fldr = getNum('\0', 2);
        Serial.print(F("\n>Play Folder "));
        Serial.print(fldr);
        return(mp3.playFolderRepeat(fldr));
      }

    case 'X':
    {
      uint8_t fldr = getNum('\0', 2);
      Serial.print(F("\n>Play Shuffle Folder "));
      Serial.print(fldr);
      return(mp3.playFolderShuffle(fldr));
    }

    case 'R':
    {
      uint8_t file = getNum('\0', 3);
      Serial.print(F("\n>Play File repeat "));
      Serial.print(file);
      return(mp3.playTrackRepeat(file));
    }

    default: Serial.print(F("\n>Play ?")); Serial.print(c); break;
  }

  return(false);
}

bool processVolume(void)
// Process the second level character(s) for the Volume options
{
  char c = getNextChar(true);

  switch (toupper(c))
  {
  case '+': Serial.print(F("\n>Volume Up"));  return(mp3.volumeInc());
  case '-': Serial.print(F("\n>Volume Down"));  return(mp3.volumeDec());
  case 'M':
    {
      uint8_t cmd = getNum('\0', 1);
      Serial.print(F("\n>Volume Enable "));
      Serial.print(cmd);
      return(mp3.volumeMute(cmd != 0));
    }

  default:
    {
      uint16_t v = getNum(c, 2);
      Serial.print(F("\n>Volume ")); 
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
  case 'E': Serial.print(F("\n>Query Equalizer"));  return(mp3.queryEqualizer());
  case 'F': Serial.print(F("\n>Query File"));       return(mp3.queryFile());
  case 'S': Serial.print(F("\n>Query Status"));     return(mp3.queryStatus());
  case 'V': Serial.print(F("\n>Query Volume"));     return(mp3.queryVolume());
  case 'X': Serial.print(F("\n>Query Folder Count"));  return(mp3.queryFolderCount());
  case 'Y': Serial.print(F("\n>Query Tracks Count"));  return(mp3.queryFilesCount());
  case 'Z': 
    {
      uint8_t fldr = getNum('\0', 2);
      Serial.print(F("\n>Query Folder Files Count "));
      Serial.print(fldr);
      return(mp3.queryFolderFiles(fldr));
    }

  default: Serial.print(F("\n>Query ?")); Serial.print(c);
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

    case 'S': Serial.print(F("\n>Sleep"));   bRet = mp3.sleep();  break;
    case 'W': Serial.print(F("\n>Wake up")); bRet = mp3.wakeUp(); break;
    case 'Z': Serial.print(F("\n>Reset"));   bRet = mp3.reset();  break;
    case 'E': 
    {
      uint8_t e = getNum('\0', 1);
      Serial.print(F("\n>Equalizer "));
      Serial.print(e);
      return(mp3.equalizer(e));
    }

    case 'X':
    {
      uint8_t cmd = getNum('\0', 1);
      Serial.print(F("\n>Shuffle "));
      Serial.print(cmd);
      return(mp3.shuffle(cmd != 0));
    }

    case 'R':
    {
      uint8_t cmd = getNum('\0', 1);
      Serial.print(F("\n>Repeat "));
      Serial.print(cmd);
      return(mp3.repeat(cmd != 0));
    }

    case 'Y':
    {
      uint8_t cmd = getNum('\0', 1);
      setSynchMode(cmd != 0);
    }
    break;
    case 'C':
    {
      uint8_t cmd = getNum('\0', 1);
      setCallbackMode(cmd != 0);
    }
    break;

    default: Serial.print(F("\n>Command ?")); Serial.print(c); break;
    }
  }

  return(bRet);
}

void setup()
{
  Serial.begin(57600);
  mp3.begin();

  help();
  setCallbackMode(bUseCallback);
  setSynchMode(bUseSynch);
}

void loop()
{
  if (processCmd() && !bUseCallback)
    cbResponse(mp3.getStatus());

  mp3.check();
}
