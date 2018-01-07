#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "fuggveny.h"

//JÁTÉKBAN HASZNÁLT RGB SZÍNEK
static SDL_Color feher = { 255, 255, 255};
static SDL_Color fekete = { 0, 0, 0 };
static SDL_Color voros = { 210, 100,  90};
static SDL_Color zold = { 5, 215,  100};
static SDL_Color kek = { 5, 165,  215};
static SDL_Color sarga = { 235, 200,  10};

//FELBONTÁS - 4:3-as arány
static int szelesseg = 1024;
static int magassag = 768;



// EZT a függvényt hívja meg az idõzítõ, beilleszt az események közé egy usereventet is
Uint32 idozit(Uint32 ms, void *param){
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms; //újabb várakozas
}

Uint32 idozit2(Uint32 ms, void *param){
    SDL_Event ev;
    ev.type = SDL_USEREVENT+1;
    SDL_PushEvent(&ev);
    return ms; //újabb várakozas
}

Uint32 idozit3(Uint32 ms, void *param){
    SDL_Event ev;
    ev.type = SDL_USEREVENT+2;
    SDL_PushEvent(&ev);
    return ms; //újabb várakozas
}

int main(int argc, char *argv[]) {

    //deklarációk és értékeadások
    SDL_Surface *screen;
    SDL_Event event;
    SDL_TimerID id, id2, id3; //idõzítõ
    int quit = 0; //kilépsi feltétel
    int i;
    Uint16 player1_name[100];
    Uint16 player2_name[100];

    int start = 0; //labda mehete

    //menüváltozók
    int menu = 0;
    int cpu = 0; //van-e számitógépes ellenfél
    int nehez = 0; //van-e nehezített mód
    int cpu_szint = 1; //milyen nehézségű a számítógépes ellenfél
    int jatekos_p, nehez_p, cpu_szint_p;

    int ms=0, min=0;

    int point;
    char name[100];

    uto Uto1, Uto2;
    Labda ball;
    fal akadaly;

    szamlalo counter;
    reset_pontok(&counter);

    reset(&ball, &Uto1, &Uto2);

    ranklist *first;

    srand(time(NULL)); //random generátor függvény

    // SDL inicializálása és ablak megnyitása
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(szelesseg, magassag, 0, SDL_FULLSCREEN);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("PONG", "PONG");

    //font inicializáció
    TTF_Init();
    TTF_Font *font_100, *font_70, *font_40; //három méretű font pointer meghívása
    font_100 = TTF_OpenFont("BAUHS93.TTF",100);
    font_70 = TTF_OpenFont("BAUHS93.TTF",70);
    font_40 = TTF_OpenFont("BAUHS93.TTF",40);
    if (!font_100 || !font_70 || !font_40) {
        fprintf(stderr, "Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(2);
    }

    id = SDL_AddTimer(20, idozit, NULL); //idõzítõ 20 ms meghívási idõvel
    id2 = SDL_AddTimer(3000, idozit2, NULL);
    id3 = SDL_AddTimer(1000, idozit3, NULL);

    while (!quit){
        SDL_WaitEvent(&event);
            switch (event.type){

                case SDL_USEREVENT:

                    clear(screen);
                    if (menu == 0){
                        reset(&ball, &Uto1, &Uto2);
                        reset_pontok(&counter);

                        //kilépés jelző
                        boxRGBA(screen, szelesseg-10, 10, szelesseg-230, 58, voros.r,voros.g, voros.b, 255); //háttér
                        felirat_rajzolo(screen, "(Q) - KILÉPÉS", font_40, szelesseg-228, 13, feher);

                        //eredmény jelző
                        boxRGBA(screen, 10, 10, 280, 58, kek.r, kek.g, kek.b, 255); //háttér
                        felirat_rajzolo(screen, "(E) - EREDMÉNY", font_40, 12, 13, feher);

                        //JELÖLŐ PONTOK
                        //jatékosok száma
                        if (cpu == 0)
                            jatekos_p = szelesseg/4+530;
                        else if (cpu == 1)
                            jatekos_p = szelesseg/4+480;
                        pont_rajzolo(screen, jatekos_p, 400, 8, zold); //kirajzolja a pontot

                        //nehezített mod
                        if (nehez == 0)
                            nehez_p = szelesseg/4+480;
                        else if (nehez == 1)
                            nehez_p = szelesseg/4+580;
                        pont_rajzolo(screen, nehez_p, 480, 8, zold); //kirajzolja a pontot

                        //SZÖVEGFELIRATOK

                        felirat_rajzolo(screen, "PONG", font_100, szelesseg/3+70, 100, feher); //címfelirat
                        felirat_rajzolo(screen, "(ENTER) - INDÍTÁS", font_70, szelesseg/4, 220, feher); //indítás

                        felirat_rajzolo(screen, "(1) - JÁTÉKOSOK SZÁMA:", font_40, szelesseg/4, 400, feher);
                        felirat_rajzolo(screen, "1", font_40, szelesseg/4+480, 400, feher);
                        felirat_rajzolo(screen, "2", font_40, szelesseg/4+530, 400, feher);

                        felirat_rajzolo(screen, "(2) - NEHEZÍTETT MÓD:", font_40, szelesseg/4, 480, feher);
                        felirat_rajzolo(screen, "NEM", font_40, szelesseg/4+480, 480, feher);
                        felirat_rajzolo(screen, "IGEN", font_40, szelesseg/4+580, 480, feher);

                        if (cpu == 1){
                            //jelölő pont
                            if (cpu_szint == 0)
                                cpu_szint_p = szelesseg/4+480;
                            else if (cpu_szint == 1)
                                cpu_szint_p = szelesseg/4+530;
                            else if (cpu_szint == 2)
                                cpu_szint_p = szelesseg/4+580;

                            pont_rajzolo(screen, cpu_szint_p, 560, 8, zold );

                            //szöveg

                            felirat_rajzolo(screen, "(3) - CPU NEHÉZSÉGE:", font_40, szelesseg/4, 560, feher);
                            felirat_rajzolo(screen, "1", font_40, szelesseg/4+480, 560, feher);
                            felirat_rajzolo(screen, "2", font_40, szelesseg/4+530, 560, feher);
                            felirat_rajzolo(screen, "3", font_40, szelesseg/4+580, 560, feher);
                        }
                    }
                    if (menu == 1){
                            input_text(player1_name, 1024/2, 768/2, 400, 40, fekete, feher, font_40, screen, &menu, cpu);
                    }
                    if (menu == 2){
                        input_text(player2_name, 1024/2, 768/2, 400, 40, fekete, feher, font_40, screen, &menu, cpu);
                    }
                    if (menu == 3){
                        for(i=magassag/10;i <= magassag-magassag/10; i += magassag/20)
                            boxRGBA(screen, szelesseg/2-2, i, szelesseg/2+2, i+20, feher.r, feher.g, feher.b, 255);

                        uto_rajzolo(screen, &Uto1, zold, start);

                        if (cpu == 0)
                            uto_rajzolo(screen, &Uto2, sarga, start);
                        else if (cpu == 1)
                            cpu_rajzolo(screen, &Uto2, ball, voros, cpu_szint);
                        if (nehez == 1)
                            boxRGBA(screen, akadaly.x1, akadaly.y1, akadaly.x2, akadaly.y2, feher.r, feher.g, feher.b, 255);

                        labda_rajzolo(screen, &ball, &Uto1, &Uto2, &akadaly, feher, &start, &counter, &menu);

                        //aktuális állás kirajzolása
                        char *player1_pts;
                        player1_pts = (char*) malloc(5*sizeof(char));
                        char *player2_pts;
                        player2_pts = (char*) malloc(5*sizeof(char));

                        szamlalo_stringbe(&counter, player1_pts, player2_pts);

                        felirat_rajzolo(screen, player1_pts, font_40, 10, 10, zold);
                        felirat_rajzolo_uint16(screen, player1_name, font_40, 60, 10, zold);
                        if (cpu == 0){
                            felirat_rajzolo(screen, player2_pts, font_40, szelesseg/2+10, magassag-60, sarga);
                            felirat_rajzolo_uint16(screen, player2_name, font_40, szelesseg/2+60, magassag-60, sarga);
                        }
                        else if (cpu == 1){
                            felirat_rajzolo(screen, player2_pts, font_40, szelesseg/2+10, magassag-60, voros);
                            felirat_rajzolo(screen, "CPU", font_40, szelesseg/2+60, magassag-60, voros);
                        }
                        if (start == 0){
                            felirat_rajzolo(screen, "SPACE - START", font_40, 10, magassag-60,feher);
                        }

                        if (start == 1){
                            felirat_rajzolo(screen, "P - PAUSE  R - RESET", font_40, 10, magassag-60,feher);
                        }

                        free(player1_pts);
                        free(player2_pts);

                        //Pálya állandó részeinek felrajzolása
                        boxRGBA(screen, 0, magassag/12, szelesseg, magassag/11, feher.r, feher.g, feher.b, 255); // felsõ sáv
                        boxRGBA(screen, 0, magassag-magassag/11, szelesseg, magassag-magassag/12, feher.r, feher.g, feher.b, 255); // alsó sáv

                        //kilépés jelző
                        boxRGBA(screen, szelesseg-10, 10, szelesseg-230, 58, 210,100, 90, 255); //háttér
                        felirat_rajzolo(screen, "(Q) - MENÜ", font_40, szelesseg-228, 13, feher);

                        game_end(&counter, cpu, cpu_szint, nehez, &point, min, ms);
                    }
                    if (menu == 4){

                        //kilépés jelző
                        boxRGBA(screen, szelesseg-10, 10, szelesseg-230, 58, 210,100, 90, 255); //háttér
                        felirat_rajzolo(screen, "(Q) - MENÜ", font_40, szelesseg-228, 13, feher);

                        game_end(&counter, cpu, cpu_szint, nehez ,&point, min, ms);

                        char *point_str;
                        point_str = (char*) malloc (5*sizeof(char));

                        sprintf(point_str, "%d", point);

                        if (cpu == 1 && counter.player2 == 10)
                            felirat_rajzolo(screen, "SAJNOS MOST VESZTETTÉL!", font_70, szelesseg/6,magassag/2-35, voros);
                        else if (cpu == 0 && counter.player2 == 10){
                            felirat_rajzolo(screen, "GRATULÁLUNK", font_70 , szelesseg/6, magassag/2-35, feher);
                            felirat_rajzolo(screen, "NYERTÉL!", font_70, szelesseg/3, magassag/2+50, feher);
                            //pontszám kiírása
                            felirat_rajzolo(screen, "PONTSZÁMOD:", font_40, 12, 13, feher);
                            felirat_rajzolo(screen, point_str, font_70, 280, 13, kek);
                            //átalakítja utf8 kódolsáúvá
                            unicode_2_utf8(player2_name, name);
                            felirat_rajzolo(screen, name, font_70, szelesseg/2+100, magassag/2-35, sarga);
                        }
                        else if (counter.player1 == 10){
                            felirat_rajzolo(screen, "GRATULÁLUNK", font_70 , szelesseg/6, magassag/2-35, feher);
                            felirat_rajzolo(screen, "NYERTÉL!", font_70, szelesseg/3, magassag/2+50, feher);
                            //pontszám kiírása
                            felirat_rajzolo(screen, "PONTSZÁMOD:", font_40, 12, 13, feher);
                            felirat_rajzolo(screen, point_str, font_70, 280, 13, kek);
                            //átalakítja utf8 kódolsáúvá
                            unicode_2_utf8(player1_name, name);
                            felirat_rajzolo(screen, name, font_70, szelesseg/2+100, magassag/2-35, zold);
                        }
                    }
                    if (menu == 5){
                        felirat_rajzolo(screen, "EREDMÉNYEK", font_100, 12, 12, feher);
                        //kilépés jelző
                        boxRGBA(screen, szelesseg-10, 10, szelesseg-230, 58, 210,100, 90, 255); //háttér
                        felirat_rajzolo(screen, "(Q) - MENÜ", font_40, szelesseg-228, 13, feher);

                        felirat_rajzolo(screen, "készítette: DAUNER ÁGOSTON", font_40, 500, magassag-60, kek);

                        rank_kiiras(screen, first, font_40);
                    }
                    break;

                case SDL_USEREVENT+1:
                    if (nehez == 1 && start == 1){
                        nehezitett_rajzolo(&akadaly);
                    }
                    break;
                case SDL_USEREVENT+2: //időszámláló
                    if (menu == 3 && start == 1){
                        timer (&ms, &min);
                    }
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym){
                            //Q illentyû lenyomására kilép
                            case SDLK_q:
                                if (menu == 0)
                                    quit = 1;
                                else if (menu == 3){
                                    menu = 0;
                                    start = 0;
                                    reset(&ball, &Uto1, &Uto2);
                                    reset_pontok(&counter);
                                    reset_ido(&min, &ms);
                                    reset_fal(&akadaly);
                                }
                                else if (menu == 4){
                                    menu = 0;
                                    reset_ido(&min, &ms);
                                    first = rank_beolvasas();
                                    first = rank_beszuras(first, point, name);
                                    rank_fajlbair(first);
                                    rank_felszabadit(first);
                                }
                                else if (menu == 5){
                                    menu = 0;
                                    rank_felszabadit(first);
                                }
                                break;

                            case SDLK_RETURN:
                                if (menu == 0)
                                    menu = 1;
                                break;

                            if (menu == 3){
                                case SDLK_SPACE:
                                    start = 1;
                                    break;

                                case SDLK_p:
                                    start = 0;
                                    break;
                                //TESZTHEZ SZÜKSÉGES BILLENTYŰK
                                case SDLK_c: //játékmód váltás 1 személy és 2 személyes között
                                    if (menu == 3){
                                        if (cpu == 0){
                                            cpu = 1;
                                        }
                                        else if (cpu == 1){
                                            cpu = 0;
                                        }
                                    }
                                    break;
                                case SDLK_v: //cpu könnyű szinten
                                    if (menu == 3){
                                        cpu_szint=0;
                                    }
                                    break;
                                case SDLK_b: //cpu közép szinten
                                    if (menu == 3){
                                        cpu_szint=1;
                                    }
                                    break;
                                case SDLK_n: //cpu nehez szinten
                                    if (menu == 3){
                                        cpu_szint=2;
                                    }
                                    break;
                                case SDLK_m: //nehezített mód teszt
                                    if (menu == 3){
                                        if (nehez == 0){
                                            nehez = 1;
                                            reset_fal(&akadaly);
                                        }
                                        else if (nehez == 1){
                                            nehez = 0;
                                            reset_fal(&akadaly);
                                        }
                                    }
                                    break;
                                case SDLK_h:
                                    if (menu == 3){
                                        counter.player1 = 9;
                                        counter.player2 = 9;
                                    }
                                    break;
                                //állás törlése, resetel mindent
                                case SDLK_r:
                                    if (menu == 3){
                                        reset(&ball, &Uto1, &Uto2);
                                        reset_pontok(&counter);
                                        reset_ido(&min,&ms);
                                        reset_fal(&akadaly);
                                        start = 0;
                                    }
                                    break;
                                //ÜTÕ1 irányítása fel és le irányba
                                case SDLK_w:
                                    Uto1.irany = 1;
                                    break;
                                case SDLK_s:
                                    Uto1.irany = -1;
                                    break;

                                //ÜTÕ2 irányítása fel és le irányba
                                case SDLK_UP:
                                    if (cpu == 0)
                                        Uto2.irany = 1;
                                    break;
                                case SDLK_DOWN:
                                    if (cpu == 0)
                                        Uto2.irany = -1;
                                    break;
                            }
                                case SDLK_e:
                                    if (menu == 0){
                                        first = rank_beolvasas();
                                        menu = 5;

                                    }
                                    break;
                                case SDLK_1: //hány ellenfél
                                    if (menu == 0){
                                        if (cpu == 1)
                                            cpu = 0;
                                        else
                                        cpu++;
                                    }
                                    break;
                                case SDLK_2: //nehézségi szint
                                    if (menu == 0){
                                        if (nehez == 1)
                                            nehez = 0;
                                        else{
                                        nehez++;
                                        reset_fal(&akadaly);
                                        }
                                    }
                                    break;
                                case SDLK_3: //cpu nehézsége
                                    if (menu == 0){
                                        if (cpu == 1){
                                            if (cpu_szint == 2)
                                                cpu_szint = 0;
                                            else
                                                cpu_szint++;
                                        }
                                    }
                                    break;
                            //egyéb billentyûkre ne reagáljon
                            default:
                                break;
                        }
                        break;

                case SDL_KEYUP:
                        switch(event.key.keysym.sym){
                            if (menu == 3){
                                // ÜTÕ1 megáll
                                case SDLK_w:
                                    Uto1.irany = 0;
                                    break;
                                case SDLK_s:
                                    Uto1.irany = 0;
                                    break;

                                // ÜTÕ2 megáll
                                case SDLK_UP:
                                    if (cpu == 0)
                                        Uto2.irany = 0;
                                    break;
                                case SDLK_DOWN:
                                    if (cpu == 0)
                                        Uto2.irany = 0;
                                    break;
                            }

                            //egyéb billentyûkre ne reagáljon
                            default:
                                break;
                        }
                        break;
            }
        SDL_Flip(screen); //kirajzolás

    }
    //idõztõ törlése
    SDL_RemoveTimer(id);

    //fontok felszabadítása
    TTF_CloseFont(font_100);
    TTF_CloseFont(font_70);
    TTF_CloseFont(font_40);

    // ablak bezarasa
    SDL_Quit();

    return 0;
}
