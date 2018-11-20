#include "Channel.h"

Channel::Channel(bool isNoisy) : fIsNoisy(isNoisy){
    if(Qbit::DEBUG) printf("\nCreating Channel..\n");

}

Channel::~Channel() {
    if(Qbit::DEBUG) printf("\nDestrying Channel..\n");
}


void Channel::PassQbit(Qbit* qbit) {
    if(Qbit::DEBUG) {
        printf("\nPassing qbit through channel..\n");
        std::cout<<*qbit<<std::endl;
    }

    if(fIsNoisy){
        qbit->AddNoise();
    }

    if(Qbit::DEBUG) {
        printf("\n..passed qbit through channel\n");
        std::cout<<*qbit<<std::endl;
    }
}