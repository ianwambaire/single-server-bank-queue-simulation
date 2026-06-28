#ifndef CSV_HANDLER_H
#define CSV_HANDLER_H

#include <vector>
#include <string>
#include "Customer.h"
#include "QueueStatistics.h"

class CSVHandler {
public:
    static void exportSimulationResults(
        const std::vector<Customer>& customers,
        const std::string& fileName
    );

    static void exportQueueStatistics(
        const QueueStatistics& stats,
        const std::string& fileName
    );

    static bool importInputData(
        const std::string& fileName,
        std::vector<double>& interArrivalTimes,
        std::vector<double>& serviceTimes
    );
};

#endif