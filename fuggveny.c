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

//Ez a függvény mindent alaphelyzetbe állít
void reset(Labda *a, uto *b1, uto *b2){
    //LABDÁt kezdeti helyre állítja
    a->r = magassag/48;

    a->x = szelesseg/2;
    a->y = magassag/2;

    a->vx = (rand() % 5)+5;
    a->vy = (rand() % 5)+5;

    if (rand()%2 == 0){
        a->vx *= -1;  }
    if (rand()%2 == 0){
        a->vy *= -1;  }

    //ÜTÕ1-t kezdeti helyre állítja
    b1->x1=0;
    b1->y1=magassag/2-magassag/12;
    b1->x2=magassag/24;
    b1->y2=magassag/2+magassag/12;
    b1->vy=11;
    b1->irany = 0;

    //ÜTÕ2-t kezdeti helyre állítja
    b2->x1=szelesseg-magassag/24;
    b2->y1=magassag/2-magassag/12;
    b2->x2=szelesseg;
    b2->y2=magassag/2+magassag/12;
    b2->vy=11;
    b2->irany = 0;


}

void reset_pontok(szamlalo *c){
    c->player1 = 0;
    c->player2 = 0;
}

void reset_ido(int *min, int*ms){
    *min = 0;
    *ms = 0;
}

void reset_fal(fal *a){
    a->x1 = 0;
    a->y1 = 0;
    a->x2 = 0;
    a->y2 = 0;
}

void clear (SDL_Surface *screen){
    boxRGBA(screen, 0, 0, szelesseg, magassag, fekete.r, fekete.g, fekete.b, 255);//lefedi az egész képernyõt feketére, hogy újra lehessen rajzolni
}

//Ez a függvény szöveget rajzol ki, megkönnyítve a szövegmegjelenítést
void felirat_rajzolo (SDL_Surface *screen, char arr[], TTF_Font *font, int x_koor, int y_koor, SDL_Color szin){

    SDL_Rect hova = { 0, 0, 0, 0 };
    SDL_Surface *felirat;

    felirat = TTF_RenderUTF8_Blended(font, arr, szin);
        hova.x = x_koor;
        hova.y = y_koor;
        SDL_BlitSurface(felirat, NULL, screen, &hova);
        SDL_FreeSurface(felirat);
}

void felirat_rajzolo_uint16 (SDL_Surface *screen, Uint16 arr[], TTF_Font *font, int x_koor, int y_koor, SDL_Color szin){

    SDL_Rect hova = { 0, 0, 0, 0 };
    SDL_Surface *felirat;

    felirat = TTF_RenderUNICODE_Blended(font, arr, szin);
        hova.x = x_koor;
        hova.y = y_koor;
        SDL_BlitSurface(felirat, NULL, screen, &hova);
        SDL_FreeSurface(felirat);
}

//Ez a függvény sima szélű pontokat képes kirajzolni
void pont_rajzolo(SDL_Surface *screen, int x, int y, int r, SDL_Color szin){
    aacircleRGBA(screen, x, y, r, szin.r, szin.g, szin.b, 255);
    filledCircleRGBA(screen, x, y, r, szin.r, szin.g, szin.b, 255);
}

//Ez a függvény az ütőt képes kirajzolni
void uto_rajzolo(SDL_Surface *screen, uto *b, SDL_Color szin, int start){
    if (start == 0)
        //álló helyzet
        boxRGBA(screen, b->x1, b->y1, b->x2, b->y2, szin.r, szin.g, szin.b,255);
    else if (start == 1){
        //ha nem mozdult
        if ( (b->irany == 0) || (b->irany == 1 && b->y1 <= magassag/10) || (b->irany == -1 && b->y2 >= magassag-magassag/10))
            boxRGBA(screen, b->x1, b->y1, b->x2, b->y2, szin.r, szin.g, szin.b,255);
        //ha felfelé indul
        if ((b->irany == 1) && (b->y1 > magassag/10)){
            b->y1 -= b->vy;
            b->y2 -= b->vy;
            boxRGBA(screen, b->x1, b->y1, b->x2, b->y2, szin.r, szin.g, szin.b,255);
        }
        //ha lefelé indul
        if ((b->irany == -1) && (b->y2 < magassag - magassag/10)){
            b->y1 += b->vy;
            b->y2 += b->vy;
            boxRGBA(screen, b->x1, b->y1, b->x2, b->y2, szin.r, szin.g, szin.b,255);
        }
    }
}

