//
// Created by Filippo Valle on 03/10/2018.
//

#ifndef STOPWATCH_H
#define STOPWATCH_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
#include <chrono>
#endif

#if defined(_OPENMP)
#include <omp.h>
#endif

class StopWatch {
public:
    inline StopWatch(){
        fSystemTime = std::chrono::system_clock::now();
        fCPUTime = std::clock();
#if defined(_OPENMP)
        fomptime = omp_get_wtime();
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
    inline double GetCPU() const {return (std::clock() - fCPUTime) / (double)CLOCKS_PER_SEC;}
#if defined(_OPENMP)
    inline double GetOMP() const {return (omp_get_wtime() - fomptime);}
#endif

private:
    std::chrono::time_point<std::chrono::system_clock> fSystemTime;
    std::clock_t fCPUTime;
#if defined(_OPENMP)
    double fomptime;
#endif
};


#endif //STOPWATCH_H