#include "gamelib.h"

// Questo file continene solo la definizione della funzione main
// con il menu principale 1-2-3

int main( )
{
  srand(time(NULL));
  system("clear");
  unsigned short scelta = 0;
  int appoggio = 0;

  printGREEN("Per evitare bug grafici si consiglia di mettere il terminale a schermo intero\n");
  printf(PURPLE"Ogni riferimento a persone o fatti realmente esistenti e' PURAMENTE CASUALE\n"RESET);
  do
  {
    printBLUE("Premi invio per continuare\n");
  } while( getchar() != '\n' );
  system("clear");
  logo ();
  do
  {
    printf("\n");
    if (settato == false)
      printf("Per prima cosa seleziona "CYAN"Imposta gioco"RESET" per impostare i giocatori e selezionare la difficolta', dopo che l'avrai fatto sarai pronto a \""BROWN"defecare nel tuo intimo"RESET"\"\n");
    printf("1) Imposta gioco\n");
    printf("2) Gioca\n");
    printf("3) Regole\n");
    printf("4) Esci dal gioco? :(\n");
    printf("Scelta: ");
    scanf ("%hu", &scelta);
    //Lo spazzino del main
    do
    {
      appoggio = getchar();
    } while ( appoggio != '\n' && appoggio != EOF);
    switch (scelta)
    {
      case 1:
        system("clear");
        imposta_gioco();
        settato = true;
        break;

      case 2:
        system("clear");
        if (settato == true)
        {
          gioca();
        } else
        {
          logo ();
          printRED("ERRORE: PER GIOCARE DEVI PRIMA IMPOSTARE IL GIOCO");
        }
        break;

      case 3:
        system("clear");
        regole();
        break;

      case 4:
        system("clear");
        termina_gioco();
        break;

      default:
        system("clear");
        logo ();
        printRED("ERRORE: IL CARATTERE CHE HAI DIGITATO NON E' VALIDO.");
    }
  } while( scelta!=4 );

  return 0;
}
