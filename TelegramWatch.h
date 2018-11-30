//
// Created by Filippo Valle on 04/11/2018.
//

#ifndef NEURALANALYZER_TELEGRAMWATCH_H
#define NEURALANALYZER_TELEGRAMWATCH_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "StopWatch.h"
#include <string>
#include <algorithm>
#include <iostream>
#endif

#include <curl/curl.h>                        // strumento per interagire con il web

using std::string;

class TelegramWatch : public StopWatch {      // classe che eredita da StopWatch
public:
    TelegramWatch();
    TelegramWatch(const char* name);
    ~TelegramWatch();
private:
    void callIFTTT() const;                   // IFTTT = If This Then That. Servizio per mettere in comunicazione + dispositivi. Es imposto di chiamare il mio cellulare.
    static const char* fURL;               
    const char* fName;
};


#endif //NEURALANALYZER_TELEGRAMWATCH_H
