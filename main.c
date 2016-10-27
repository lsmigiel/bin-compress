//Lukasz Smigielski, 270823
//Program tworzy drzewo binarne kompresujace N-elementowy ciag bitow.
//Praca w dwoch trybach: interaktywnym (klawiatura-ekran) lub plikowym (z pliku do pliku)
//Data powstania: 20.05.2015, 23:47
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 16
struct wezel {
    char c;
    struct wezel* lewy;
    struct wezel* prawy;
};
typedef struct wezel typ;            //utworzenie nowego typu dla skrocenia kodu

int rodzaj(char *ciag, int a, int b);//zwraca -1 dla niejednorodnego ciagu na odcinku [a, b], 0 dla samych zer, 1 dla jedynek
void utworz(typ** nowy, char* ciag, int a, int b, int strona); //funkcja tworzy drzewo binarne dla danego ciagu. a, b - granice w ktorych badamy ciag, strona - determinuje czy badamy lewy czy prawy podciag
void linijka();     //pomaga wpisac ciag o okreslonej dlugosci (tryb interaktywny)
void wypisz(typ* wezel); //wypisuje zawartosc drzewa na ekran (tryb interaktywny)
void wypiszplik(typ* wezel, FILE **strumien); //zapisuje zawartosc drzewa do pliku (tryb plikowy)
void uwolnij(typ** node); //zwalnia pamiec zaalokowana dla utworzonego drzewa

int main()
{
    FILE *odczyt, *zapis; //deklaracja wskaznikow na strumienie
    char plik[40]; //nazwa pliku wynikowego
    char wejscie[40];
    char napis[N+2]; //tablica przechowujaca ciag bitow
    char tryb;     //tryb pracy programu: interaktywny lub plikowy (decyduje uzytkownik)
    char wybor = '0';  //wybor dotyczacy nadpisania pliku lub nie, 0 oznacza ze uzytkownik nie musial podejmowac takiego wyboru (plik o takiej nazwie nie istnial)
    typ* korzen = NULL;
    int i = 0;

    printf("Wybierz tryb pracy programu (i -> interaktywny, p -> plikowy): ");

    while( scanf("%c", &tryb) != 1  ||  (tryb != 'i' && tryb != 'p') )
    {
        while(getchar() != '\n') continue; //czyszcenie bufora
        printf("Wpisz i lub p: ");
    }
    while(getchar() != '\n') continue; //czyszcenie bufora

    if(tryb == 'i') //jesli tryb interaktywnny
    {
        printf("Wpisz %d-elementowy ciag binarny:\n", N);
        linijka();
        fgets(napis, N+1, stdin);
    }
    else if(tryb == 'p') //jesli tryb plikowy
    {

        printf("Podaj nazwe pliku wejsciowego: ");
        fgets(wejscie, N+1, stdin);
        wejscie[strlen(wejscie)-1] = '\0';

        if( (odczyt = fopen(wejscie, "rt")) == NULL )
        {
            fprintf(stderr, "Blad odczytu\n");
            exit(1);
        }
        while( (napis[i] = getc(odczyt)) != EOF )
            i++;
        napis[i] = '\0';
        fclose(odczyt);
    }

    utworz(&korzen, napis, 0, N-1, 0);  //ostatni argument jest 0 bo za pierwszym wywolaniem sprawdzamy caly ciag
                                        //(czyli nie idziemy ani na lewo ani na prawo)

    if(tryb == 'i')  //jesli tryb interaktywny
    {
        wypisz(korzen);
        putchar('\n');
    }
    else if(tryb == 'p') //jesli tryb plikowy
    {
        printf("Podaj nazwe pliku wynikowego: ");
        fgets(plik, N+1, stdin);
        plik[strlen(plik)-1] = '\0';

        while( (zapis = fopen(plik, "rt")) != NULL && wybor != 't' ) //jesli taki plik juz istnieje
        {

            printf("Taki plik juz istnieje. Czy nadpisac? (t -> tak, n -> nie): ");
            while( scanf("%c", &wybor) != 1  ||  (wybor != 't' && wybor != 'n') )
            {
                while(getchar() != '\n') continue; //czyszczenie bufora
                printf("Wpisz t lub n: ");
            }
            while(getchar() != '\n') continue; //czyszcenie bufora

            if(wybor == 'n')
            {
                printf("Podaj nazwe pliku wynikowego: ");
                fgets(plik, N+1, stdin);
                plik[strlen(plik)-1] = '\0';
            }
        }
        fclose(zapis);



        if( (zapis = fopen(plik, "wt")) == NULL ) //otwieramy strumien
        {
            fprintf(stderr, "Blad zapisu\n");
            exit(3);
        }


        wypiszplik(korzen, &zapis);
        fclose(zapis);
        printf("Wykonano\n");
    }//jesli tryb plikowy

    uwolnij(&korzen);

    return 0;
}

