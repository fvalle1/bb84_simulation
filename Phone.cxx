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
    delete fQbitA;
}

void Phone::InitResults(CommunicationInfos &data){          // init data
    data.Ntot = 0;
    data.SameBasisUntouched = 0;
    data.SameBasisAltered = 0;
}


//SetNewQbit
void Phone::SetNewQbit(Qbit *qbitA)
{
    //saving qbitA generated by Alice
    fQbitA->~Qbit();
    new(fQbitA) Qbit(*qbitA);     // vado a prendere il valore puntato dal qbit di alice e lo salvo sul datamember fQbitA
}

void Phone::MakeCallClassicalChannel(Qbit *qbit, CommunicationInfos &data) {
    //saving qbitB generated by Bob
    //checking if qbitA == qbitB, if baseA == baseB ecc...
    if(fQbitA) {
        data.Ntot++;                                   // aggiorno contatore N sent
        if(fQbitA->GetBase() == qbit->GetBase()) {     // controllo che quello di Bob sia nella stessa base di quello di Alice
            if (*fQbitA == *qbit) {
                data.SameBasisUntouched++;  //sono identici untouched
            } else {
                data.SameBasisAltered++;    //sono diversi altered
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
        std::cout << "No intercetp: " << data.SameBasisUntouched << std::endl;
        std::cout << "Intercept: " << data.SameBasisAltered << std::endl;
    }
}
