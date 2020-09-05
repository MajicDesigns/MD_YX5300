// Test program for the MD_YX5300 library
//
// Menu driven interface using the Serial Monitor to test individual functions.
//

#ifndef SOFTWARESERIAL
#define USE_SOFTWARESERIAL 1   ///< Set to 1 to use SoftwareSerial library, 0 for native serial port
#endif

#include <MD_YX5300.h>

#if USE_SOFTWARESERIAL
#include <SoftwareSerial.h>

// Connections for serial interface to the YX5300 module
const uint8_t ARDUINO_RX = 4;    // connect to TX of MP3 Player module
const uint8_t ARDUINO_TX = 5;    // connect to RX of MP3 Player module

SoftwareSerial  MP3Stream(ARDUINO_RX, ARDUINO_TX);  // MP3 player serial stream for comms
#define Console Serial           // command processor input/output stream
#else
#define MP3Stream Serial2  // Native serial port - change to suit the application
#define Console   Serial   // command processor input/output stream
#endif

// Define YX5300 global variables
MD_YX5300 mp3(MP3Stream);
bool bUseCallback = true; // use callbacks?
bool bUseSynch = false;   // use synchronous? 

void cbResponse(const MD_YX5300::cbData *status)
// Used to process device responses either as a library callback function
// or called locally when not in callback mode.
{
  if (bUseSynch)
    Console.print(F("\nSync Status: "));
  else
    Console.print(F("\nCback status: "));

  switch (status->code)
  {
  case MD_YX5300::STS_OK:         Console.print(F("STS_OK"));         break;
  case MD_YX5300::STS_TIMEOUT:    Console.print(F("STS_TIMEOUT"));    break;
  case MD_YX5300::STS_VERSION:    Console.print(F("STS_VERSION"));    break;
  case MD_YX5300::STS_CHECKSUM:   Console.print(F("STS_CHECKSUM"));    break;
  case MD_YX5300::STS_TF_INSERT:  Console.print(F("STS_TF_INSERT"));  break;
  case MD_YX5300::STS_TF_REMOVE:  Console.print(F("STS_TF_REMOVE"));  break;
  case MD_YX5300::STS_ERR_FILE:   Console.print(F("STS_ERR_FILE"));   break;
  case MD_YX5300::STS_ACK_OK:     Console.print(F("STS_ACK_OK"));     break;
  case MD_YX5300::STS_FILE_END:   Console.print(F("STS_FILE_END"));   break;
  case MD_YX5300::STS_INIT:       Console.print(F("STS_INIT"));       break;
  case MD_YX5300::STS_STATUS:     Console.print(F("STS_STATUS"));     break;
  case MD_YX5300::STS_EQUALIZER:  Console.print(F("STS_EQUALIZER"));  break;
  case MD_YX5300::STS_VOLUME:     Console.print(F("STS_VOLUME"));     break;
  case MD_YX5300::STS_TOT_FILES:  Console.print(F("STS_TOT_FILES"));  break;
  case MD_YX5300::STS_PLAYING:    Console.print(F("STS_PLAYING"));    break;
  case MD_YX5300::STS_FLDR_FILES: Console.print(F("STS_FLDR_FILES")); break;
  case MD_YX5300::STS_TOT_FLDR:   Console.print(F("STS_TOT_FLDR"));   break;
  default: Console.print(F("STS_??? 0x")); Console.print(status->code, HEX); break;
  }

  Console.print(F(", 0x"));
  Console.print(status->data, HEX);
}

void setCallbackMode(bool b)
{
  bUseCallback = b;
  Console.print(F("\n>Callback "));
  Console.print(b ? F("ON") : F("OFF"));
  mp3.setCallback(b ? cbResponse : nullptr);
}

void setSynchMode(bool b)
{
  bUseSynch = b;
  Console.print(F("\n>Synchronous "));
  Console.print(b ? F("ON") : F("OFF"));
  mp3.setSynchronous(b);
}

