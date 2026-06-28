#ifndef CUSTOMER_H
#define CUSTOMER_H

struct Customer {
    int customerNo;
    double randomIAT;
    double interArrivalTime;
    double arrivalTime;
    double randomService;
    double serviceTime;
    double serviceStartTime;
    double waitingTime;
    double departureTime;
    double timeInSystem;
    double serverIdleTime;
    int queueLength;
};

#endif