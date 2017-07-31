#ifndef POMPE_H
#define POMPE_H


class Pompe
{
	int pin = 5;


	public: Pompe(void);

	public: void start(void);

	public: void stop(void);

};

#endif