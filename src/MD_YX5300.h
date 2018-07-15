#ifndef MD_YX5300_H
#define MD_YX5300_H

#include <Arduino.h>
#include <SoftwareSerial.h>

/**
* \file
* \brief Main header file for the MD_MAXPanel library
*/

/**
\mainpage Arduino LED Matrix Panel Library
The MD_YX5300 Library
---------------------
This library implements functions that allows cascaded MAX72xx LED modules
(64 individual LEDs)to be used for LED matrix panels, allowing the programmer
to use the LED matrix as an pixel addressable display device, as shown in the
photo below.

![MD_MAXPanel Display Panel] (MAXPanel_Display.jpg "MD_MAXPanel Display Panel")

Topics
------
- \subpage pageSoftware
- \subpage pageRevisionHistory
- \subpage pageCopyright
- \subpage pageDonation

\page pageDonation Support the Library
If you like and use this library please consider making a small donation using [PayPal](https://paypal.me/MajicDesigns/4USD)

\page pageCopyright Copyright
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

\page pageRevisionHistory Revision History
Jul 2018 version 1.0.0
- First Release

\page pageSoftware Software Library
The Library
-----------
The library implements functions that allow the MAX72xx matrix modules
to be used cascaded and built up into LED matrix panels. This allows the
programmer to control the individual panel LEDs using cartesian coordinates.
The library provides support for standard graphics elements (such as lines,
triangles, rectangles, circles) and text.

The library is relies on the related MD_MAX72xx library to provide the
device control elements.
*/

/**
* Core object for the MD_YX5300 library
*/
class MD_YX5300
{
public:
 /**
  * Status code enumerated type specification.
  *
  * Used by the cbData status structure in the code field to 
  * identify the type of status data contained.
  */
  enum status_t
  {
    STS_OK = 0x00,         ///< No error
    STS_TIMEOUT = 0x01,    ///< Timeout on response message
    STS_TF_INSERT = 0x3a,  ///< TF Card was inserted
    STS_TF_REMOVE = 0x3b,  ///< TF card was removed
    STS_ERR = 0x40,        ///< Generic error occurred
    STS_ACK_OK = 0x41,     ///< Message acknowledged ok
    STS_FILE_END = 0x3d,   ///< Track/file has ended
    STS_STATUS = 0x42,     ///< Current status
    STS_VOLUME = 0x43,     ///< Current volume level
    STS_TOT_FILES = 0x48,  ///< TF Total file count
    STS_PLAYING = 0x4c,    ///< Current file playing
    STS_FLDR_FILES = 0x4e, ///< Total number of files in the folder
    STS_TOT_FLDR = 0x4f,   ///< Total number of folders
  };

 /**
  * Status return structure specification.
  *
  * Used to return (through callback or getStatus() method) the
  * status value of the last device request.
  */
  struct cbData
  {
    status_t code;    ///< code for operation
    uint16_t data;    ///< data returned
  };

  /**
  * Class Constructor.
  *
  * Instantiate a new instance of the class. The parameters passed are used to
  * connect the software to the hardware.
  *
  * \param pinRx The pin for receiving serial data, connected to the device Tx pin.
  * \param pinTx The pin for sending serial data, connected to the device Rx pin.
  */
  MD_YX5300(uint8_t pinRx, uint8_t pinTx) :
    _S(pinRx, pinTx), _timeout(200), _synch(true), _cbStatus(nullptr) {};

 /**
  * Class Destructor.
  *
  * Released any necessary resources and and does the necessary to clean up once 
  * the object is no longer required.
  */
  ~MD_YX5300(void) {};
  
 /**
  * Initialize the object.
  *
  * Initialize the object data. This needs to be called during setup() to initialize
  * new data for the class that cannot be done during the object creation.
  */
  void begin(void);

