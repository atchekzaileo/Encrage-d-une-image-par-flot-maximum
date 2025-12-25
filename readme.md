# Projet LIFAPC — TP9 : Encrage d’une image par flot maximum (min-cut)
Université Claude Bernard Lyon 1 — L3 Informatique  
Module : Algorithmique, Programmation et Complexité (LIFAPC) — 2025/2026

## Objectif
Le but est de binariser une image en niveaux de gris (format PGM ASCII P2) en noir/blanc via un modèle par graphe et le calcul d’un flot maximum / coupe minimale.  
Contrairement à un seuillage pixel par pixel, la décision tient compte du voisinage (continuité et contraste).

## Modélisation
- Chaque pixel correspond à un sommet du graphe.
- Connexité 4-voisins : N, S, E, O.
- Deux sommets supplémentaires :
  - `S` : source (classe “encre” / objet)
  - `P` : puits (classe “fond”)
- Arcs ajoutés :
  - `S -> pixel`
  - `pixel -> P`

L’image est stockée dans un tableau 1D de taille `L*C`.  
Le pixel `(i, j)` est à l’indice `k = i*C + j`.

## Capacités
### Arcs entre voisins
Pour deux pixels voisins `p` et `q` :
Cap(p,q) = round( H * exp( - (I(p)-I(q))² / (2σ²) ) )

### Arcs vers la source et le puits
On utilise une formulation logarithmique avec un epsilon pour éviter `log(0)` :
Cap(S,p) = round( -α * ln( (I(p) + ε) / (255 + 2ε) ) )
Cap(p,P) = round( -α * ln( (255 - I(p) + ε) / (255 + 2ε) ) )

Valeurs typiques (recommandations sujet) :
- `H = 100`
- `σ` dans `[3..30]`
- `α` dans `[100..500]`
Ces paramètres dépendent de l’image.

## Algorithme
Le flot maximum est calculé avec Edmonds–Karp :
- BFS dans le graphe résiduel pour trouver un chemin augmentant `S -> P`
- calcul du goulot d’étranglement `delta`
- mise à jour du résiduel (diminution des capacités directes et augmentation des inverses)
- répétition jusqu’à absence de chemin augmentant

## Coupe minimale et binarisation
Une fois le flot maximum atteint :
- on lance un BFS depuis `S` dans le graphe résiduel final
- les pixels atteignables depuis `S` sont classés côté source, les autres côté puits

Binarisation produite :
- pixels atteignables depuis `S` : noir (`0`)
- autres pixels : blanc (`255`)

Le résultat est écrit dans `resultat.pgm` (PGM ASCII P2).

## Organisation des fichiers
- `Image.*` : lecture/écriture PGM, stockage 1D, accès aux pixels/voisins
- `Pixel.*` : intensité + tableaux `cap[6]` et `flot[6]`
- `MaxFlow.*` : construction du graphe, BFS, Edmonds–Karp, coupe minimale
- `main.cpp` : exécution complète (lecture -> flot max -> coupe -> écriture)
- `Makefile` : compilation

## Compilation / Exécution
```bash
make
./tp9
make clean
```
