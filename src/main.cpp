#include "main.hpp"

int initGPIO(gpio *gpioBouton, char* numBroche, char* direction, char* edge, char* dossierMode){
	char dir[64];
	FILE *pFile;
	
	sprintf(gpioBouton->mode, "%s", GPIO_MODE);
	sprintf(gpioBouton->dossierMode, "/sys/kernel/debug/omap_mux/%s", dossierMode);
	if ((pFile = fopen(gpioBouton->dossierMode, "w")) == NULL){
		return -1;
	}
	if (fwrite(GPIO_MODE, sizeof(GPIO_MODE), 1, pFile) != 1){
		return -2;
	}
	fclose(pFile);

	sprintf(gpioBouton->numBroche, "%s", numBroche);
	sprintf(gpioBouton->dossier, "/sys/class/gpio/gpio%s/", numBroche);

	if ((pFile = fopen("/sys/class/gpio/export", "w")) == NULL){
		return -1;
	}
	if (fwrite(gpioBouton->numBroche, sizeof(gpioBouton->numBroche), 1, pFile) != 1){
		return -2;
	}
	fclose(pFile);

	sprintf(dir, "%sdirection", gpioBouton->dossier);
	if ((pFile = fopen(dir, "w")) == NULL){
		return -1;
	}
	strcpy(gpioBouton->direction, direction);
	if (fwrite(gpioBouton->direction, sizeof(gpioBouton->direction), 1, pFile) != 1){
		return -2;
	}
	fclose(pFile);
	
	sprintf(dir, "%sedge", gpioBouton->dossier);
	if ((pFile = fopen(dir, "w")) == NULL){
		return -1;
	}
	strcpy(gpioBouton->edge, edge);
	if (fwrite(gpioBouton->edge, sizeof(gpioBouton->edge), 1, pFile) != 1){
		return -2;
	}
	fclose(pFile);

	return 0;
}

int recuperationMessage(char* messageRecu, char* msgRecup){
	int i, j;
	unsigned char angle;
	unsigned short ang;
	char turn;
	strcpy(msgRecup, "");
	// On regarde si on a un l à la bonne position pour savoir si le robot tourne à gauche
	// On vérifie également qu'on a bien @ à la bonne position indiquant que juste après on aura la valeur qui nous intéresse.
	if (messageRecu[2] == 'l' && messageRecu[8] == '@'){
		// Le robot tourne à gauche
		sscanf(messageRecu,"V:leftxx@%3dxxxx",&angle);
		turn = 'g';
	}
	else if (messageRecu[2] == 'r' && messageRecu[8] == '@'){
		// On regarde si on a un r à la bonne position pour savoir si le robot tourne à droite.
		// On vérifie également qu'on a bien @ à la bonne position indiquant que juste après on aura la valeur qui nous intéresse.
		sscanf(messageRecu,"V:rightx@%3dxxxx",&angle);
		turn = 'd';
	}
	else{
		puts("Erreur Message");
		return -1;
	}

	if (angle == 0){
		ang = 0;
		puts("On va tout droit");
		return 1;
	}
	else if (angle > 0 && angle <= 15){
		ang = 10;
	}
	else if (angle > 15 && angle <= 25){
		ang = 20;
	}
	else if (angle > 25 && angle <= 35){
		ang = 30;
	}
	else if (angle > 35 && angle <= 45){
		ang = 40;
	}
	else if (angle > 45 && angle <= 55){
		ang = 50;
	}
	else if (angle > 55 && angle <= 65){
		ang = 60;
	}
	else if (angle > 65 && angle <= 75){
		ang = 70;
	}
	else if (angle > 75 && angle <= 85){
		ang = 80;
	}
	else{
		ang = 90;
	}

	sprintf(msgRecup, "%d%c", ang, turn);
	printf("angle %d\n", ang);
	return 0;
}

