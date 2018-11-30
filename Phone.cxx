#include "Phone.h"

Phone::Phone() {
  if(Qbit::DEBUG) printf("\nCreating Phone by default\n");
  fQbitA = new Qbit();
}

Phone::Phone(const Phone& phone){                      // copy constructor
  if(Qbit::DEBUG) printf("\nCopying Phone\n");
  
  if(phone.fQbitA){
    fQbitA = new Qbit(*phone.fQbitA);
  }else{
    fQbitA = new Qbit();
  }
  
}

//destructor
Phone::~Phone()
{
  if(Qbit::DEBUG) printf("\nDestroying Phone\n");
  delete fQbitA;                                       // ??? qua usando delete il compilatore sa già che deve usare il distruttore definito nella classe qbit?
}

void Phone::InitResults(fStructToSave &data){          // ??? ma 'data' dove l'ho definito? 
  data.Ntot = 0;
  data.SameBasisNoIntercept = 0;
  data.SameBasisIntercept = 0;
}


//SetNewQbit
void Phone::SetNewQbit(Qbit *qbitA)
{
  //saving qbitA generated by Alice
  fQbitA->~Qbit();
  new(fQbitA) Qbit(*qbitA);                           // vado a prendere il valore puntato dal qbit di alice e lo salvo sul datamember fQbitA
}

bool Phone::MakeCallClassicalChannel(Qbit *qbit, fStructToSave &data) {
  bool wasSameBasis = false;
  //saving qbitB generated by Bob
  //checking if qbitA == qbitB, if baseA == baseB ecc...
  if(fQbitA) {
    data.Ntot++;                                   // controllo che Alice abbia inviato un qbit
    if(fQbitA->GetBase() == qbit->GetBase()) {     // controllo che quello di Bob sia nella stessa base di quello di Alice
      wasSameBasis = true;
      if (*fQbitA == *qbit) {                      // ??? in che modo qua riesco a apire se sono anche nello stesso stato? --> perchè sto usando == che mi dà vero solo se sia base che stato sono uguali
	data.SameBasisNoIntercept++;
      } else {
	data.SameBasisIntercept++;
      }
    }
  }else{
    std::cerr<<"null pointer on qbit inside phone!"<<std::endl;
  }
  if(Qbit::DEBUG) {
    printf("\n\n**********\nMaking phone call..\n");
    std::cout << "qbitA: " << *fQbitA << std::endl;
    std::cout << "qbitB: " << *qbit << std::endl;
    std::cout << "N: " << data.Ntot << std::endl;
    std::cout << "No intercetp: " << data.SameBasisNoIntercept << std::endl;
    std::cout << "Intercept: " << data.SameBasisIntercept << std::endl;
  }
  
  return wasSameBasis;
}
