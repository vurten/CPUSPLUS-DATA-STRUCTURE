/* UQAM / Département d'informatique
   INF3105 - Structures de données et algorithmes
   Squelette pour classe générique ArbreMap<K,V> pour le Lab8 et le TP2.

*/

#if !defined(__ARBREMapAVL_H__)
#define __ARBREMapAVL_H__
#include <iostream>
#include <cassert>
#include "pile.h"

template <class K, class V>
class ArbreMapAVL
{
  public:
    ArbreMapAVL();
    ArbreMapAVL(const ArbreMapAVL &);
    ~ArbreMapAVL();

    // Annonce l'existence d'une classe Iterateur.
    class Iterateur;

    void inserer(const K &valeur, const V &element);
    bool contient(const K &valeur) const;
    bool vide() const;
    void vider();

    // Fonctions pour obtenir un itérateur (position dans l'arbre)
    Iterateur debut() const;
    Iterateur fin() const;
    Iterateur rechercher(const K &) const;

    // Accès aux éléments de l'arbre via un intérateur.
    const V &operator[](const K &c) const;
    K &operator[](const Iterateur &);

    // Copie d'un arbre AVL.
    ArbreMapAVL &operator=(const ArbreMapAVL &);

  // Noeud de notre arbre qui prend une valeur et une element
  private:
    struct Noeud
    {
        Noeud(const K &valeur_, const V &element);
        K valeur;
        V element;
        int equilibre;
        Noeud *gauche, *droite;
        friend class ArbreMapAVL;
    };
    Noeud *racine;

    // Fonctions internes
    bool inserer(Noeud *&noeud, const K &valeur, const V &element);
    void rotationGaucheDroite(Noeud *&);
    void rotationDroiteGauche(Noeud *&);
    void vider(Noeud *&);
    void copier(const Noeud *, Noeud *&) const;

  public:

    class Iterateur
    {
      public:
        Iterateur(const ArbreMapAVL &a);
        Iterateur(const Iterateur &a);
        Iterateur(const ArbreMapAVL &a, Noeud *c);

        operator bool() const;

        Iterateur &operator++();

      private:
        const ArbreMapAVL &arbre_associe;
        Noeud *courant;
        Pile<Noeud *> chemin;

        friend class ArbreMapAVL;
    };
};


// construie un nouveau noeud
template <class K, class V>
ArbreMapAVL<K, V>::Noeud::Noeud(const K &c, const V &v)
    : valeur(c), element(v), equilibre(0), gauche(NULL), droite(NULL)
{
}

// mettre la racine a nul
template <class K, class V>
ArbreMapAVL<K, V>::ArbreMapAVL()
    : racine(NULL)
{
}

template <class K, class V>
ArbreMapAVL<K, V>::ArbreMapAVL(const ArbreMapAVL<K, V> &autre)
    : racine(NULL)
{
    copier(autre.racine, racine);
}

// destructeur
template <class K, class V>
ArbreMapAVL<K, V>::~ArbreMapAVL()
{
    vider(racine);
}

//cette methode prend une valeur et verifie si elle existe dans l'arbre
//@param : une valeur
//@return : vrai si elle s y trouve sinon faux
template <class K, class V>
bool ArbreMapAVL<K, V>::contient(const K &valeur) const
{
    bool trouver = false;
    Noeud *noeudTemporaire = racine;

    while (noeudTemporaire != NULL && noeudTemporaire->valeur != valeur)
    {
        if (noeudTemporaire->valeur < valeur)
            noeudTemporaire = noeudTemporaire->droite;
        else
            noeudTemporaire = noeudTemporaire->gauche;
    }

    if (noeudTemporaire == NULL){
        trouver = false;
    }else{
        trouver = true;
    }   

    return trouver; 
}

// fait appel a la methode inserer afin d inserer l'element
template <class K, class V>
void ArbreMapAVL<K, V>::inserer(const K &valeur, const V &element)
{
    inserer(racine, valeur, element);
}

//inserer un noeud dans l'arbre. Et l'equilibrer soit avec une rotation a gauche ou a droite
template <class K, class V>
bool ArbreMapAVL<K, V>::inserer(Noeud *&noeud, const K &valeur, const V &element)
{
    
    if (noeud == NULL)
    {
        noeud = new Noeud(valeur, element);
        return true;
    }
    
    if (valeur > noeud->valeur)
    {
        if (inserer(noeud->droite, valeur, element))
        {
            noeud->equilibre--;
            if (noeud->equilibre == 0)
                return false;
            if (noeud->equilibre == -1)
                return true;
            assert(noeud->equilibre == -2);
            if (noeud->droite->equilibre == 1)
                rotationGaucheDroite(noeud->droite);
            rotationDroiteGauche(noeud);
        }
        return false;
    }

    else if (valeur < noeud->valeur)
    {
        if (inserer(noeud->gauche, valeur, element))
        {
            noeud->equilibre++;
            if (noeud->equilibre == 0)
                return false;
            if (noeud->equilibre == 1)
                return true;
            assert(noeud->equilibre == 2);
            if (noeud->gauche->equilibre == -1)
                rotationDroiteGauche(noeud->gauche);
            rotationGaucheDroite(noeud);
        }
        return false;
    }

    noeud->valeur = valeur;
    noeud->element = element;
    return false;
}

