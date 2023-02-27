#include "gamelib.h"

/*DICHIARAZIONE VARIABILI*/

//puntatore alla lista dei giocatori
static struct Giocatore* GiocatorePtr = NULL;

//variabile che contiene il numero di giocatori partecipanti
static unsigned short nGiocatori;

//variabile che contiene la difficolta del gioco
static enum difficolta_gioco difficolta;

//il carceriere tiene in trappola chi cerca di fuggire senza impostare il gioco
static int carceriere [3] = {0,0,0};

//variabile che contiene il numero di stanze
static int nStanze = 0;

//puntatore in cima alla lista Zona_mappa
static struct Zona_mappa* pFirst = NULL;

//puntatore in fondo alla lista Zona_mappa
static struct Zona_mappa* pLast = NULL;

//variabile che controlla che gli oggetti di raccolta prove siano presi una volta sola
static int InventarioCaravan [3] = {1,1,1};

//quando si radunano tutti i pezzi di exodia il proibito la partita termina (quando si portano le 3 prove al caravan)
static int exodia = 0;

//variabile contenente il nome del fantasma
static char NomeFantasma [27];

//tiene conto del numero di morti durante la partita
int nMorti = 0;

/*DICHIARAZIONE DELLE FUNZIONI*/

//Funzioni utili per il programma
static void spazzino (void);
static int casuale (int);
static void SpostaCelle (int*, int);
static void scambio (int*, int*);
static void piantone (void);

//Funzioni che raccolgono un pezzo di programma

//FUNZIONI NELLA FUNZIONE "imposta_gioco"
static void ControlloImpostazioni (void);
static void ContaGiocatori (void);
static void ImpostaGiocatori (void);
static void ImpostaDifficolta (void);
static void ImpostaOggettoIniziale (void);
static void StampaImpostazioni (void);
static void ImpostaMappa (void);
static void InserisciZona (void);
static void CancellaZona (void);
static void StampaDatiMappa (void);
static void StampaCasaMenu (void);
static void StampaCasaGioca (void);
static void LevelUpp (void);
static const char* StampaOggettoIniziale (struct Giocatore*, int);
static const char* StampaProveRaccolte (struct Giocatore*);
static const char* StampaProveDroppate (struct Zona_mappa*);
static const char* StampaPosizione (struct Zona_mappa*);
static const char* StampaNomeStanza (struct Zona_mappa*);
static const char* StampaOggettoStanza (struct Zona_mappa*);
static const char* StampaOggettoStanzaMenu (struct Zona_mappa*);
static const char* StampaOggettoGiocatore (struct Giocatore*, int);

//FUNZIONE NELLA FUNZIONE "gioca"
static char* ImpostaFantasma (void);
static void Lore (void);
static void torna_caravan (struct Giocatore*);
static void stampa_giocatore (struct Giocatore*);
static void stampa_stanza (struct Zona_mappa*);
static void avanza (struct Giocatore*);
static void raccogli_prova (struct Giocatore*);
static void raccogli_oggetto (struct Giocatore*);
static void usa_oggetto (struct Giocatore*, int*);
static void passa (void);
static void spoiler (struct Zona_mappa*);
static void ControllaBattito (struct Giocatore*);

//funzioni che permette di usare gli oggetti
static void UsaDollari (struct Giocatore*, int);
static void UsaColtello (struct Giocatore*, int);

static void ColoreGiocatore (int);
static void vittoria (void);
static void ResettaVariabiliGioco (void);

static void Legenda (void);
static const char* FrasiFatte (void);

//DEFINIZIONE DELLE FUNZIONI

//Lo spazzino si dedica alla pulizia del buffer con molta passione
static void spazzino (void)
{
  while (getchar () != '\n' && getchar () != EOF);
}

//genera un numero casuale da 0 a un numero dato in input
static int casuale (int max)
{
  int r = (rand()%max);
  return r;
}

//scambia le celle di un array in modo casuale
static void SpostaCelle (int* arr, int nElementi)
{
  for (int i = (nElementi - 1); i > 0; i--)
  {
    scambio(&arr[casuale(i)], &arr[i]);
  }
}

//usa dei puntatori per scambiare due variabili
static void scambio (int* a, int* b)
{
  int n = *a;
  *a = *b;
  *b = n;
}

//il piantone (per gli anglofoni "gatekeeper") non ti fa passare se non fai come dice lui
static void piantone (void)
{
  do
  {
    printBLUE("Premi invio per continuare\n");
  } while( getchar() != '\n' );
}

//Se il gioco e' gia' stato impostato e si torna su imposta gioco questa funzione si assicura che non sia un errore dell'utente
static void ControlloImpostazioni (void)
{
  printRED("ATTENZIONE: IL GIOCO E' GIA STATO IMPOSTATO, PROSEGUENDO PERDERAI LE TUE PRECEDENTI IMPOSTAZIONI.\n");
  printBLUE("DIGITA 0 PER PROSEGUIRE ALTRIMENTI DIGITA UN QUALSIASI ALTRO NUMERO PER TORNARE INDIETRO\n");
  scanf("%d", (int*)&settato);
  system("clear");
  if (settato)
  return;
  ResettaVariabiliGioco ();
  system("clear");
}

//menu per impostare il numero di giocatori
static void ContaGiocatori (void)
{
  logo ();
  do
  {
    printf("Digita il numero di giocatori: \n1)\t2)\n3)\t4)\n");
    printf("Scelta: ");
    scanf("%hu", &nGiocatori);
    spazzino();
    if (nGiocatori > 4 || nGiocatori < 1)
    {
      system("clear");
      logo ();
      printRED("ERRORE: Devi inserire un numero tra 1 e 4\n");
    }
  } while (nGiocatori > 4 || nGiocatori < 1);
  system("clear");
}

//menu per impostare il nome dei giocatori e gli altri valori della struct Giocatore, fatta eccezione per la posizione
static void ImpostaGiocatori (void)
{
  struct Giocatore* tmp = GiocatorePtr; //puntatore alla struct
  struct Giocatore* ControllaGiocatori; //serivira' piu' avanti
  for (int i = 0; i < nGiocatori; i++, tmp++)
  {
    int controlla_nomi;
    logo ();
    do
    {
      printf("\nInserisci il nome del giocatore %d\n", (i+1));
      ColoreGiocatore (i);
      fgets(tmp -> nome_giocatore, 63, stdin);
      printf(RESET);
      //controlla che alla fine dell'array non ci sia "\n" al posto di "\0"
      if ((tmp -> nome_giocatore [strlen(tmp -> nome_giocatore) - 1]) == '\n')
      {
        tmp -> nome_giocatore [strlen(tmp -> nome_giocatore) - 1] = '\0';
      } else spazzino ();
      //controlla che ogni giocatore abbia un nome diverso
      for (ControllaGiocatori = GiocatorePtr, controlla_nomi = 0; controlla_nomi < i; ControllaGiocatori++, controlla_nomi++)
      {
        if (strcmp(tmp -> nome_giocatore, ControllaGiocatori -> nome_giocatore) == 0)
        {
          system ("clear");
          logo ();
          controlla_nomi = 7;
          printRED("Il nome che hai selezionato e' gia' stato scelto da un altro giocatore");
        }
      }
      if (strlen(tmp -> nome_giocatore) == 0) //controlla che non sia stato messo come nome invio
      {
        system ("clear");
        logo ();
        controlla_nomi = 7;
        printRED("NOME NON VALIDO");
      }
    } while(controlla_nomi > 6); //vincolo che evita che due giocatori scelgano lo stesso nome o che inseriscano come nome il carattere invio
    tmp -> sanita_mentale = 100;
    tmp -> movimento = 0;
    tmp -> immunita = 0;
    tmp -> posizione = NULL;
    tmp -> colore = i;
    tmp -> stato = vivo;
    tmp -> zaino [0] = niente;
    tmp -> zaino [1] = nessun_oggetto;
    tmp -> zaino [2] = nessun_oggetto;
    tmp -> zaino [3] = nessuna_prova;
    tmp -> zaino [4] = niente;
    if (strcmp(tmp -> nome_giocatore, "Drinkwater") == 0)
    {
      tmp -> sanita_mentale = 50;
    }
    if (strcmp(tmp -> nome_giocatore, "Santoni") == 0)
    {
      tmp -> zaino [1] = adrenalina;
    }
    if (strcmp(tmp -> nome_giocatore, "Panfili") == 0)
    {
      tmp -> zaino [1] = coltello;
    }
    if (strcmp(tmp -> nome_giocatore, "Reigen") == 0)
    {
      tmp -> zaino [4] = sale;
    }
    if (strcmp(tmp -> nome_giocatore, "Berlusconi") == 0)
    {
      tmp -> zaino [1] = cento_dollari;
    }
    system("clear");
  }
  system("clear");
}

//menu per impostare gli oggetti iniziali
static void ImpostaOggettoIniziale (void)
{
  logo ();
  int arrpl [(nGiocatori)];
  for (int i = 0; i < nGiocatori; i++)
  {
    arrpl [i] = i;
  }
  int arrza [5];
  for (int i = 0; i < 5; i++)
  {
    arrza [i] = i;
  }
  SpostaCelle (arrza,5); //array randomizzato che contiene i possibili oggetti selezionabili
  SpostaCelle (arrpl,nGiocatori); //array randomizzato che contiene l'ordine in cui i giocatori potranno selezionare gli oggetti
  if (nGiocatori == 1)
    arrza [0] = casuale (3);
  if (nGiocatori == 2 && arrza [0] > 2 && arrza [1] > 2)
    arrza [casuale(nGiocatori)] = casuale (3);
  struct Giocatore* tmp;
  int sceltatmp;
  for (int i = nGiocatori; i > 0; i--) //ciclo che fa scegliere un oggetto a ciascun giocatore
  {
    tmp = GiocatorePtr + arrpl [(i-1)];
    do
    {
      do
      {
        ColoreGiocatore (tmp -> colore);
        printf("%s"RESET", scegli un oggetto da mettere nel tuo zaino\n", tmp -> nome_giocatore);
        printf("0) se vuoi consultare la "GREEN"legenda"RESET"\n");
        for (int k = 0; k < i; k++)
        {
          printf("%d) ",(k+1));
          switch (arrza [k])
          {
            case EMF_reader:
              printf (PURPLE"EMF reader\n"RESET);
              break;
            case spirit_box:
              printf (PURPLE"spirit box\n"RESET);
              break;
            case videocamera:
              printf (PURPLE"videocamera\n"RESET);
              break;
            case calmanti:
              printf (BLUE"calmanti\n"RESET);
              break;
            case sale:
            printf (BLUE"sale\n"RESET);
            break;
          }
        }
        printf("Scelta: ");
        scanf("%d", &sceltatmp);
        spazzino ();
        system("clear");
        if (sceltatmp == 0)
        Legenda (); //in caso di necessita' c'e' anche la possibilita' di consultare la legenda per capire l'utilita' degli oggetti
      } while(sceltatmp == 0);
      logo ();
      if (sceltatmp < 1 || sceltatmp > i)
      {
        printf(RED "ERRORE: Devi inserire un numero tra 0 e %d" RESET "\n", i);
      }
      else
      {
        if (arrza[sceltatmp-1] < 3)
        {
          tmp -> zaino [0] = arrza[sceltatmp-1]; //se sceglie uno strumento per raccogliere prove finisce nello slot 0 dello zaino
          printf("Hai scelto %s\n", StampaOggettoIniziale (tmp, 0));
          switch (tmp -> zaino [0]) //in caso di selezione di un oggetto per raccogliere prove viene aggiornata la variabile che tiene conto di questo,
          {                         //dato che gli oggetti per raccogliere prove sono unici
            case EMF_reader:
              InventarioCaravan [0] = 0;
              break;
            case spirit_box:
              InventarioCaravan [1] = 0;
              break;
            case videocamera:
              InventarioCaravan [2] = 0;
              break;
          }
        } else
        {
          tmp -> zaino [4] = arrza[sceltatmp-1]; //se non sceglie uno strumento per raccogliere prove finisce nelo slot 4 dello zaino
          printf("Hai scelto %s\n", StampaOggettoIniziale (tmp, 4));
        }

        scambio(&arrza[(i-1)],&arrza[(sceltatmp-1)]);
        system("clear");
        logo ();
      }
    } while(sceltatmp < 1 || sceltatmp > i);
  }
  system("clear");
  logo ();
  printGREEN("GIOCATORI IMPOSTATI CON SUCCESSO");
}

