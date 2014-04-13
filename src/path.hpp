
#ifndef PATH_H
#define PATH_H

/**
	@file path.hpp
	@author ISE2 | Alexis G. - Kévin R. - William V.
	@version 2.1
	@date 3 mars 2014

	@brief Fonctions permettant de gérer le système de fichier sous Linux.

	@details Permet de récupérer un chemin en utilisant un modèle.
*/

#include <iostream>
#include <string>
#include <glob.h>

//************************************************************************************************
/**
	@brief Permet de trouver le chemin d'un fichier/dossier à partir d'un modèle.

	@details Utilisation d'un modèle avec des wildcards(*) pour trouver le chemin réel dans le 
			 système, pour ensuite être utilisé dans 'l'ouverture de fichier.

 	@param	pathern		Modèle a fournir pour trouver le chemin.
 	@return	path 		Chemin retourné (vide si introuvable).
*/
std::string findPath(std::string pathern);

#endif
