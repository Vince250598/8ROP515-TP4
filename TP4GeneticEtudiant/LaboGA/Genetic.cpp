#include <windows.h>
#include "Entete.h"
#include <stdlib.h>
#include <time.h>
#pragma comment (lib,"GeneticDLL.lib")

/*Les modifications que nous avons effectuer commencent aux lignes: 70, 90, 144, 166, 184, 290, 306*/

//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Pour une exécution à l'aide du compilateur:
//Le fichier de probleme (.txt)  doit se trouver dans le répertoire courant du projet;
//les fichiers de la DLL (GeneticDLL.dll et GeneticDLL.lib) doivent se trouver dans le répertoire courant du projet et
//dans le répertoire de l'exécutable (DEBUG ou RELEASE).
//Indiquer les arguments du programme dans "propriétés du projet" - "débogage" - "arguements".
//Sinon, utiliser le répertoire EXECUTION.

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem & unProb, TGenetic &unGenetic);

//DESCRIPTION:	Fonction d'affichage à l'écran permettant de voir si les données du fichier problème ont été lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme(TProblem unProb);

//DESCRIPTION:	Évaluation de la fonction objectif d'une solution et MAJ du compteur d'évaluation. 
//				Retourne un long représentant la somme des pénalités d'avance et de retard
extern "C" _declspec(dllimport) void EvaluerSolution(TIndividu & uneSol, TProblem unProb, TGenetic &unGenetic);

//DESCRIPTION: Fonction qui génére une population initiale en s'assurant d'avoir des solutions valides. L'évaluation de leur fonction objectif est réalisée*/
extern "C" _declspec(dllimport)void CreerPopInitialeAlea(std::vector<TIndividu> & unePop, TProblem unProb, TGenetic & unGenetic);

//DESCRIPTION: Fonction qui affiche le détail des solutions (de Debut jusqu'a Fin-1) dans la population courante. 
//Le dernier paramètre permet de faire afficher le détail de l'évaluation de la solution
extern "C" _declspec(dllimport) void AfficherSolutions(std::vector<TIndividu> unePop, int Debut, int Fin, int Iter, TProblem unProb, bool AvecCalcul);

//DESCRIPTION: Fonction de tri croissant des individus dans la population entre Debut et Fin-1 INCLUSIVEMENT 
extern "C" _declspec(dllimport) void TrierPopulation(std::vector<TIndividu> & unePop, int Debut, int Fin);

//DESCRIPTION: Copie de la séquence et de la fonction objectif dans une nouvelle TSolution. La nouvelle TSolution est retournée.
extern "C" _declspec(dllimport) void CopierSolution(const TIndividu uneSol, TIndividu &Copie, TProblem unProb);

//DESCRIPTION: Fonction qui réalise la MUTATION (modification aléatoire) d'une solution: Inversion de sous-séquence (50%) ou échange de 2 tâches(50%)
extern "C" _declspec(dllimport) void Mutation(TIndividu & Mutant, TProblem unProb, TGenetic & unGen);

//DESCRIPTION: Fonction de sélection d'un individu par tournoi (taille 2). Validation afin de ne pas sélectionner 2 fois le même individu
extern "C" _declspec(dllexport) int Selection(std::vector<TIndividu> unePop, int _Taille, TProblem unProb);

//DESCRIPTION: Fonction affichant les résultats de l'algorithme génétique
extern "C" _declspec(dllexport) void AfficherResultats(TIndividu uneBest, TProblem unProb, TGenetic unGen);

//DESCRIPTION: Fonction affichant les résultats de l'algorithme génétique dans un fichier texte
extern "C" _declspec(dllexport) void AfficherResultatsFichier(TIndividu uneBest, TProblem unProb, TGenetic unGen, std::string FileName);

//DESCRIPTION:	Libération de la mémoire allouée dynamiquement
extern "C" _declspec(dllexport) void LibererMemoireFinPgm(std::vector<TIndividu> & unePop, std::vector<TIndividu> & unePopEnfant, TIndividu & uneBest, TProblem & unProb, TGenetic unGen);