int tabCharToInt(char* message, int *nombre){
	int i;
	puts("nombre");
	// Erreur si le message contient autre chose que des chiffres ou un signe - au début du message
	if (message[2] != '-' && (message[2] < '0' || message[2] > '9')){
		return -1;
	}
	printf("longueur message %d\n", strlen(message));
	for(i = 0; i < strlen(message) - 1 - 3; i++){
		if (message[strlen(message) - 1 - i] < '0' || message[strlen(message) - 1 - i] > '9'){	
			return -1;
		}
		// Chaque caractère récupéré est converti en int et additionné à nombre.
		*nombre += (message[strlen(message) - 1 - i] - 48)* pow(10,i);
	}
	
	// Si on a un signe - au début du message on multiplie par -1 sinon on ajoute la valeur au nombre.
	if (message[2] == '-'){
		*nombre *= -1;
	}
	else{
		*nombre += (message[2] - 48)* pow(10, strlen(message) - 3 - 1);	
	}
	printf("nombre : %d\n", *nombre);
	return 0;
}

int bipPersonne(char* message, unsigned char iteration, Buzzer &buzzer){
	pthread_t thread;
	Sound sound;

	static int differenceAnglePrec = 0;
	int *differenceAngle;
	differenceAngle = (int*)calloc(1, sizeof(int));

	sprintf(sound.directory, "/home/root/BB_person/son/");

	puts("bip");
	sscanf(message,"A:%4dxxxxxxxxxx",differenceAngle);
	
	// Il ne faudra jouer le son qu'une seule fois ou quand au coup d'avant on n'avait lancer le son contraire ou si la personne
	// a tourné dans le mauvais sens.
	if (iteration == 0 || (*differenceAngle > 0 && differenceAnglePrec < 0) || 
		(*differenceAngle < 0 && differenceAnglePrec > 0) || (*differenceAngle > 0 && differenceAnglePrec < *differenceAngle) ||
		(*differenceAngle < 0 && differenceAnglePrec > *differenceAngle)){
		if (*differenceAngle > 0){
			// Lecture d'un son
			sprintf(sound.soundFile, "%s", TURN_LEFT);
			pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
			pthread_join(thread, NULL);
		}
		else{
			// Lecture d'un son
			sprintf(sound.soundFile, "%s", TURN_RIGHT);
			pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
			pthread_join(thread, NULL);
		}
	}
	differenceAnglePrec = *differenceAngle;

	// On actualise la fréquence du buzzer en fonction des directions du robot et de la personne
	buzzer.setFrequency(100 - (abs(*differenceAngle) * 100 / (ANGLE_MAX)));
	// On remet le volume à 100%
	buzzer.setVolume(100);

	free(differenceAngle);

	return 0;
}

void *emissionVocale(void *arg){
	Sound *Arg = (Sound*) arg;
	char cmd[64];

	sprintf(cmd, "aplay %s%s.wav", Arg->directory, Arg->soundFile);
	system(cmd);
}

void *interruption(void *arg){
	char dirInterruption[64];
	unsigned char rc, compteur;
	struct pollfd interruptGPIO;
	gpio gpioBouton;

	if (initGPIO(&gpioBouton, GPIO_2, DIRECTION, EDGE, GPIO_2_DOSSIER_MODE) != 0){
		puts("Init GPIO Failed");
	}

	interruptGPIO.events = POLLPRI;
	
	sprintf(dirInterruption, "%svalue", gpioBouton.dossier);
	if ((gpioBouton.fd = open(dirInterruption, O_RDONLY | O_NONBLOCK)) == -1){
		puts("File doesn't exist");
	}

	compteur = 0;
	while (1){
		memset((void*)&interruptGPIO, 0, sizeof(interruptGPIO));

		interruptGPIO.events = POLLPRI;
		interruptGPIO.fd = gpioBouton.fd;

		rc = poll(&interruptGPIO, 1, 1000);
		if (rc == -1){
			puts("erreur poll");
		}
		//puts("interruption 2");
		if (interruptGPIO.revents & POLLPRI){
			read(interruptGPIO.fd, &gpioBouton.value, 1);
			compteur++;
			if(compteur == 2){
				// L'utilisateur a appuyé sur le bouton pour arrêtr le programme
				close(gpioBouton.fd);
				finProgramme = true;
				break;
			}
		}
		if (finProgramme == true){
			// Le robot a demandé l'arrêt du programme
			break;
		}
	}
}

