
#ifndef PWM_H
#define PWM_H

/**
	@file pwm.hpp
	@author ISE2 | Alexis G. - Kévin R. - William V.
	@version 2.2
	@date 8 mars 2014

	@brief Classe permettant de manipuler les PWM d'une beaglebone sur noyau 3.8.

	@warning Nécessite le montage de la cape am33xx_pwm pour l'utilisation des PWM,
	    	 et d'une cape par pin dans le "device tree".
*/

#include <iostream>
#include <fstream>
#include <string>

#include "path.hpp"

//************************************************************************************************
/**
	@brief Permet le stockage des flux des fichiers de configuration d'une PWM.

	@details Les flux des fichiers sont initialisés avec le constructeur de la classe
			 en spécifiant le chemin du dossier de la PWM que l'on souhaite utiliser.
			 Il est possible d'écrire et de lire la valeur de chaque fichier à l'aide des 
			 setter/getter de chaque fichier.
	
			 6 fichiers de configuration : 
			 	- duty 		(rw)
			 	- modalias 	(r )
			 	- periode 	(rw) 
			 	- polarity  (rw)
			 	- run  		(rw)
			 	- uevent 	(r )

*/
class Pwm 
{
private:
	std::fstream  _duty;      ///< Réglage du rapport cyclique en nanoseconde
	std::fstream  _period;    ///< Réglage de la periode en nanoseconde. Inchangeable en kernel 3.8 avec 2 PWM actives sur la même paire.
	std::fstream  _polarity;  ///< Réglage de la polarité
	std::fstream  _run;       ///< Start/stop du signal 
	std::ifstream _uevent;    ///< ? 

public:
	//--------------------------------------------------------------------------------------------
	/**
		@brief Constructeur paramètrique. Ouverture des fichiers pour la gestion d'une PWM.

		@details Tous les fichiers sont ouvert avec le mode 'r+', sauf le fichier
				 'modalias' qui est en lecture seule, et le fichier 'period'. Utilisation des 
				 fstream pour la gestion des fichiers.

		@param path 	Chemin du dossier contenant les fichiers d'une PWM
	*/
	Pwm(std::string path = "");
	
	/**
		@brief Destructeur. Fermeture des fichiers de gestion d'une PWM.

		@details Tous les fichiers ouvert avec le constructeur sont fermés.
	*/
	virtual ~Pwm();

	//--------------------------------------------------------------------------------------------
	/**
		@brief Vérifie les fichiers de configuration

		@details Permet de vérifier si tout les fichiers de configuration de la PWM sont ouverts.

		@return		Retourne 1 si il y a une erreur, 0 sinon.
	*/
	bool check();

	//--------------------------------------------------------------------------------------------
	/**
		@brief Lecture de la valeur du fichier 'duty' de la PWM.

		@return 	Données lues dans le fichier (4 octets max.)
	*/
	long getDuty();
	/**
		@brief Lecture de la valeur du fichier 'period' de la PWM.

		@return 	Données lues dans le fichier (4 octets max.)
	*/
	long getPeriod();
	/**
		@brief Lecture de la valeur du fichier 'polarity' de la PWM.

		@return 	Données lues dans le fichier (4 octets max.)
	*/
	bool getPolarity();
	/**
		@brief Lecture de la valeur du fichier 'run' de la PWM.

		@return 	Données lues dans le fichier (1 ou 0)
	*/
	bool getRun();
	/**
		@brief Lecture de la valeur du fichier 'uevent' de la PWM.

		@return 	Données lues dans le fichier (1 ou 0)
	*/
	long getUevent();

	//--------------------------------------------------------------------------------------------
	/**
		@brief Ecriture dans le fichier 'duty' de la PWM.

		@param 	data 	Données a écrire (4 octets max.)
	*/
	void setDuty(long data);
	/**
		@brief Ecriture dans le fichier 'Polarity' de la PWM.

		@param 	data 	Données a écrire (1 ou 0)
	*/
	void setPolarity(bool data);
	/**
		@brief Ecriture dans le fichier 'run' de la PWM.

		@param 	data 	Données a écrire (1 ou 0)
	*/
	void setRun(bool data);
	/**
		@brief Ecriture dans le fichier 'period' de la PWM.

		@param 	data 	Données a écrire (4 octets max.)
	*/
	void setPeriod(long data);

	//--------------------------------------------------------------------------------------------
	#ifdef DEBUG
	void print();
	#endif
};

#endif
