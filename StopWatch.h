//
// Created by Filippo Valle on 03/10/2018.
//

#ifndef STOPWATCH_H
#define STOPWATCH_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
#include <chrono>
#endif

#if defined(_OPENMP)   // per fare calcolo parallelo         ---> PERCHÉ FACCIO TUTTO SIA CON <chrono>, SIA CON <omp.h> ???
#include <omp.h>       // Open MUltiprocessing
#endif

class StopWatch {
 public:                                                     // quando creo lo StopWatch faccio partire tutti i tempi, quando distruggo lo StopWath, fermo tutti i tempi e li stampo 
  inline StopWatch(){                                        // implemento tutto qua perché tanto chiamerò l'oggetto StopWatch solo una volta, quindi conviene implementarlo subito, credo
    fSystemTime = std::chrono::system_clock::now();          // tempo REALE
    fCPUTime = std::clock();                                 // tempo del clock della CPU 
#if defined(_OPENMP)
    fomptime = omp_get_wtime();                              //anche questo misura i tempi, nel caso stia usando calcolo parallelo (quindi nel caso abbia chiamato omp
#endif
  };
  inline ~StopWatch(){
    std::cout<<"\nRunned in: "<<GetWall()<<" sec [Wall time]"<<std::endl;
    std::cout<<"Runned in: "<<GetCPU()<<" sec [CPU time]"<<std::endl;
#if defined(_OPENMP)
    std::cout<<"Runned in: "<<GetOMP()<<" sec [OMP time]"<<std::endl;
#endif
  }
  
 protected:
  inline double GetWall() const {return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - fSystemTime).count()/1000.;};
                                                                                                // tempo finale reale (che converto in millisecondi) - tempo iniziale reale/1000 (sono microsecondi)
  inline double GetCPU() const {return (std::clock() - fCPUTime) / (double)CLOCKS_PER_SEC;}     // tempo finale CPU - tempo iniziale CPU : questo mi dà i clock della CPU, poi trasformo in secondi
#if defined(_OPENMP)
  inline double GetOMP() const {return (omp_get_wtime() - fomptime);}                          
#endif
  
 private:
  std::chrono::time_point<std::chrono::system_clock> fSystemTime;    // variabile del tipo system_clock
  std::clock_t fCPUTime;                                             // variabile del tipo clock_t
#if defined(_OPENMP)       
  double fomptime;
#endif
};


#endif //STOPWATCH_H
