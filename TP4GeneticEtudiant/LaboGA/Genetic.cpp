#include <windows.h>
#include "Entete.h"
#include <stdlib.h>
#include <time.h>
#pragma comment (lib,"GeneticDLL.lib")

//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Pour une ex�cution � l'aide du compilateur:
//Le fichier de probleme (.txt)  doit se trouver dans le r�pertoire courant du projet;
//les fichiers de la DLL (GeneticDLL.dll et GeneticDLL.lib) doivent se trouver dans le r�pertoire courant du projet et
//dans le r�pertoire de l'ex�cutable (DEBUG ou RELEASE).
//Indiquer les arguments du programme dans "propri�t�s du projet" - "d�bogage" - "arguements".
//Sinon, utiliser le r�pertoire EXECUTION.

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem & unProb, TGenetic &unGenetic);

//DESCRIPTION:	Fonction d'affichage � l'�cran permettant de voir si les donn�es du fichier probl�me ont �t� lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme (TProblem unProb);

//DESCRIPTION:	�valuation de la fonction objectif d'une solution et MAJ du compteur d'�valuation. 
//				Retourne un long repr�sentant la somme des p�nalit�s d'avance et de retard
extern "C" _declspec(dllimport) void EvaluerSolution(TIndividu & uneSol, TProblem unProb, TGenetic &unGenetic);

//DESCRIPTION: Fonction qui g�n�re une population initiale en s'assurant d'avoir des solutions valides. L'�valuation de leur fonction objectif est r�alis�e*/
extern "C" _declspec(dllimport)void CreerPopInitialeAlea(std::vector<TIndividu> & unePop, TProblem unProb, TGenetic & unGenetic);

//DESCRIPTION: Fonction qui affiche le d�tail des solutions (de Debut jusqu'a Fin-1) dans la population courante. 
//Le dernier param�tre permet de faire afficher le d�tail de l'�valuation de la solution
extern "C" _declspec(dllimport) void AfficherSolutions(std::vector<TIndividu> unePop, int Debut, int Fin, int Iter, TProblem unProb, bool AvecCalcul);

//DESCRIPTION: Fonction de tri croissant des individus dans la population entre Debut et Fin-1 INCLUSIVEMENT 
extern "C" _declspec(dllimport) void TrierPopulation(std::vector<TIndividu> & unePop, int Debut, int Fin);

//DESCRIPTION: Copie de la s�quence et de la fonction objectif dans une nouvelle TSolution. La nouvelle TSolution est retourn�e.
extern "C" _declspec(dllimport) void CopierSolution (const TIndividu uneSol, TIndividu &Copie, TProblem unProb);

//DESCRIPTION: Fonction qui r�alise la MUTATION (modification al�atoire) d'une solution: Inversion de sous-s�quence (50%) ou �change de 2 t�ches(50%)
extern "C" _declspec(dllimport) void Mutation(TIndividu & Mutant, TProblem unProb, TGenetic & unGen);

//DESCRIPTION: Fonction de s�lection d'un individu par tournoi (taille 2). Validation afin de ne pas s�lectionner 2 fois le m�me individu
extern "C" _declspec(dllexport) int Selection (std::vector<TIndividu> unePop, int _Taille, TProblem unProb);

//DESCRIPTION: Fonction affichant les r�sultats de l'algorithme g�n�tique
extern "C" _declspec(dllexport) void AfficherResultats (TIndividu uneBest, TProblem unProb, TGenetic unGen);

//DESCRIPTION: Fonction affichant les r�sultats de l'algorithme g�n�tique dans un fichier texte
extern "C" _declspec(dllexport) void AfficherResultatsFichier (TIndividu uneBest, TProblem unProb, TGenetic unGen, std::string FileName);

//DESCRIPTION:	Lib�ration de la m�moire allou�e dynamiquement
extern "C" _declspec(dllexport) void LibererMemoireFinPgm (std::vector<TIndividu> & unePop, std::vector<TIndividu> & unePopEnfant, TIndividu & uneBest, TProblem & unProb, TGenetic unGen);