 /**
  * Receive and process serial messages.
  *
  * The check function should be called repeatedly in loop() to allow the 
  * library to receive and process device messages. The MP3 device can send
  * messages as a reponse to a request or unsolicited to inform of state changes,
  * such a track play completing. A true value returned indicates that a message 
  * has been received and the status is ready to be processed. 
  * 
  * With callbacks disabled, the application should use getStatus() to retrieve 
  * and process this status. With callbacks enabled, the check() will cause the 
  * callback to be processed before returning to the application.
  *
  * If synchronous mode is enabled only unsolicited messages will be processed
  * through the call to check(), as the other messages will have been processed 
  * synchronously as part of the request.
  *
  * \sa cbData, setCallback(), setSynchronous(), getStatus()
  *
  * \return true if a message has been received and processed, false otherwise.
  */
  bool check(void);

  //--------------------------------------------------------------
  /** \name Methods for object management.
  * @{
  */
  
 /**
  * Set or clear Synchronous mode.
  *
  * Set/reset synchronous mode operation for the library. In synchronous mode,
  * the library will wait for device response message immediately after sending 
  * the request. On return the status result of the operation will be available
  * using the getStatus() method. If synchronous mode is disabled, then the code 
  * must be retrieved using getStatus() when the check() method returns true.
  *
  * Synchronous mode and callbacks are set and operate independently.
  *
  * \sa getStatus(), check(), setCallback()
  *
  * \param b true to set the mode, false to disable.
  * \return No return value.
  */
  inline void setSynchronous(bool b) { _synch = b; }

 /**
  * Set serial response timeout.
  *
  * Set the device reponse timeout in milliseconds. If a message is not received
  * within this time a timeout error status will be generated. The default timeout
  * is 200 milliseconds.
  *
  * \param t timeout in milliseconds
  * \return No return value.
  */
  inline void setTimeout(uint32_t t) { _timeout = t; }

 /**
  * Set the status callback.
  *
  * Set status callback function that will be invoked when a device status serial
  * message is received. The callback will include the status encoded in the serial 
  * message. If callback is not required, the callback should be set to nullptr 
  * (the default). The callback is invoked when the last character of response message
  * is received from the device.
  *
  * Callbacks and synchronous mode are set and operate independently.
  *
  * \sa getStatus(), setSynchronous()
  *
  * \param cb the address of the callback function in user code.
  * \return No return value.
  */
  inline void setCallback(void(*cb)(const cbData *data)) { _cbStatus = cb; }

 /**
  * Get the status of last device operation.
  *
  * Get the status of the last requested operation of the MP3 device. A pointer
  * to the library's status block is returned to the application. The status code is 
  * one of the status_t enumerated types and the data component returned will depend
  * status value. The code and data depend on the original request made of the device.
  * 
  * \sa cbData, getStsCode(), getStsData()
  *
  * \return Pointer to the 'last status' data structure.
  */
  inline const cbData *getStatus(void) { return(&_status); }

 /**
  * Get the status code of last device operation.
  *
  * Get the status code of the of the last MP3 operation. The status code is
  * one of the status_t enumerated types. 
  *
  * \sa cbData, getStatus(), getStsData()
  *
  * \return The requested status code.
  */
  inline status_t getStsCode(void) { return(_status.code); }

 /**
  * Get the status data of last device operation.
  *
  * Get the status data of the of the last MP3 operation. The meaning of the
  * data will depend on the associated status code.
  *
  * \sa cbData, getStatus(), getStsCode()
  *
  * \return The requested status data.
  */
  inline uint16_t getStsData(void) { return(_status.data); }

  /** @} */

  //--------------------------------------------------------------
  /** \name Methods for device management.
  * @{
  */

