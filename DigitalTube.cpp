/*
 * Written by IOXhop : www.ioxhop.com
 * Author : Sonthaya Nongnuch (www.fb.me/maxthai)
 */
#ifndef DIGITALTUBE_CPP
#define DIGITALTUBE_CPP

#include <Arduino.h>
#include "DigitalTube.h"

#include <string.h>

#if defined(ESP8266)
os_timer_t __TudeTimer;
#elif defined(ESP32)
hw_timer_t * __TudeTimer = NULL;
#endif // ESP8266

DigitalTube::DigitalTube() {
	_DigiTubeConfig.pin_sclk = 4;
	_DigiTubeConfig.pin_rclk = 5;
	_DigiTubeConfig.pin_dio = 6;
}


DigitalTube::DigitalTube(int sclk, int rclk, int dio) {
	_DigiTubeConfig.pin_sclk = sclk;
	_DigiTubeConfig.pin_rclk = rclk;
	_DigiTubeConfig.pin_dio = dio;
}

void DigitalTube::begin() {
	pinMode(_DigiTubeConfig.pin_sclk, OUTPUT);
	pinMode(_DigiTubeConfig.pin_rclk, OUTPUT);
	pinMode(_DigiTubeConfig.pin_dio, OUTPUT);
	
	digitalWrite(_DigiTubeConfig.pin_rclk, HIGH);

#if defined(ESP8266)
	os_timer_setfn(&__TudeTimer, [](void *pArg) {
#elif defined(ESP32)
	__TudeTimer = timerBegin(0, 80, true);
	timerAttachInterrupt(__TudeTimer, [](){
#else
	Timer1.initialize(2000);
	Timer1.attachInterrupt([](){
#endif // ESP8266 or ESP32
		digitalWrite(_DigiTubeConfig.pin_rclk, LOW);
		shiftOut(_DigiTubeConfig.pin_dio, _DigiTubeConfig.pin_sclk, LSBFIRST, _DigiTubeConfig.characters[_DigiTubeConfig.disp[_DigiTubeConfig.NextDigi]]);
		shiftOut(_DigiTubeConfig.pin_dio, _DigiTubeConfig.pin_sclk, LSBFIRST, 0x80>>_DigiTubeConfig.NextDigi);
		digitalWrite(_DigiTubeConfig.pin_rclk, HIGH);
		_DigiTubeConfig.NextDigi = (_DigiTubeConfig.NextDigi >= 3 ? 0 : _DigiTubeConfig.NextDigi+1);
#if defined(ESP8266)
	}, NULL);
	os_timer_arm(&__TudeTimer, 2, true);
#elif defined(ESP32)
	}, true);
	timerAlarmWrite(__TudeTimer, 2000, true);
	timerAlarmEnable(__TudeTimer);
#else
	});
#endif // ESP8266 or ESP32
}

void DigitalTube::print(char* text) {

//	strrev(text);

	short n_digit = 0;
	for (int i=0; i<strlen(text) && (n_digit<4 ? true : text[i]=='.') ; i++) {
		char val = text[i];
		if (val == '.'){
			if(i == 7) val = 74;
			else {
				_DigiTubeConfig.disp[(3-n_digit)+1] += 10;
			}
		}
		else if((val >= 32)&&(val <= 47)) val = (val == 45) ? 72 : 73;
		else if((val >= '0')&&(val <= '9')) val -= 48;
		else if((val >= 'A')&&(val <= 'Z')) val -= 45;
		else if((val >= 'a')&&(val <= 'z')) val -= 51;
    
    	if (*(text+i) != '.') _DigiTubeConfig.disp[3-(n_digit++)] = val;
	}
//	strrev(text);
}

void DigitalTube::print(double number) {
	char char_array[5] = {int(number/10)%100+'0', int(number)%10+'0', '.', int(number*10)%10+'0', int(number*100)%10+'0' };
	print(char_array);
}

#endif