//menu per impostare la difficolta
static void ImpostaDifficolta (void)
{
  unsigned short n;
  logo ();
  do
  {
    printf("Digita:\n");
    printf("1)  Per impostare la difficolta a "GREEN"dilettante\n"RESET);
    printf("2)  Per impostare la difficolta a "BROWN"intermedio\n"RESET);
    printf("3)  Per impostare la difficolta a "RED"incubo\n"RESET);
    printf("Scelta: ");
    scanf("%hu", &n);
    spazzino();
    system("clear");
    logo ();
    if (n == 3 || n == 1 || n == 2)
    {
      difficolta = n;
    } else printRED("ERRORE: Devi inserire un numero tra 1 e 3\n");
  } while(n!=3 && n!=1 && n!=2);
  printGREEN("DIFFICOLTA' IMPOSTATA CON SUCCESSO");
}

//funzione che stampa le impostazioni selezionate richiamando altre sottofunzioni
static void StampaImpostazioni (void)
{
  struct Giocatore* tmp = GiocatorePtr;
  logo ();
  if (carceriere [0] == 1)
  {
    for(int i = 0; i < nGiocatori; i++, tmp++)
    {
      printf("Nome: ");
      ColoreGiocatore (tmp -> colore);
      printf("%s"RESET"\n", tmp -> nome_giocatore);
      printf("Sanita' mentale: %d\n", tmp -> sanita_mentale);
      printf("Oggetto iniziale: %s\n\n", StampaOggettoIniziale (tmp, 5)); //l'unico caso in cui serve l'utilizzo del 5
    }
  } else printRED("DEVI ANCORA IMPOSTARE I GIOCATORI\n");
  if (carceriere [1] == 1)
  {
    printf("La difficolta' inserita e': ");
    switch (difficolta)
    {
      case dilettante: printGREEN("dilettante\n");
      break;
      case intermedio: printBROWN("intermedio\n");
      break;
      case incubo: printRED("incubo\n");
      break;
    }
  } else printRED("DEVI ANCORA IMPOSTARE LA DIFFICOLTA'\n");
  if (carceriere [2] == 1)
    StampaCasaMenu ();
  else
    printRED("DEVI ANCORA IMPOSTARE LA CASA\n");

  piantone ();
  system("clear");
  logo ();
}

//menu contenente le funzioni per impostare la mappa
static void ImpostaMappa (void)
{
  int sceltamappa;
  bool MiniSettato = false;
  logo ();
  if (nStanze == 0)
    printCYAN("Benvenuto nel menu' di impostazione della mappa");
  printf("\n");
  do
  {
    printf("1) Inserisci stanza\n");
    printf("2) Cancella stanza\n");
    printf("3) Visualizza mappa\n");
    printf("4) Torna al menu' di impostazione\n");
    printf("Scelta: ");
    scanf("%d",&sceltamappa);
    spazzino ();
    switch (sceltamappa)
    {
      case 1:
        if (nStanze < 50)
        {
          system ("clear");
          logo ();
          InserisciZona ();
        } else printRED ("E' STATO GIA' RAGGIUNTO IL LIMITE MASSIMO DI 50 STANZE");
        printf ("\n");
        break;
      case 2:
        system ("clear");
        logo ();
        if (nStanze > 0)
        {
          CancellaZona ();
        } else printRED("NON CI SONO PIU' STANZE DA CANCELLARE");
        printf ("\n");
        break;
      case 3:
        system ("clear");
        logo ();
        if (nStanze > 0)
        {
          StampaCasaMenu ();
          StampaDatiMappa ();
          logo ();
        } else printRED("ERRORE: DEVI PRIMA IMPOSTARE LA MAPPA.");
        printf ("\n");
        break;
      case 4:
        system ("clear");
        logo ();
        if (nStanze > 2)
        {
          MiniSettato = true;
        } else printRED ("ERRORE: DEVI INSERIRE ALMENO 2 STANZE PRIMA DI USCIRE\n");
        break;
      default:
        system("clear");
        logo ();
        printRED("ERRORE: DEVI INSERIRE UN NUMERO TRA 1 E 4\n");
    }
  } while(!MiniSettato);
  pLast -> prossima_zona = pFirst; //prima di uscire da questo menu collega l'ultima stanza alla prima
}

//funzione che permette di inserire da 1 a 10 stanze
static void InserisciZona ()
{
  int appStanze;
  printf("\n");
  do
  {
    printf("Quante stanze vuoi inserire?\n");
    scanf("%d",&appStanze);
    spazzino ();
    if (appStanze > 10)
    {
      system("clear");
      logo ();
      printRED("ERRORE: PUOI INSERIRE UN MASSIMO DI 10 STANZE ALLA VOLTA\n");
    } else if (appStanze < 1)
    {
      system("clear");
      logo ();
      printRED("ERRORE: DEVI INSERIRE UN NUMERO VALIDO\n");
    }
  } while (appStanze > 10 || appStanze < 1);
  if (nStanze + appStanze > 50)
  {
    piantone ();
    printRED("IL NUMERO DI STANZE E' STATO RIDIMENSIONATO PER NON SFORARE IL LIMITE DI 50 STANZE\n");
    while(nStanze > 50)
    {
      appStanze--;
      nStanze--;
    }
  }
  nStanze += appStanze;
  for (int i = 0; i < appStanze; i++)
  {
    struct Zona_mappa *Nuova = (struct Zona_mappa*) malloc (sizeof(struct Zona_mappa));
    if (pFirst == NULL && pLast == NULL)
    {
      pFirst = Nuova;
    } else
      pLast -> prossima_zona = Nuova;
    pLast = Nuova;
    Nuova -> zona = casuale (6);
    Nuova -> prova = nessuna_prova;
    Nuova -> nCivico = (nStanze - appStanze + i) + 1;
    if (casuale(100) > 40)
    {
      Nuova -> oggetto_zona = casuale(3);
    } else Nuova -> oggetto_zona = 3;
  }
  system("clear");
  logo ();
  printGREEN("STANZE INSERITE CON SUCCESSO");
}

//funzione che cancella l'ultima stanza
static void CancellaZona ()
{
  if (nStanze == 0)
  {
    printRED("ERRORE: DEVI INSERIRE ALMENO UNA STANZA PER CANCELLARLA");
  }
  else
  {
    struct Zona_mappa* tmp = pFirst;
    if (pFirst != pLast)
    {
      struct Zona_mappa* NLast;
      do
      {
        NLast = tmp;
        tmp = tmp -> prossima_zona;
      } while(tmp != pLast);
      pLast = NLast;
    }
    else
    {
      pFirst = NULL;
      pLast = NULL;
    }
    free (tmp);
    nStanze--;
    printGREEN("ULTIMA STANZA ELIMINATA CON SUCCESSO");
  }
}

//funzione che stampa il nome e l'oggetto contenuto in ogni stanza
static void StampaDatiMappa ()
{
  printPURPLE("\nSOG(giorno)   CAM(era)   BAG(no)   GAR(age)   SEM(interrato)   CUC(ina)\n\n");
  printCYAN("ADR(enalina)   DOL(lari)   COL(tello)\n\n");
  piantone ();
  system("clear");
}

//funzione che stampa uno degli oggetti selezionabili dal caravan
static const char* StampaOggettoIniziale (struct Giocatore* m, int f)
{
  if (f == 5) //se e' 5 significa che non si sa se il giocatore ha selezionato uno strumeto per raccogliere prove o altro
  {           //(cosa possibile soltanto nella stampa delle impostazioni) quindi controlla quale tra gli slot dello zaino e' stato impostato
    if (m -> zaino [0] == niente)
    {
      f = 4;
    } else f = 0;
  }
  switch (m -> zaino [f])  //f permette di stampare lo slot 0 o lo slot 4 in base alle necessita'
  {
    case EMF_reader: return "EMF reader";
    case spirit_box: return "spirit box";
    case videocamera: return "videocamera";
    case calmanti: return "calmanti";
    case sale: return "sale";
    default: return "nessun oggetto";
  }
}

//funzione che stampa le prove raccolte
static const char* StampaProveRaccolte (struct Giocatore* m)
{
  switch (m -> zaino [3])
  {
    case prova_EMF: return "prova EMF";
    case prova_spirit_box: return "prova spirit box";
    case prova_videocamera: return "prova videocamera";
    default: return "nessuna prova";
  }
}

//funzione che stampa le prove presenti in una stanza
static const char* StampaProveDroppate (struct Zona_mappa* m)
{
  switch (m -> prova)
  {
    case prova_videocamera: return "prova videocamera";
    case prova_EMF: return "prova EMF";
    case prova_spirit_box: return "prova spirit box";
    default: return "nessuna prova";
  }
}

//funzione che stampa l'oggetto presente in una stanza
static const char* StampaOggettoStanza (struct Zona_mappa* m)
{
  switch (m -> oggetto_zona)
  {
    case coltello      : return "coltello";
    case adrenalina    : return "adrenalina";
    case cento_dollari : return "cento dollari";
    default: return "nessun oggetto";
  }
}

//funzione che stampa l'oggetto presente in una stanza
static const char* StampaOggettoStanzaMenu (struct Zona_mappa* m)
{
  switch (m -> oggetto_zona)
  {
    case coltello      : return "COL";
    case adrenalina    : return "ADR";
    case cento_dollari : return "DOL";
    default: return "   ";
  }
}

//funzione che stampa gli oggetti raccolti dal giocatore dentro la casa
static const char* StampaOggettoGiocatore (struct Giocatore* m, int f)
{
  switch (m -> zaino[f])
  {
    case 0: return "adrenalina";
    case 1: return "cento dollari";
    case 2: return "coltello";
    default: return "nessun oggetto";
  }
}

//funzione che stampa le il nome della stanza richiamata
static const char* StampaPosizione (struct Zona_mappa* m)
{
  switch (m -> zona)
  {
    case caravan: return "caravan";
    case soggiorno: return "soggiorno";
    case camera: return "camera";
    case bagno: return "bagno";
    case garage: return "garage";
    case seminterrato: return "seminterrato";
    case cucina: return "cucina";
    default: return "nessuna stanza";
  }
}

