/*
 * MGColor.h
 *
 *  Created on: 18.07.2010
 *      Author: Администратор
 */

#ifndef MGCOLOR_H_
#define MGCOLOR_H_

class MGColor
{
private:
	float fcomponents[4];
	unsigned char bcomponents[4];

public:

	static MGColor whiteColor();
	static MGColor redColor();
	static MGColor greenColor();
	static MGColor blueColor();
	static MGColor blackColor();
	static MGColor transparentColor();

	float* getFloatComponents();
	unsigned char* getByteComponents();

	float getR();
	float getG();
	float getB();
	float getA();
	void setR(float v);
	void setG(float v);
	void setB(float v);
	void setA(float v);
	unsigned char getRB();
	unsigned char getGB();
	unsigned char getBB();
	unsigned char getAB();
	void setR(unsigned char v);
	void setG(unsigned char v);
	void setB(unsigned char v);
	void setA(unsigned char v);

	MGColor();
	MGColor(float r, float g, float b, float a);
	MGColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	MGColor(int r, int g, int b, int a);
	~MGColor();
};

#endif /* MGCOLOR_H_ */
