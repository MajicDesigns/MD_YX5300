/***********************************************************/
//Demo for the Serial MP3 Player by Catalex
//Hardware: Serial MP3 Player *1
//               Touch Sensor *1 http://www.dx.com/p/323904
//               Rotary Angle Sensor *1 http://www.dx.com/p/323737
//Board:  Arduino UNO R3
//IDE:	  Arduino-1.0
//Function:  In the process that the Rotation Angle Sensor is rotated from
//               the 'Min' side to the 'Max' side, the volume is gradually greater.
//               If you touch the Touch Sensor,it will play or pause.
//Store: http://www.aliexpress.com/store/1199788
//          http://www.dx.com/
#include <SoftwareSerial.h>

#define ARDUINO_RX 5//should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 6//connect to RX of the module
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);

unsigned char playmode = 1; 
  #define PLAY  1
  #define PAUSE 0
static int8_t Send_buf[8] = {0} ;
  
/************Command byte**************************/
#define CMD_NEXT_SONG 0X01
#define CMD_PREV_SONG 0X02
#define CMD_PLAY_W_INDEX 0X03
#define CMD_VOLUME_UP 0X04
#define CMD_VOLUME_DOWN 0X05
#define CMD_SET_VOLUME 0X06
#define CMD_SINGLE_CYCLE_PLAY 0X08
#define CMD_SEL_DEV 0X09
  #define DEV_TF 0X02
#define CMD_SLEEP_MODE 0X0A
#define CMD_WAKE_UP 0X0B
#define CMD_RESET 0X0C
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_PLAY_FOLDER_FILE 0X0F
#define CMD_STOP_PLAY 0X16
#define CMD_FOLDER_CYCLE 0X17
#define CMD_SET_SINGLE_CYCLE 0X19
  #define SINGLE_CYCLE_ON 0X00
  #define SINGLE_CYCLE_OFF 0X01
#define CMD_SET_DAC 0X1A
  #define DAC_ON  0X00
  #define DAC_OFF 0X01
#define CMD_PLAY_W_VOL 0X22

/*********************************************************************/
/*macro definitions of Rotary angle sensor and LED pin*/
#define ROTARY_ANGLE_SENSOR A0

#define ADC_REF 5//reference voltage of ADC is 5v
#define VCC     5    //the default value of VCC of the control interface is 5v
#define FULL_ANGLE 280//full value of the rotary angle is 280 degrees

void setup() 
{
	Serial.begin(9600);
	mySerial.begin(9600);
	delay(500);
	attachInterrupt(0, playOrPause, RISING);//pin2 -> INT0, and the Touch Sensor 
                                          //is connected with pin2 of Arduino
    sendCommand(CMD_SEL_DEV, DEV_TF);  
	delay(200);
	sendCommand(CMD_PLAY_W_VOL, 0X0F01);
}
static int8_t pre_vol = 0x0f; 
void loop() 
{
  int degrees;
  degrees = getDegree();
  	
  int8_t volume;
	/*The degrees is 0~280, should be converted to be 0~255 to control the*/
	/*brightness of LED */
  volume = map(degrees, 0, 280, 0, 30); 
  if(volume != pre_vol)
  {
    sendCommand(CMD_SET_VOLUME, volume);
    pre_vol = volume;
  }
  delay(100);
}

void sendCommand(int8_t command, int16_t dat)
{
  delay(20);
  Send_buf[0] = 0x7e; //
  Send_buf[1] = 0xff; //
  Send_buf[2] = 0x06; //
  Send_buf[3] = command; //
  Send_buf[4] = 0x00;//
  Send_buf[5] = (int8_t)(dat >> 8);//datah
  Send_buf[6] = (int8_t)(dat); //datal
  Send_buf[7] = 0xef; //
  for(uint8_t i=0; i<8; i++)//
  {
    mySerial.write(Send_buf[i]) ;
  }
}
/********************************************************************************/
/*Function: Get the angle between the mark on the potentiometer cap and the starting position	*/
/*Parameter:-void                                                                                                          */
/*Return:     -int,the range of degrees is 0~280                                                                 */
int getDegree()
{
	int sensor_value = analogRead(ROTARY_ANGLE_SENSOR);
	float voltage;
	voltage = (float)sensor_value*ADC_REF/1023;
	float degrees = (voltage*FULL_ANGLE)/VCC;
	return degrees;
}

/*Interrupt service routine*/
void playOrPause()
{
  cli();
  if(playmode == PLAY)
  {
  	playmode = PAUSE;
	sendCommand(CMD_PAUSE,0);
  }
  else
  {
  	playmode = PLAY;
	sendCommand(CMD_PLAY,0);
  }
  sei();
}


