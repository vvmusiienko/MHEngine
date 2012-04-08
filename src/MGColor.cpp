/*
 * MGColor.cpp
 *
 *  Created on: 18.07.2010
 *      Author: Администратор
 */

#include "MGColor.h"
#include <string>

MGColor::MGColor() {
	setR((unsigned char)255);
	setG((unsigned char)255);
	setB((unsigned char)255);
	setA((unsigned char)255);
}


MGColor::MGColor(float r, float g, float b, float a) {
	setR(r);
	setG(g);
	setB(b);
	setA(a);
}


MGColor::MGColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	setR(r);
	setG(g);
	setB(b);
	setA(a);
}


MGColor::MGColor(int r, int g, int b, int a) {
	setR((unsigned char)r);
	setG((unsigned char)g);
	setB((unsigned char)b);
	setA((unsigned char)a);
}

MGColor MGColor::blackColor() {
	return MGColor((unsigned char)0, (unsigned char)0, (unsigned char)0, (unsigned char)255);
}

MGColor MGColor::whiteColor() {
	return MGColor((unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255);
}


MGColor MGColor::redColor() {
	return MGColor((unsigned char)255, (unsigned char)0, (unsigned char)0, (unsigned char)255);
}


MGColor MGColor::greenColor() {
	return MGColor((unsigned char)0, (unsigned char)255, (unsigned char)0, (unsigned char)255);
}


MGColor MGColor::blueColor() {
	return MGColor((unsigned char)0, (unsigned char)0, (unsigned char)255, (unsigned char)255);
}


MGColor MGColor::transparentColor() {
	return MGColor((unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)0);
}


float* MGColor::getFloatComponents() {
	return fcomponents;
}


unsigned char* MGColor::getByteComponents() {
	return bcomponents;
}


float MGColor::getR() {
	return fcomponents[0];
}
float MGColor::getG() {
	return fcomponents[1];
}
float MGColor::getB() {
	return fcomponents[2];
}
float MGColor::getA() {
	return fcomponents[3];
}
void MGColor::setR(float v){
	fcomponents[0] = v;
	bcomponents[0] = (unsigned char)(v * 255);
}
void MGColor::setG(float v){
	fcomponents[1] = v;
	bcomponents[1] = (unsigned char)(v * 255);
}
void MGColor::setB(float v){
	fcomponents[2] = v;
	bcomponents[2] = (unsigned char)(v * 255);
}
void MGColor::setA(float v){
	fcomponents[3] = v;
	bcomponents[3] = (unsigned char)(v * 255);
}
unsigned char MGColor::getRB(){
	return bcomponents[0];
}
unsigned char MGColor::getGB(){
	return bcomponents[1];
}
unsigned char MGColor::getBB(){
	return bcomponents[2];
}
unsigned char MGColor::getAB(){
	return bcomponents[3];
}
void MGColor::setR(unsigned char v){
	bcomponents[0] = v;
	fcomponents[0] = 0.003921568627450980392156862745098f * v;
}
void MGColor::setG(unsigned char v){
	bcomponents[1] = v;
	fcomponents[1] = 0.003921568627450980392156862745098f * v;
}
void MGColor::setB(unsigned char v){
	bcomponents[2] = v;
	fcomponents[2] = 0.003921568627450980392156862745098f * v;
}
void MGColor::setA(unsigned char v) {
	bcomponents[3] = v;
	fcomponents[3] = 0.003921568627450980392156862745098f * v;
}


MGColor::~MGColor() {

}