//*****************************************************************************************
// Prototype des fonctions locales pour vos développements
//*****************************************************************************************
TIndividu Croisement(TIndividu Parent1, TIndividu Parent2, TProblem unProb, TGenetic & unGen);
void Remplacement(vector<TIndividu> & Parents, vector<TIndividu> Enfants, TProblem unProb, TGenetic unGen, float proportion);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char *Param[])
{
	//INITIALISATION DU RANDOM
	srand(time(NULL));
	TProblem LeProb;					//**Définition de l'instance de problème
	TGenetic LeGenetic;					//**Définition des paramètres de l'algorithme
	std::vector<TIndividu> Pop;			//**Population composée de Taille_Pop Individus 
	std::vector<TIndividu> PopEnfant;	//**Population d'enfant
	TIndividu Best;						//**Meilleure solution depuis le début de l'algorithme

	int Pere, Mere;						//**Indices de solution des parents
	int i;
	double Alea;
	float proportion;

	string NomFichier;

	//**Lecture des paramètres
	NomFichier.assign(Param[1]);
	LeGenetic.TaillePop = atoi(Param[2]);
	LeGenetic.ProbCr = atof(Param[3]);
	LeGenetic.ProbMut = atof(Param[4]);
	//MODIFICATION
	proportion = (float) 0.50;
	LeGenetic.NB_EVAL_MAX = atoi(Param[5]);
	LeProb.H = atof(Param[6]);
	LeGenetic.TaillePopEnfant = (int)ceil(LeGenetic.ProbCr * LeGenetic.TaillePop);
	LeGenetic.Gen = 0;

	//**Définition de la dimension des tableaux
	Pop.resize(LeGenetic.TaillePop);				//**Le tableau utilise les indices de 0 à TaillePop-1.
	PopEnfant.resize(LeGenetic.TaillePopEnfant);	//**Le tableau utilise les indices de 0 à TaillePopEnfant-1

	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LeGenetic);
	//AfficherProbleme(LeProb);

	//**Initialisation de la population initiale NB: Initialisation de la population entraine des evaluation de solutions.
	//**CptEval est donc = TaillePop au retour de la fonction.
	CreerPopInitialeAlea(Pop, LeProb, LeGenetic);
	//AfficherSolutions(Pop, 0, LeGenetic.TaillePop, LeGenetic.Gen, LeProb, false);

	//**Tri de la population
	TrierPopulation(Pop, 0, LeGenetic.TaillePop);
	//AfficherSolutions(Pop, 0, LeGenetic.TaillePop, LeGenetic.Gen, LeProb, false);

	//**Initialisation de de la meilleure solution
	CopierSolution(Pop[0], Best, LeProb);
	cout << endl << "Meilleure solution de la population initiale: " << Best.FctObj << endl << endl;  //**NE PAS ENLEVER

	//**Boucle principale de l'algorithme génétique
	do
	{
		LeGenetic.Gen++;
		//**Sélection et croisement
		for (i = 0; i < LeGenetic.TaillePopEnfant; i++)
		{
			//**SÉLECTION de deux parents
			Pere = Selection(Pop, LeGenetic.TaillePop, LeProb);
			Mere = Selection(Pop, LeGenetic.TaillePop, LeProb);

			//**CROISEMENT entre les deux parents. Création d'UN enfant.
			PopEnfant[i] = Croisement(Pop[Pere], Pop[Mere], LeProb, LeGenetic);

			//**MUTATION d'une solution
			Alea = double(rand()) / double(RAND_MAX);
			if (Alea < LeGenetic.ProbMut)
			{
				//Vérification pour ne pas perdre la meilleure solution connue avant mutation
				if (Best.FctObj > PopEnfant[i].FctObj)
					CopierSolution(PopEnfant[i], Best, LeProb);
				Mutation(PopEnfant[i], LeProb, LeGenetic);
			}
		}
		//AfficherSolutions(PopEnfant, 0, LeGenetic.TaillePopEnfant, LeGenetic.Gen, LeProb, false);

		//**REMPLACEMENT de la population pour la prochaine génération
		Remplacement(Pop, PopEnfant, LeProb, LeGenetic, proportion); //PARTIEL

		//AfficherSolutions(Pop, 0, LeGenetic.TaillePop, LeGenetic.Gen, LeProb, false);
		//**Conservation de la meilleure solution
		TrierPopulation(Pop, 0, LeGenetic.TaillePop);
		if (Best.FctObj > Pop[0].FctObj)				//**NE PAS ENLEVER
			CopierSolution(Pop[0], Best, LeProb);
		cout << "Meilleure solution trouvee (Generation# " << LeGenetic.Gen << "): " << Best.FctObj << endl;

	} while (LeGenetic.CptEval < LeGenetic.NB_EVAL_MAX);	//**NE PAS ENLEVER

	AfficherResultats(Best, LeProb, LeGenetic);		//**NE PAS ENLEVER
	AfficherResultatsFichier(Best, LeProb, LeGenetic, "Resutats.txt");

	LibererMemoireFinPgm(Pop, PopEnfant, Best, LeProb, LeGenetic);

	system("PAUSE");
	return 0;
}

