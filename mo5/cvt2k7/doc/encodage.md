# Structure logique des fichiers binaires (images memoire)

Ces explications sont celles de Daniel Coulom (2005).
La version orginale se trouve :
[ici](http://dcmoto.free.fr/forum/messages/591147_0.html).

## Fichier binaire

Un fichier binaire est constitue par une suite d'enregistrements.

Ces enregistrements sont de deux types :

* Enregistrement contenant des donnees :

    * 0 type d'enregistrement = 00
    * 1-2 longueur des données
    * 3-4 adresse de chargement
    * 5-x données

* Enregistrement contenant l'adresse d’exécution :

    * 0 type d'enregistrement = FF
    * 1-2 longueur des données = 0000
    * 3-4 adresse d’exécution

Dans fichier binaire, il peut exister plusieurs enregistrements de type 00.
Il n'existe qu'un seul enregistrement contenant l'adresse d’exécution.
C'est le dernier enregistrement du fichier.

## Structure logique des fichiers Basic

Un fichier Basic est constitue d'un seul enregistrement
- de code FF pour les programmes non protégés
- de code FE pour les programmes protégés

0 type d'enregistrement FF ou FE
1-2 longueur du programme
3-x programme (crypte si type = FE)

## Structure logique des fichiers de données

Un fichier de données est constitué de lignes de longueurs
variables. Les lignes sont séparées par CR-LF (0D0A).

## Structure physique d'un fichier sur cassette

Pour être enregistré sur cassette, un fichier logique est découpé
en blocs. Il y a un bloc d’en-tête, un ou plusieurs blocs contenant
le fichier, un bloc de fin.

En-tête de bloc : pour permettre la synchronisation de la lecture,
les blocs sont tous précédés d'une en-tête composée de 16 octets 01,
suivis de deux octets contenant les caractères <Z (3C5A).

Espace entre les blocs : les blocs sont separes sur la bande par un
intervalle sans signal, pour donner le temps au MO5 de les traiter
après les avoir lus. Selon le type de fichier et le type de bloc,
cet intervalle varie de quelques millisecondes a près d'une seconde.

Les blocs physiques sont stockés sur la cassette grâce à l'enregistrement MFM

### Bloc d'en-tete (type 00)

* 00 type de bloc = 00
* 01 longueur du bloc = &h10
* 02-09 nom du fichier
* 0A-0C extension (sans le point)
* 0D type de fichier 00=Basic 01=Data 02=Binaire
* 0E mode du fichier 00=Binaire FF=Texte
* 0F identique à l'octet precedent (a verifier)
* 10 checksum

La checksum est calculée de telle façon que la somme des octets
02 à 10, modulo 256, soit nulle.

### Blocs contenant le fichier (type 01)

* 00 type de bloc = 01
* 01 longueur du bloc = x (attention, &h00 signifie 256)
* 02
* ... contenu du fichier (y = x - 1)
* (adresse y)
* (adresse x) checksum

Normalement tous ces blocs ont une longueur = 00 (256 octets),
sauf le dernier.

La checksum est calculee de telle façon que la somme des octets
02 à xx, modulo 256, soit nulle.


### Bloc de fin (type FF)

* 00 type de bloc = FF
* 01 longueur du bloc = 02
* 02 checksum = 00

Remarques
=======

1) Toutes ces informations sont données sans aucune garantie. Merci de signaler d'eventuelles erreurs.

2) Elles concernent uniquement les fichiers standard MO5 créés par les commandes Basic. On peut trouver des fichiers MO5 de structure différente. Ils ont étè créés par des routines spéciales en langage machine, le plus souvent pour empècher la copie des programmes commerciaux par les debutants.

3) Les techniques de protection les plus courantes consistent a charger un petit programme en langage machine, le loader. Il est contenu dans un fichier binaire normal. Ce loader utilise des routines speciales pour charger le reste du programme, qui n'est pas au format standard et ne peut pas etre lu par les routines d'origine du MO5.

Voici une liste non exhaustive des procedes employes :

- en-tete de bloc non standard
- checksum calculee avec un algorithme different
- longueur du bloc non specifiee
- longueur du bloc superieure a 256 octets
- vitesse d'enregistrement plus rapide que la normale
- etc.

Ces procedes peuvent etre employes simultanement, et de plus les donnees sont parfois cryptees.

On rencontre aussi des protections a deux niveaux : le loader principal est lui-meme protege, et doit etre charge par un loader non protege de deuxieme niveau.

Daniel 