//funzione che stampa le il nome della stanza richiamata
static const char* StampaNomeStanza (struct Zona_mappa* m)
{
  switch (m -> zona)
  {
    case soggiorno: return "SOG";
    case camera: return "CAM";
    case bagno: return "BAG";
    case garage: return "GAR";
    case seminterrato: return "SEM";
    case cucina: return "CUC";
    default: return "nessuna stanza";
  }
}

//INIZIO FUNZIONI ESCLUSIVE DELLA FUNZIONE gioca

//funzione che fa "tornare al caravan", anche se in realta' la propria posizione non cambia
static void torna_caravan (struct Giocatore* temp)
{
  system ("clear");
  int risposta = 0; //nel caravan le azioni si possono fare interagendo con un npc
  printPURPLE("*dentro il caravan trovi il tuo capo Genny Tillini che ti aspettava*\n");
  do
  {
    printf(PURPLE"Genny:"RESET" hey ");
    ColoreGiocatore (temp -> colore);
    printf("%s"RESET"! Cosa ci fai qui sul caravan?\n", temp -> nome_giocatore);
    do
    {
      printf("1) Ti ho portato delle prove!\n");
      printf("2) Mi serve della nuova attrezzatura.\n");
      printf("3) Avevo tanta paura :(\n");
      scanf("%d", &risposta);
      spazzino ();
      if (risposta > 3 || risposta < 1) printRED("Genny: non ho capito nulla di quello che mi hai detto, digita un numero da 1 a 3 che non stiamo qui a perdere tempo ok?\n");
    } while(risposta > 3 || risposta < 1);

    if (risposta == 3)
    {
      risposta = 0;
      do //se e' stata selezionata come risposta 3, si fa una verifica che le intenzioni fossero quelle prima di rispedirlo nella casa
      {
        ColoreGiocatore (temp -> colore);
        printf("%s: "RESET"Avevo tanta paura :(\n", temp -> nome_giocatore);
        printf(PURPLE"Genny:"RESET" Sei veramente venuto qui solo per questo!?\n");
        printf("1) SI :(\n");
        printf("2) No, ti stavo prendendo in giro!\n");
        scanf("%d", &risposta);
        spazzino ();
        if (risposta != 1 && risposta != 2)
        {
          printRED("Genny: non ho capito nulla di quello che mi hai detto, digita un numero tra 1 e 2 che non stiamo qui a perdere tempo ok?\n");
          printf(PURPLE"Genny:"RESET" hey ");
          ColoreGiocatore (temp -> colore);
          printf("%s"RESET"! Cosa ci fai qui sul caravan?\n", temp -> nome_giocatore);
        }
      } while(risposta > 2 || risposta < 1);
      if (risposta == 1)
      {
        ColoreGiocatore (temp -> colore);
        printf("%s:"RESET" SI :(\n", temp -> nome_giocatore);
        printf(PURPLE"Genny:"RESET" Non ti preoccupare, non tutti posso essere come la grande Genny Tillini, avvicinati\n");
        printPURPLE("*Genny ti da una pacca sulla spalla*\n");
        printf(PURPLE"Genny:"RESET" %s", FrasiFatte());
        printf(GREEN"Le parole del tuo capo ti confortano e ti fanno recuperare +%d sanita' mentale\n"RESET, (40-(10*difficolta)));
        printf(PURPLE"Genny:"RESET" Ora pero' sbrigati e caccia quel dannato %s dalla casa, OKAY?!\n", NomeFantasma);
        temp -> sanita_mentale += (40-(10*difficolta));
        if (temp -> sanita_mentale > 100) temp -> sanita_mentale = 100;
        risposta = 635372;
      } else
      {
        ColoreGiocatore (temp -> colore);
        printf("%s:"RESET" No, ti stavo prendendo in giro!\n", temp -> nome_giocatore);
        printf(PURPLE"Genny:"RESET" Non sei per niente divertente, okay?\n");
        do
        {
          printf(PURPLE"Genny:"RESET" Perche' sei venuto qua?\n");
          printf("1) Ti ho portato delle prove!\n");
          printf("2) Mi serve della nuova attrezzatura.\n");
          scanf("%d", &risposta);
          spazzino ();
          system("clear");
          if (risposta != 1 && risposta != 2) printRED("Genny: non ho capito nulla di quello che mi hai detto, digita un numero tra 1 e 2 che non stiamo qui a perdere tempo ok?\n");
        } while(risposta < 1 || risposta > 2);
        printf(PURPLE"Genny:"RESET" Perche' sei venuto qua?\n");
      }
    }

    if (risposta == 1) //si vogliono consegnare delle prove
    {
      ColoreGiocatore (temp -> colore);
      printf("%s:"RESET" Ti ho portato delle prove!\n", temp -> nome_giocatore);
      printf(PURPLE"Genny:"RESET" Veramente?! Fammi un po' controllare\n");
      if (temp -> zaino [3] == nessuna_prova) //se non si ha nessuna prova si sentira' presa in giro e ti togliera' un po' di sanita mentale
      {
        printf(PURPLE"Genny:"RESET" HEY MA TU NON HAI NESSUNA PROVA!\n");
        printPURPLE("*la sfuriata di Genny ti mette un po' di angoscia*\n");
        if (temp -> immunita > 0)
        {
          printGREEN("MA TU HAI ANCORA DEL SALE E COMINCI A GETTARLO A DESTRA E A MANCA\n"); //ma tu sei immune alle decrescite di sanita mentale
          printBLUE("Genny vedendo l'allievo che supera il maestro finisce immediatamente la sfuriata e ti applaude\n");
        } else
        {
          printf(RED"-%d sanita' mentale\n"RESET, (difficolta * 5));
          temp -> sanita_mentale -= (5 * difficolta);
        }
        printf(PURPLE"Genny:"RESET" Dai su, ricominciamo da capo\n");
        piantone ();
        system ("clear");
      } else //hai effettivamente una prova, quindi se la prende e ti ripaga con un nuovo strumento
      {
        printf(PURPLE"Genny:"RESET" E' proprio magnifica questa %s!\n", StampaProveRaccolte(temp));
        printPURPLE("*Genny prende la tua prova*\n");
        exodia ++;
        if (exodia == 3) vittoria ();
        printf(PURPLE"Genny:"RESET" G R A N D I O S O, prendi questo, ti sara' utile per continuare la ricerca\n");
        for (int i = 0; i < 3; i++)
        {
          if (InventarioCaravan [i] != 0)
          {
            temp -> zaino [0] = i;
            InventarioCaravan [i] = 0;
            i = 3;
            printf(CYAN"*Genny ti da in mano %s*\n", StampaOggettoIniziale(temp, 0)); //se ci sono strumenti te ne da uno
          } else
          {
            temp -> zaino [4] = casuale(2) + 3;
            if (i == 2)
            printf(CYAN"*Genny ti da in mano %s*\n", StampaOggettoIniziale(temp, 4)); //se non ci sono strumenti ti da o il sale o dei calmanti
          }
        }
        printf(PURPLE"Genny:"RESET" ORA VA E CONQUISTA IL MONDO, OK?\n");
        temp -> zaino [3] = nessuna_prova;
        risposta = 635372;
      }
    }

    if (risposta == 2) //hai chiesto degli oggetti
    {
      ColoreGiocatore (temp -> colore);
      printf("%s:"RESET" Mi serve della nuova attrezzatura.\n", temp -> nome_giocatore);
      printf(PURPLE"Genny:"RESET" NON TI PREOCCUPARE, c'e' la tua Genny Tillini a rifornirti\n");
      do
      {
        do //menu di selezione dell'oggetto (stampa a prescindere oggetti disponibili e non)
        {  //gli oggetti non disponibili sarebbero gli strumenti gia' presi
          printf(PURPLE"Genny:"RESET" Dimmi cosa potrebbe servirti e vedo se ce l'ho?\n");
          printf("1) Mi servirebbe un EMF reader\n");
          printf("2) Mi servirebbe una spirit box\n");
          printf("3) Mi servirebbe una videocamera\n");
          printf("4) Mi servirebbero dei calmanti\n");
          printf("5) Mi servirebbe del sale\n");
          scanf("%d", &risposta);
          spazzino ();
          if (risposta < 1 || risposta > 5) printRED("Genny: Non ho capito nulla di quello che mi hai detto, digita un numero da 1 a 5 che non stiamo qui a perdere tempo ok?\n");
        } while (risposta < 1 || risposta > 5);
        risposta--;
        if (risposta == 0 || risposta == 1 || risposta == 2)
        {
          if (InventarioCaravan [risposta] == 0) //hai scelto uno strumento che e' gia' stato preso, quindi ti fa riscegliere
          {
            printf(PURPLE"Genny:"RESET" Mi dispiace tanto ma non sono riuscita a trovare nulla\n");
          } else
          {
            temp -> zaino [0] = risposta;
            InventarioCaravan [risposta] = 0;
            risposta = 635372;
            printf(CYAN"*Genny ti da %s*\n", StampaOggettoIniziale(temp, 0));
          }
        } else
        {
          temp -> zaino [4] = risposta;
          risposta = 635372;
          printf(CYAN"*Genny ti da %s*\n", StampaOggettoIniziale(temp, 4));
        }
      } while(risposta != 635372);
      printf(PURPLE"Genny:"RESET" Spero ti sia utile, per qualsiasi cosa io sono qui ok?\n");
    }
  } while(risposta != 635372);
  printf(CYAN"*sei rientrato nella casa e sei tornato nella stanza di prima: %s, pero' tutta questa strada ti ha stancato molto*\n"RESET, StampaPosizione(temp -> posizione));
  temp -> movimento--; //per andare al caravan si consuma un movimento
  piantone ();
  system ("clear");
}

//funzione che stampa tutti i dati del giocatore di turno
static void stampa_giocatore (struct Giocatore* temp)
{
  printf("Nome giocatore: ");
  ColoreGiocatore (temp -> colore);
  printf("%s"RESET"\n", temp -> nome_giocatore);
  printf("Sanita' mentale: %d\n", temp -> sanita_mentale);
  printf("Zaino:\n");
  printf("\tStrumenti per raccogliere prove:"PURPLE" %s\n"RESET, StampaOggettoIniziale (temp, 0)); //lo slot 0 e' lo slot degli strumenti per raccogliere prove
  printf("\tStrumenti speciali:"BLUE" %s\n"RESET, StampaOggettoIniziale (temp, 4)); //lo slot 4 e' lo slot del sale e dei calmanti
  printf("\tOggetti \"presi in prestito\" da Santoni:"CYAN);
  for (int i = 1; i < 3; i++) //gli slot 1 e 2 sono gli slot degli strumenti raccoglibili nella casa
  {
    switch (temp -> zaino [i])
    {
      case coltello      : printf(" coltello");
      break;
      case adrenalina    : printf(" adrenalina");
      break;
      case cento_dollari : printf(" cento dollari");
      break;
      case nessun_oggetto: printf(" nessun oggetto");
      break;
    }
    if (i == 1)
    printf(RESET","CYAN);
  }
  printf(RESET"\n\tProve raccolte:"GREEN" %s\n"RESET, StampaProveRaccolte(temp)); //lo slot 3 e' lo slot delle prove raccolte
  piantone ();
}