// methode pour faire une rotation gauche droite afin d equilibrer l arbre
//@param noeud
template <class K, class V>
void ArbreMapAVL<K, V>::rotationGaucheDroite(Noeud *&racinesousarbre)
{
    Noeud *temp = racinesousarbre->gauche;
    int ea = temp->equilibre;
    int eb = racinesousarbre->equilibre;
    int neb = -(ea > 0 ? ea : 0) - 1 + eb;
    int nea = ea + (neb < 0 ? neb : 0) - 1;

    temp->equilibre = nea;
    racinesousarbre->equilibre = neb;
    racinesousarbre->gauche = temp->droite;
    temp->droite = racinesousarbre;
    racinesousarbre = temp;
}

// methode pour faire une rotation droite gauche afin d equilibrer l arbre
//@param noeud
template <class K, class V>
void ArbreMapAVL<K, V>::rotationDroiteGauche(Noeud *&racinesousarbre)
{
    Noeud *temp = racinesousarbre->droite;
    int ea = temp->equilibre;
    int eb = racinesousarbre->equilibre;
    int neb = -(ea < 0 ? ea : 0) + 1 + eb;
    int nea = ea + (neb > 0 ? neb : 0) + 1;

    temp->equilibre = nea;
    racinesousarbre->equilibre = neb;
    racinesousarbre->droite = temp->gauche;
    temp->gauche = racinesousarbre;
    racinesousarbre = temp;
}


// vider l'arbre
template <class K, class V>
void ArbreMapAVL<K, V>::vider()
{
    vider(racine);
}

//vider un noeud
template <class K, class V>
void ArbreMapAVL<K, V>::vider(Noeud *&noeud)
{
    if(noeud != NULL){
        vider(noeud->droite);
        vider(noeud->gauche);
        delete noeud;
    }
}

// fait une copie du noeud
//@param : noeud source
//@param : noeud qu on veut creer
template <class K, class V>
void ArbreMapAVL<K, V>::copier(const Noeud *source, Noeud *&noeud) const
{   

    if (source != NULL){
        noeud = new Noeud (source->valeur, source->element);
        noeud->equilibre = source->equilibre;
        copier(source->gauche, noeud->gauche);
        copier(source->droite, noeud->droite);
      }
}

//
template <class K, class V>
const V &ArbreMapAVL<K, V>::operator[](const K &c) const
{
    return rechercher(c).courant->element;
}


template <class K, class V>
K &ArbreMapAVL<K, V>::operator[](const Iterateur &iterateur)
{
    assert(&iterateur.arbre_associe == this);
    assert(iterateur.courant);
    return iterateur.courant->valeur;
}

template <class K, class V>
ArbreMapAVL<K, V> &ArbreMapAVL<K, V>::operator=(const ArbreMapAVL &autre)
{
    if (this == &autre){
        return *this;
    }    
    vider();
    copier(autre.racine, racine);
    return *this;

}


template <class K, class V>
ArbreMapAVL<K, V>::Iterateur::Iterateur(const ArbreMapAVL &a)
    : arbre_associe(a), courant(NULL)
{
}

template <class K, class V>
ArbreMapAVL<K, V>::Iterateur::Iterateur(const ArbreMapAVL<K, V>::Iterateur &a)
    : arbre_associe(a.arbre_associe)
{
    courant = a.courant;
    chemin = a.chemin;
}

// Pré-incrément
template <class K, class V>
typename ArbreMapAVL<K, V>::Iterateur &ArbreMapAVL<K, V>::Iterateur::operator++()
{

    if (courant->droite == NULL){
       if (!chemin.vide()){
          courant = chemin.depiler();
       }else{
          courant = NULL;
       }
   }else {
       courant = courant->droite;

      while (courant->gauche != NULL){
          chemin.empiler(courant);
           courant = courant->gauche;
      }
   }
}


template <class K, class V>
ArbreMapAVL<K, V>::Iterateur::operator bool() const
{
    return courant != NULL;
}


template <class K, class V>
typename ArbreMapAVL<K, V>::Iterateur ArbreMapAVL<K, V>::debut() const
{
    Iterateur iter(*this);
    iter.courant = racine;
    if (iter.courant->gauche != NULL){
        while (iter.courant->gauche != NULL)
        {
            iter.chemin.empiler(iter.courant);
            iter.courant = iter.courant->gauche;
        }
    }    
    return iter;
}

template <class K, class V>
typename ArbreMapAVL<K, V>::Iterateur ArbreMapAVL<K, V>::fin() const
{
   return Iterateur(*this);
}

template <class K, class V>
typename ArbreMapAVL<K, V>::Iterateur ArbreMapAVL<K, V>::rechercher(const K &e) const
{
    Iterateur iter(*this);
    Noeud *noeud = racine;

    if (noeud != NULL){
        while (noeud)
        {
            if (noeud->valeur < e )
            {
                iter.chemin.empiler(noeud);
                noeud = noeud->droite;
            }
            else if (noeud->valeur > e){
                noeud = noeud->gauche;
            }else
            {
            iter.courant = noeud;
            return iter;
            }
        }
    }
    iter.chemin.vider();
    return iter;
}

#endif
