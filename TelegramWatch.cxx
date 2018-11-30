//
// Created by Filippo Valle on 04/11/2018.
//

#include "TelegramWatch.h"

const char* TelegramWatch::fURL = "https://maker.ifttt.com/trigger/cpp/with/key/95ZnJdPvDZmBx8yfEy6WL";   //puntatore ad una stringa di caratteri costante

TelegramWatch::TelegramWatch(): StopWatch(),
                                fName("")
{
  
}

TelegramWatch::TelegramWatch(const char *name) : StopWatch(),
                                                 fName(name) {
  
						 }

TelegramWatch::~TelegramWatch(){             // quando distruggo TelegramWatch faccio la chiamata
  callIFTTT();
}

void TelegramWatch::callIFTTT() const {      
  CURL *curl;
  CURLcode res;
  
  curl_global_init(CURL_GLOBAL_DEFAULT);   // devo inizializzare la libreria curl prima di poter usare qualsiasi sua funzione
  
  curl = curl_easy_init();                 // inizio una sessione di libcurl
  if(curl) {                               // se curl (che è un ptr) punta a qualcosa , allora:
    char encWall[25];                      // encoded Wall: conterrà il valore tempo REALE, però scritto in una stringa
    char encCPU[20];                       // encoded CPU: tempo della CPU scritto in caratteri
    char url[200];
    
    char* encName = curl_easy_escape(curl, fName, 25);            // This function converts the given input string to a URL encoded string and returns that as a new allocated string.
    sprintf(encWall, "%4.2fsec[Wall_time]", this->GetWall());     // Scrivo in caratteri il tempo reale (troncato alla seconda cifra decimale)
    sprintf(encCPU, "%4.2fsec[CPU_time]", this->GetCPU());
    sprintf(url, "%s?value1=%s&value2=%s&value3=%s", fURL, encName, curl_easy_escape(curl, encWall, 19), curl_easy_escape(curl, encCPU, 18));
                                                                  // scrivo: URL + nome + tempo REALE (in formato url) + tempo CPU (in formato URL) 
    // printf("%s\n", url);
    curl_easy_setopt(curl, CURLOPT_URL, url);                     // Pass in a pointer to the URL to work with. The parameter should be a char * to a zero terminated string which must be URL-encoded
    
#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif
    
#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);            // Invoke this function after curl_easy_init and all the curl_easy_setopt calls are made, and will perform the transfer as described in                                                                   // the options
#endif
    
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  
  curl_global_cleanup();
}

