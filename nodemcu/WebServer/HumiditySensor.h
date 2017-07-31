#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H



class HumiditySensor
{

	int pin = 0;

	public: HumiditySensor(int pin = 0);

	public: int get(void);


};

#endif