int main(int argc, char **argv){
	int anglePersonne, i;
	unsigned char iteration = 0;
	char compteurErreur = 0;
	char bufferEnv[64];
	char bufferR[64];

	pthread_t thread, threadInterruption;

// Initialisation
//********************************************************************************************************
// Attente d'une interruption pour déclencher l'initialisation (positionnement du robot par rapport à la personne)
	int rc, nb;
	char dirInterruption[64];
	struct pollfd interruptGPIO;
	gpio gpioBouton;
	Sound sound;

	finProgramme = false;

	sprintf(sound.directory, "/home/root/BB_person/son/");

	// Initialiser le zigbee
	TTY_INFO *zigbee;
	zigbee = readyTTY(0);
	if (setTTYSpeed(zigbee, 57600) != 0){
		puts("erreur init speed");
	}
	if (setTTYParity(zigbee, 8, 's', 1) != 0){
		puts("erreur init parite stop databits");
	}

	// Création du thread qui attend une interruption hardware afin d'arrêter le programme.
	pthread_create(&threadInterruption, NULL, interruption, NULL);
	// Message informatif
	sprintf(sound.soundFile, "%s", FIN_PROGRAMME);
	pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
	pthread_join(thread, NULL);

	// Tant que l'on a pas appuyé que l'autre bouton, on continue de run le prog (2 boutons) (peut être mettre une sécurité
	// pour le bouton stop afin d'éviter d'arrêter le programme sans le vouloir, par exemple 2 appuis)
	while(1){
		// Initialisation de l'interruption
		unsigned char compteur;
		if (initGPIO(&gpioBouton, GPIO_1, DIRECTION, EDGE, GPIO_1_DOSSIER_MODE) != 0){
			puts("Init GPIO Failed");
		}

		interruptGPIO.events = POLLPRI;
		
		sprintf(dirInterruption, "%svalue", gpioBouton.dossier);
		if ((gpioBouton.fd = open(dirInterruption, O_RDONLY | O_NONBLOCK)) == -1){
			puts("File doesn't exist");
		}

		// Lecture d'un son
		sprintf(sound.soundFile, "%s", INFORMATION);
		pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
		pthread_join(thread, NULL);

		compteur = 0;

		while (1){
			puts("interrupt");
			memset((void*)&interruptGPIO, 0, sizeof(interruptGPIO));

			interruptGPIO.events = POLLPRI;
			interruptGPIO.fd = gpioBouton.fd;

			rc = poll(&interruptGPIO, 1, 1000);
			
			if (rc == 0){
				printf(".");
			}
			if (rc == -1){
				puts("erreur poll");
			}

			if (interruptGPIO.revents & POLLPRI){
				read(interruptGPIO.fd, &gpioBouton.value, 1);
				compteur++;
				if(compteur == 2){
					break;
				}
			}
		}

		close(gpioBouton.fd);

		puts("Interruption !");

	//********************************************************************************************************
		
		// Initialisation du magnétomètre.
		if (i2c_w(ADDR_MAGNETOMETRE, CMD_A, NULL) != 0){
			puts("Magnétomètre failed");
			// Arrêt du programme
			finProgramme = true;
			return -1;
		}
		
		// Initialisation du buzzer.
		Buzzer buzzer;
		if (buzzer.getInitBuzzer() != 1){
		 	puts("Init buzzer failed");
		 	// Arrêt du programme
			finProgramme = true;
			return -1;
		}
		buzzer.setVolume(0);

		// Lecture d'un son
		sprintf(sound.soundFile, "%s", START_INITIALISATION);
		pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
        pthread_join(thread, NULL);

        //Lance l'initialisation du positionnement avec le robot.
        usleep(30000);
        sprintf(bufferEnv, "I:start_initxxxx");
		printf("bufferEnv %s\n", bufferEnv);
		
		sendnTTY(zigbee, bufferEnv, 17);
		usleep(100000);
		
		// Algo principal, on attend un ordre du robot à exécuter.
		while(1){
			puts("\nAttente de message");
			
			nb = recvnTTY(zigbee, bufferR, 17);
			printf("bufferR %s\n", bufferR);

			if (finProgramme == true){
				// Fin du programme par l'utilisateur
				// On avertit le robot de la fin du programme pour qu'il s'arrête aussi
				usleep(300000);
				sprintf(bufferEnv, "end_programxxxxx");
				printf("bufferEnv %s\n", bufferEnv);
				
				sendnTTY(zigbee, bufferEnv, 17);
				usleep(50000);

				// On synchronise la fin des threads
				pthread_join(threadInterruption, NULL);
				
				sprintf(sound.soundFile, "%s", PROGRAMME_TERMINE);
				pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				pthread_join(thread, NULL);
				
				puts("Fin du programme");
				
				cleanTTY(zigbee);
				
				return 0;
			}

			if (bufferR[0] == 'M'){
				puts("\nM");
				
				iteration = 0;
				
				// Remise du volume à 0 car on a plus besoin du bip sonore
				buzzer.setVolume(0);
				buzzer.setFrequency(0);

				if (strcmp(bufferR, "M:stopxxxxxxxxxx") == 0){	
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", STOP);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);

					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
				else if (strcmp(bufferR, "M:rightxxxxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", RIGHT);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);

					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
				else if (strcmp(bufferR, "M:leftxxxxxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", LEFT);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);

					
					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
				else if	(strcmp(bufferR, "M:go_straightxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", GO_STRAIGHT);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);

					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
				else if (strcmp(bufferR, "M:lostxxxxxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", LOST);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);

					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				} 
				else if (strcmp(bufferR, "M:detect_failxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", DETECTION_FAILED);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);

					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
				else if (strcmp(bufferR, "M:detect_success") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", DETECTION_SUCCESS);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);

					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
				else if (strcmp(bufferR, "M:magneto_succes") == 0){
					sprintf(sound.soundFile, "%s", MAGNETO_SUCCESS);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);

					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
				else if (strcmp(bufferR, "M:many_obstacles") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", TOO_MANY_OBSTACLES);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);

					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);

					// Arrêt du pogramme
					finProgramme = true;
					pthread_join(threadInterruption, NULL);
					puts("Fin du programme");

					cleanTTY(zigbee);
					return 1;
				}
				else{
					puts("Erreur Message");
					
					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "erreur_message");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
			}
			else if (bufferR[0] == 'I'){
				puts("\nI");

				iteration = 0;
				
				// Remise du volume à 0 car on a plus besoin du bip sonore
				buzzer.setVolume(0);
				buzzer.setFrequency(0);
				
				if (strcmp(bufferR, "I:init_photoxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", INIT_PHOTO);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);
				}
				else if (strcmp(bufferR, "I:annonce_photos") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", ANNONCE_PHOTO);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);
				}
				else if (strcmp(bufferR, "I:decompte_2xxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", DECOMPTE_2);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);
				}
				else if (strcmp(bufferR, "I:decompte_1xxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", DECOMPTE_1);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);
				}
				else if (strcmp(bufferR, "I:decompte_0xxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", DECOMPTE_0);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);
				}
				else if (strcmp(bufferR, "I:init_successxx") == 0){
					// Lire le son
					sprintf(sound.soundFile, "%s", INIT_SUCCESS);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);

					usleep(300000);
					sprintf(bufferEnv, "playing_successx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
				else if (strcmp(bufferR, "I:init_failedxxx") == 0){
					// Pwm robot pas initialisé
					// Lire le son : "Le robot s'est mal initialisé, arrêt du programme"
					sprintf(sound.soundFile, "%s", INIT_FAILED);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);

					// Arrêt du programme
					finProgramme = true;
					pthread_join(threadInterruption, NULL);
					
					puts("Fin du programme");

					cleanTTY(zigbee);
					
					return 1;
				}
				else if (strcmp(bufferR, "I:not_foundxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", NOT_FOUND);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);
					
					puts("Initialisation va etre relancee");
					break;
				}
				else if (strcmp(bufferR, "I:recup_failedxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", RECUPERATION_FAILED);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);
			
					// Arrêt du programme
					finProgramme = true;
					pthread_join(threadInterruption, NULL);
					puts("Fin du programme");
					
					cleanTTY(zigbee);
					
					return 1;
				}
				else{
					puts("Erreur Message");
					
					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "erreur_messagexx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
			}
			else if (bufferR[0] == 'V'){
				puts("\nV");
				
				char rc;
				char msg[64];
				
				iteration = 0;
				
				// Remise du volume à 0 car on a plus besoin du bip sonore
				buzzer.setVolume(0);
				buzzer.setFrequency(0);
				
				// Message que l'on va recevoir pendant que le robot tourne
				rc = recuperationMessage(bufferR, msg); 
				if (rc == 0){
					printf("bufferR : %s\n", msg);
					sprintf(sound.soundFile, "virage%s", msg);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
					pthread_join(thread, NULL);

					sprintf(bufferEnv, "playing_successx");
				}
				else if (rc == -1){
					puts("Erreur Message");
					sprintf(bufferEnv, "erreur_messagexx");
				}
				
				usleep(300000);
				printf("bufferEnv %s\n", bufferEnv);
				
				sendnTTY(zigbee, bufferEnv, 17);
				usleep(50000);
			}
			else if (bufferR[0] == 'A'){
				puts("\nA");
				// On a reçu la différence d'angle entre le robot et la position -> il va falloir en déduire le dutycycle 
				// de la pwm à réaliser pour pouvoir réaliser le bip sonore indiquant si la personne est de plus en plus proche
				// de la direction du robot

				// valeur de la différence comprise entre -180 et 180
				if (bipPersonne(bufferR, iteration, buzzer) != 0){
					puts("Erreur Message");
				}
				
				// Sorte de synchro avec le robot pour éviter de perdre des messages:
				usleep(750000);
				sprintf(bufferEnv, "playing_successx");
				printf("bufferEnv %s\n", bufferEnv);
				
				sendnTTY(zigbee, bufferEnv, 17);
				usleep(50000);
				
				iteration++;
			}
			else if (bufferR[0] == 'P'){
				puts("\nP");
				
				iteration = 0;
				
				// Remise du volume à 0 car on a plus besoin du bip sonore
				buzzer.setVolume(0);
				buzzer.setFrequency(0);
				
				if (strcmp(bufferR, "P:photo_1xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_1);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else if (strcmp(bufferR, "P:photo_2xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_2);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else if (strcmp(bufferR, "P:photo_3xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_3);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else if (strcmp(bufferR, "P:photo_4xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_4);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else if (strcmp(bufferR, "P:photo_5xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_5);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else if (strcmp(bufferR, "P:photo_6xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_6);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else if	(strcmp(bufferR, "P:photo_7xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_7);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else if (strcmp(bufferR, "P:photo_8xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_8);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else if (strcmp(bufferR, "P:photo_9xxxxxxx") == 0){
					// Lecture d'un son
					sprintf(sound.soundFile, "%s", PHOTO_9);
					pthread_create(&thread, NULL, emissionVocale, (void*) &sound);
				}
				else{
					puts("Erreur Message");
					//messageEnvoi = "erreur_message";
					// Sorte de synchro avec le robot pour éviter de perdre des messages:
					usleep(300000);
					sprintf(bufferEnv, "erreur_messagexx");
					printf("bufferEnv %s\n", bufferEnv);
					
					sendnTTY(zigbee, bufferEnv, 17);
					usleep(50000);
				}
			}
			else if (strcmp(bufferR, "mesurexxxxxxxxxx") == 0){
				puts("\nMesure");
				
				char message[8];

				// Prise de mesure Magnétomètre
				i2c_r(ADDR_MAGNETOMETRE, CMD_A, 'w', NULL);
				anglePersonne = (int)i2c_r(ADDR_MAGNETOMETRE, CMD_A, 'w', NULL);
				anglePersonne = (int)(anglePersonne/10);
				
				usleep(300000);
				sprintf(bufferEnv, "%3d", anglePersonne);
				strcat(bufferEnv,"xxxxxxxxxxxxx");
				printf("bufferEnv %s\n", bufferEnv);
				
				sendnTTY(zigbee, bufferEnv, 17);
				usleep(50000);
			}
			else if (strcmp(bufferR, "end_programxxxxx") == 0){
				// Fin du programme par le robot
				finProgramme = true;
				pthread_join(threadInterruption, NULL);

				puts("Fin du programme");
				
				cleanTTY(zigbee);
				
				return 0;
			}
			else{
				puts("\nErreur Message");

				// Sorte de synchro avec le robot pour éviter de perdre des messages:
				usleep(300000);
				sprintf(bufferEnv, "erreur_messagexx");
				printf("bufferEnv %s\n", bufferEnv);
				
				sendnTTY(zigbee, bufferEnv, 17);
				usleep(50000);
			}
		}
	}
	
	cleanTTY(zigbee);
	return -1;
}