 /**
  * Set the readback device.
  *
  * Set the readback device to the specifed type. Currently the only type 
  * available is a TF device (CMD_OPT_TF). This is set during the class 
  * initialisation ans should not be required in application code.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \param devId the device id for the type of device. Currently only 0x02 (TF).
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool device(uint8_t devId) { return sendRqst(CMD_SEL_DEV, PKT_DATA_NUL, devId); }

 /**
  * Set sleep mode.
  *
  * Enables the MP3 player sleep mode. The device will stop playing but still respond to 
  * serial messages. Use wakeUp() to disable sleep mode.
  *
  * \sa wakeUp(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool sleep(void) { return sendRqst(CMD_SLEEP_MODE, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Set awake mode.
  *
  * Wakes up the MP3 player from sleep mode. Use sleep() to enablke sleep mode.
  *
  * \sa sleep(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool wakeUp(void) { return sendRqst(CMD_WAKE_UP, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Reset the MP3 player.
  *
  * Put the MP3 player into reste mode. The payer will return to its power up state.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool reset(void) { return sendRqst(CMD_RESET, PKT_DATA_NUL, PKT_DATA_NUL); }

  /** @} */

  //--------------------------------------------------------------
 /** \name Methods for controlling playing MP3 files.
  * @{
  */

 /**
  * Play the next MP3 file.
  * 
  * Play the next MP3 file in numeric order.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playNext(void) { return sendRqst(CMD_NEXT_SONG, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Play the previous MP3 file.
  *
  * Play the previous MP3 file in numeric order.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playPrev(void) { return sendRqst(CMD_PREV_SONG, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Stop playing the current MP3 file.
  *
  * Stop playing the currenbt MP3 file and cancel the current playing mode.
  * playPause() should be used for a temporary playing stop.
  *
  * \sa playPause(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playStop(void) { return sendRqst(CMD_STOP_PLAY, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Pause playing the current MP3 file.
  *
  * Pause playing playing the current MP3 file. playStart() should follow to restart
  * the same MP3 file. playStop() should be use to stop playing and abort current 
  * playing mode.
  *
  * \sa playPause(), playStart(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playPause(void) { return sendRqst(CMD_PAUSE, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Restart playing the current MP3 file.
  *
  * Restart playing playing the current MP3 file after a playPause().
  *
  * \sa playPause(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playStart(void) { return sendRqst(CMD_PLAY, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Play a specific file.
  *
  * Play a file by specifying the fiule index number.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \param t the file indexd (0-255) to be played.
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playTrack(uint8_t t) { return sendRqst(CMD_PLAY_WITH_INDEX, PKT_DATA_NUL, t); }

 /**
  * Control shuffle playing mode.
  *
  * Set or reset the playing mode to/from random shuffle.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \param b true to enable mode, false to disable.
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playShuffle(bool b) { return sendRqst(CMD_SHUFFLE_PLAY, PKT_DATA_NUL, b ? CMD_OPT_ON : CMD_OPT_OFF); }

 /**
  * Control repeat play mode (current file).
  *
  * Set or reset the repeat playing mode for the current track.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \param b true to enable mode, false to disable.
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playRepeat(bool b) { return sendRqst(CMD_SET_SNGL_CYCL, PKT_DATA_NUL, b ? CMD_OPT_ON : CMD_OPT_OFF); }

 /**
  * Play a specific file in a folder.
  *
  * Play a file by specifying the folder and file to be played.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \param fldr the folder number containing the files.
  * \param file the file within the folder.
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playSpecific(uint8_t fldr, uint8_t file) { return sendRqst(CMD_PLAY_FOLDER_FILE, fldr, file); }

 /**
  * Control repeat play mode (specific folder).
  *
  * Set or reset the repeat playing mode for the specified folder track.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \param folder the folder number containing the files to repeat.
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool playFolderRepeat(uint8_t folder) { return sendRqst(CMD_FOLDER_CYCLE, folder, PKT_DATA_NUL); }

  /** @} */

  //--------------------------------------------------------------
  /** \name Methods for controlling MP3 output volume.
  * @{
  */

