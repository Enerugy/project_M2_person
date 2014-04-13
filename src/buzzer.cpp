
/**
	@file buzzer.cpp
	@author ISE2 | Alexis G. - Kévin R. - William V. 
	@version 1.1
	@date 9 mars 2014

	@brief Classe permettant de manipuler un buzzer en volume et fréquence.
		   (cf. buzzer.hpp pour plus de détails)

*/

#include "buzzer.hpp"

#ifdef DEBUG
#include "debug.hpp"
#include <sstream>
extern Debug debug;
#endif

using namespace std;

//************************************************************************************************

const unsigned long Buzzer::_periodMax = 50000000; //  20 Hz
const unsigned long Buzzer::_periodMin =  5000000; // 200 Hz

#ifdef BEAGLEBONE
const string Buzzer::_dirPwm = "/sys/class/pwm/ehrpwm.1:0";
#else
const string Buzzer::_dirPwm = "/tmp/beaglebone/pwm_P9_14";
#endif

//------------------------------------------------------------------------------------------------
Buzzer::Buzzer(unsigned char volume, unsigned char frequency) : _volume(volume), _frequency(frequency)
{
	#ifdef DEBUG
	debug++;
	debug.open("Buzzer::Buzzer", Debug::colorYellow);
	#endif

	this->_pwm = new Pwm(Buzzer::_dirPwm);

	if (!this->_pwm->check())
	{
		this->_pwm->setPolarity(0);
		this->_pwm->getPolarity();

		this->setFrequency(frequency);
		this->setVolume(volume);

		this->_pwm->setRun(1);
		this->_pwm->getRun();

		this->_initBuzzer = true;
	}
	else
	{
		this->_initBuzzer = false;
	}

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}
Buzzer::~Buzzer()
{
	#ifdef DEBUG
	debug++;
	debug.open("Buzzer::~Buzzer", Debug::colorYellow);
	#endif

	this->_pwm->setDuty(0);
	this->_pwm->getDuty();

	this->_pwm->setRun(0);
	this->_pwm->getRun();

	this->_initBuzzer = false;

	delete this->_pwm;

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}

//------------------------------------------------------------------------------------------------
void Buzzer::setVolume(unsigned char volume)
{
	long duty = 0;

	#ifdef DEBUG
	stringstream str;

	debug++;
	debug.open("Buzzer::setVolume", Debug::colorYellow);

	str << "volume:" << (int)volume;
	debug << str.str();
	#endif

	if (volume <= 100)
	{
		this->_volume = volume;

		duty = (this->_pwm->getPeriod() / 2) * ((float)volume / 100.0);

		this->_pwm->setDuty(duty);
		this->_pwm->getDuty();
	}

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}

void Buzzer::setFrequency(unsigned char freq)
{
	unsigned long period;

	#ifdef DEBUG
	stringstream str;

	debug++;
	debug.open("Buzzer::setFrequency", Debug::colorYellow);

	str << "frequency:" << (int)freq;
	debug << str.str();
	#endif

	if (freq <= 100)
	{
		this->_frequency = freq;

		this->_pwm->setDuty(0);
		this->_pwm->getDuty();

		period = Buzzer::_periodMax - (Buzzer::_periodMax - Buzzer::_periodMin) * ((float)freq / 100.0);
		this->_pwm->setPeriod(period);
		this->_pwm->getPeriod();

		this->setVolume(this->_volume);
	}

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}

//------------------------------------------------------------------------------------------------
Buzzer& Buzzer::operator++()
{
	if (this->_frequency < 100)
	{
		this->_frequency++;
		this->setFrequency(this->_frequency);
	}
}

Buzzer& Buzzer::operator--()
{
	if (this->_frequency > 0)
	{
		this->_frequency--;
		this->setFrequency(this->_frequency);
	}
}

