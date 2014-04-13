
/**
	@file path.cpp
	@author ISE2 | Alexis G. - Kévin R. - William V.
	@version 2.1
	@date 3 mars 2014

	@brief Fonctions permettant de gérer le système de fichier sous Linux.
		   (Pour plus de détails, voir path.h)
*/

#include "path.hpp"

using namespace std;

#ifdef DEBUG
#include <sstream>
#include "debug.hpp"
extern Debug debug;
#endif

string findPath(string pathern)
{
	string path;
	glob_t g;

	#ifdef DEBUG
	stringstream str;
	debug++;
	
	debug.open("findPath");
	debug << "pathern = " + pathern;
	#endif

	glob(pathern.c_str(), GLOB_ERR, NULL, &g);

	#if DEBUG
	str << "find : " << g.gl_pathc;
	debug << str.str();

	for (unsigned char i = 0; i < g.gl_pathc; i++)
		debug << g.gl_pathv[i];
	#endif

	if (g.gl_pathc != 0)
		path = g.gl_pathv[0];
	else
		path = "";

	globfree(&g);

	#ifdef DEBUG
	debug << "path = " + path;
	debug.close("return");
	debug--;
	#endif

	return path;
}