 /**
  * Set specified volume.
  *
  * Set the output volume to the specified level. Maximum volume is 30.
  *
  * \sa volumeInc(), volumeDec(), check(), getStatus(), setSynchronous()
  *
  * \param vol the volume specified [0..30].
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool volume(uint8_t vol) { return sendRqst(CMD_SET_VOLUME, PKT_DATA_NUL, (vol > MAX_VOLUME ? MAX_VOLUME : vol)); }

 /**
  * Increment the volume.
  *
  * Increment the output volume by 1.
  *
  * \sa volume(), volumeDec(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool volumeInc(void) { return sendRqst(CMD_VOLUME_UP, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Decrement the volume.
  *
  * Decrement the output volume by 1.
  *
  * \sa volume(), volumeInc(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool volumeDec(void) { return sendRqst(CMD_VOLUME_DOWN, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Mute the sound output.
  *
  * Mute the sound output by supressing the output from the DAC. The MP3 file
  * will continue playing but will not be heard. To temporarily halt the playing use
  * playPause().
  *
  * \sa playPause(), check(), getStatus(), setSynchronous()
  *
  * \param b true to enable mode, false to disable.
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool volumeMute(bool b) { return sendRqst(CMD_SET_DAC, PKT_DATA_NUL, b ? CMD_OPT_OFF : CMD_OPT_ON); }

 /**
  * Query the current volume setting.
  *
  * Request the current volume setting from the device. This is a wrapper alternative 
  * for queryVolume().
  *
  * \sa queryVolume(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool volumeQuery(void) { return sendRqst(CMD_QUERY_VOLUME, PKT_DATA_NUL, PKT_DATA_NUL); }

  /** @} */

