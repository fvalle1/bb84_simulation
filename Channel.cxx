#include "Channel.h"

ClassImp(Channel)

Channel::Channel(bool isNoisy) : fIsNoisy(isNoisy){
    if(Qbit::DEBUG) printf("\nCreating Channel\n");

}

Channel::~Channel() {
    if(Qbit::DEBUG) printf("\nDestrying Channel\n");
}


void Channel::PassQbit(Qbit* qbit) {
    if(Qbit::DEBUG) printf("\nPassing qbit through channel\n");

    if(fIsNoisy){
        qbit->AddNoise();
    }
}