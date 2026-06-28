#include "QueueSimulator.h"
#include <random>
#include <algorithm>

QueueSimulator::QueueSimulator(
    int numberOfCustomers,
    double minIAT,
    double maxIAT,
    double minService,
    double maxService,
    unsigned int seed
) {
    this->numberOfCustomers = numberOfCustomers;
    this->minIAT = minIAT;
    this->maxIAT = maxIAT;
    this->minService = minService;
    this->maxService = maxService;
    this->seed = seed;
}

std::vector<Customer> QueueSimulator::generateRandomSimulation() {
    std::vector<double> interArrivalTimes;
    std::vector<double> serviceTimes;

    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> randomIAT(0.0, 1.0);
    std::uniform_real_distribution<double> randomService(0.0, 1.0);

    for (int i = 0; i < numberOfCustomers; i++) {
        double r1 = randomIAT(generator);
        double r2 = randomService(generator);

        double iat = minIAT + r1 * (maxIAT - minIAT);
        double service = minService + r2 * (maxService - minService);

        interArrivalTimes.push_back(iat);
        serviceTimes.push_back(service);
    }

    return runSimulationFromInput(interArrivalTimes, serviceTimes);
}

std::vector<Customer> QueueSimulator::runSimulationFromInput(
    const std::vector<double>& interArrivalTimes,
    const std::vector<double>& serviceTimes
) {
    std::vector<Customer> customers;

    double previousArrivalTime = 0.0;
    double previousDepartureTime = 0.0;

    for (int i = 0; i < numberOfCustomers; i++) {
        Customer customer;

        customer.customerNo = i + 1;
        customer.randomIAT = 0.0;
        customer.randomService = 0.0;
        customer.interArrivalTime = interArrivalTimes[i];
        customer.serviceTime = serviceTimes[i];

        customer.arrivalTime = previousArrivalTime + customer.interArrivalTime;

        if (customer.arrivalTime > previousDepartureTime) {
            customer.serviceStartTime = customer.arrivalTime;
            customer.serverIdleTime = customer.arrivalTime - previousDepartureTime;
        } else {
            customer.serviceStartTime = previousDepartureTime;
            customer.serverIdleTime = 0.0;
        }

        customer.waitingTime = customer.serviceStartTime - customer.arrivalTime;
        customer.departureTime = customer.serviceStartTime + customer.serviceTime;
        customer.timeInSystem = customer.departureTime - customer.arrivalTime;

        int queueCount = 0;

        for (const auto& previousCustomer : customers) {
            if (
                previousCustomer.arrivalTime <= customer.arrivalTime &&
                previousCustomer.serviceStartTime > customer.arrivalTime
            ) {
                queueCount++;
            }
        }

        customer.queueLength = queueCount;

        customers.push_back(customer);

        previousArrivalTime = customer.arrivalTime;
        previousDepartureTime = customer.departureTime;
    }

    return customers;
}

QueueStatistics QueueSimulator::calculateStatistics(const std::vector<Customer>& customers) {
    QueueStatistics stats{};

    stats.totalCustomers = customers.size();
    stats.customersWhoWaited = 0;
    stats.maxQueueLength = 0;

    stats.totalInterArrivalTime = 0.0;
    stats.totalServiceTime = 0.0;
    stats.totalWaitingTime = 0.0;
    stats.totalTimeInSystem = 0.0;
    stats.totalIdleTime = 0.0;

    for (const auto& customer : customers) {
        stats.totalInterArrivalTime += customer.interArrivalTime;
        stats.totalServiceTime += customer.serviceTime;
        stats.totalWaitingTime += customer.waitingTime;
        stats.totalTimeInSystem += customer.timeInSystem;
        stats.totalIdleTime += customer.serverIdleTime;

        if (customer.waitingTime > 0) {
            stats.customersWhoWaited++;
        }

        if (customer.queueLength > stats.maxQueueLength) {
            stats.maxQueueLength = customer.queueLength;
        }
    }

    stats.totalSimulationTime = customers.back().departureTime;

    stats.averageInterArrivalTime = stats.totalInterArrivalTime / stats.totalCustomers;
    stats.averageServiceTime = stats.totalServiceTime / stats.totalCustomers;
    stats.averageWaitingTime = stats.totalWaitingTime / stats.totalCustomers;
    stats.averageTimeInSystem = stats.totalTimeInSystem / stats.totalCustomers;

    stats.probabilityOfWaiting =
        static_cast<double>(stats.customersWhoWaited) / stats.totalCustomers;

    stats.serverUtilization =
        stats.totalServiceTime / stats.totalSimulationTime;

    stats.serverIdleProbability =
        stats.totalIdleTime / stats.totalSimulationTime;

    stats.averageNumberInQueue =
        stats.totalWaitingTime / stats.totalSimulationTime;

    stats.averageNumberInSystem =
        stats.totalTimeInSystem / stats.totalSimulationTime;

    stats.arrivalRate =
        stats.totalCustomers / stats.totalSimulationTime;

    stats.serviceRate =
        stats.totalCustomers / stats.totalServiceTime;

    return stats;
}