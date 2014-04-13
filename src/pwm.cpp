
/**
	@file pwm.cpp
	@author ISE2 | Alexis G. - Kévin R. - William V.
	@version 2.2
	@date 8 mars 2014

	@brief Fonctions permettant de manipuler les PWM d'une beaglebone sur noyau 3.8.
		   (Pour plus de détails, voir pwm.hpp)
*/

#include "pwm.hpp"

using namespace std;

#ifdef DEBUG
#include <sstream>
#include "debug.hpp"
extern Debug debug;
#endif

Pwm::Pwm(string path)
{
	string dir;
	string buf;

	#ifdef DEBUG
	debug++;
	debug.open("Pwm::Pwm", Debug::colorBlue);
	debug << "path = " + path;
	#endif

	// -----------------------------------------------------------------
	// 	Verif. chemin
	// -----------------------------------------------------------------
	#ifdef DEBUG
	debug << "find path for "; //<< path;
	#endif

	dir = findPath(path);

	if (dir == "")
	{
		#ifdef DEBUG
		debug.error("no directory found");
		#endif
	}
	else
	{
		// -----------------------------------------------------------------
		// 	Ouverture du fichier duty
		// -----------------------------------------------------------------
		#ifdef DEBUG
		debug << "open [rw] duty";
		#endif

		buf = dir + "/duty_ns";
		this->_duty.open(buf.c_str());

		// -----------------------------------------------------------------
		// 	Ouverture du fichier period (read only)
		// -----------------------------------------------------------------
		#ifdef DEBUG
		debug << "open [r ] period";
		#endif

		buf = dir + "/period_ns";
		this->_period.open(buf.c_str());

		// -----------------------------------------------------------------
		// 	Ouverture du fichier polarity
		// -----------------------------------------------------------------
		#ifdef DEBUG
		debug << "open [rw] polarity";
		#endif

		buf = dir + "/polarity";
		this->_polarity.open(buf.c_str());

		// -----------------------------------------------------------------
		// 	Ouverture du fichier run
		// -----------------------------------------------------------------
		#ifdef DEBUG
		debug << "open [rw] run";
		#endif

		buf = dir + "/run";
		this->_run.open(buf.c_str());

		// -----------------------------------------------------------------
		// 	Ouverture du fichier uevent (read only)
		// -----------------------------------------------------------------
		#ifdef DEBUG
		debug << "open [r ] uevent";
		#endif

		buf = dir + "/uevent";
		this->_uevent.open(buf.c_str());
	}

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}

Pwm::~Pwm()
{
	#ifdef DEBUG
	debug++;
	debug.open("Pwm::~Pwm", Debug::colorBlue);
	#endif

	// -----------------------------------------------------------------
	// 	Fermeture du fichier duty
	// -----------------------------------------------------------------
	#ifdef DEBUG
	debug << "close duty";
	#endif

	this->_duty.close();

	// -----------------------------------------------------------------
	// 	Fermeture du fichier period
	// -----------------------------------------------------------------
	#ifdef DEBUG
	debug << "close period";
	#endif

	this->_period.close();

	// -----------------------------------------------------------------
	// 	Fermeture du fichier polarity
	// -----------------------------------------------------------------
	#ifdef DEBUG
	debug << "close polarity";
	#endif

	this->_polarity.close();

	// -----------------------------------------------------------------
	// 	Fermeture du fichier run
	// -----------------------------------------------------------------
	#ifdef DEBUG
	debug << "close run";
	#endif	

	this->_run.close();

	// -----------------------------------------------------------------
	// 	Fermeture du fichier uevent
	// -----------------------------------------------------------------
	#ifdef DEBUG
	debug << "close uevent";
	#endif

	this->_uevent.close();


	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}


//------------------------------------------------------------------------------------------------
bool Pwm::check()
{
	unsigned char error = 0;

	#ifdef DEBUG
	stringstream str;
	
	debug++;
	debug.open("Pwm::check", Debug::colorBlue);
	#endif

	// -----------------------------------------------------------------
	// 	Vérif. fichier duty
	// -----------------------------------------------------------------
	if (!this->_duty.is_open())
	{
		#ifdef DEBUG
		debug.error("duty is close");
		#endif
		error++;
	}

	// -----------------------------------------------------------------
	// 	Vérif. fichier period
	// -----------------------------------------------------------------
	if (!this->_period.is_open())
	{
		#ifdef DEBUG
		debug.error("period is close");
		#endif
		error++;
	}

	// -----------------------------------------------------------------
	// 	Vérif. fichier polarity
	// -----------------------------------------------------------------
	if (!this->_polarity.is_open())
	{
		#ifdef DEBUG
		debug.error("polarity is close");
		#endif
		error++;
	}

	// -----------------------------------------------------------------
	// 	Vérif. fichier run
	// -----------------------------------------------------------------
	if (!this->_run.is_open())
	{
		#ifdef DEBUG
		debug.error("run is close");
		#endif
		error++;
	}

	// -----------------------------------------------------------------
	// 	Vérif. fichier uevent
	// -----------------------------------------------------------------
	if (!this->_uevent.is_open())
	{
		#ifdef DEBUG
		debug.error("uevent is close");
		#endif
		error++;
	}

	#ifdef DEBUG
	str << "error = " << (int)error;
	debug << str.str();
	
	debug.close("return");
	debug--;
	#endif

	return (error == 0) ? false : true;
}