  //--------------------------------------------------------------
  /** \name Methods for querying MP3 device parameters.
  * @{
  */
  inline bool queryStatus(void) { return sendRqst(CMD_QUERY_STATUS, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Query the current volume setting.
  *
  * Request the current volume setting from the device. This is a wrapper alternative
  * for volumeQuery().
  *
  * \sa volumeQuery(), check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool queryVolume(void) { return volumeQuery(); }

 /**
  * Query the current equalizer setting.
  *
  * Request the current equalizer setting from the device.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool queryEqualizer(void) { return sendRqst(CMD_QUERY_EQUALIZER, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Query the number of files in the specified folder.
  *
  * Request the count of files in the specified folder number.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \param folder the folder number whose files are to be counted.
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool queryFolderTracks(uint8_t folder) { return sendRqst(CMD_QUERY_FLDR_FILES, PKT_DATA_NUL, folder); }

 /**
  * Query the total number of folders.
  *
  * Request the count of folder on the TF device.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool queryFolderCount(void) { return sendRqst(CMD_QUERY_TOT_FLDR, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Query the total number of tracks.
  *
  * Request the count of tracks on the TF device.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool queryTracksCount(void) { return sendRqst(CMD_QUERY_TOT_FILES, PKT_DATA_NUL, PKT_DATA_NUL); }

 /**
  * Query the track currently playing.
  *
  * Request the index of the track currentlybeing played.
  *
  * \sa check(), getStatus(), setSynchronous()
  *
  * \return In synchronous mode, true when the message has been received and processed. Otherwise
  *         ignore the return value and process using callback or check() and getStatus().
  */
  inline bool queryTrack(void) { return sendRqst(CMD_QUERY_PLAYING, PKT_DATA_NUL, PKT_DATA_NUL); }

  /** @} */

private:
  // Miscellaneous
  const uint8_t MAX_VOLUME = 32;

  // Enumerated type for serial message commands
  enum cmdSet_t
  {
    CMD_NUL = 0x00,             ///< No command
    CMD_NEXT_SONG = 0x01,       ///< Play next song
    CMD_PREV_SONG = 0x02,       ///< Play previous song
    CMD_PLAY_WITH_INDEX = 0x03, ///< Play song with index number
    CMD_VOLUME_UP = 0x04,       ///< Volume increase by one
    CMD_VOLUME_DOWN = 0x05,     ///< Volume decrease by one
    CMD_SET_VOLUME = 0x06,      ///< Set the volume to level specified
    CMD_SNG_CYCL_PLAY = 0x08,   ///< Single cycler play the first song
    CMD_SEL_DEV = 0x09,         ///< Select storage device to TF card
    CMD_SLEEP_MODE = 0x0a,      ///< Chip enters sleep mode
    CMD_WAKE_UP = 0x0b,         ///< Chip wakes up from sleep mode
    CMD_RESET = 0x0c,           ///< Chip reset
    CMD_PLAY = 0x0d,            ///< Playback restart
    CMD_PAUSE = 0x0e,           ///< Playback is paused
    CMD_PLAY_FOLDER_FILE = 0x0f,///< Play the song with the specified folder and index number
    CMD_STOP_PLAY = 0x16,       ///< Playback is stopped
    CMD_FOLDER_CYCLE = 0x17,    ///< Cycle playback from specified folder
    CMD_SHUFFLE_PLAY = 0x18,    ///< Playback shuffle mode
    CMD_SET_SNGL_CYCL = 0x19,   ///< Set single cycle payback on/off
    CMD_SET_DAC = 0x1a,         ///< DAC on/off control
    CMD_PLAY_W_VOL = 0x22,      ///< Volume set to the specified level
    CMD_QUERY_STATUS = 0x42,    ///< Query Device Status
    CMD_QUERY_VOLUME = 0x43,    ///< Query Volume level
    CMD_QUERY_EQUALIZER = 0x44, ///< Query current equalizer
    CMD_QUERY_TOT_FILES = 0x48, ///< Query total files in all folders
    CMD_QUERY_PLAYING = 0x4c,   ///< Query which track playing
    CMD_QUERY_FLDR_FILES = 0x4e,///< Query total files in folder
    CMD_QUERY_TOT_FLDR = 0x4f,  ///< Query number of folders
  };
  
  // Command options
  const uint8_t CMD_OPT_ON = 0x00;    ///< On indicator
  const uint8_t CMD_OPT_OFF = 0x01;   ///< Off indicator
  const uint8_t CMD_OPT_DEV_TF = 0X02;///< Device option TF card

  // Protocol Message Characters
  const uint8_t PKT_SOM = 0x7e;       ///< Start of message delimiter character
  const uint8_t PKT_VER = 0xff;       ///< Version information
  const uint8_t PKT_LEN = 0x06;       ///< Data packet length in bytes (excluding SOM, EOM)
  const uint8_t PKT_FB_OFF = 0x00;    ///< Command feedback OFF
  const uint8_t PKT_FB_ON = 0x01;     ///< Command feedback ON
  const uint8_t PKT_DATA_NUL = 0x00;  ///< Packet data place marker 
  const uint8_t PKT_EOM = 0xef;       ///< End of message delimiter character

  // variables
  SoftwareSerial  _S; ///< used for communications

  void(*_cbStatus)(const cbData *data); ///< callback function
  cbData _status;     ///< callback status data

  bool _synch;        ///< synchronous (wait for response) if true
  uint32_t _timeout;  ///< timeout for return serial message

  uint8_t _bufRx[20]; ///< receive buffer for serial comms
  uint8_t _bufIdx;    ///< index for next char into _bufIdx
  uint32_t _timeSent; ///< time last serial message was sent
  bool _waitResponse; ///< true when we are waiting response to a query

  // Methods
  bool sendRqst(cmdSet_t cmd, uint8_t data1, uint8_t data2);  ///< Send serial message (Rqst)
  void processResponse(bool bTimeout = false);                ///< Process a serial response message
  void dumpMessage(uint8_t *msg, uint8_t len, char *psz);     ///< Dump a message to the debug stream
  char itoh(uint8_t i);                                       ///< Integer to Hex string
};

#endif