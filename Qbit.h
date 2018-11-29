//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_QBIT_H
#define BB84_SIMULATION_QBIT_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TMath.h>
#include <TRandom3.h>
#include <Riostream.h>
#endif

using std::ostream;

typedef enum basis{              // definisco il tipo "basis", cheè un enumeration. Cioè qualcosa che può assumere solo i valori Plusminuse ZeroOne
  PlusMinus,
  ZeroOne
} basis;

typedef bool polarization;       // definisco il tipo "polarization", che è un booleano. Sarà 0 o 1

class Qbit {                     // non la faccio ereditare da TObject?
 public:
  Qbit(bool islogic = false);             // costruttore
  Qbit(const Qbit &qbit);                 // copy constructor: mi prende un qbit (che non modifico) e ne restituisce un altro.
  Qbit &operator=(const Qbit &source);    // overload dell'operatore =. Mi prende l'indirizzo di un oggetto Qbit, e mi restituisce un oggetto Qbit
  virtual ~Qbit();                        // distruttore
  
  inline basis GetBase() const {return fBase;}                   // funzione per sapere in che base è il Qbit. la funzione è implementata direttamente
  inline polarization GetState() const {return fState;}          // funzione per vedere in che stato di polarizzazione si trova il Qbit
  inline int GetNPhysicalQbits() const {return fPhysicsQbits;}   // funzione per vedere il numero di Qbit fisici che compongono il Qbit logico
  
  inline void SetBase(basis b) {fBase = b;}                      // funzione per settare la base del qbit
  
  void SetState(polarization pol) {Qbit::fState = pol;}          // funzione per settare lo stato.  ??? perchè qua uso Qbit::fState e per la base non uso Qbit::fBase ??? perché non ho usato "inline" ???
  void PrepareState(basis b, polarization pol);                  // preparo il Qbit --> servirà ad Alice
  void MeasureState(basis b);                                    // Misuro lo stato
  
  void AddNoise();
  
  bool operator==(const Qbit &qitCompared) const;         // overload dell'operatore ==. La funzione non modifica l'argomento (che è l'indirizzo ad un oggetto Qbit)
  bool operator!=(const Qbit &) const;                    // overload dell'operatore !=, prende un indirizzo di un oggetoo Qbit (che non può modificare) e mi resituisce un bool. 
  friend ostream& operator<<(ostream& os, Qbit q);        // ??? non ho ben capito: l'operatore << della classe ostream ha accesso a tutti i data member privati della classe Qbit anche se non è una 
                                                          // funzione della classe...  cioè: ho capito cosa vuol dire friend, ma non ho capito cosa è friend di chi. ???
  static const bool DEBUG = false;                        
  
 private:
  double* fTheta;                                         // angolo di polarizzazione, è un ptr. Man mano che il qubit viene misurato l'angolo cambia ==> ptr è sempre lo stesso, il valore puntato cambia
  basis fBase;                                            // base con cui è stato misurato (PlusMinus o ZeroOne)
  polarization fState;                                    // stato di polarizzazione: 0 o 1. ci sono 4 stati possibili di polarizzazione, sono def. da (base, polarizzazione)             
  bool fIsLogic;                                          // se è un qbit logico
  int fPhysicsQbits;                                      // numero di qbit fisici che compongono il qbit logico
  static constexpr double fSindromeEpsilon = 0.0001;      // valore piccolo, mi servirà per vedere se due qbit fisici sono uguali (a meno di questo epsilon) --> static, mantiene lo stesso valore x tutti                                                          // gli oggetti qbit che creerò, inoltre è una constexpr, cioè è una variabile cost e in più deve essere inizializzata in fase di compilazione.
  void PrepareTheta();                                    // funzione per preparare il qbit iniziale, quello di Alice  ???
  void MeasurePhisicalqbit(int q);                        // funzione per fare la misura del qbit fisico
};


#endif //BB84_SIMULATION_QBIT_H