//*****************************************************************************************
// Prototype des fonctions locales pour vos d�veloppements
//*****************************************************************************************
TIndividu Croisement(TIndividu Parent1, TIndividu Parent2, TProblem unProb, TGenetic & unGen);
void Remplacement(std::vector<TIndividu> & Parents, std::vector<TIndividu> Enfants, TProblem unProb, TGenetic unGen);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char *Param[])
{
	TProblem LeProb;					//**D�finition de l'instance de probl�me
	TGenetic LeGenetic;					//**D�finition des param�tres de l'algorithme
	std::vector<TIndividu> Pop;			//**Population compos�e de Taille_Pop Individus 
	std::vector<TIndividu> PopEnfant;	//**Population d'enfant
	TIndividu Best;						//**Meilleure solution depuis le d�but de l'algorithme
	
	int Pere, Mere;						//**Indices de solution des parents
	int i;
	double Alea;
	
	string NomFichier;
	
	//**Lecture des param�tres
	NomFichier.assign(Param[1]);
	LeGenetic.TaillePop		= atoi(Param[2]);
	LeGenetic.ProbCr		= atof(Param[3]);
	LeGenetic.ProbMut		= atof(Param[4]);
	LeGenetic.NB_EVAL_MAX	= atoi(Param[5]);
	LeProb.H				= atof(Param[6]);
	LeGenetic.TaillePopEnfant	= (int)ceil(LeGenetic.ProbCr * LeGenetic.TaillePop);
	LeGenetic.Gen = 0;

	//**D�finition de la dimension des tableaux
	Pop.resize(LeGenetic.TaillePop);				//**Le tableau utilise les indices de 0 � TaillePop-1.
	PopEnfant.resize(LeGenetic.TaillePopEnfant);	//**Le tableau utilise les indices de 0 � TaillePopEnfant-1

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
	
	//**Boucle principale de l'algorithme g�n�tique
	do 
	{
		LeGenetic.Gen ++;
		//**S�lection et croisement
		for (i=0; i<LeGenetic.TaillePopEnfant; i++)
		{
			//**S�LECTION de deux parents
			Pere = Selection(Pop, LeGenetic.TaillePop, LeProb);
			Mere = Selection(Pop, LeGenetic.TaillePop, LeProb);
			
			//**CROISEMENT entre les deux parents. Cr�ation d'UN enfant.
			PopEnfant[i] = Croisement(Pop[Pere], Pop[Mere], LeProb, LeGenetic);

			//**MUTATION d'une solution
			Alea = double(rand()) / double(RAND_MAX);
			if (Alea < LeGenetic.ProbMut)
			{
				//V�rification pour ne pas perdre la meilleure solution connue avant mutation
				if (Best.FctObj > PopEnfant[i].FctObj) 
					CopierSolution(PopEnfant[i], Best, LeProb);
				Mutation(PopEnfant[i], LeProb, LeGenetic);
			}
		}
		//AfficherSolutions(PopEnfant, 0, LeGenetic.TaillePopEnfant, LeGenetic.Gen, LeProb, false);
		
		//**REMPLACEMENT de la population pour la prochaine g�n�ration
		Remplacement(Pop, PopEnfant, LeProb, LeGenetic);
		//AfficherSolutions(Pop, 0, LeGenetic.TaillePop, LeGenetic.Gen, LeProb, false);

		//**Conservation de la meilleure solution
		TrierPopulation(Pop, 0, LeGenetic.TaillePop);
		if (Best.FctObj > Pop[0].FctObj)				//**NE PAS ENLEVER
			CopierSolution(Pop[0], Best, LeProb);
		cout << "Meilleure solution trouvee (Generation# "<< LeGenetic.Gen << "): " << Best.FctObj << endl;

	}while (LeGenetic.CptEval < LeGenetic.NB_EVAL_MAX);	//**NE PAS ENLEVER

	AfficherResultats (Best, LeProb, LeGenetic);		//**NE PAS ENLEVER
	AfficherResultatsFichier (Best, LeProb, LeGenetic, "Resutats.txt");
	
	LibererMemoireFinPgm(Pop, PopEnfant, Best, LeProb, LeGenetic);

	system("PAUSE");
	return 0;
}


