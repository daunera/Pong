#ifndef FUGGVENY_H
#define FUGGVENY_H

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

// EZT a függvényt hívja meg az idõzítõ, beilleszt az események közé egy usereventet is
Uint32 idozit(Uint32 ms, void *param);

Uint32 idozit2(Uint32 ms, void *param);

Uint32 idozit3(Uint32 ms, void *param);

// LABDA struktúra rögzítés
typedef struct Labda {
    int r; //sugár
    int x, y; // helyvektor
    int vx, vy; // irányvektor
} Labda;

// ÜTÕ struktúra rögzítés
typedef struct uto {
    int x1, y1; //bal felsõsarok koordinátái
    int x2, y2; //jobb alsósarok koordinátái
    int vy; //irányvektor
    int irany;
}uto;

typedef struct fal { //struktúra az akadálynak a nehezített módhoz
    int x1, y1;
    int x2, y2;
}fal;

typedef struct szamlalo { //struktúra a pontszámok számításához
    int player1;
    int player2;
} szamlalo;

//ranglista eredmény struktúra
typedef struct ranklist {
    int point;
    char* name;
    struct ranklist *next;
} ranklist;

//Ez a függvény mindent alaphelyzetbe állít
void reset(Labda *a, uto *b1, uto *b2);

void reset_pontok(szamlalo *c);

void reset_ido(int *min, int*ms);

void reset_fal(fal *a);

void clear (SDL_Surface *screen);

//Ez a függvény szöveget rajzol ki, megkönnyítve a szövegmegjelenítést
void felirat_rajzolo (SDL_Surface *screen, char arr[], TTF_Font *font, int x_koor, int y_koor, SDL_Color szin);

void felirat_rajzolo_uint16 (SDL_Surface *screen, Uint16 arr[], TTF_Font *font, int x_koor, int y_koor, SDL_Color szin);

//Ez a függvény sima szélű pontokat képes kirajzolni
void pont_rajzolo(SDL_Surface *screen, int x, int y, int r, SDL_Color szin);

//Ez a függvény az ütőt képes kirajzolni
void uto_rajzolo(SDL_Surface *screen, uto *b, SDL_Color szin, int start);

//Értéket ad a cpu nehézségének
int cpu_lvl(int level);

//Ez a cpu ütőjét képes kirajzolni
void cpu_rajzolo(SDL_Surface *screen, uto *b, Labda ball, SDL_Color szin, int level);

void nehezitett_rajzolo (fal *a);

//Ez a labdát képes kirajzolni a visszapattanásokat is figyelembe véve
void labda_rajzolo(SDL_Surface *screen, Labda *ball, uto *Uto1, uto *Uto2, fal *c, SDL_Color szin, int *start, szamlalo *counter, int *menu);

void game_end(szamlalo *c, int cpu, int cpu_szint, int nehez, int *point, int min, int ms);

//Ez a függvény a számlálót írja át stringbe
void szamlalo_stringbe (szamlalo *c, char *s1, char *s2);

//Ez a függvény beolvassa a neveket, forrás: infoc.eet.bme.hu
void input_text(Uint16 *dest, int x0, int y0, int sz, int m, SDL_Color hatter, SDL_Color szin, TTF_Font *font, SDL_Surface *screen, int *menu, int cpu);

//Unicode stringből UTF-8 stringet csinál, forrás: infoc.eet.bme.hu
void unicode_2_utf8(Uint16 const *be, char *ki);

//ranglista beolvasáshoz megszámolja hány betû va egy sorban lévõ névben
int how_many_letter(FILE *fp);

//ranglista beolvasás
ranklist* rank_beolvasas ();

//ranglista kiírás
void rank_kiiras(SDL_Surface *screen,ranklist *first, TTF_Font *font_40);

ranklist* rank_beszuras (ranklist* first, int point, char *name);

void rank_fajlbair (ranklist* first);

//ranglista felszabadítás
void rank_felszabadit (ranklist *r);

//idő számolóo
void timer (int *ms, int *min);

#endif
