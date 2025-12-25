# Projet LIFAPC (TP9) — Encrage d’une image par flot maximum (min-cut)
Université Claude Bernard Lyon 1 — L3 Informatique  
Module : Algorithmique, Programmation et Complexité (LIFAPC) — 2025/2026

## 🎯 Objectif
Binariser une image en niveaux de gris (PGM P2) en **noir/blanc** via un modèle graphe + **flot maximum / coupe minimale**.
Contrairement à un simple seuillage, la classification d’un pixel dépend aussi de son **contexte local** (contrastes).

## 🧠 Modélisation (graphe)
- Chaque pixel = un nœud
- Arcs vers les 4 voisins (N, S, E, O)
- Deux nœuds spéciaux :
  - **S** : source (encre noire)
  - **P** : puits (récupération / blanc)
- Arcs supplémentaires : **S → pixel** et **pixel → P**

Implémentation : image stockée en **tableau 1D de taille L×C**, pixel (i, j) à l’indice `i*C + j`.

## 🧮 Capacités des arcs (paramètres H, σ, α)
Entre deux voisins p et q :
Cap(p,q) = round( H * exp( - (I(p)-I(q))² / (2σ²) ) )

Vers la source et le puits (éviter log(0)) :
Cap(S,p) = round( -α * ln( (I(p) + ε) / (255 + 2ε) ) )
Cap(p,P) = round( -α * ln( (255 - I(p) + ε) / (255 + 2ε) ) )

> Reco sujet : H=100, σ ∈ [3..30], α ∈ [100..500] selon l’image.

## 🔁 Algorithme (Edmonds–Karp)
1) BFS dans le graphe résiduel pour trouver un chemin améliorant S → P  
2) Calcul du goulot d’étranglement `delta` sur le chemin :
- Pour arcs pixel↔pixel : `Cap(p,q) - F(p,q) + F(q,p)`
- Pour S→pixel : `Cap(S,q) - F(S,q)`
- Pour pixel→P : `Cap(p,P) - F(p,P)`
3) Augmentation du flot sur le chemin (gestion de l’arc inverse si dépassement)
4) Répéter jusqu’à absence de chemin améliorant ⇒ flot maximal

## ✂️ Coupe minimale & binarisation
BFS final depuis **S** dans le graphe résiduel :
- Pixels atteignables depuis S ⇒ **noir (0)**
- Autres pixels ⇒ **blanc (255)**

Sortie écrite dans `resultat.pgm` (PGM ASCII P2).

## 📁 Organisation
- `Image.*` : lecture/écriture PGM, stockage 1D, accès voisins
- `Pixel.*` : intensité + 6 capacités + 6 flots (voisins + S + P)
- `MaxFlow.*` : BFS, chemin, delta, mise à jour résiduel (Edmonds–Karp)
- `main.cpp` : démo de bout en bout (load → maxflow → cut → save)
- `Makefile` : compilation modulaire

## ⚙️ Compilation / Exécution
```bash
make
./tp9
make clean