//MODIFICATION
//Pour trouver un élément dans un individu
int TrouverElement(TIndividu individu, int valeurATrouver)
{
	int index;
	for (int i = 0; i < individu.Seq.size(); i++)
	{
		if (individu.Seq.at(i) == valeurATrouver)
		{
			index = i;
			break;
		}
	}

	return index;
}

/*Offre une bonne performance dans notre cas car on garde la position absolue de certain éléments faisant partie
d'une coupe et on ne se fie pas à la position relative des éléments. Il n'y a pas de rotation comme dans le problème du
voyageur de commerce.*/
void PMX(TIndividu Parent1, TIndividu Parent2, TIndividu &Enfant)
{
	int longueurMaxCoupe = (int) Enfant.Seq.size()/3;

	//Pour marquer les taches non assignees
	for (int i = 0; i < Enfant.Seq.size(); i++)
	{
		Enfant.Seq.at(i) = -999;
	}

	int indexValeurParent1DansParent2;

	//nombre aleatoire entre 0 et nombre de taches
	int debutCoupe = rand() % (Parent1.Seq.size() - 1);

	//srand(time(NULL));

	//nombre aleatoire entre debutCoupe et nombre de taches
	//int finCoupe = rand() % (Parent1.Seq.size() - debutCoupe) + debutCoupe;
	int longueurCoupe = rand() % (longueurMaxCoupe);
	int finCoupe = debutCoupe + longueurCoupe;
	if (finCoupe > (Enfant.Seq.size()-1))
	{
		finCoupe = (Enfant.Seq.size()-1);
	}

	//On place les taches compris dans la coupe aux mêmes positions dans l'enfant
	for (int i = debutCoupe; i <= finCoupe; i++)
	{
		Enfant.Seq.at(i) = Parent1.Seq.at(i);
	}

	//Boucle pour chaque tache qui fait partie de la coupe
	for (int i = debutCoupe; i <= finCoupe; i++)
	{
		//On prend la valeur qui se trouve à la position i dans le parent 2
		int valeurChercher = Parent2.Seq.at(i);
		//On chercher cette valeur dans le parent 1
		int index = TrouverElement(Parent1, valeurChercher);

		//Si la valeur se retrouve à une position qui fait partie de la coupe, la valeur est deja dans l'enfant
		if (index > finCoupe || index < debutCoupe)
		{
			bool positionTrouve = false;
			//On prend la valeur qui se trouve à la même position que i dans le parent 1
			int valeurParent1 = Parent1.Seq.at(i);
			do
			{
				//on trouve cette valeur dans parent 2
				indexValeurParent1DansParent2 = TrouverElement(Parent2, valeurParent1);

				/*si la valeur dans le parent 2 n'est pas à une position qui fait partie de la coupe,
				on insert la valeur original(celle à la position du i dans le parent 2) dans l'enfant à la position trouvé*/
				if (indexValeurParent1DansParent2 < debutCoupe || indexValeurParent1DansParent2 > finCoupe)
				{
					Enfant.Seq.at(indexValeurParent1DansParent2) = Parent2.Seq.at(i);
					positionTrouve = true;
				}
				else
				{
					//sinon, on recommence avec la valeur qui se trouve à la même position dans le parent 1 jusqu'a ce qu'on trouve une position adéquate
					valeurParent1 = Parent1.Seq.at(indexValeurParent1DansParent2);
				}
			} while (!positionTrouve);
		}
	}

	//On ajoute les éléments manquant de l'enfant avec les éléments de parent 2 qui sont aux mêmes positions
	for (int i = 0; i < debutCoupe; i++)
	{
		if (Enfant.Seq.at(i) == -999) {
			Enfant.Seq.at(i) = Parent2.Seq.at(i);
		}
	}
	for (int i = finCoupe + 1; i < Enfant.Seq.size(); i++)
	{
		if (Enfant.Seq.at(i) == -999) {
			Enfant.Seq.at(i) = Parent2.Seq.at(i);
		}
	}

	return;
}

