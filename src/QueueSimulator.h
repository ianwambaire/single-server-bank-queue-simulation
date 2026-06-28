#ifndef QUEUE_SIMULATOR_H
#define QUEUE_SIMULATOR_H

#include <vector>
#include "Customer.h"
#include "QueueStatistics.h"

class QueueSimulator {
private:
    int numberOfCustomers;
    double minIAT;
    double maxIAT;
    double minService;
    double maxService;
    unsigned int seed;

public:
    QueueSimulator(
        int numberOfCustomers,
        double minIAT,
        double maxIAT,
        double minService,
        double maxService,
        unsigned int seed
    );

    std::vector<Customer> generateRandomSimulation();
    std::vector<Customer> runSimulationFromInput(
        const std::vector<double>& interArrivalTimes,
        const std::vector<double>& serviceTimes
    );

    QueueStatistics calculateStatistics(const std::vector<Customer>& customers);
};

#endif