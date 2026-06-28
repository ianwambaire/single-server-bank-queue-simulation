#ifndef QUEUE_STATISTICS_H
#define QUEUE_STATISTICS_H

struct QueueStatistics {
    int totalCustomers;
    int customersWhoWaited;
    int maxQueueLength;

    double totalSimulationTime;
    double totalInterArrivalTime;
    double totalServiceTime;
    double totalWaitingTime;
    double totalTimeInSystem;
    double totalIdleTime;

    double averageInterArrivalTime;
    double averageServiceTime;
    double averageWaitingTime;
    double averageTimeInSystem;

    double probabilityOfWaiting;
    double serverUtilization;
    double serverIdleProbability;
    double averageNumberInQueue;
    double averageNumberInSystem;
    double arrivalRate;
    double serviceRate;
};

#endif