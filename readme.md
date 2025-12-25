# README — Projet LIFAPC — Encrage d’une image par flot maximum  
Université Claude Bernard Lyon 1 — L3 Informatique  
Module : Algorithmique, Programmation et Complexité (LIFAPC)  
TP 9 — Encrage d'une image avec deux couleurs

---

## 📌 Objectif du projet

Ce projet implémente un algorithme d’encrage (binarisation) d’une image en niveaux de gris en utilisant un **flot maximum**.  
L’image est modélisée comme un graphe où :

- chaque pixel est un nœud,
- chaque pixel est relié à ses quatre voisins (Nord, Sud, Est, Ouest),
- deux nœuds supplémentaires sont ajoutés :  
  - **S** = source (encre noire)  
  - **P** = puits (encre blanche)

Après calcul du flot maximum, la **coupure minimale** sépare automatiquement les pixels sombres (vers S) des pixels clairs (vers P).  
On obtient ainsi une binarisation plus intelligente qu’un simple seuillage.

---

## 📁 Organisation du projet

├── main.cpp
├── Image.h / Image.cpp
├── Pixel.h / Pixel.cpp
├── MaxFlow.h / MaxFlow.cpp
├── Makefile
└── README.md
---

## 🧩 Fonctionnement général

### 1. Chargement d’une image
Lecture d’un fichier **PGM ASCII (P2)**, stockage dans un tableau 1D.

### 2. Calcul des capacités
- **Entre voisins** :  
  H · exp(- (I(p)-I(q))² / (2σ²))
- **Vers la source / le puits** :  
  -α log(proportion liée à la luminosité)

### 3. Construction du graphe de flot
Création de **L×C + 2 nœuds**, avec liste d’adjacence et arêtes inverses.

### 4. Algorithme de flot maximum
Implémentation de **Edmonds–Karp** :

1. BFS pour trouver un chemin non saturé  
2. Calcul du goulot d’étranglement (delta)  
3. Mise à jour du graphe résiduel  
4. Répétition jusqu’à blocage  

### 5. Coupure minimale
BFS final dans le graphe résiduel depuis S → ensemble des pixels “noirs”.

### 6. Binarisation
- Pixels accessibles depuis S → intensité **0 (noir)**  
- Les autres → **255 (blanc)**  

### 7. Sauvegarde
Écriture dans `resultat.pgm` (format P2 ASCII).

---

## 🔧 Compilation

Sous Linux ou macOS :
make

Nettoyer :
make clean


Exécuter :
./tp9


---

## 🔢 Paramètres importants

- **H** : échelle des capacités entre voisins (souvent 100)  
- **σ (sigma)** : contraste local toléré (3 à 30)  
- **α (alpha)** : pondération clair/sombre (100 à 500)  

---

## 🖥️ Compatibilité Linux / macOS

- Fonctionne avec `g++` standard (GNU ou Apple Clang)  
- Aucun package externe nécessaire  
- Compatible Apple Silicon M1/M2/M3  
- Compatible salles de TP Linux  

---

## 👥 Auteurs

Leo 
L3 Informatique UCBL — 2025 / 2026