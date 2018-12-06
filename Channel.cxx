#include "Channel.h"

Channel::Channel() : fPdf(nullptr){
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

    if(fPdf){
        qbit->AddNoise(fPdf);
    }

    if(Qbit::DEBUG) {
        printf("\n..passed qbit through channel\n");
        std::cout<<*qbit<<std::endl;
    }
}