//Értéket ad a cpu nehézségének
int cpu_lvl(int level){
    if (level == 1)
        return 7;
    else if (level == 2)
        return 8;
    else if (level == 3)
        return 9;
    else
        return 0;
}

//Ez a cpu ütőjét képes kirajzolni
void cpu_rajzolo(SDL_Surface *screen, uto *b, Labda ball, SDL_Color szin, int level){

    if ( rand()%10 < cpu_lvl(level) ){
        //ha a labda alatt van az ÜTÕ2
        if (( b->y1 > ball.y ) && ( b->y1 > magassag/10 )){
            b->y1 -= b->vy;
            b->y2 -= b->vy;
        }

        //ha a labda felett van az ÜTÕ2
        else if ((b->y2 < ball.y) && (b->y2 < magassag - magassag/10)){
            b->y1 += b->vy;
            b->y2 += b->vy;
        }
    }
    boxRGBA(screen, b->x1, b->y1, b->x2, b->y2, szin.r, szin.g, szin.b,255);
}

void nehezitett_rajzolo (fal *a){
    int rand1 = rand()%(szelesseg/2);
    int rand2 = rand()%(magassag/2);
    a->x1 = szelesseg/4 + rand1 - 50;
    a->y1 = magassag/4 + rand2 - 50;
    a->x2 = szelesseg/4 + rand1 + 50;
    a->y2 = magassag/4 + rand2 + 50;
}

//Ez a labdát képes kirajzolni a visszapattanásokat is figyelembe véve
void labda_rajzolo(SDL_Surface *screen, Labda *ball, uto *Uto1, uto *Uto2, fal *c, SDL_Color szin, int *start, szamlalo *counter, int *menu){
    switch(*start){
        case 0:
            pont_rajzolo(screen, ball->x, ball->y, ball->r, szin);
            break;

        case 1:
            ball->x += ball->vx;
            ball->y += ball->vy;

            //visszapattanások
            // y irányú
            if ( ball->y < (magassag/10+ball->r) || ball->y > (magassag-magassag/10-ball->r) ){
                ball->vy *= -1;
            }
            else if( (ball->x > c->x1) && (ball->x < c->x2) && (ball->y > c->y1-ball->r) && (ball->y < c->y1) ){
                ball->vy *= -1;
            }
            else if( (ball->x > c->x1) && (ball->x < c->x2) && (ball->y > c->y2) && (ball->y < c->y2+ball->r) ){
                ball->vy *= -1;
            }

            // x irányú
            if (((ball->x < (magassag/24+ball->r)) && (ball->y >= Uto1->y1) && (ball->y <= Uto1->y2)) || ((ball->x > (szelesseg-magassag/24-ball->r)) && (ball->y >= Uto2->y1) && (ball->y <= Uto2->y2))){
                ball->vx *= -1;
            }
            else if( (ball->y > c->y1) && (ball->y < c->y2) && (ball->x > c->x1-ball->r) && (ball->x < c->x1) ){
                ball->vx *= -1;
            }
            else if( (ball->y > c->y1) && (ball->y < c->y2) && (ball->x > c->x2) && (ball->x < c->x2+ball->r) ){
                ball->vx *= -1;
            }

            //ha az (1.) játékos nem tudta visszaütni a labdát
            if (ball->x < ball->r){
                *start = 0;
                reset(ball, Uto1, Uto2);
                reset_fal(c);
                counter->player2 += 1;
                if (counter->player2 == 10)
                    *menu = 4;
            }
            //ha a 2. játékos (szamitogep) nem tudta vissza ütni a labdát
            if (ball->x > szelesseg-ball->r){
                *start = 0;
                reset(ball, Uto1, Uto2);
                reset_fal(c);
                counter->player1 += 1;
                if (counter->player1 == 10)
                    *menu = 4;
            }
            //ÚJRA kirajzolja a labdát
            pont_rajzolo(screen, ball->x, ball->y, ball->r, szin);
            break;
    }
}

