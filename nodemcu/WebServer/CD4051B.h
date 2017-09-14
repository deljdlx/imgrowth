#ifndef CD4051B_H
#define CD4051B_H


class CD4051B
{
	int pinA;
	int pinB;
	int pinC;


	public: CD4051B(int pinA, int pinB, int pinC);

	public: int enable(int pin);

};







#endif
