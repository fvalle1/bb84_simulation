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

#include <curl/curl.h>

using std::string;

class TelegramWatch : public StopWatch {
public:
    TelegramWatch();
    TelegramWatch(const char* name);
    ~TelegramWatch();
private:
    void callIFTTT() const;
    static const char* fURL;
    const char* fName;
};


#endif //NEURALANALYZER_TELEGRAMWATCH_H