void game_end(szamlalo *c, int cpu, int cpu_szint, int nehez, int *point, int min, int ms){
    if (c->player1 == 10 || c->player2 == 10){

        //ha az első játékos érte el a 10 pontot
        if (c->player1 == 10){
            int ellen;
            if (cpu == 0)
                ellen =  25 * 2;
            else
                ellen = 25 * cpu_szint;
            *point = (500 - (60*min+ms)) + (nehez * 50) + ((10-(c->player2)) * 10) + ellen;
        }
        //ha a 2. játékos érte el a 10 pontot
        else if (c->player2 == 10 && cpu == 0){
            *point = (500 - (60*min+ms)) + (nehez * 50) + ((10-(c->player1)) * 10) + 2*25;
        }
        //ha a számítógép érte el a tíz pontot
        else if (c->player2 == 10 && cpu == 1){
            *point = 0;
        }
    }
}

//Ez a függvény a számlálót írja át stringbe
void szamlalo_stringbe (szamlalo *c, char *s1, char *s2){
    sprintf(s1, "%d", c->player1);
    sprintf(s2, "%d", c->player2);
}

//Ez a függvény beolvassa a neveket, forrás: infoc.eet.bme.hu
void input_text(Uint16 *dest, int x0, int y0, int sz, int m, SDL_Color hatter, SDL_Color szin, TTF_Font *font, SDL_Surface *screen, int *menu, int cpu) {


    //középre helyezzük a szüvegdobozt
    int x = x0 - sz/2, y = y0 - m/2;

    SDL_Rect forras = { 0, 0, sz, m}, cel = { x, y, sz, m };
    SDL_Surface *felirat;
    SDL_Event event;
    int hossz, enter;

    hossz = 0;
    dest[hossz] = 0x0000;   /* lezaro 0 */
    SDL_EnableUNICODE(1);
    enter = 0;
    while (!enter) {

        clear (screen);
        if (*menu == 1 && cpu == 1)
            felirat_rajzolo(screen, "ADJA MEG A JÁTÉKOS NEVÉT:", font, 25, magassag/2-100, zold);
        else if (*menu == 1 && cpu == 0)
            felirat_rajzolo(screen, "ADJA MEG AZ ELSÖ JÁTÉKOS NEVÉT:", font, 25,magassag/2-100, zold);
        else if (*menu == 2)
            felirat_rajzolo(screen, "ADJA MEG A MÁSODIK JÁTÉKOS NEVÉT:", font, 25, magassag/2-100, sarga);

        felirat_rajzolo(screen, "TOVÁBB LÉPNI AZ 'ENTER' LENYOMÁSÁVAL LEHET", font, 200, magassag-60, feher);

        /* szoveg kirajzolasa */
        boxRGBA(screen, x, y, x+sz-1, y+m-1, hatter.r, hatter.g, hatter.b, 255);
        felirat = TTF_RenderUNICODE_Blended(font, dest, szin);
        SDL_BlitSurface(felirat, &forras, screen, &cel);
        SDL_FreeSurface(felirat);
        rectangleRGBA(screen, x, y, x+sz-1, y+m-1, 255, 255, 255, 255);
        /* updaterect: mint az sdl_flip, de csak a kepernyo egy darabjat */
        SDL_UpdateRect(screen, 0, 0, szelesseg, magassag);

        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.unicode) {
                    case 0x0000:
                        /* nincs neki megfelelo karakter (pl. shift gomb) */
                        break;
                    case '\r':
                    case '\n':
                        /* enter: bevitel vege */
                        enter = 1;
                        if (*menu == 1 && cpu == 1)
                            *menu = 3;
                        else if (*menu == 1 && cpu == 0)
                            *menu = 2;
                        else if (*menu == 2)
                            *menu = 3;
                        break;
                    case '\b':
                        /* backspace: torles visszafele, ha van karakter */
                        if (hossz>0)
                            dest[--hossz] = 0x0000;
                        break;
                    default:
                        /* karakter: tombbe vele, plusz lezaro nulla */
                        dest[hossz++] = event.key.keysym.unicode;
                        dest[hossz] = 0x0000;
                        break;
                }
                break;
        }
    }
}