//******************************************************************************************************
//**Fonction qui réalise le CROISEMENT (échange de genes) entre deux parents. Retourne l'enfant produit.
//******************************************************************************************************
//**A DÉFINIR PAR L'ÉTUDIANT****************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAMÈTRES AINSI QUE LE RETOUR DE LA FONCTION
//****************************************************************************************************** 
TIndividu Croisement(TIndividu Parent1, TIndividu Parent2, TProblem unProb, TGenetic & unGen)
{
	//**INDICE: Le sous-programme rand() génère aléatoirement un nombre entier entre 0 et RAND_MAX (i.e., 32767) inclusivement.
	//**Pour tirer un nombre aléatoire entier entre 0 et MAX-1 inclusivement, il suffit d'utiliser l'instruction suivante : NombreAleatoire = rand() % MAX;

	TIndividu Enfant;

	//Création de l'enfant selon la taille du problème
	Enfant.Seq.resize(unProb.NbTache);
	Enfant.Fin.resize(unProb.NbTache);
	Enfant.TAvance.resize(unProb.NbTache);
	Enfant.TRetard.resize(unProb.NbTache);
	Enfant.PAvance.resize(unProb.NbTache);
	Enfant.PRetard.resize(unProb.NbTache);

	//METHODE PMX
	PMX(Parent1, Parent2, Enfant);

	//**NE PAS ENLEVER
	EvaluerSolution(Enfant, unProb, unGen);
	return (Enfant);
}

//*******************************************************************************************************
//Fonction qui réalise le REMPLACEMENT de la population pour la prochaine génération. Cette fonction doit
//prendre les TaillePop solutions de la population "Parents" et les TaillePopEnfant solutions de la 
//population "Enfants" et retenir SEULEMENT TaillePop solutions pour commencer la prochaine génération. 
//Les TaillePop solutions retenues doivent être placées dans la population "Parents".  
//*******************************************************************************************************
//**A DÉFINIR PAR L'ÉTUDIANT*****************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAMÈTRES
//******************************************************************************************************* 
void Remplacement(vector<TIndividu> & Parents, vector<TIndividu> Enfants, TProblem unProb, TGenetic unGen, float proportion) //PARTIEL
{
	//**Pour trier toute la population temporaire, il suffit de faire l'appel suivant: TrierPopulation(Temporaire, 0, unGen.TaillePop+unGen.TaillePopEnfant);
	//**Pour copie une solution de Parents dans Temporaire, il suffit de faire l'appel suivant: CopierSolution(Parents[i], Temporaire[i], unProb);


	TrierPopulation(Parents, 0, unGen.TaillePop);
	TrierPopulation(Enfants, 0, unGen.TaillePopEnfant);
	int j = 0;
	for (int i = 0; i < unGen.TaillePop; i++)
	{
		if (i >= (int)(unGen.TaillePop * proportion))
		{
			if (j < Enfants.size()) CopierSolution(Enfants[j], Parents[i], unProb);
			j++;
		}
	}
}