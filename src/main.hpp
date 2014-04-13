#ifndef MAIN_H
#define MAIN_H

/**
    @file main.hpp
    @author ise2
    @version 1.0
    @date 7 mars 2014
 
    @brief Fonctions permettant à la beaglebone de la personne d'agir en fonction des données reçues
 
    @details Programmation de l'intelligence de la carte, permet d'effectuer différentes actions en fonction
    		 des données reçues via la liaison zigbee.\n
    		 	- lecture d'un message vocal pour diriger la personne.\n 
    		 	- prise de mesure avec le magnétomètre.\n
    		 	- déduction de la rapidité d'émission du bip pour indiquer à la personne si elle se 
    		 	  rapproche de la bonne direction au suivre ou non en fonction de la différence entre 
    		 	  les directions actuelles du robot et de la personne.\n
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include "i2c.hpp"
#include "zigbee.hpp"
#include "buzzer.hpp"

//******************************************* Define ***********************************************

//**************************************** Magnétonmètre *******************************************
/**
    @def ADDR_MAGNETOMETRE
    Adresse du magnétomètre i2c
*/
#define ADDR_MAGNETOMETRE 0x00

/**
    @def CMD_A
    Commande permettant d'indiquer au magnétomètre de faire une acquisition
*/
#define CMD_A 0x41

//************************************* Temporisation Son ******************************************
/**
	@def TEMPO_750MS
	Temporisation de 0.75sec afin de laisser le temps à la beaglebone de charger le son et de 
	commencer à le jouer une fois la tempo effectuée on va indiquer au robot que le son a été 
	joué alors qu'il ne l'est pas encore totalement, afin de gagner un peu de temps

*/
#define TEMPO_750MS 750000

//************************************* Chaine de caractère *****************************************
/**
	@def LONGUEUR_MAX
	Longueur maximale d'une chaine de caractères
*/
#define LONGUEUR_MAX 32

//**************************************** tabCharToInt **********************************************
/**
	@def ANGLE_MAX
	Angle représentant l'angle maximal en valeur absolue que peuvent former les 2 beaglebones
*/
#define ANGLE_MAX 180


//**************************************** Interruption **********************************************
/**
    @def GPIO
    GPIO utilisée pour détecter une interruption hardware
*/
#define GPIO_1 "60"

/**
    @def GPIO
    GPIO utilisée pour détecter une interruption hardware
*/
#define GPIO_2 "48"

/**
    @def GPIO_MODE
    Configuration du mode du gpio (in pull-up + gpio)
*/
#define GPIO_MODE "77"

/**
                                            @def GPIO_1_DOSSIER_MODE
    Dossier où l'on peut changer le mode du gpio1_28
*/
#define GPIO_1_DOSSIER_MODE "gpmc_ben1"

/**
    @def GPIO_2_DOSSIER_MODE
    Dossier où l'on peut changer le mode du gpio1_16
*/
#define GPIO_2_DOSSIER_MODE "gpmc_a0"

/**
    @def DIRECTION
    Direction de la broche que l'on configure pour l'interruption
*/
#define DIRECTION "in"

/**
    @def EDGE
    Front sur lequel on va détecter l'interruption
*/
#define EDGE "falling"

//****************************************** Phrases **********************************************
/**
    @def TURN_LEFT
    Phrase à émettre
*/
#define TURN_LEFT "turn_left" //"Tournez sur vous même sur la gauche"

/**
    @def TURN_RIGHT
    Phrase à émettre
*/
#define TURN_RIGHT  "turn_right" //"Tournez sur vous même sur la droite"

/**
    @def STOP
    Phrase à émettre
*/
#define STOP  "stop" //"Arretez-vous et suivez les instructions"

/**
    @def RIGHT
    Phrase à émettre
*/
#define RIGHT  "right" //"Faite un pas à droite"

/**
    @def LEFT
    Phrase à émettre
*/
#define LEFT  "left" //"Faite un pas à gauche"    

/**
    @def GO_STRAIGHT
    Phrase à émettre
*/
#define GO_STRAIGHT  "go_straight" //"Vous pouvez avancer"

/**
    @def LOST
    Phrase à émettre
*/
#define LOST  "lost" //"Le robot vous a perdu, arrêtez vous et suivez les instructions"

/**
    @def DETECTION_FAILED
    Phrase à émettre
*/
#define DETECTION_FAILED  "detection_failed" //"Le robot ne vous a pas détecté, suivez les instructions"

