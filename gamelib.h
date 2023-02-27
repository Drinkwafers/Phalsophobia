
// Dichiarazione delle funzioni da chiamare in main.c
// Definizione dei tipi necessari alla libreria
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define CYAN    "\x1b[36m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define BROWN   "\x1b[33m"
#define BLUE    "\x1b[34m"
#define PURPLE  "\x1b[35m"
#define RESET   "\x1b[0m"


#define printRED(X) printf("%s%s%s",RED,X,RESET)
#define printBLUE(X) printf("%s%s%s",BLUE,X,RESET)
#define printCYAN(X) printf("%s%s%s",CYAN,X,RESET)
#define printGREEN(X) printf("%s%s%s",GREEN,X,RESET)
#define printBROWN(X) printf("%s%s%s",BROWN,X,RESET)
#define printPURPLE(X) printf("%s%s%s",PURPLE,X,RESET)


void logo ();
void imposta_gioco ();
void gioca ();
void regole ();
void termina_gioco ();

enum Tipo_oggetto_iniziale {EMF_reader,spirit_box,videocamera,calmanti,sale,niente};
enum Tipo_oggetto_zona {adrenalina,cento_dollari,coltello,nessun_oggetto};
enum Tipo_zona {soggiorno,camera,bagno,garage,seminterrato,cucina,caravan};
enum Tipo_prova {nessuna_prova,prova_EMF,prova_spirit_box,prova_videocamera};
enum Stato_giocatore {morto, vivo};

enum difficolta_gioco {dilettante = 1,intermedio = 2,incubo = 3};

//varibile che controlla se abbiamo gia' impostato il gioco o meno, utile a non far partire il gioco senza avere le regole impostate
static bool settato = false;

typedef struct Giocatore
{
  char nome_giocatore [64];
  unsigned char sanita_mentale;
  struct Zona_mappa* posizione;
  unsigned char zaino [5];
  enum Stato_giocatore stato;
  unsigned short colore; //ad ogni giocatore viene assegnato un colore
  short movimento; //tiene conto dei movimenti di ogni giocatore, mantenendo i "debuff" anche nei turni successivi
  short immunita; //questa variabile tiene conto se si e' immuni alle decrescite di sanita mentale
} Giocatore;

typedef struct Zona_mappa
{
  enum Tipo_zona zona;
  enum Tipo_oggetto_zona oggetto_zona;
  enum Tipo_prova prova;
  unsigned short nCivico; //variabile che assegna un numero ad ogni stanza (necessaria per la stampa della posizione del giocatore)
  struct Zona_mappa * prossima_zona;
} Zona_mappa;

/*funzione nel menu di GIOCO di nome spoiler che dice nel rest del percorso, non tornando indietro, gli oggetti che ci sono*/