int rodzaj(char *ciag, int a, int b)
{
    int i, zero = 0, jeden = 0; //zero informuje o wystapieniu w ciagu cyfry 0, a jeden o wystapieniu cyfry 1

    if(a == b) //gdy ciag jest jednoelementowy
    {
        if(ciag[a] == '0')
            return 0;
        else if(ciag[a] == '1')
            return 1;
    }

    for(i = a; i <= b; i++) //idziemy po wszystkich indeksach od a do b wlacznie
    {
        if(ciag[i] == '0') //jesli gdzies trafimy na 0
            zero = 1;
        if(ciag[i] == '1') //jesli gdzies trafimy na 1
            jeden = 1;
    }

    if(zero == 1 && jeden == 1)
        return -1;                   //ciag jest niejednorodny
    else if(zero == 1 && jeden == 0)
        return 0;                    //w ciagu sa same 0
    else if(zero == 0 && jeden == 1)
        return 1;                    //w ciagu sa same 1;
    return 0;
}

void linijka()
{
    int i;
    for(i = 0; i < N; i++)
        putchar('^');
    putchar('\n');
}

void utworz(typ** nowy, char* ciag, int a, int b, int strona)
{

    if(strona == -1) //jesli sprawdzamy lewy podciag
    {                //prawa granica (czyli zmienna b) musi zostac zmniejszona (zmiennej a nie ruszamy)

            if(a-b == 1 || a-b == -1) //jesli roznica miedzy a i b wynosi 1, trzeba zastosowac inny wzor niz wzor (1)
                b--;
            else
                b = b - (b-a)/2 - 1; //wzor (1)
    }
    else if(strona == 1) //jesli sprawdzamy prawy podciag
    {                    //lewa granica (czyli zmienna a) musi zostac zwiekszona (zmiennej b nie ruszamy)

            if(a-b == 1 || a-b == -1) //jesli roznica miedzy a i b wynosi 1, trzeba zastosowac inny wzor niz wzor (2)
                a++;
            else
                a = a + (b-a)/2 + 1; //wzor(2)
    }

    *nowy = (typ*)malloc(sizeof(typ));

    if(rodzaj(ciag, a, b) == -1) //jesli jest niejednorodny
    {

        (*nowy)->c = '*';
        utworz(&(*nowy)->lewy, ciag, a, b, -1); //wywolanie dla lewego podciagu
        utworz(&(*nowy)->prawy, ciag, a, b, 1); //wywolanie dla prawego podciagu
    }
    else if(rodzaj(ciag, a, b) == 0) //jesli sa same zera
    {
        (*nowy)->c = '0';
        (*nowy)->lewy = NULL;  //NULL bo wezel jest lisciem
        (*nowy)->prawy = NULL; //jak wyzej
    }
    else if(rodzaj(ciag, a, b) == 1) //jesli sa same jedynki
    {
        (*nowy)->c = '1';
        (*nowy)->lewy = NULL; //wezel jest lisciem
        (*nowy)->prawy = NULL; //j.w.
    }
}

void wypisz(typ* wezel)
{
    printf("%c", wezel->c); //wypisujemy zawartosc pola struktury ( * dla wezla, 0 lub 1 dla liscia)
    if(wezel->c == '*')     //jesli wezel ma rozgalezienia
    {
        wypisz(wezel->lewy); //wypisujemy jego lewy podciag
        wypisz(wezel->prawy); //potem prawy
    }
}

void wypiszplik(typ* wezel, FILE **strumien)
{
    putc(wezel->c, *strumien); //wypisujemy zawartosc pola struktury ( * dla wezla, 0 lub 1 dla liscia)
    if(wezel->c == '*')     //jesli wezel ma rozgalezienia
    {
        wypiszplik(wezel->lewy, strumien); //wypisujemy jego lewy podciag
        wypiszplik(wezel->prawy, strumien); //potem prawy
    }
}

void uwolnij (typ** node)
{
    typ* pom;
    if (*node != NULL) //jesli nie jest lisciem
    {
        uwolnij(&(*node)->lewy); //uwalniamy lewe poddrzewo
        uwolnij(&(*node)->prawy); //uwalniamy prawe poddrzewo
        pom = *node;           //kiedy uwolnimy poddrzewa to uwalniamy argument funkcji
        *node = NULL;
        free(pom);
    }
}