//funzione che stampa tutti i dati della stanza dove si trova il giocatore di turno
static void stampa_stanza (struct Zona_mappa* temp)
{
  printf("Nome stanza: "PURPLE"%s\n"RESET, StampaPosizione (temp));
  printf("Oggetti presenti: "CYAN"%s\n"RESET, StampaOggettoStanza (temp));
  printf("Prove lasciate dal fantasma: "GREEN"%s\n"RESET, StampaProveDroppate (temp));
  temp = temp -> prossima_zona;
  printf("Prossima stanza: "PURPLE"%s\n"RESET, StampaPosizione (temp));
  piantone ();
}

//funzione che permette al giocatore di passare alla stanza successiva
static void avanza (struct Giocatore* temp)
{
  temp -> posizione = temp -> posizione -> prossima_zona; //stampa anche l'oggetto presente nella stanza
  printf("Sei entrato in %s dove vedi per terra %s\n", StampaPosizione(temp -> posizione), StampaOggettoStanza(temp -> posizione));
  if (casuale(100) < 34)
  {
    printf("Entrando nella stanza senti degli strani rumori\n");
    temp -> posizione -> prova = (casuale(3) + 1);
  } else temp -> posizione -> prova = 0;
  temp -> movimento--;

  piantone ();
  system ("clear");
}

//funzione che permette di raccogliere una prova
static void raccogli_prova (struct Giocatore* temp)
{
  struct Zona_mappa* appoggio = temp -> posizione;
  if (appoggio -> prova == nessuna_prova)
  {
    printf("Non c'e' nessuna prova qui, ti senti bene?\n");
    if (temp -> immunita > 0) //se si prova a raccogliere una prova inesistente viene tolta della sanita mentale
    {
      printf("SI CHE MI SENTO BENE, C'E' DEL SALE QUI\n");
      printGREEN("COMINCI A LANCIARE SALE A DESTRA E A MANCA\n"); //ma si puo essere immuni anche a questo
    } else
    {
    printf(RED"-%d sanita mentale\n"RESET, (5 * difficolta));
    temp -> sanita_mentale -= (5 * difficolta);
    }
  } else
  {
    if (appoggio -> prova == prova_videocamera) //doppio controllo se c'e' sia la prova che lo strumento giusto
    {
      if (temp -> zaino [0] == videocamera)     //non sullo stesso if erche' almeno potevo mettere il print ad hoc
      {
        temp -> zaino [3] = prova_videocamera;
        temp -> zaino [0] = niente;
        appoggio -> prova = 0;
      } else printf("Per registrare questa prova ti serve una videocamera!\n");
    } else if (appoggio -> prova == prova_spirit_box)
    {
      if (temp -> zaino [0] == spirit_box)
      {
        temp -> zaino [3] = prova_spirit_box;
        temp -> zaino [0] = niente;
        appoggio -> prova = 0;
      } else printf("Per registrare questa prova ti serve una spirit box!\n");
    } else if (appoggio -> prova == prova_EMF)
    {
      if (temp -> zaino [0] == EMF_reader)
      {
        temp -> zaino [3] = prova_EMF;
        temp -> zaino [0] = niente;
        appoggio -> prova = nessuna_prova;
      } else printf("Per registrare questa prova ti serve un'EMF reader!\n");
    }
    if (appoggio -> prova == nessuna_prova)
    {
      printGREEN("La prova e' stata registrata\n");
      if ((casuale (11) * difficolta * (exodia + 1)) > 24)
      {
        printf("Ma a %s non e' stato molto bene\n", NomeFantasma); //possibilita' che il fantasma appaia mentre stai raccogliendo una prova
        struct Giocatore* maradona = GiocatorePtr;
        for (int i = 0; i < nGiocatori; i++, maradona++)
        {
          if (maradona -> posizione == temp -> posizione)
          {
            ColoreGiocatore (maradona -> colore);
            printf("%s"RESET" viene attaccato dal fantasma di %s\n", maradona -> nome_giocatore, NomeFantasma);
            if (temp -> immunita > 0)
            {
              printGREEN("MA TU HAI ANCORA DEL SALE E USI LO SPAVENTI USANDO LA TECNICA DEL SALT SPASH\n");
            } else
            {
              printf(RED"-%d sanita mentale\n"RESET, (10*difficolta + (exodia + 1) * 10));
              temp -> sanita_mentale -= (10*difficolta + (exodia + 1) * 10);
            }
          }
        }
      }
    }
  }
  piantone ();
}

//funzione che permette di raccogliere un oggetto da terra
static void raccogli_oggetto (struct Giocatore* temp)
{
  struct Zona_mappa* appoggio = temp -> posizione;
  if (appoggio -> oggetto_zona != nessun_oggetto)
  {
    if (temp -> zaino [1] == nessun_oggetto)
    {
      temp -> zaino [1] = appoggio -> oggetto_zona;
      appoggio -> oggetto_zona = nessun_oggetto;
    } else if (temp -> zaino [2] == nessun_oggetto)
    {
      temp -> zaino [2] = appoggio -> oggetto_zona;
      appoggio -> oggetto_zona = nessun_oggetto;
    } else
    {
      int k;
      printf("Il tuo zaino e' gia' pieno, digita 0 se vuoi sostituire %s con un tuo oggetto altrimenti digita un numero qualunque\n", StampaOggettoStanza(appoggio));
      scanf("%d", &k);
      spazzino();
      if (k == 0)
      {
        printf("Con quale oggetto lo vuoi sostituire?\n");
        do
        {
          for (int i = 1; i < 3; i++)
          {
            switch (temp -> zaino [i])
            {
              case coltello      : printf("%d) coltello", i);
              case adrenalina    : printf("%d) adrenalina", i);
              case cento_dollari : printf("%d) cento dollari", i);
              default: printf("Non dovrei poter scrivere questo ma lo sto facendo comunque, fossi in te ricontrollerei raccogli oggetto\n");
            }
          }
          scanf("%d", &k);
          spazzino ();
          if (k != 1 || k != 2) printRED("ERRORE: LE UNICHE OPZIONI VALIDE SONO 1 E 2\n");
        } while (k != 1 || k != 2);
        temp -> zaino [k] = appoggio -> oggetto_zona;
        appoggio -> oggetto_zona = nessun_oggetto;
      } else printf("Non hai raccolto nessun oggetto\n");
    }
    if (appoggio -> oggetto_zona == nessun_oggetto) printGREEN("OGGETTO RACCOLTO\n");
  } else
  printf("Non c'e' nessun oggetto da raccogliere\n");
  piantone ();
}

//funzione che permette di usare un oggetto
static void usa_oggetto (struct Giocatore* temp, int* Pchoice)
{
  int i = 0, min = 1, max = 1;

  if (temp -> zaino [4] != niente || temp -> zaino [1] != nessun_oggetto || temp -> zaino [2] != nessun_oggetto)
  {
    do
    {
      printf("Quale oggetto vuoi utilizzare?\n"); //nel gioco vengono stampati solo i pezzi di zaino dove si ha qualcosa
      if (temp -> zaino [4] != niente)
      {
      min = 0;
      printf("0) %s\n", StampaOggettoIniziale  (temp, 4));
      }
      if (temp -> zaino [1] != nessun_oggetto)
      {
      printf("%d) %s\n", max, StampaOggettoGiocatore (temp, 1));
      max++;
      }
      if (temp -> zaino [2] != nessun_oggetto)
      {
      printf("%d) %s\n",max, StampaOggettoGiocatore (temp, 2));
      max ++;
      }
      printf("%d) Non voglio usare nulla\n", max);
      scanf("%d", &i);
      spazzino ();
    } while(i < min || i > max);
    if (i == 0) //i = 0 solo se c'era qualcosa nello slot 4 dello zaino e se lo abbiamo selezionato
    {
      if (temp -> zaino [4] == sale) //se si vuole usare il sale
      {
        temp -> zaino [4] = nessun_oggetto;
        ColoreGiocatore (temp -> colore);
        printf("%s"RESET" tiene una manciata di sale in mano, pronto per usare \"salt splash\"\n", temp -> nome_giocatore);
        printf("Per i prossimi 3 turni sei pronto a contrattaccare a qualsiasi imprevisto\n");
        temp -> immunita += 3;
      } else
      if (temp -> zaino [4] == calmanti) //se si vuole usare i calmanti
      {
        temp -> zaino [4] = nessun_oggetto;
        ColoreGiocatore (temp -> colore);
        printf("%s"RESET" si prepara una tisana rilassante alle erbe che gli fa riacquisire un po' di sanita' mentale\n", temp -> nome_giocatore);
        temp -> sanita_mentale += 40;
        if (temp -> sanita_mentale > 100)
        {
          if (temp -> sanita_mentale > 120)
          {
            printf("Pero' sare i farmaci quando si e' sani non e' mai buona cosa, infatti stai sentendo dei forti crampi allo stomaco che ti impediranno di muoverti per 1 turno\n");
            temp -> movimento--;
          }
          temp -> sanita_mentale = 100;
        }
        temp -> zaino [4] = niente;
      }
    } else
    if (i != max) //i = max solo se si e' selezionato di non voler usare nulla
    {
      if (temp -> zaino [i] == coltello)
      {
        UsaColtello (temp, i); //richiamo alla funzione per usare il coltello
        if (temp -> zaino [i] != coltello)
          *Pchoice = 8; //se si e' usato il coltello si passa il turno
      }
      if (temp -> zaino [i] == adrenalina) //se si e' selezionato adrenalina
      {
        ColoreGiocatore (temp -> colore);
        printf("%s"RESET" tira fuori lo shaker che ogni acchiappafantasmi ha sempre con se' e lo utilizza\n", temp -> nome_giocatore);
        printf("per degustare la squisita bevanda energetica \""GREEN);
        LevelUpp ();
        printf(RESET"Ti senti pieno di energie, al punto che riusciresti addirittura a percorrere ben 2 stanze in un turno\n");
        temp -> movimento++;
        temp -> zaino [i] = nessun_oggetto;
      } else if (temp -> zaino [i] == cento_dollari) UsaDollari (temp, i); //richiamo alla funzione per usare i dollari
    } else printf("Non hai usato nulla\n");
  } else printf("Non puoi usare un oggetto se non hai nulla!\n");
  piantone ();
  system ("clear");
}

//funzione che permette di usare l'oggetto "coltello"
static void UsaColtello (struct Giocatore* temporaneo, int k)
{
  printf("Ti torna in mente di avere un coltello nello zaino");
  if (temporaneo -> sanita_mentale < 31) //si entra solo se si e' impazziti abbastanza
  {
    printf(" e lo tiri fuori\n");
    struct Giocatore* ammazza = GiocatorePtr;
    printf("Nella stanza c'e' una luce molto fioca ma intravedi delle ombre\n");
    printf("Colpisci una di queste ombre e scopri che era");
    for (int s = 0; s < nGiocatori; s++, ammazza++) //ciclo for che controlla se c'e' qualcuno da uccidere nella stanza
    {
      if (ammazza -> posizione == temporaneo -> posizione && ammazza -> nome_giocatore != temporaneo -> nome_giocatore)
      {
        ColoreGiocatore (ammazza -> colore);
        printf(" %s"RESET, ammazza -> nome_giocatore);
        ControllaBattito (ammazza);
        temporaneo -> zaino [k] = 4;
      }
    }
    if (temporaneo -> zaino [k] == 4) //risulta vero solo se si e' ucciso qualcuno
    {
      printRED("\nTi rendi che cio' che hai colpito non e' piu' in vita e scappi dalla scena del crimine gettando il coltello per terra\n");
      temporaneo -> posizione = temporaneo -> posizione -> prossima_zona;
    } else
      printf(" semplicemente un'ombra, cosi' decidi di riporre il coltello\n"); //se non si uccide nessuno il coltello resta al proprietario
  } else printf(" ma non vedi quale vantaggio potrebbe darti contro un nemico intangibile e decidi di non tirarlo fuori\n");
}