/*Offre une bonne performance dans notre cas car on garde la position absolue de certain �l�ments faisant partie
d'une coupe et on ne se fie pas � la position relative des �l�ments. Il n'y a pas de rotation comme dans le probl�me du
voyageur de commerce.*/
void PMX(TIndividu Parent1, TIndividu Parent2, TIndividu &Enfant)
{
	//Pour g�n�rer un nombre al�atoire � chaque fois
	srand(time(NULL));

	//nombre aleatoire entre 0 et nombre de taches
	int debutCoupe = rand() % (Parent1.Seq.size() - 1); //-1?

	srand(time(NULL));

	//nombre aleatoire entre debutCoupe et nombre de taches
	int finCoupe = rand() % (Parent1.Seq.size() - debutCoupe) + debutCoupe;

	//On place les taches compris dans la coupe aux m�mes positions dans l'enfant
	for (int i = debutCoupe; i <= finCoupe; i++)
	{
		Enfant.Seq.at(i) = Parent1.Seq.at(i);
	}

	//Boucle pour chaque tache qui fait partie de la coupe
	for (int i = debutCoupe; i <= finCoupe; i++)
	{
		//On prend la valeur qui se trouve � la position i dans le parent 2
		int valeurChercher = Parent2.Seq.at(i);
		//On chercher cette valeur dans le parent 1
		vector<int>::iterator it = find(Parent1.Seq.begin() + debutCoupe, Parent1.Seq.begin() + finCoupe + 1, valeurChercher);

		//Si la valeur se retrouve � une position qui fait partie de la coupe, on doit chercher une nouvelle position � cette valeur dans l'enfant
		if (it == Parent1.Seq.begin() + finCoupe + 1)
		{
			bool positionTrouve = false;
			//On prend la valeur qui se trouve � la m�me position que i dans le parent 1
			int valeurParent1 = Parent1.Seq.at(i);
			int indexValeurParent1DansParent2;
			do
			{
				//on trouve cette valeur dans parent 2
				vector<int>::iterator it = find(Parent2.Seq.begin(), Parent2.Seq.end(), valeurParent1);
				if (it != Parent2.Seq.end()) {
					indexValeurParent1DansParent2 = distance(Parent2.Seq.begin(), it);
				}
				/*si la valeur dans le parent 2 n'est pas � une position qui fait partie de la coupe,
				on insert la valeur original(celle � la position du i dans le parent 2) dans l'enfant � la position trouv�*/
				if (indexValeurParent1DansParent2 < debutCoupe || indexValeurParent1DansParent2 > finCoupe)
				{
					Enfant.Seq.at(indexValeurParent1DansParent2) = Parent2.Seq.at(i);
					positionTrouve = true;
				}
				else
				{
					//sinon, on recommence avec la valeur qui se trouve � la m�me position dans le parent 1 jusqu'a ce qu'on trouve une position ad�quate
					valeurParent1 = Parent1.Seq.at(indexValeurParent1DansParent2);
				}
			} while (!positionTrouve);
		}
	}

	//On ajoute les �l�ments manquant de l'enfant avec les �l�ments de parent 2 qui sont aux m�mes positions
	for (int i = 0; i < Enfant.Seq.size(); i++)
	{
		if (Enfant.Seq.at(i) == 0) {
			Enfant.Seq.at(i) = Parent2.Seq.at(i);
		}
	}
}


//******************************************************************************************************
//**Fonction qui r�alise le CROISEMENT (�change de genes) entre deux parents. Retourne l'enfant produit.
//******************************************************************************************************
//**A D�FINIR PAR L'�TUDIANT****************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAM�TRES AINSI QUE LE RETOUR DE LA FONCTION
//****************************************************************************************************** 
TIndividu Croisement(TIndividu Parent1, TIndividu Parent2, TProblem unProb, TGenetic & unGen)
{
	//**INDICE: Le sous-programme rand() g�n�re al�atoirement un nombre entier entre 0 et RAND_MAX (i.e., 32767) inclusivement.
	//**Pour tirer un nombre al�atoire entier entre 0 et MAX-1 inclusivement, il suffit d'utiliser l'instruction suivante : NombreAleatoire = rand() % MAX;
	
	TIndividu Enfant;
	int j;

	//Cr�ation de l'enfant selon la taille du probl�me
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
//Fonction qui r�alise le REMPLACEMENT de la population pour la prochaine g�n�ration. Cette fonction doit
//prendre les TaillePop solutions de la population "Parents" et les TaillePopEnfant solutions de la 
//population "Enfants" et retenir SEULEMENT TaillePop solutions pour commencer la prochaine g�n�ration. 
//Les TaillePop solutions retenues doivent �tre plac�es dans la population "Parents".  
//*******************************************************************************************************
//**A D�FINIR PAR L'�TUDIANT*****************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAM�TRES
//******************************************************************************************************* 
void Remplacement(std::vector<TIndividu> & Parents, std::vector<TIndividu> Enfants, TProblem unProb, TGenetic unGen)
{
	//**D�claration et dimension dynamique d'une population temporaire pour contenir tous les parents et les enfants
	//std::vector<TIndividu> Temporaire;
	//Temporaire.resize(unGen.TaillePop + unGen.TaillePopEnfant);

	//**Pour trier toute la population temporaire, il suffit de faire l'appel suivant: TrierPopulation(Temporaire, 0, unGen.TaillePop+unGen.TaillePopEnfant);
	//**Pour copie une solution de Parents dans Temporaire, il suffit de faire l'appel suivant: CopierSolution(Parents[i], Temporaire[i], unProb);
	

	//METHODE BIDON: La population Parents demeure inchang�e

	
	//**Lib�ration de la population temporaire
	//for(i=0; i< unGen.TaillePop; i++)
	//{
	//	Temporaire[i].Seq.clear();
	//	Temporaire[i].Fin.clear();
	//	Temporaire[i].PAvance.clear();
	//	Temporaire[i].PRetard.clear();
	//	Temporaire[i].TAvance.clear();
	//	Temporaire[i].TRetard.clear();
	//}
	//Temporaire.clear();
}
