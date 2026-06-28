#include "CSVHandler.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>

void CSVHandler::exportSimulationResults(
    const std::vector<Customer>& customers,
    const std::string& fileName
) {
    std::ofstream file(fileName);

    file << "Customer No,"
         << "Inter Arrival Time,"
         << "Arrival Time,"
         << "Service Time,"
         << "Service Start Time,"
         << "Waiting Time,"
         << "Departure Time,"
         << "Time In System,"
         << "Server Idle Time,"
         << "Queue Length\n";

    file << std::fixed << std::setprecision(4);

    for (const auto& customer : customers) {
        file << customer.customerNo << ","
             << customer.interArrivalTime << ","
             << customer.arrivalTime << ","
             << customer.serviceTime << ","
             << customer.serviceStartTime << ","
             << customer.waitingTime << ","
             << customer.departureTime << ","
             << customer.timeInSystem << ","
             << customer.serverIdleTime << ","
             << customer.queueLength << "\n";
    }

    file.close();
}

void CSVHandler::exportQueueStatistics(
    const QueueStatistics& stats,
    const std::string& fileName
) {
    std::ofstream file(fileName);

    file << "Statistic,Value\n";
    file << std::fixed << std::setprecision(4);

    file << "Total Customers," << stats.totalCustomers << "\n";
    file << "Customers Who Waited," << stats.customersWhoWaited << "\n";
    file << "Maximum Queue Length," << stats.maxQueueLength << "\n";
    file << "Total Simulation Time," << stats.totalSimulationTime << "\n";
    file << "Total Inter Arrival Time," << stats.totalInterArrivalTime << "\n";
    file << "Total Service Time," << stats.totalServiceTime << "\n";
    file << "Total Waiting Time," << stats.totalWaitingTime << "\n";
    file << "Total Time In System," << stats.totalTimeInSystem << "\n";
    file << "Total Idle Time," << stats.totalIdleTime << "\n";
    file << "Average Inter Arrival Time," << stats.averageInterArrivalTime << "\n";
    file << "Average Service Time," << stats.averageServiceTime << "\n";
    file << "Average Waiting Time," << stats.averageWaitingTime << "\n";
    file << "Average Time In System," << stats.averageTimeInSystem << "\n";
    file << "Probability Of Waiting," << stats.probabilityOfWaiting << "\n";
    file << "Server Utilization," << stats.serverUtilization << "\n";
    file << "Server Idle Probability," << stats.serverIdleProbability << "\n";
    file << "Average Number In Queue," << stats.averageNumberInQueue << "\n";
    file << "Average Number In System," << stats.averageNumberInSystem << "\n";
    file << "Arrival Rate," << stats.arrivalRate << "\n";
    file << "Service Rate," << stats.serviceRate << "\n";

    file.close();
}

bool CSVHandler::importInputData(
    const std::string& fileName,
    std::vector<double>& interArrivalTimes,
    std::vector<double>& serviceTimes
) {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        return false;
    }

    std::string line;
    getline(file, line);

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string iatValue;
        std::string serviceValue;

        getline(ss, iatValue, ',');
        getline(ss, serviceValue, ',');

        if (!iatValue.empty() && !serviceValue.empty()) {
            interArrivalTimes.push_back(stod(iatValue));
            serviceTimes.push_back(stod(serviceValue));
        }
    }

    file.close();
    return true;
}