//funzione che permette di usare l'oggetto "cento_dollari"
static void UsaDollari (struct Giocatore* temporaneo, int k)
{
  int answer;
  printPURPLE("*appena tiri fuori i soldi arriva Robb Ioni, venditore affiliato alla \"Big Pharma S.p.A.\"*\n"); //npc che ti vende sale e calmanti per soli 100 dollari
  do
  {
    printf(PURPLE"Robb:"RESET" Carissimo, ho percepito il tuo bisogno di aiuto e si da il caso che io sia il piu' fornito della zona\n");
    printf(PURPLE"Robb:"RESET" Quest'oggi mi sono portato dietro:\n");
    if (temporaneo  -> zaino [4] != niente) //se hai gia' un oggetto speciale l'onesto venditore te lo segnalera'
    printf(PURPLE"Robb:"RESET" Ma hey, tu hai gia' un oggetto speciale, tu hai: %s, sei sicuro di volerlo buttare cosi'?\n", StampaOggettoIniziale(temporaneo, 4));
    printf("1) calmanti\n");
    printf("2) sale\n");
    printf("3) niente\n");
    printf(PURPLE"Robb:"RESET" quale ti fa piu' comodo?\n");
    printf("Scelta: \n");
    scanf("%d", &answer);
    spazzino ();
  } while (answer < 1 || answer > 3);
  if (answer == 3) //entra se si e' digitato "niente"
  {
    printf(PURPLE"Robb:"RESET" va bene, se ti servira' qualcosa chiama sempre Robb Ioni!\n");
  } else
  { //print solo se si e' volutamente scelto di sostituire il proprio oggetto con quello venduto da Robb Ioni
    if (temporaneo  -> zaino [4] != niente) printf(PURPLE"Robb:"RESET" Beh, se la metti cosi' allora non credo ti dispiaccia se il tuo %s me lo intasco io\n", StampaOggettoIniziale (temporaneo, 4));
    temporaneo -> zaino [4] = (answer + 2);
    temporaneo -> zaino [k] = nessun_oggetto;
    printf(CYAN"*Robb Ioni ti da %s*\n"RESET,StampaOggettoIniziale(temporaneo, 4));
    printf(PURPLE"Robb:"RESET" Il mio lavoro qui e' finito, se ti serve qualcos'altro tira fuori altri soldi e Robb Ioni arrivera' in un baleno\n");
    printPURPLE("*Robb Ioni sparisce nel buio*\n");
  }
}

static void spoiler (struct Zona_mappa* temp)
{
  printf("Gli oggetti da qui fino alla fine della casa sono:\n");
  for (int i = temp -> nCivico; i <= nStanze; i++)
  {
    printf("%s", StampaOggettoStanza (temp));
    if (i != nStanze)
      printf (", ");
    else
      printf("\n");
    temp = temp -> prossima_zona;
  }
  piantone ();
}



//funzione vuota ma richiesta dalla traccia
static void passa (void)
{

}

//funzione che controlla se il giocatore di turno e' ancora vivo e, in caso non lo sia gli fa fare tutte quelle cose che dovrebbe fare un morto
static void ControllaBattito (struct Giocatore* temp)
{
   nMorti++;
   printf(RED"La follia ha preso il sopravvento su %s che viene portato nel regno delle ombre dal fantasma\n", temp -> nome_giocatore);
   piantone ();
   if (temp -> zaino [0] < 3) //controlla se il giocatore avesse oggetti per raccogliere prove
   {
     InventarioCaravan [temp -> zaino [0]] = 1;
   }
   if (temp -> zaino [3] > 0) //controlla se il giocatore avesse raccolto prove
   {
     InventarioCaravan [temp -> zaino [3] - 1] = 1;
   }
   temp -> stato = morto;
   temp -> posizione = NULL;
   piantone ();
   system ("clear");
}

//funzione comprendente tutte le cose carine se si vince
static void vittoria (void)
{
  system ("clear");
  printGREEN("                                                                                    .                                  @                                                                        \n");
  printGREEN("                             .               @            @@@  **                  /@.                       @@@  */  @                 &#@%@@#                      *@                         \n");
  printGREEN("        &@@  @@#           @@%@@              @(           .@@@(@@.              #@ .@@             /@%#@@@.  &@@&@@@ %&@(           ..@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@/@@      ,@@,%&&@@@@@@    \n");
  printGREEN("          @@&@@,          @@@@@@           .*@&@            @@@@@@                @@ @@/             @@%/@@    @@@@@@  @,@&          @@@&(@@@@@     %@@@@@@@@@.@@@@@@@@@@  @&%@@@@* *@@@@@@@@@% \n");
  printGREEN("          ( @@@           .@&#@@           && @#(            @@@@#                 @,&&@             @@#&@     @@%@@   .@@@@         .@@@. @(        @ @@%@@           @@ #@@@@@@/        &@@@& \n");
  printGREEN("           @@@@           ,@@@@,          %@*@,@@            @@@@@                  @ %@             @@%@@     #@@@@    @%@%@&        @@@  /@        #%@@@@&           &@ @@@.             @%@@ \n");
  printGREEN("           @@@@            @*@@          @@@@ @*@@           @@@@                   %&@@*            %@ @*     ,@@@@    @%@@ @@.      @@@   @/        @@@@@,           % %@@@              &@@@.\n");
  printGREEN("            @@@            *,@@         (&(@*  @&@%          @@@@                    @@#@            %@(@       @@@@    @@@ @@/@@.    &@@             /@@@@               @@&               @*@(\n");
  printGREEN("            @@@            ,%@@         %(@&   /&@@@         &@@@                    ,@@@            (@@.       @@@@    @@@   @(@@#   @@@              @@@@               @@.               @ @@\n");
  printGREEN("            %@@     *&@&@@@@@@@        @@@*     # @@@        @@@@                     @@/%           @@@        @@@#    @@@     @@@.  @@@              &@@@               @@                @ @@\n");
  printGREEN("           @@@@@@@@@@@@@@@@@@@@%@      @@@       @%@@%       (&@@                      @@@          *@@         @@@,    @%&      .@@@ @@@              &@@#               @@                @(@#\n");
  printGREEN("       (@@@@@@@@            @@     ./ (@@    (#@@@@@@@@@     /^@@                      *@@(         @@/         @@@,    @@         .@@@@@              @@@#               @@               .@@@.\n");
  printGREEN("    (@@@@@@@@@.(            @@       #@@@@@@@@@@@@&%@@@       /@@                       &@@         @@           @@*    @@          .@@@@              @@@.               /@.               @@/ \n");
  printGREEN("       @@@  *@#%            #@ @/ %@@@@@             @@@      /@@                        @@*       @@#           @@,    @%           %@@@              &@@                 @@              @@#  \n");
  printGREEN("      @*@   ,@@*          /@%@.@%@   @@,              @@(     ,@@                        @@@      @@@            @%,    @             @@@               @@                 @@@            ,@&   \n");
  printGREEN("             @%&            &/      /@@                &@     ,@&                         @@&    (@@             @#.    @              %@               @@                  (@@@          &#    \n");
  printGREEN("             @ #            @(      .@@                 @@    .@@                         %(@   %@@,             @*,    @/             *@               .@                    @@@       (@&     \n");
  printGREEN("             @ @            (,.     &@                  /@#    @#                           .@@*&@@              (      @@              @               .@                      @@%(   @@@      \n");
  printGREEN("             / %            ,%.     ,@                   @@    %&                            /@@@@               * .      .             @               .@                        *@  @@*       \n");
  printGREEN("               *                    #                    .@.   %&                               %                % .                    @.              .@                           /          \n");
  printGREEN("                              ,                           &&  ,@                                                 @                      @,               @                                      \n");
  printGREEN("                             .                             &  *(                                                 @                      &                #                                      \n");
  printGREEN("                              (                            &                                                                                             *                                      \n");

  printf("Ma a che costo:\n");
  struct Giocatore* jiji = GiocatorePtr;
  for (int i = 0; i < nGiocatori; i++, jiji++)
  {
    printf("Nome: ");
    ColoreGiocatore (jiji -> colore);
    printf("%s\n"RESET, jiji -> nome_giocatore);
    printf("Stato: ");
    if (jiji -> sanita_mentale == 0)
    {
      printRED ("disperso\n");
    } else printGREEN ("sopravvissuto\n");
  }
  piantone ();
  system ("clear");
  ResettaVariabiliGioco ();
  imposta_gioco ();
}

//funzione che resetta tutte le variabili "static" alle impostazioni di fabbrica
static void ResettaVariabiliGioco (void)
{
  free(GiocatorePtr);
  do
  {
    free(pFirst);
    pFirst = pFirst -> prossima_zona;
  } while(pFirst != pLast);
  free(pLast);
  GiocatorePtr = NULL;
  nGiocatori = 0;
  difficolta = 0;
  carceriere [0] = 0, carceriere [1] = 0, carceriere [2] = 0;
  nStanze = 0;
  pFirst = NULL;
  pLast = NULL;
  InventarioCaravan [0] = 1, InventarioCaravan [1] = 1, InventarioCaravan [2] = 1;
  exodia = 0;
  settato = false;
}