//------------------------------------------------------------------------------------------------
void Pwm::setDuty(long data)
{
	#ifdef DEBUG
	stringstream str;
	
	debug++;
	debug.open("Pwm::setDuty", Debug::colorBlue);
	
	str << "data = " << data;
	debug << str.str();
	#endif

	// Si le fichier est ouvert
	if (this->_duty.is_open())
	{
		// Effacement du fichier
		this->_duty.flush();
		this->_duty.seekg(0);

		// Ecriture dans le fichier
		this->_duty << data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("duty is not open");
		#endif
	}

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}

long Pwm::getDuty()
{
	long data = 0;

	#ifdef DEBUG
	stringstream str;
	
	debug++;
	debug.open("Pwm::getDuty", Debug::colorBlue);
	#endif

	if (this->_duty.is_open())
	{
		// Retour au debut du fichier
		this->_duty.seekg(0);

		// Lecture du fichier
		this->_duty >> data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("duty is not open");
		#endif
	}

	#ifdef DEBUG
	str << "data = " << data;
	debug << str.str();
	
	debug.close("return");
	debug--;
	#endif

	return data;
}

//------------------------------------------------------------------------------------------------
void Pwm::setPolarity(bool data)
{
	#ifdef DEBUG
	stringstream str;
	
	debug++;
	debug.open("Pwm::setPolarity", Debug::colorBlue);
	
	str << "data = " << data;
	debug << str.str();
	#endif

	// Si le fichier est ouvert
	if (this->_polarity.is_open())
	{
		// Effacement du fichier
		this->_polarity.flush();
		this->_polarity.seekg(0);

		// Ecriture dans le fichier
		this->_polarity >> data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("polarity is not open");
		#endif
	}

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}

bool Pwm::getPolarity()
{
	bool data = false;

	#ifdef DEBUG
	stringstream str;
	
	debug++;
	debug.open("Pwm::getPolarity", Debug::colorBlue);
	#endif

	if (this->_polarity.is_open())
	{
		// Retour au debut du fichier
		this->_polarity.seekg(0);

		// Lecture du fichier
		this->_polarity >> data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("polarity is not open");
		#endif
	}

	#ifdef DEBUG
	str << "data = " << data;
	debug << str.str();
	
	debug.close("return");
	debug--;
	#endif

	return data;
}

//------------------------------------------------------------------------------------------------
void Pwm::setRun(bool data)
{
	#ifdef DEBUG
	stringstream str;
	
	debug++;
	debug.open("Pwm::setRun", Debug::colorBlue);
	
	str << "data = " << data;
	debug << str.str();
	#endif

	// Si le fichier est ouvert
	if (this->_run.is_open())
	{
		// Effacement du fichier
		this->_run.flush();
		this->_run.seekg(0);

		// Ecriture dans le fichier
		this->_run << data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("run is not open");
		#endif
	}

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}

bool Pwm::getRun()
{
	bool data = false;

	#ifdef DEBUG
	stringstream str;

	debug++;
	debug.open("Pwm::getRun", Debug::colorBlue);
	#endif

	if (this->_run.is_open())
	{
		// Retour au debut du fichier
		this->_run.seekg(0);

		// Lecture du fichier
		this->_run >> data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("run is not open");
		#endif
	}

	#ifdef DEBUG
	str << "data = " << data;
	debug << str.str();

	debug.close("return");
	debug--;
	#endif

	return data;
}

//------------------------------------------------------------------------------------------------
long Pwm::getUevent()
{
	long data = 0;

	#ifdef DEBUG
	stringstream str;

	debug++;
	debug.open("Pwm::getUevent", Debug::colorBlue);
	#endif

	if (this->_uevent.is_open())
	{
		// Retour au debut du fichier
		this->_uevent.seekg(0);

		// Lecture du fichier
		this->_uevent >> data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("uevent is not open");
		#endif
	}

	#ifdef DEBUG
	str << "data = " << data;
	debug << str.str();

	debug.close("return");
	debug--;
	#endif

	return data;
}

//------------------------------------------------------------------------------------------------
long Pwm::getPeriod()
{
	long data = 0;

	#ifdef DEBUG
	stringstream str;

	debug++;
	debug.open("Pwm::getPeriod", Debug::colorBlue);
	#endif

	if (this->_period.is_open())
	{
		// Retour au debut du fichier
		this->_period.seekg(0);

		// Lecture du fichier
		this->_period >> data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("period is not open");
		#endif
	}

	#ifdef DEBUG
	str << "data = " << data;
	debug << str.str();
	
	debug.close("return");
	debug--;
	#endif

	return data;
}

void Pwm::setPeriod(long data)
{
	#ifdef DEBUG
	stringstream str;
	
	debug++;
	debug.open("Pwm::setPeriod", Debug::colorBlue);
	
	str << "data = " << data;
	debug << str.str();
	#endif

	// Si le fichier est ouvert
	if (this->_period.is_open())
	{
		// Effacement du fichier
		this->_period.flush();
		this->_period.seekg(0);

		// Ecriture dans le fichier
		this->_period << data;
	}
	else
	{
		#ifdef DEBUG
		debug.error("period is not open");
		#endif
	}

	#ifdef DEBUG
	debug.close("return");
	debug--;
	#endif
}

//------------------------------------------------------------------------------------------------
#ifdef DEBUG
void Pwm::print()
{
	debug++;
	debug.open("Pwm::print", Debug::colorBlue);
	
	debug << "duty";
	this->getDuty();
	
	debug << "period";
	this->getPeriod();
	
	debug << "polarity";
	this->getPolarity();
	
	debug << "run";
	this->getRun();
	
	debug << "uevent";
	this->getUevent();

	debug.close("return");
	debug--;
}
#endif