//Unicode stringből UTF-8 stringet csinál, forrás: infoc.eet.bme.hu
void unicode_2_utf8(Uint16 const *be, char *ki) {
    int pb, pk;

    pk=0;
    for (pb=0; be[pb]!=0x0000; ++pb) {
        /* 00000000 0xxxxxxx    0x0000-0x007F   0xxxxxxx */
        if (be[pb]<=0x007F)
            ki[pk++] = be[pb];
        else
        /* 00000yyy yyxxxxxx    0x0080-0x07FF   110yyyyy 10xxxxxx */
        if (be[pb]<=0x07FF) {
            ki[pk++] = 0xC0 | be[pb]>>6;     /* 0xC0 = 11000000 */
            ki[pk++] = 0x80 | (be[pb]&0x3F); /* 0x80 = 10000000, 0x3F = 00111111 */
        }
        /* zzzzyyyy yyxxxxxx    0x0800-0xFFFF   1110zzzz 10yyyyyy 10xxxxxx */
        else {
            ki[pk++] = 0xE0 | be[pb]>>12;    /* 0xE0 = 11100000 */
            ki[pk++] = 0x80 | ((be[pb]>>6)&0x3F);
            ki[pk++] = 0x80 | (be[pb]&0x3F);
        }
    }

    ki[pk]=0;
}

//ranglista beolvasáshoz megszámolja hány betû van egy sorban lévõ névben
int how_many_letter(FILE *fp){
    int letter = 0;
    long pos = ftell(fp);

    fscanf(fp, " ");
    fscanf(fp, "%*s%n", &letter);
    fseek(fp, pos, SEEK_SET);

    return letter;
}

//ranglista beolvasás
ranklist* rank_beolvasas (){

    FILE *fp = fopen("result.txt","r");
    int name_length;
    ranklist  *curr, *first = NULL, *prev;
    int point;
    int siker_beolv = fscanf(fp, "%d", &point);

    if (siker_beolv == 0){
        fprintf(stderr, "Nem sikerult megnyitni a fájlt! %s\n", TTF_GetError());
        exit(3);
    }

    while(siker_beolv == 1){
        curr = malloc(sizeof *curr);
        curr->next = NULL;
        curr->point = point;
        name_length = how_many_letter(fp);
        curr->name = malloc(name_length + 1);
        fscanf(fp, "%s", curr->name);

        if(first == NULL) {
            first = prev = curr;
            }
        else {
            prev->next = curr;
            prev = curr;
            }
        siker_beolv = fscanf(fp, "%d", &point);
    }
    fclose(fp);

    return first;
}

//ranglista kiírás
void rank_kiiras(SDL_Surface *screen, ranklist *first, TTF_Font *font_40){
    int i= 1;

    while (i<11){

        char *s1, *s2;
        s1 =  malloc(5*sizeof(char));
        s2 =  malloc(15*sizeof(char));

        sprintf(s1, "%d.", i);
        sprintf(s2, "%d pont  - ", first->point);

        felirat_rajzolo(screen, s1, font_40, 120, (100+i*40), feher);
        felirat_rajzolo(screen, s2, font_40, 180, (100+i*40), feher);
        felirat_rajzolo(screen, first->name, font_40, 380, (100+i*40), feher);

        free(s1);
        free(s2);

        //SDL_Flip(screen);

        first = first->next;
        i++;
    }
}

ranklist* rank_beszuras (ranklist* first, int point, char *name) {

    //új rankbejegyzés
    ranklist *uj;
    uj = malloc(sizeof(ranklist));
    if(uj == NULL) {
        printf("Nem sikerült memoriát foglalni!");
        return uj;
    }
    uj->point = point;
    uj->name  = (char*) malloc(100*sizeof(char));
    uj->name  = name;
    uj->next = NULL;

    //ha a a legnagyobb lenne az új tag
    if (point > first->point) {
        uj->next = first;
        return uj;
    }

    ranklist *temp , *prev;
    temp = first->next;
    prev = first;
    while(temp != NULL && temp->point > point) {
        prev = temp;
        temp = temp->next;
    }
    uj->next = temp;
    prev->next = uj;

    return first;
}

void rank_fajlbair (ranklist* first){
    FILE *fp = fopen( "result.txt", "w+");

    while (first != NULL){
        fprintf(fp,"%d %s\n", first->point, first->name);

        first = first->next;
    }
    fclose(fp);
}

//ranglista felszabadítás
void rank_felszabadit (ranklist *r){
    while( r != NULL ){
    ranklist *tmp = r->next;
    free(r->name);
    free(r);
    r = tmp;
    }
}

//idő számoló
void timer (int *ms, int *min){
    if (*ms == 60){
        *min += 1;
        *ms = 0;
    }
    else
    *ms += 1;
}
