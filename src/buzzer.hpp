
#ifndef BUZZER_H
#define BUZZER_H

/**
	@file buzzer.hpp
	@author ISE2 | Alexis G. - Kévin R. - William V.
	@version 1.1
	@date 9 mars 2014

	@brief Classe permettant de manipuler un buzzer en volume et fréquence.

*/

#include <iostream>
#include <string>
#include <unistd.h>

#include "pwm.hpp"

//************************************************************************************************
/**
	@brief Permet de réger un buzzer en fréquence et en volume.

	@details Utilisation d'une PWM pour contrôler le buzzer. \n
			 Le contrôle en fréquence s'effectue sur une plage de fréquence définie (20 à 200 Hz). \n
			 Le volume est contrôler à l'aide du rapport cyclique. 0% = éteint, 50% = volume max.
*/
class Buzzer
{
private:
	static const unsigned long _periodMax;	///< Période maximale, fréquence minimale. ( 20 Hz)
	static const unsigned long _periodMin;  ///< Période minimale, fréquence maximale. (200 Hz)
	static const std::string _dirPwm;		///< Dossier correspondant à la PWM sélectionnée.

	Pwm *_pwm;					///< Gestion d'une PWM.
	bool _initBuzzer;			///< Permet de savoir si la PWM est initialisée.

	unsigned char _volume;		///< Dernier volume enregistré, en pourcent. (0 à 100)
	unsigned char _frequency;	///< Dernière fréquence enregistrée, en poucent. (0 à 100)

public:
	//--------------------------------------------------------------------------------------------
	/**
		@brief Constructeur. Initialisation de la PWM.

		@details Initialise la PWM P9_14 d'une beaglebone à la période maximale, avec un rapport
				 cyclique à zéro en configuration par défaut. \n
				 Possibilité de fixer la volume et la fréquence de démarrage.

		@param 	volume 		Volume de départ, en pourcent. (0 à 100)
		@param 	frequency 	Fréquence de départ, en pourcent. (0 à 100)
	*/
	Buzzer(unsigned char volume = 0, unsigned char frequency = 0);

	/**
		@brief Destructeur. Permet d'arrêter la PWM.

		@details Met le rapport cyclique de la PWM à zéro, et arrête la PWM.
	*/
	virtual ~Buzzer();

	//--------------------------------------------------------------------------------------------
	/**
		@brief Permet de vérifier si la PWM est correctement initialisée.
	*/
	bool getInitBuzzer() {
		return this->_initBuzzer;
	}

	/**
		@brief Permet de régler le volume du buzzer.

		@details Le rapport cyclique de la PWM permet le contrôle du volume. Réglable de 0% à 50%.\n
				 Récupère la période actuelle de la PWM, divise cette valeur par 2 et effectue un
				 ratio du volume à mettre sur 100 de cette valeur. Une fois le calcul effectué, 
				 la valeur du rapport cyclique est modifiée.

		@param 	volume 	Nouveau volume en pourcent. (0 à 100)
	*/
	void setVolume(unsigned char volume);

	/**
		@brief Permet de connaitre le dernier volume enregistré, en pourcent.
	*/
	unsigned char getVolume() {
		return this->_volume;
	}

	/**
		@brief Permet de régler la fréquence du buzzer.

		@details Régle la fréquence du buzzer en fonction de la plage de fréquence définie. \n
				 Mise à zéro du rapport cyclique, calcul de la nouvelle période en fonction du 
				 pourcentage et de la plage de fréquence, remise du volume à son ancienne valeur.

		@param 	freq 	Nouvelle fréquence en pourcent. (0 à 100)
	*/
	void setFrequency(unsigned char freq);

	/**
		@brief Permet de connaitre la dernière fréquence enregistrée, en pourcent.
	*/
	unsigned char getFrequency() {
		return this->_frequency;
	}

	//--------------------------------------------------------------------------------------------
	/**
		@brief Permet d'augmenter de 1% la fréquence du buzzer. (pré-incrémentation)
	*/
	Buzzer& operator++();
	/**
		@brief Permet de diminuer de 1% la fréquence du buzzer. (pré-décrémentation)
	*/
	Buzzer& operator--();
};

#endif

