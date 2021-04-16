

/*
Bonjour, ce travail est fait par deux étudiant :

CHAOUKI Mohamed
code permanent: CHAM27088802

Christian Koy Okitapoy
code permanent: OKIK08078702

*/

#include "DocumentXML.hpp"
#include "ElementXML.hpp"
#include "Histoire.hpp"
#include "Lecteur.hpp"
#include "arbreMapAvl.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// structure qui stoque la frequece du mot dans chaque titre d histoire
struct FrequenceHistoire {
  double frequence;
  string titre;
  // Trie un tableau de frequence d'histoire du plus grand au plus petit.
  bool operator() (FrequenceHistoire i,FrequenceHistoire j) {
    return (i.frequence > j.frequence);}
} frequenceHistoire;

vector<FrequenceHistoire> tableauFH;
FrequenceHistoire fh;
string motUtilisateur;
double idf = 0;
double sommeFrequence = 0;

void genererArbreFrequence(const vector<Histoire *> *histoires, ArbreMapAVL<string,int> *&avl,ArbreMapAVL<int,string> &arbreTitres);
void genererArbreIDF(int nbrHistoires, ArbreMapAVL<string,int> *&avl, ArbreMapAVL<string,double> &arbreIDF);
void appliquerFormulerIDF(ArbreMapAVL<string,double> &arbreIDF, int nbrHistoires);

vector< Histoire * > *
lireDocuments( string a_nomFichier )
{
    vector< Histoire * > * histoires = new vector< Histoire * >();
    DocumentXML * listeFichiers = lireFichierXML( a_nomFichier );

    assert( nullptr != listeFichiers );
    ElementXML * courant = listeFichiers->racine();
    assert( nullptr != courant );

    courant = courant->prochainElement();

    assert( nullptr != courant );
    for( NoeudXML * contenu : * courant )
    {
        assert( nullptr != contenu );
        if( contenu->estElement() )
        {
            ElementXML * element = ( ElementXML * )contenu;

            assert( nullptr != element );
            DocumentXML * doc = lireFichierXML( element->attribut( string( "fichier" ) ) );

            assert( nullptr != doc );
            vector< Histoire * > * h = extraireHistoires( * doc );

            assert( nullptr != h );
            assert( nullptr != histoires );
            histoires->insert( histoires->end(), h->begin(), h->end() );
        }
    }

    return histoires;
}


/*
  Cette methode cree Un arbre d'histoire et insere les titres d'histoires a l'interieur ensuite
  elle insere dans l'arbre de titres d'histoire les mots.
  @param : vecteur de pointeur d'nbrHistoires
  @param : un arbre avlmap
  @param : un arbre de titre d'histoires
*/
void genererArbreFrequence(const vector<Histoire *> *histoires, ArbreMapAVL<string,int> *&avl,ArbreMapAVL<int,string> &arbreTitres){


  int tempChiffre = 0;
  int index = 0;

  for( Histoire * histoire : * histoires )
  {
        arbreTitres.inserer(index,histoire->titre());
        avl[index] = ArbreMapAVL<string,int>();

            for( Phrase p : * histoire )
            {

                for(auto iterPhrase = p.begin(); iterPhrase != p.end(); ++iterPhrase){
                    string mot =  *iterPhrase;

                    if(avl[index].contient(mot)){
                        tempChiffre =  avl[index].operator[](mot) + 1;
                        avl[index].inserer(mot,tempChiffre);

                    }else{
                        avl[index].inserer(mot,1);
                    }

                }

            }
        ++index;
    }


}


/*
  Cette methode genere un arbre IDF. Elle cherche le mot dans l'arbre avl puis une fois trouver
  si il est deja dedans elle l'increment de 1 pour connaitre son nombre d'occurences.
  Si elle ne le trouve pas, elle l ajoute a l arbre et initialise son nombre d occurence a 1.
  @param : nombre d'hitroires
  @param : arbre map avl
  @param : arbre IDF
*/
void genererArbreIDF(int nbrHistoires, ArbreMapAVL<string,int> *&avl, ArbreMapAVL<string,double> &arbreIDF){

  string mot;
  double nbrOccurence = 0;

  for(int i = 0; i < nbrHistoires; i++){

    if(i == 0){

          for(auto iter = avl[i].debut(); iter != avl[i].fin(); ++iter){
              mot =  avl[i].operator[](iter);
              arbreIDF.inserer(mot,1.0);
          }

    }else{
          for(auto iter = avl[i].debut(); iter != avl[i].fin(); ++iter){

              mot =  avl[i].operator[](iter);

              if(arbreIDF.contient(mot)){
                  nbrOccurence = arbreIDF.operator[](mot) + 1;
                  arbreIDF.inserer(mot,nbrOccurence);
              }else{
                  arbreIDF.inserer(mot,1.0);
              }

          }

    }

  }

  appliquerFormulerIDF(arbreIDF,nbrHistoires);


}

/*
Cette methode prend tous les mots qui se trouvent dans l arbre et applique
sur eux la formule IDF.
@param : arebre idf
@param : nombre d histoire
*/
void appliquerFormulerIDF(ArbreMapAVL<string,double> &arbreIDF, int nbrHistoires){

  string mot ;
  double cc = 0;
  double idf = 0;
  for(auto iter = arbreIDF.debut(); iter != arbreIDF.fin(); ++iter){
      mot = arbreIDF.operator[](iter);
      cc = arbreIDF.operator[](mot);
      idf = log2(nbrHistoires/cc);
      arbreIDF.inserer(mot,idf);

  }
}

//main
int main() {
    // gardez la ligne suivante, elle lit le corpus et le place dans la structure de base.
    // Vous avez donc un vecteur d'histoire qui contient l'information sur les histoires,
    // les Phrases et les mots qu'elles contiennent.		arbreavl.o
    vector< Histoire * > * histoires = lireDocuments( string( "listeDocument.xml" ) );

// Creation des arbres
  ArbreMapAVL<string,int> *avl = new ArbreMapAVL<string,int>[histoires->size()];
  ArbreMapAVL<int,string> arbreTitres;
  ArbreMapAVL<string,double> arbreIDF;

// Generer les arbres
  genererArbreFrequence(histoires,avl,arbreTitres);
  genererArbreIDF(histoires->size(),avl,arbreIDF);

  string stringUtilisateur = "";

  do{
      cout << "Entrez votre requete : "<< endl;

      getline (cin, stringUtilisateur);

// si le cin est vide on vide les arbres puis on quitte le programme
      if (stringUtilisateur.length()==0){
          avl->vider();
          arbreTitres.vider();
          arbreIDF.vider();
          exit (-1);
      }

      Phrase entree(stringUtilisateur);

// deuxieme calcul
      for(int i = 0; i < histoires->size(); ++i){

          for(auto iter = entree.begin(); iter != entree.end(); ++iter){

              motUtilisateur = *iter;

              if(avl[i].contient(motUtilisateur)){
                  idf = arbreIDF.operator[](motUtilisateur);
                  sommeFrequence += (idf * avl[i].operator[](motUtilisateur));
              }

          }

          fh.frequence = sommeFrequence;
          fh.titre = arbreTitres.operator[](i);
          tableauFH.push_back(fh);

          idf = 0;
          sommeFrequence = 0;
      }

// trier le resultat par le premier double en premier
      sort(tableauFH.begin(), tableauFH.end(), frequenceHistoire);

      for (int k = 0; k < 5; ++k) {
          cout << tableauFH[k].frequence << " : " << tableauFH[k].titre << endl;

      }

      tableauFH.clear();


  }while(stringUtilisateur.length()!=0);

  return 0;

}