//funzione che printa il logo del gioco
void logo (void)
{
  printf("                                                        :7                                                                                                                         .               \n");
  printf("    .^~~~^.            .        ..^         !       ~G^?&Y                   ..             .....     .^~~~^.            .         .^        ......                    JY:^~       ^^          .:7 \n");
  printf(" .?P#@@@@&#BPY!.   ~P!7&~      YBG&.      .!B^      :@5&@^                 ~G&&G!     .^7Y5GB#&&BBJ:?G#@@@@@#BPJ!.   !P~?&^     .5BB#   :~?YPBB#&&GB!.^~?J5PGBBBY!^    ?@B&#      :JP       :5?75? \n");
  printf(" !G#@@P&Y!~!?5PBG~. ?&@&:      P&&B       75&Y       J@@#.                J@#?!YP?.  :G#@@GYJJ7J@@#?G&@@P&J!~!?5GBP~  J&@&.      G#&G  !B&@&PJJJ75@@BPP&&#YYYJ5B@@B    .&&@Y     :YP@~      .BB&&. \n");
  printf(":7.:&@5B.      7@@? .P&#.      J#@P      !BPG&:      :&@P               .P@P.   !#P.~G&BJ^.     5@@?.^@@5G.      ?@@7 :P@B       5#@Y 7B&G7:    :~#&#::YGJ     7P@J     P@@?     YGP#P       J@@5  \n");
  printf("    5@&5       7@B.  J@#       !#@?     .G@Y5#B      .B@P               P@P      ~B::#@~        !G&!  P@&Y       J@G   Y@G       ?#@7 !@#.      . JG&: 5&J    !5BY      5@@~    !&#JG@J      ~@@!  \n");
  printf("    ^@@P     :5@&^   !@B       .P@!     J&B.~B@J      5@5              ?#@P!:     7: B@:        .YBY  ~@@5     :5@#^   7@G       :G@~ ^@G         ^5&! 5&?   YGY~       ?@@^   .G&J Y#&~     :&#.  \n");
  printf("    .#@Y   .?B@P.    .&#:::~7?JJ&@^    ^#&^  ?B@7     5@Y             .P@@#GP5J?~^~: G#          ?G5  :&@J   .?B@5.    :@B:::~7?JJ@&^ .&5         .J#! !@7.7B@5~.       ?@&.   ?@P  .P&#:    .&5   \n");
  printf("     P@J  ~PGG7    .~?&@@&&&#&##&@G5^  ?@J    5@&~:   P@Y               ^?J5555PB&&&^Y5         .P#J   G@7  ~PGG7    .~J&@@&&&#&##&@G5^#7         ^G&~ ~@BBGG&@@&GY~.   !&#.   G&^   ^B@P^.  .&?   \n");
  printf("   ::5@Y:?5~:.   ~Y#@@@@J^::... P#::^.:#&?YPGPP&@@!   P@Y                       .J#@!7G          B@~ ^:P@J:J5~:.   ~Y#@@@&?^::... GB.^^G?         ^&#. 7@!...:^~P&&@&P^ ^G#   !@G?5PG5B@@B:  ^@!   \n");
  printf("   :^?@?..      .~5@G?&B~       JB^~?P#@B5J?!^^:5@7   ?@Y           .:            J@5~@~        :&?  :^J@7..      :~P@GJ&B^       YG   Y#.        7@~  ?#.       .?@@@7  5&!JG&@GYJ7~^^~B#:   :    \n");
  printf("     ^@!          ?P: PB^      :JBYYJ!G#.       .B&.  5@#.          77            :@B.BB:       P5     ~@^         .?5..GG:       ?G   ^&5.      .#7   ?P         7@@G:^^P&Y?!@Y        !@5 :?JJ~  \n");
  printf("     .&7         ..   ?5^       !5:  .&J         .B5 ~#&@5.        .G~?.         .P@# :G&7     JP      :&~         ..   JY:       !Y    ~##^     PJ    JY        :#@B.   YG  7@^         ~&!PBGBG  \n");
  printf("      BJ              !?7       !5   :G^          ~&5&#G&@&G?^     YB ^Y~      ^YBBJ!   ?&P^  J&:       #7              !?!       75     .5&J. .GG   .~~?       ^B@P:    !5. ?5           5G!GBP^  \n");
  printf("      Y?              :!7       ~P.  ~5            PG::.^!7?JPPJJJP@#. .JPY~~7PBJ^       :?5!Y#^        57              ^!!       !P       ^JY!GG.    ?BJ     :5@B7      ~?  ??           :&~ .    \n");
  printf("      ^J              .:~       .7   :^            ~B         .:~75#&^   ^?JJ7:.           .^^.         ~?              .:~       :!         :^:       ^5GBP5PBP!.       ?7  ^:            5Y      \n");
  printf("       J                .       :7.                 P^             .^^                                  .J                .       :7                     :!7!~.          Y^                ^P      \n");
  printf("       ~:                       .!.                 ^:                                                   ~:                       .!.                                    .                  !      \n");
  printf("       ..                                           ..                                                   ..                                                                                 .      \n");

  printGREEN("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
  printf    ("|L|O| |S|P|E|T|T|R|O| |D|E|L| |F|R|E|E|B|O|O|T|I|N|G|\n");
  printGREEN("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
}

//funzione comprendente il menu per impostare il gioco
void imposta_gioco ()
{
  int sceltah = 0;
  logo ();
  if (settato)
  {
    ControlloImpostazioni (); //se si rientra in imposta_gioco dopo aver gia' impostato
    logo ();                  //il gioco questa fuznione evita di dover reimpostare tutto in caso di errore
    if (settato) return;
  }
  printCYAN("Scegli cosa vuoi impostare\n");
  do
  {
    printf("1) Imposta giocatori\n");
    printf("2) Imposta la difficolta' del gioco\n");
    printf("3) Imposta la mappa di gioco\n");
    printf("4) Visualizza impostazioni\n");
    printf("5) Torna al menu principale\n");
    printf("Scelta: ");
    scanf("%d",&sceltah);
    spazzino ();
    switch (sceltah)
    {
      case 1:
        system("clear");
        if (carceriere [0] == 1)
        {
          free(GiocatorePtr);
          GiocatorePtr = NULL;
          nGiocatori = 0;
        }
        ContaGiocatori ();
        GiocatorePtr = (struct Giocatore*) calloc(nGiocatori, sizeof(struct Giocatore));
        ImpostaGiocatori ();
        ImpostaOggettoIniziale ();
        carceriere [0] = 1;
        printf ("\n");
        break;
      case 2:
        system("clear");
        ImpostaDifficolta ();
        carceriere [1] = 1;
        printf ("\n");
        break;
      case 3:
        system("clear");
        ImpostaMappa ();
        carceriere [2] = 1;
        printf ("\n");
        break;
      case 4:
        system("clear");
        StampaImpostazioni ();
        printf ("\n");
        break;
      case 5:
        system ("clear");
        logo ();
        if (carceriere [0] != 1 || carceriere [1] != 1 || carceriere [2] != 1)
        {
          printRED("ERRORE: PRIMA DI TORNARE AL MENU' PRINCIPALE DEVI FINIRE DI IMPOSTARE IL GIOCO\n");
        } else settato = true;
        break;
      default:
        system("clear");
        logo ();
        printRED("ERRORE: DEVI INSERIRE UN NUMERO TRA 1 E 5\n");
    }
  } while(!settato);
}

//funzione "scheletro" del gioco che richiama le altre funzioni
void gioca ()
{
  strncpy (NomeFantasma, ImpostaFantasma (), 27);
  Lore ();
  int choice;
  int OrdGiocatori [(nGiocatori)];
  struct Giocatore* tmp = GiocatorePtr;
  for (int i = 0; i < nGiocatori; i++, tmp ++) //imposto l'ordine dei giocatori come default
  {
    OrdGiocatori [i] = i;
    tmp -> posizione = pFirst;
  }
  do
  {
    SpostaCelle (OrdGiocatori,nGiocatori); //randomizzo l'ordine dei giocatori ogni macro-turno
    for (int i = 0; i < nGiocatori; i++)
    {
      tmp = GiocatorePtr + OrdGiocatori [i];
      if (tmp -> stato == vivo) //se si e' morti evita di entrare nel menu
      {
        tmp -> movimento = 1;
        do
        {
          if ((tmp -> sanita_mentale - 1) < 100)
          {
            choice = 0;
            StampaCasaGioca ();
            printf("\nE' il turno di ");
            ColoreGiocatore (tmp -> colore);
            printf("%s"RESET"\n", tmp -> nome_giocatore);
            printf("Scegli attentamente cosa fare\n");
            printf("1) Torna al caravan\n");
            printf("2) Visualizza i tuoi dati\n");
            printf("3) Visualizza i dati della stanza\n");
            printf("4) Avanza\n");
            printf("5) Raccogli una prova\n");
            printf("6) Raccogli un oggetto\n");
            printf("7) Usa un oggetto\n");
            printf("8) Passa il turno\n");
            printf("9) SPOILERRR\n");
            printf("Scelta: ");
            scanf("%d", &choice);
            spazzino ();
            printf("\n");
            switch (choice)
            {
              case 1:
                system ("clear");
                choice = 9; //serve per non far apparire il fantasma anche se non fa  nulla
                if (tmp -> posizione -> prova == 0)
                {
                  if (tmp  -> movimento > 0)
                  {
                    torna_caravan (tmp);
                    choice = 1; //serve per far apparire il fantasma, dato che si e' mosso
                  } else printGREEN("Sei troppo stanco per fare tutta questa strada\n");
                } else printf(GREEN"%s e' ancora nella stanza e ti impedisce di tornare al caravan\n"RESET, NomeFantasma);
                break;
              case 2:
                stampa_giocatore (tmp);
                system ("clear");
                break;
              case 3:
                stampa_stanza (tmp -> posizione);
                system ("clear");
                break;
              case 4:
                choice = 9; //serve per non far apparire il fantasma anche se non fa  nulla
                system ("clear");
                if (tmp -> movimento > 0)
                {
                  avanza (tmp);
                  choice = 4; //serve per far apparire il fantasma, dato che si e' mosso
                } else printGREEN("Sei troppo stanco per fare tutta questa strada\n");
                break;
              case 5:
                raccogli_prova (tmp);
                system ("clear");
                break;
              case 6:
                raccogli_oggetto (tmp);
                system ("clear");
                break;
              case 7:
                usa_oggetto (tmp, &choice);
                system ("clear");
                break;
              case 8:
                passa ();
                system ("clear");
                break;
              case 9:
               spoiler (tmp -> posizione);
               system ("clear");
               break;
              default:
                system ("clear");
                printRED("ERRORE: DEVI DIGITARE UN NUMERO VALIDO\n");
                break;
            }
            if (choice == 1 || choice == 4) //si attiva se si e' fatto delle azioni che al fantasma potrebbero non essere piaciute
            {
              if (casuale(100) < 10 * difficolta)
              {
                printf(RED"E' apparso il fantasma e ");
                if (tmp -> immunita > 0)
                {
                  printGREEN("TU HAI ANCORA DEL SALE E COMINCI A LANCIARLO SPAVENTANDO IL FANTASMA\n");
                } else
                {
                printf("-%d sanita mentale\n"RESET, (5 +( 5 * difficolta)));
                tmp -> sanita_mentale -= (5 +( 5 * difficolta));
                }
                piantone ();
                system("clear");
              }
            }
          } else choice = 8;
          if ((tmp -> sanita_mentale - 1) > 100)
          {
            //ci si entra solo la prima volta che si porta la propria sanita_mentale sotto lo 0
            choice = 8;
            ControllaBattito (tmp);
          }
        } while(choice != 8);
        if (tmp -> immunita > 0) //prima di passare il proprio turno se si ha utilizzato del sale tiene conto di quanti turni si ha ancora validi
        {
          tmp -> immunita--;
          if (tmp -> immunita == 0)
          {
            printPURPLE("*Ti cade dalle mani quel poco sale che ti era rimasto e perdi la tua arma segreta\n*");
            piantone ();
            system ("clear");
          }
        }
      }
    }
  }while(nGiocatori > nMorti);
  printRED("                                              @            @@(                                        *                                                                                                 \n");
  printRED("          /@  /@@           @(,@#             (#           &@@*@@@              ,@@@@@@%((@@@&(        /@@@@@@@,        .@@@&@@##@@@@@#                    *@@@@@              *@@##*@&@@@@@#           \n");
  printRED("           @@%@@&          @@@@@#           %#(@            @@@@@&              @@@@@@*@@&@@@@@@@@@,    @@@@@@@@@@@@@@@@@@@@ @      ,&@@*@@@              @@@@@@@/@       (@,@@@@@, (@@@@@@@@           \n");
  printRED("            @@@@/          /@@@@(          &*.@@#           @@#@@             @@*%@@@,@@          #@@@/    @@&@@        @@@@& /@@@@@@&&@@@(@@@@,&       &@@@.    /@.*    .@@@@@@&        @@@@           \n");
  printRED("            @*@@*          #@@@@          #&/@*@@           (@@@@            .    @@@%@&           @@@@,   @@@@&         @@@   @ @@@       .@@@@@@&    @@@#       .@&@ ,&@@@,            #@@@*          \n");
  printRED("             #@@            @(@@         ,/@@.@,@@          *@@@@                 &@@@#.           @@@     &@@%          @@@   /@@@@          (@@@(   @@&#          @    @@@              @.@@          \n");
  printRED("             @@@            % @@         #%@%  @&@&          @@@&                  @@@@,         .@@@/     *@@&      #/  / %    @@%@           @@@   @ @@(           @   @@@              &,@@          \n");
  printRED("             @@@.           %#@#        %@@@   ,@@@@         @@@#                  @@@@        ,@@@@        @@@@@@@@@@@    *     @(@          @@@   .@@@& ,@&        %   @@(               /@@          \n");
  printRED("              @@*    *@@@@@@@@@        /@@&     /.@@@        @@@/                  /@@@      .@&@@#         @@@    @@@@          @ @       ,@@&@     @@@@@@@@@(.@@...*   @@.               *@@          \n");
  printRED("            (@@@@@@@@@@@@@@@@@@@%@     *@@       @%@@/       &@@                    @@@     @,@.@           &@@     @@@@         @@@@#&@@@@(@@          (@  */&@@@@@@@@& &@               &(@@          \n");
  printRED("        ,@@@(@@@@            @@        @@,  ,@@@@@@@@@@&     &@@.                   @@@   @#& /             %@@      @@.         @@@@@@@@@@@@@&/                   @@@@* #@               ,@@@          \n");
  printRED("      @.@@@@@&@#&            @@      %@@@@@@@@@&&%/^@@@       @@,                (%&@@@  #                  @@,      /@          @@@     %@&@@@@#                   #@@@  @(              @@%           \n");
  printRED("         @@  @@@@            /@&# %@@@@@             @@@      @@,                   (@@                     @@(       &    @     &@@         @@@@/                   #@@  @@              %@            \n");
  printRED("       ,/ *   @@           @ *@@@    (@@              @@,     &#.                    @&                     @@/           @/     #&@          @@@.                   ,@@, (@&.           @@             \n");
  printRED("              @(&            #@      @@                @@     &%                     @@                      @,           @*      %&           @@&*.                &@@@/   @@@         @%              \n");
  printRED("              # *            %@      #@                .@@    (#.                    @@                   #@@@@@         *@       &%           .@@ ,@              @@@@@.    /@@&      @@               \n");
  printRED("               ,,.           #@      .*                 (@,   *                      &@                @@@@\\%@@@@@@    *@@,      @             @@   @@         @@@/           &@ @  (@@                \n");
  printRED("               .,,           /,     ((.                  @@   @                      @@                /         .@@@@@@@@@@                    @#     @@@.*&@@@&                 @( @@                 \n");
  printRED("                             *                           /@   @                       %                               .  @@@.                   ,*                                                      \n");
  printRED("                             ..                           @*  @                       (,                                    @@                  *                                                       \n");
  piantone ();
  system ("clear");
  logo ();
  ResettaVariabiliGioco ();
}

//funzione che stampa le regole del gioco
void regole ()
{
  logo ();
  printf("In Phalsophobia siamo degli investigatori del paranormale che si ritrovano a dover raccogliere prove sulle presenze che si aggirano in una casa infestata.\n");
  printf("Lo scopo del gioco e' quindi quello di raccogliere le prove che riusciamo a trovare indagando nella casa e portarle al caravan prima di impazzire.\n");
  printf("Il caravan e' la base degli investigatori dove possono consegnare le prove e raccogliere nuovi strumenti utili per la raccolta di nuove prove\n\n");
  Legenda ();
}

//funzione che stampa informazioni sugli oggetti prendibili al caravan
static void Legenda (void)
{
  printf ("Gli strumenti necessari per la raccolta di prove sono:\n\t");
  printf (PURPLE"EMF reader"RESET": l'EMF reader e' uno strumento che riesce a registrare l'Electro-Magnetic Field (EMF), un campo emesso quando un fantasma interagisce con l'ambiente circostante.\n\t");
  printf (PURPLE"spirit box"RESET": lo spirit box e' un apparecchio all'avanguardia che permette la rilevazione di frequenze che consentono la comunicazione vocale tra umani e fantasmi.\n\t");
  printf (PURPLE"videocamera"RESET": la videocamera DSLR è stata messa a punto per riprendere video paranormali, infatti grazie alla sua speciale lente riesce a registrare i nuclei dei fantasmi.\n");
  printf ("Altri strumenti utili alla propria sopravvivenza sono:\n\t");
  printf (BLUE"sale"RESET": come insegna il grande Arataka Reigen il sale e' la miglior arma contro gli spiriti maligni, infatti se utilizzato in caso di apparizione la propria salute mentale non decrescera'.\n\t");
  printf (BLUE"calmanti"RESET": i calmanti sono essenziali per queste situazioni infatti se utilizzati la propria sanità mentale aumentera' di un po', l'importante e' leggere comunque il foglio illustrativo.\n");
  printf("Questi sono invece alcuni oggetti cari al signor Santoni che potresti trovare nella casa\n\t");
  printf (CYAN"100 dollari"RESET": se \"accidentalmente\" dovesse finire il denaro dimenticato da Santoni nel tuo portafogli, nello zaino potrebbe comparire un oggetto a caso tra calmante e sale.\n\t");
  printf (CYAN"coltello"RESET": un coltello e' un oggetto utilissimo contro un nemico intangibile, soprattutto quando la propria sanita' mentale vacilla e si e' circondati dai propri cari.\n\t");
  printf (CYAN"adrenalina"RESET": per non addormentarsi sopra il pc Santoni utilizza sempre \"Level Upp\", un energy drink che ti dara' l'adrenalina per poter avanzare una volta in piu'.\n");
  piantone ();
  system("clear");
  logo ();
}

static const char* FrasiFatte (void)
{
  switch (casuale (9))
  {
    case 0: return "Ci sono due regole nella vita: 1. Non mollare mai; 2. Non dimenticare mai la regola n.1\n";
    case 1: return "Se qualcosa non ti piace, cambiala. Se non puoi cambiarla, cambia il tuo atteggiamento.\n";
    case 2: return "Prima ti ignorano, poi ti deridono, poi ti combattono. Poi vinci.\n";
    case 3: return "Credi in te stesso quando nessun altro lo fa. Ciò ti rende all’istante un vincitore.\n";
    case 4: return "Cadendo, la goccia scava la pietra, non per la sua forza, ma per la sua costanza.\n";
    case 5: return "Non importa chi tu sia, o da dove tu venga. La capacità di trionfare inizia con te. Sempre.\n";
    case 6: return "Ricordati sempre, non importa quanto vai piano, l'importante è non fermarsi.\n";
    case 7: return "Il successo non è definitivo e l’insuccesso non è fatale. L’unica cosa che conta davvero è il coraggio di continuare.\n";
    case 8: return "Se l’opportunità non bussa, costruisci una porta.";
    default: return "Non dovrei stampare questo, controlla la funzione FrasiFatte";
  }
}

static void LevelUpp (void)
{
  switch (casuale (5))
  {
    case 0: printf("Galaxy ");
    case 1: printf("Atomic ");
    case 2: printf("Masseian ");
    case 3: printf("Shiny ");
    case 4: printf("Bubble ");
    default: printf("Non dovrei stampare questo, controlla la funzione LevelUpp\n");
  }

  switch (casuale (5))
  {
    case 0: printf("Edition\"");
    case 1: printf("Green\"");
    case 2: printf("Tempest\"");
    case 3: printf("Dragon\"");
    case 4: printf("Boom\"");
    default: printf("Non dovrei stampare questo, controlla la funzione LevelUpp\n");
  }
}
static void Lore (void)
{
  printf(PURPLE"Santoni:"RESET" Benvenuti ragazzi nella mia tenuta, per quanto la gente possa non credermi la mia dimora e' stata "GREEN"infestata.\n"RESET);
  printf(PURPLE"Santoni:"RESET" Nel corso della mia illustre carriera ho \"preso ispirazione\" da numerosi prodotti videoludici e sono sicuro che nella mia casa aleggia il fantasma di "PURPLE"%s!\n"RESET, NomeFantasma);
  printf(PURPLE"Santoni:"RESET" Il vostro compito e' quello di raccogliere prove sulla sua esistenza. Gli strumenti per farlo ce li avete, "RED"MI AFFIDO A VOI\n"RESET);

  piantone ();
  system ("clear");
}

static char* ImpostaFantasma (void)
{
  switch (casuale (5))
  {
    case 0: return "Hideo Kojima";
    case 1: return "Sid Meier";
    case 2: return "Hironobu Sakaguchi";
    case 3: return "Gabe Newell";
    case 4: return "Shigeru Miyamoto";
    default: return "Alessandro Bevilacqua";
  }
}

//funzione che chiude il gioco
void termina_gioco (void)
{
  system("clear");
}

static void StampaCasaMenu (void)
{
  struct Zona_mappa* pCasa = pFirst;

  int appStanze = nStanze;

  if (nStanze == 1)
  {
    printf("+-------+\n");
    printf("|  "PURPLE"%s"RESET"  |\n", StampaNomeStanza (pCasa));
    printf("|       |\n");
    printf("|  "CYAN"%s"RESET"  |\n", StampaOggettoStanzaMenu (pCasa));
    printf("+-------+\n");
    return;
  }

  for (int k = 0; k < 2; k++) //deve farlo due volte perche' la casa e' organizzata su 2 livelli
  {
    if (appStanze % 2 == 1) printf("    "); //se le stanze sono dispari le mette piu' centrali
    for (int j = 0; j < (appStanze/2); j++) //stampa tutta la parte superiore della stanza
    {
      if (nStanze % 2 == 0)
      {
        printf("+--");
        if (k == 1 && (j == 0 || j == (appStanze/2 -1)))
          printf ("   ");
        else
          printf ("---");
        printf("--");
      } else
      {
        printf("+");
        if (k == 1 && j == (appStanze/2 - 1))
          printf("   ");
        else
          printf("---");
        if (k == 0)
          printf("-");
        else
          printf("+");
        if (k == 1 && j == 0)
          printf("   ");
        else
          printf("---");
      }
    } printf("+\n");

    for (int l = 0; l < 3; l++) //stampa la parte centrale della stanza
    {

      if (appStanze % 2 == 1) printf("    ");
      pCasa = pFirst;
      for (int i = 0; i < k*nStanze/2; i++)
        pCasa = pCasa -> prossima_zona;
      for (int j = 1; j <= (appStanze/2); j++)
      {
        if (l != 1 || j == 1)
          printf("|  ");
        else
          printf("   ");
        if (l == 0)
          printf(PURPLE"%s"RESET, StampaNomeStanza(pCasa));
        else if (l == 1)
          printf("   ");
        else
          printf(CYAN"%s"RESET, StampaOggettoStanzaMenu(pCasa));
        printf("  ");
        pCasa = pCasa -> prossima_zona;
      } printf("|\n");
    }


    if (k == 1) //stampa la parte inferiore della stanza, e lo fa solo una volta perche' la parte superiore fa da parte inferiore del "primo livello"
    {

      if (appStanze % 2 == 1) printf("    ");
      for (int j = 0; j < (appStanze/2); j++)
      {
        printf("+-------");
      } printf("+\n");
    }

    if (appStanze % 2 == 1) appStanze++;
  }

}

static void ColoreGiocatore (int colore)
{
  switch (colore)
  {
    case 0:
      printf(CYAN);
      break;
    case 1:
      printf(RED);
      break;
    case 2:
      printf(GREEN);
      break;
    case 3:
      printf(BROWN);
      break;
  }
}


void StampaCasaGioca (void)
{
  int localizzatore [nGiocatori][2];
  int contaloc;         //contaloc cresce ogni volta che si entra nel ciclo per stampare le posizioni, a prescindere che vengano stampate o meno
                           //e si resetta ogni volta che si va a capo nella stampa
  int nstampati = 0;    //nstampati incrementa ogni volta che delle persone sono stampate
  int contastampati [nGiocatori];
  //contastampati sposta in fondo all'array i numeri dei giocatori che sono gia' stati stampati, cosi' da lasciare spazio a tutti
  for (int m = 0; m < nGiocatori; m++)
    contastampati [m] = m;

  //azzero la matrice
  for (int i = 0; i < nGiocatori; i++)
  {
    for (int k = 0; k < 2; k++)
    {
      localizzatore [i][k] = 0;
    }
  }
  struct Giocatore* pTemp = GiocatorePtr;

  //la prima colonna ha il numero della stanza in cui si trova ogni giocatore mentre nella seconda ha quanti giocatori si trovano in quella stanza
  int condizione = nGiocatori;
  for (int i = 0; i < condizione; i++, pTemp++)
  {
    localizzatore [i][0] = pTemp -> posizione -> nCivico;
    localizzatore [i][1]++;
    for (int k = 0; k <= i; k++)
    {
      if (i != k && localizzatore [i][0] == localizzatore [k][0])
      {
        localizzatore [k][1]++;
        localizzatore [i][0] = 0;
        localizzatore [i][1] = 0;
        i--;
        condizione--;
      }
    }
  }

  //ordino la matrice dal nCivico piu' piccolo a quello piu' grande
  for (int r = 0; r < nGiocatori - 1; r++)
  {
    int min_idx = r;
    for (int t = r + 1; t < nGiocatori; t++)
        if (localizzatore[t][0] != 0 && localizzatore[t][0] < localizzatore[min_idx][0])
            min_idx = t;
    scambio(&localizzatore[min_idx][0], &localizzatore[r][0]);
    scambio(&localizzatore[min_idx][1], &localizzatore[r][1]);
  }

                                         //stampa tutta la parte superiore della casa
  if (nStanze % 2 == 1) printf("    "); //se le stanze sono dispari le mette piu' centrali
  for (int j = 0; j < (nStanze/2); j++)
  {
    printf("+-------");
  } printf("+\n");

  pTemp = GiocatorePtr;
  for (int l = 0; l < 3; l++) //stampa il contenuto delle stanze superiori
  {
    contaloc = 0; //contaloc si azzera ad ogni ciclo
                //ma se e' uguale a 0 va "alla ricerca" di una stanza non vuota
    while ((localizzatore [contaloc][0] == 0) && (contaloc != nGiocatori-1))
      contaloc++;
    if (nStanze % 2 == 1) printf("    ");
    for (int j = 1; j <= (nStanze/2); j++)
    {
      if (j == localizzatore [contaloc][0]) //entra nell'if se c'e' qualcuno nella stanza
      {
        int m; //m e' il contatore dei for ma viene dichiarata qui per utile anche allo scambio tra giocatori stampati e non
        if (l != 1 || j == 1)
          printf("| ");
        else
          printf("  ");
        if ((l == 0 && localizzatore [contaloc][1] != 1) || (l == 2 && localizzatore[contaloc][1] == 4)) //primo controllo se le condizioni sono giuste per stampare una x
        {
          pTemp = GiocatorePtr + contastampati[0];
          m = 0;
          while (pTemp -> posizione -> nCivico != localizzatore [contaloc][0]) //cerca un giocatore presente nella stanza
          {
            m++;
            pTemp = GiocatorePtr + contastampati[m];
          }
          ColoreGiocatore (pTemp -> colore);
          printf("X "RESET);
          nstampati++;
          scambio (&contastampati[m], &contastampati[nGiocatori-nstampati]); //spedisce il giocatore stampato in fondo all'array
        } else
          printf("  ");
        if (l == 1 && (localizzatore [contaloc][1] == 1 || localizzatore [contaloc][1] == 3)) //secondo controllo se le condizioni sono giuste per stampare una x
        {
          pTemp = GiocatorePtr + contastampati[0];
          m = 0;
          while (pTemp -> posizione -> nCivico != localizzatore [contaloc][0]) //cerca un giocatore presente nella stanza
          {
            m++;
            pTemp = GiocatorePtr + contastampati[m];
          }
          ColoreGiocatore (pTemp -> colore);
          printf("X "RESET);
          nstampati++;
          if (localizzatore [contaloc][1] == 1)
          {
            localizzatore [contaloc][0] = 0;
            localizzatore [contaloc][1] = 0;
          }
          scambio (&contastampati[m], &contastampati[nGiocatori-nstampati]); //spedisce il giocatore stampato in fondo all'array
        } else
          printf("  ");
        if ((l == 2 && localizzatore [contaloc][1] != 1) || (l == 0 && localizzatore[contaloc][1] == 4)) //terzo controllo se le condizioni sono giuste per stampare una x
        {
          pTemp = GiocatorePtr + contastampati[0];
          m = 0;
          while (pTemp -> posizione -> nCivico != localizzatore [contaloc][0]) //cerca un giocatore presente nella stanza
          {
            m++;
            pTemp = GiocatorePtr + contastampati[m];
          }
          ColoreGiocatore (pTemp -> colore);
          printf("X "RESET);
          nstampati++;
          if (l == 2)
          {
            localizzatore [contaloc][0] = 0;
            localizzatore [contaloc][1] = 0;
          }
          scambio (&contastampati[m], &contastampati[nGiocatori-nstampati]); //spedisce il giocatore stampato in fondo all'array
        } else
          printf("  ");
        contaloc++;   //contaloc incrementa in modo da visualizzare la prossima stanza da stampare
      }
      else //entra qui se non c'e' nessuno nella stanza
      {
        if (l != 1 || j == 1)
          printf("|");
        else
          printf(" ");
        printf("       ");
      }
    } printf("|\n");
  }

  for (int j = 0; nStanze % 2 == 0? j < (nStanze/2): j <= (nStanze/2); j++) //stampa la parte inferiore delle stanze superiori (che e' anche la parte superiore delle stanze inferiori)
  {
    if (nStanze % 2 == 0)
    {
      printf("+--");
      if (j == 0 || j == (nStanze/2 -1))
        printf ("   ");
      else
        printf ("---");
      printf("--");
    } else
    {
      printf("+");
      if (j == (nStanze/2))
        printf("   +");
      else
        printf("---+");
      if (j == 0)
        printf("   ");
      else
        printf("---");
    }
  } printf("+\n");

  //riordino il localizzatore dalla stanza piu' grande a quella piu' piccola
  for (int r = 0; r < nGiocatori - 1; r++)
  {
    int max_idx = r;
    for (int t = r + 1; t < nGiocatori; t++)
        if (localizzatore[t][0] != 0 && localizzatore[t][0] > localizzatore[max_idx][0])
            max_idx = t;
    scambio(&localizzatore[max_idx][0], &localizzatore[r][0]);
    scambio(&localizzatore[max_idx][1], &localizzatore[r][1]);
  }

  for (int l = 0; l < 3; l++) //stampa il contenuto delle stanze inferiori
  {
    contaloc = 0; //contaloc si azzera ad ogni ciclo
                //ma se e' uguale a 0 va "alla ricerca" di una stanza non vuota
    while ((localizzatore [contaloc][0] == 0) && (contaloc != nGiocatori-1))
      contaloc++;

    for (int j = nStanze; j > nStanze/2; j--)
    {
      if (j == localizzatore [contaloc][0])
      {
        int m; //m e' il contatore dei for ma viene dichiarata qui per utile anche allo scambio tra giocatori stampati e non
        if (l != 1 || j == nStanze)
          printf("| ");
        else
          printf("  ");
        if ((l == 0 && localizzatore [contaloc][1] != 1) || (l == 2 && localizzatore[contaloc][1] == 4))
        {
          pTemp = GiocatorePtr + contastampati[0]; //cancella da qui
          m = 0;
          while (pTemp -> posizione -> nCivico != localizzatore [contaloc][0]) //cerca un giocatore presente nella stanza
          {
            m++;
            pTemp = GiocatorePtr + contastampati[m];
          }
          ColoreGiocatore (pTemp -> colore);
          printf("X "RESET);
          nstampati++;
          scambio (&contastampati[m], &contastampati[nGiocatori-nstampati]); //spedisce il giocatore stampato in fondo all'array //a qui se non funziona
        } else
          printf("  ");
        if (l == 1 && (localizzatore [contaloc][1] == 1 || localizzatore [contaloc][1] == 3))
        {
          pTemp = GiocatorePtr + contastampati[0]; //cancella da qui
          m = 0;
          while (pTemp -> posizione -> nCivico != localizzatore [contaloc][0]) //cerca un giocatore presente nella stanza
          {
            m++;
            pTemp = GiocatorePtr + contastampati[m];
          }
          ColoreGiocatore (pTemp -> colore);
          printf("X "RESET);
          nstampati++;
          scambio (&contastampati[m], &contastampati[nGiocatori-nstampati]); //spedisce il giocatore stampato in fondo all'array //a qui se non funziona
          if (localizzatore [contaloc][1] == 1)
          {
            localizzatore [contaloc][1] = 0;
            localizzatore [contaloc][0] = 0;
          }
        } else
          printf("  ");
        if ((l == 2 && localizzatore [contaloc][1] != 1) || (l == 0 && localizzatore[contaloc][1] == 4))
        {
          pTemp = GiocatorePtr + contastampati[0]; //cancella da qui
          m = 0;
          while (pTemp -> posizione -> nCivico != localizzatore [contaloc][0]) //cerca un giocatore presente nella stanza
          {
            m++;
            pTemp = GiocatorePtr + contastampati[m];
          }
          ColoreGiocatore (pTemp -> colore);
          printf("X "RESET);
          nstampati++;
          scambio (&contastampati[m], &contastampati[nGiocatori-nstampati]); //spedisce il giocatore stampato in fondo all'array //a qui se non funziona
          if (l == 2)
          {
            localizzatore [contaloc][1] = 0;
            localizzatore [contaloc][0] = 0;
          }
        } else
          printf("  ");
        contaloc++;
      }
      else
      {
        if (l != 1 || j == nStanze)
          printf("|   ");
        else
          printf("    ");
        printf("    ");
      }
    } printf("|\n");
  }

  for (int j = 0; nStanze % 2 == 0? j < (nStanze/2): j <= (nStanze/2); j++) //stampa la parte inferiore delle stanze inferiori
  {
    printf("+-------");
  } printf("+\n");
}