/**
    @def DETECTION_SUCCESS
    Phrase à émettre
*/
#define DETECTION_SUCCESS  "detection_success" //"Le robot vous a retrouvé, le robot va vous indiquer quand vous pourrez bouger"

/**
    @def MAGNETO_SUCCESS
    Phrase à émettre
*/
#define MAGNETO_SUCCESS  "magneto_success" //"Ne bougez plus, vous êtes de nouveau dans la même direction que le robot, attendez les instructions"

/**
    @def TOO_MANY_OBSTACLES
    Phrase à émettre
*/
#define TOO_MANY_OBSTACLES  "too_many_obstacles" //"Le robot ne peut plus se déplacer, arrêt du programme"

/**
    @def PHOTO_1
    Phrase à émettre
*/
#define PHOTO_1 "photo_1" //"Veuillez pour cette photo garder la tête droite"

/**
    @def PHOTO_2
    Phrase à émettre
*/
#define PHOTO_2  "photo_2" //"Veuillez pour cette photo tourner la tête vers la gauche"

/**
    @def PHOTO_3
    Phrase à émettre
*/
#define PHOTO_3  "photo_3" //"Veuillez pour cette photo tourner la tête vers la droite"

/**
    @def PHOTO_4
    Phrase à émettre
*/
#define PHOTO_4 "photo_4" //"Veuillez pour cette photo baisser la tête en la gardant droite"

/**
    @def PHOTO_5
    Phrase à émettre
*/
#define PHOTO_5 "photo_5" //"Veuillez pour cette photo baisser la tête sur la gauche"

/**
    @def PHOTO_6
    Phrase à émettre
*/
#define PHOTO_6 "photo_6" //"Veuillez pour cette photo baisser la tête sur la droite"

/**
    @def PHOTO_7
    Phrase à émettre
*/
#define PHOTO_7  "photo_7" //"Veuillez pour cette photo lever la tête en la gardant droite"

/**
    @def PHOTO_8
    Phrase à émettre
*/
#define PHOTO_8  "photo_8" //"Veuillez pour cette photo lever la tête sur la gauche"

/**
    @def PHOTO_9
    Phrase à émettre
*/
#define PHOTO_9  "photo_9" //"Veuillez pour cette photo lever la tête sur la droite"

/**
    @def ANNONCE_PHOTO
    Phrase à émettre
*/
#define ANNONCE_PHOTO  "annonce_photo" //"Cette photo sera prise dans 3 secondes"

/**
    @def DECOMPTE_2
    Phrase à émettre
*/
#define DECOMPTE_2  "decompte_2" //"2"

/**
    @def DECOMPTE_1
    Phrase à émettre
*/
#define DECOMPTE_1  "decompte_1" //"1"

/**
    @def DECOMPTE_0
    Phrase à émettre
*/
#define DECOMPTE_0  "decompte_0" //"cheese"

/**
    @def INIT_PHOTO
    Phrase à émettre
*/
#define INIT_PHOTO  "init_photo" //"Prise de photo, veuillez suivre les instructions"

/**
    @def FIN_PROGRAMME
    Phrase à émettre
*/
#define FIN_PROGRAMME  "fin_programme" //"Si vous souhaitez arrêter le robot appuyé sur le bouton intérieur."

/**
    @def PROGRAMME_TERMINE
    Phrase à émettre
*/
#define PROGRAMME_TERMINE  "programme_termine" //"Fin du programme"

/**
    @def START_INITIALISATION
    Phrase à émettre
*/
#define START_INITIALISATION  "start_initialisation" //"Veuillez attendre les instructions"

/**
    @def NOT_FOUND
    Phrase à émettre
*/
#define NOT_FOUND  "not_found" //"Aucune personne n'a été trouvée par le robot, veuillez réappuyer sur le bouton pour relancer l'initialisation"

/**
    @def RECUPERATION_FAILED
    Phrase à émettre
*/
#define RECUPERATION_FAILED  "recuperation_failed" //"La personne a été complètement perdue, arrêt du programme"

/**
    @def INIT_SUCCESS
    Phrase à émettre
*/
#define INIT_SUCCESS  "init_success" //"L'initialisation est terminé, attendez les instructions"