char getNextChar(bool block = false)
{
  char c = '\0';

  if (!block) return(Console.available() ? Console.read() : c);

  while (c == '\0')
  {
    if (Console.available())
      c = Console.read();
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
  Console.print(F("\n[MD_YX5300 Test Menu]"));
  Console.print(F("\nh,?\thelp"));
  Console.print(F("\n\np!\tPlay"));
  Console.print(F("\npyyy\tPlay file index yyy (0-255)"));
  Console.print(F("\npp\tPlay Pause"));
  Console.print(F("\npz\tPlay Stop"));
  Console.print(F("\np>\tPlay Next"));
  Console.print(F("\np<\tPlay Previous"));
  Console.print(F("\nptxxyyy\tPlay Track folder xx, file yyy"));
  Console.print(F("\npfxx\tPlay loop folder xx"));
  Console.print(F("\npxaa\tPlay shuffle folder aa"));
  Console.print(F("\npryyy\tPlay loop file index yyy"));
  Console.print(F("\n\nv+\tVolume up"));
  Console.print(F("\nv-\tVolume down"));
  Console.print(F("\nvxx\tVolume set xx (max 30)"));
  Console.print(F("\nvb\tVolume Mute on (b=1), off (b=0)"));
  Console.print(F("\n\nqe\tQuery equalizer"));
  Console.print(F("\nqf\tQuery current file"));
  Console.print(F("\nqs\tQuery status"));
  Console.print(F("\nqv\tQuery volume"));
  Console.print(F("\nqx\tQuery folder count"));
  Console.print(F("\nqy\tQuery total file count"));
  Console.print(F("\nqzxx\tQuery files count in folder xx"));
  Console.print(F("\n\ns\tSleep"));
  Console.print(F("\nw\tWake up"));
  Console.print(F("\nen\tEqualizer type n"));
  Console.print(F("\nxb\tPlay Shuffle on (b=1), off (b=0)"));
  Console.print(F("\nrb\tPlay Repeat on (b=1), off (b=0)"));
  Console.print(F("\nz\tReset"));
  Console.print(F("\nyb\tSynchronous mode on (b=1), off (b=0)"));
  Console.print(F("\ncb\tCallback mode on (b=1), off (b=0)"));
  Console.print(F("\n\n"));
}

bool processPlay(void)
// Process the second level character(s) for the Play options
{
  char c = getNextChar(true);

  switch (toupper(c))
  {
    case '!': Console.print(F("\n> Play Start")); return(mp3.playStart());
    case 'P': Console.print(F("\n>Play Pause")); return(mp3.playPause());
    case 'Z': Console.print(F("\n>Play Stop")); return(mp3.playStop());
    case '>': Console.print(F("\n>Play Next")); return(mp3.playNext());
    case '<': Console.print(F("\n>Play Prev")); return(mp3.playPrev());
    case '0'...'9':
      {
        uint8_t t = getNum(c, 3);
        Console.print(F("\n>Play Track "));
        Console.print(t);
        return(mp3.playTrack(t));
      }

    case 'T':
      {
        uint8_t fldr = getNum('\0', 2);
        uint8_t file = getNum('\0', 3);
        Console.print(F("\n>Play Specific Fldr "));
        Console.print(fldr);
        Console.print(F(", "));
        Console.print(file);
        return(mp3.playSpecific(fldr, file));
      }

    case 'F':
      {
        uint8_t fldr = getNum('\0', 2);
        Console.print(F("\n>Play Folder "));
        Console.print(fldr);
        return(mp3.playFolderRepeat(fldr));
      }

    case 'X':
    {
      uint8_t fldr = getNum('\0', 2);
      Console.print(F("\n>Play Shuffle Folder "));
      Console.print(fldr);
      return(mp3.playFolderShuffle(fldr));
    }

    case 'R':
    {
      uint8_t file = getNum('\0', 3);
      Console.print(F("\n>Play File repeat "));
      Console.print(file);
      return(mp3.playTrackRepeat(file));
    }

    default: Console.print(F("\n>Play ?")); Console.print(c); break;
  }

  return(false);
}

bool processVolume(void)
// Process the second level character(s) for the Volume options
{
  char c = getNextChar(true);

  switch (toupper(c))
  {
  case '+': Console.print(F("\n>Volume Up"));  return(mp3.volumeInc());
  case '-': Console.print(F("\n>Volume Down"));  return(mp3.volumeDec());
  case 'M':
    {
      uint8_t cmd = getNum('\0', 1);
      Console.print(F("\n>Volume Enable "));
      Console.print(cmd);
      return(mp3.volumeMute(cmd != 0));
    }

  default:
    {
      uint16_t v = getNum(c, 2);
      Console.print(F("\n>Volume ")); 
      Console.print(v);
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
  case 'E': Console.print(F("\n>Query Equalizer"));  return(mp3.queryEqualizer());
  case 'F': Console.print(F("\n>Query File"));       return(mp3.queryFile());
  case 'S': Console.print(F("\n>Query Status"));     return(mp3.queryStatus());
  case 'V': Console.print(F("\n>Query Volume"));     return(mp3.queryVolume());
  case 'X': Console.print(F("\n>Query Folder Count"));  return(mp3.queryFolderCount());
  case 'Y': Console.print(F("\n>Query Tracks Count"));  return(mp3.queryFilesCount());
  case 'Z': 
    {
      uint8_t fldr = getNum('\0', 2);
      Console.print(F("\n>Query Folder Files Count "));
      Console.print(fldr);
      return(mp3.queryFolderFiles(fldr));
    }

  default: Console.print(F("\n>Query ?")); Console.print(c);
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

    case 'S': Console.print(F("\n>Sleep"));   bRet = mp3.sleep();  break;
    case 'W': Console.print(F("\n>Wake up")); bRet = mp3.wakeUp(); break;
    case 'Z': Console.print(F("\n>Reset"));   bRet = mp3.reset();  break;
    case 'E': 
    {
      uint8_t e = getNum('\0', 1);
      Console.print(F("\n>Equalizer "));
      Console.print(e);
      return(mp3.equalizer(e));
    }

    case 'X':
    {
      uint8_t cmd = getNum('\0', 1);
      Console.print(F("\n>Shuffle "));
      Console.print(cmd);
      return(mp3.shuffle(cmd != 0));
    }

    case 'R':
    {
      uint8_t cmd = getNum('\0', 1);
      Console.print(F("\n>Repeat "));
      Console.print(cmd);
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

    default: Console.print(F("\n>Command ?")); Console.print(c); break;
    }
  }

  return(bRet);
}

void setup()
{
  // YX5300 Serial interface
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
  mp3.begin();
  setCallbackMode(bUseCallback);
  setSynchMode(bUseSynch);

  // command line interface
  Console.begin(57600);
  help();
}

void loop()
{
  if (processCmd() && !bUseCallback)
    cbResponse(mp3.getStatus());

  mp3.check();
}
