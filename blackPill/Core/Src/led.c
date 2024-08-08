/*
 * led.c
 *
 *  Created on: Jul 22, 2024
 *      Author: User
 */


#include "led.h"


												// functions releases

void dot() {
	GPIOC->ODR&=~(1<<13);
	HAL_Delay(DOT_DELAY);
	GPIOC->ODR|=(1<<13);
	HAL_Delay(DOT_DELAY);
}
void dash() {
	GPIOC->ODR&=~(1<<13);
	HAL_Delay(DASH_DELAY);
	GPIOC->ODR|=(1<<13);
	HAL_Delay(DASH_DELAY);
}
void timeout() {
	HAL_Delay(TIMEOUT);
}

//letters
void letter_s() {
	for (int i = 0; i < 3; i++)
		dot();
	timeout();
}

void letter_p() {
	dot();
	dash();
	dash();
	dot();
	timeout();
}
void letter_o() {
	for (int i = 0; i < 3; i++)
		dash();
	timeout();
}
void letter_r() {
	dot();
	dash();
	dot();
	timeout();
}
void letter_a() {
	dot();
	dash();
	timeout();
}
void letter_d() {
	dash();
	dot();
	dot();
	timeout();
}
void letter_i() {
	dot();
	dot();
	timeout();
}
void letter_k() {
	dash();
	dot();
	dash();
	timeout();
}


//words

void word_SOS() {
	letter_s();
	letter_o();
	letter_s();
}
void word_Sporadik() {
	letter_s();
	letter_p();
	letter_o();
	letter_r();
	letter_a();
	letter_d();
	letter_i();
	letter_k();

}