/**
    @def INIT_FAILED
    Phrase à émettre
*/
#define INIT_FAILED  "init_failed" //"Le robot s'est mal initialisé, arrêt du programme"

/**
    @def INFORMATION
    Phrase à émettre
*/
#define INFORMATION  "information" //"Appuyez sur le bouton extérieur de la carte pour lancer l'initialisation"

//************************************* Variable Globale ******************************************
/**
    @var static bool finProgramme
    Booléen indiquant si le programme doit s'arrêter
*/
static bool finProgramme;


//***************************************** Structure *********************************************

/**
    Structure contenant toutes les données permettant de jouer un son.
*/
typedef struct {
    char directory[32];
    char soundFile[32];
}Sound;

/**
    Structure contenant toutes les données permettant de configurer un GPIO.
*/
typedef struct 
{
    char dossier[32];
    char numBroche[4];
    char value;
    char edge[8];
    char direction[4];
    char dossierMode[32];
    char mode[8];
    int fd;
}gpio;

/**
    @brief  Permet d'initialiser le GPIO que l'on veut utiliser pour capturer une interruption.
    
    @details    Permet de configurer la broche en faisant un export, puis en mettant la broche en 
                in/out ainsi que le front sur lequel on veut détecter l'interruption (edge).

    @param gpioBouton   Structure gpio à configurer.
    @param numBroche    Numéro de la broche à configurer.
    @param direction    Direction dans laquelle on veut configurer la broche.
    @param edge         Front sur lequel on veut détecter l'interruption sur la broche.
    @param dossierMode  Dossier permettant de configurer le mode du gpio.
    @param mode         Mode dans lequel on veut configurer le gpio.
    @return 0: pas d'erreur \n
            -1: erreur d'ouverture de fichier.\n
 */
int initGPIO(gpio *gpioBouton, char* numBroche, char* direction, char* edge, char* dossierMode);

/**
    @brief  Permet d'analyser les messages que l'on reçoit lorsque le robot tourne.
    
    @details    On analyse le message reçu pour en déduire le message que l'on doit retransmettre vocalement à
                la personne, direction et angle.

    @param messageRecu  Message que l'on doit analyser.
    @param msgRecup     Message qui sera transmis à la personne.
    @return 0: pas d'erreur \n
            -1: erreur d'ouverture de fichier.\n
 */
            // A faire
int recuperationMessage(char* messageRecu, char* msgRecup);

/**
    @brief	Permet de faire la convertion d'un char* en un int.
    
    @details	Permet de récupérer une valeur reçue via la liaison zigbee qui est contenu dans un char*.

    @param message 	string contenant le message à convertir en int
    @param nombre	Paramètre permettant de récupérer le résultat de la conversion
    @return 0: pas d'erreur \n
            -1: erreur, le message n'est pas un entier.\n
 */
int tabCharToInt(char* message, int *nombre);

/**
    @brief	Permet de réaliser un bip indiquant à la personne s'il est plus ou moins dans la bonne direction.
    @details On ajuste le rapport cyclique de la pwm permettant d'obtenir un bip audible en fonction de l'écart d'angle que l'on 
    		 observe entre le robot et la personne
 	
 	@param message    Message contenant la différence d'angle entre la personne et le robot

 	@param iteration	Variable permettant de savoir si c'est la première fois que l'on rentre dans cette fonction, 
 						dans ce cas on va devoir lire un fichier son.

    @param buzzer   Objet permettant de modifier la fréquence du bip ainsi que le volume du bip.
     
    @return 0: pas d'erreur \n
            -1: erreur liée au message \n
			-2: erreur liée à l'écriture du rapport cyclique de la pwm
 */
int bipPersonne(char* message, unsigned char iteration, Buzzer &buzzer);

/**
    @brief Permet de lire un fichier audio.
    @details Quand ce thread est lancé on va alors récupérer les informations contenues dans 
    		 la structure liée à ce thread
 	
 	@param 	Paramètre permettant de récupérer les informations relatives au fichier audio 
 			que l'on doit lire (emplacement et nom du fichier).
 */
void *emissionVocale(void *arg);

/**
    @brief Permet de capter une interruption hardware
    @details Ce thread va attendre de recevoir une interruption hardware de la part de l'utilisateur
            ce qui signifiera qu'il voudra arrêter le programme. 
    
    @param arg  Paramètre permettant de passer à un thread des données qui lui sont propres.
 */
void *interruption(void *arg);

#endif