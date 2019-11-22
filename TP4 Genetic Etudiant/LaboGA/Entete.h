#ifndef __ENTETE_H_
#define __ENTETE_H_

#include <cstdio>
#include <cstdlib> 
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>  
#include <cmath>
#include <vector>
using namespace std;


struct TProblem							//**Définition du problème:
{
	std::string Nom;					//**Nom du fichier de données
	int NbTache;						//**Nombre de tâches
	std::vector<int> Temps;				//**Temps de réalisation pour effectuer une tâche. NB: Tableau de 0 à NbTache-1.  
	std::vector<int> PenAv;				//**Pénalité encourue par unité de temps si une tâche est réalisée d'avance. NB: Tableau de 0 à NbTache-1.
	std::vector<int> PenRe;				//**Pénalité encourue par unité de temps si une tâche est réalisée en retard. NB: Tableau de 0 à NbTache-1.
	int DateDue;						//**Date due commune pour l'ensemble des tâches
	int Somme;							//**Somme des temps de réalisation pour l'ensemble des tâches (pour calculer date due commune)
	double H;							//**Paramètre du problème permettant de calculer la date d'échéance (due) commune
};

struct TIndividu						//**Définition d'une solution: 
{
	std::vector<int> Seq;				//**Ensemble de gènes: indique la séquence de réalisation des tâches. NB: Tableau de 0 à NbTache-1.
	long FctObj;						//**Valeur de la fonction obj: Sommation des pénalités de retard et d'avance
	std::vector<int> Fin;				//**Temps de terminaison d'une tâche dans la séquence
	std::vector<int> TAvance;			//**Indique le nombre d'unités de temps qu'une tâche est terminée d'avance
	std::vector<int> TRetard;			//**Indique le nombre d'unités de temps qu'une tâche est terminée en retard
	std::vector<int> PAvance;			//**Calcul de la pénalité d'avance pour une tâche (PAvance * TAvance)
	std::vector<int> PRetard;			//**Calcul de la pénalité de retard pour une tâche (PRetard * TRetard)
};

struct TGenetic
{
	int		TaillePop;					//**Taille de la population (nombre d'individus)
	int		TaillePopEnfant;			//**Taille de la populationEnfant (nombre d'enfants)
	double	ProbCr;						//**Probabilité de croisement [0%,100%]
	double	ProbMut;					//**Probabilité de mutation [0%,100%] 
	int		Gen;						//**Compteur du nombre de générations

	int		CptEval;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
};

#endif