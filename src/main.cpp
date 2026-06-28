#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include <random>
#include "Customer.h"
#include "QueueSimulator.h"
#include "QueueStatistics.h"
#include "CSVHandler.h"

using namespace std;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pauseScreen() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int getIntegerInput(string prompt, int minimumValue) {
    int value;

    while (true) {
        cout << prompt;
        cin >> value;

        if (!cin.fail() && value >= minimumValue) {
            return value;
        }

        cout << "Invalid input. Please enter a number greater than or equal to "
             << minimumValue << ".\n";
        clearInput();
    }
}

double getDoubleInput(string prompt, double minimumValue) {
    double value;

    while (true) {
        cout << prompt;
        cin >> value;

        if (!cin.fail() && value >= minimumValue) {
            return value;
        }

        cout << "Invalid input. Please enter a value greater than or equal to "
             << minimumValue << ".\n";
        clearInput();
    }
}

void displayHeader(string title) {
    cout << "\n============================================================\n";
    cout << title << "\n";
    cout << "============================================================\n";
}

void displayInputScreen(
    int numberOfCustomers,
    double minIAT,
    double maxIAT,
    double minService,
    double maxService,
    unsigned int seed
) {
    displayHeader("INPUT SCREEN");

    cout << "Simulation Type                 : Single-server bank queue\n";
    cout << "Number of customers             : " << numberOfCustomers << "\n";
    cout << "Inter-arrival distribution      : Uniform("
         << minIAT << ", " << maxIAT << ") minutes\n";
    cout << "Service time distribution       : Uniform("
         << minService << ", " << maxService << ") minutes\n";
    cout << "Random seed                     : " << seed << "\n";
    cout << "Server                          : One bank teller\n";
    cout << "Queue discipline                : First Come First Served\n";
}

void displaySimulationTable(const vector<Customer>& customers) {
    displayHeader("OUTPUT SCREEN 1: SIMULATION RESULTS TABLE");

    cout << left
         << setw(6)  << "No"
         << setw(10) << "IAT"
         << setw(10) << "Arrive"
         << setw(10) << "Service"
         << setw(12) << "Start"
         << setw(10) << "Wait"
         << setw(12) << "Depart"
         << setw(12) << "System"
         << setw(10) << "Idle"
         << setw(8)  << "Queue"
         << "\n";

    cout << "----------------------------------------------------------------------------------------------------\n";
    cout << fixed << setprecision(2);

    for (const auto& customer : customers) {
        cout << left
             << setw(6)  << customer.customerNo
             << setw(10) << customer.interArrivalTime
             << setw(10) << customer.arrivalTime
             << setw(10) << customer.serviceTime
             << setw(12) << customer.serviceStartTime
             << setw(10) << customer.waitingTime
             << setw(12) << customer.departureTime
             << setw(12) << customer.timeInSystem
             << setw(10) << customer.serverIdleTime
             << setw(8)  << customer.queueLength
             << "\n";
    }
}

void displayStatistics(const QueueStatistics& stats) {
    displayHeader("OUTPUT SCREEN 2: QUEUE STATISTICS");

    cout << fixed << setprecision(4);

    cout << "Total number of customers              : " << stats.totalCustomers << "\n";
    cout << "Total simulation time                  : " << stats.totalSimulationTime << " minutes\n";
    cout << "Total service time                     : " << stats.totalServiceTime << " minutes\n";
    cout << "Total waiting time                     : " << stats.totalWaitingTime << " minutes\n";
    cout << "Total idle time                        : " << stats.totalIdleTime << " minutes\n";
    cout << "------------------------------------------------------------\n";
    cout << "Average inter-arrival time             : " << stats.averageInterArrivalTime << " minutes\n";
    cout << "Average service time                   : " << stats.averageServiceTime << " minutes\n";
    cout << "Average waiting time                   : " << stats.averageWaitingTime << " minutes\n";
    cout << "Average time in system                 : " << stats.averageTimeInSystem << " minutes\n";
    cout << "------------------------------------------------------------\n";
    cout << "Customers who waited                   : " << stats.customersWhoWaited << "\n";
    cout << "Probability of waiting                 : " << stats.probabilityOfWaiting << "\n";
    cout << "Maximum queue length                   : " << stats.maxQueueLength << "\n";
    cout << "Average number in queue                : " << stats.averageNumberInQueue << "\n";
    cout << "Average number in system               : " << stats.averageNumberInSystem << "\n";
    cout << "------------------------------------------------------------\n";
    cout << "Server utilization                     : " << stats.serverUtilization * 100 << "%\n";
    cout << "Server idle probability                : " << stats.serverIdleProbability * 100 << "%\n";
    cout << "Arrival rate                           : " << stats.arrivalRate << " customers/minute\n";
    cout << "Service rate                           : " << stats.serviceRate << " customers/minute\n";
}

void createSampleInputFile() {
    ofstream file("data/sample_input.csv");

    file << "InterArrivalTime,ServiceTime\n";

    mt19937 generator(12345);
    uniform_real_distribution<double> iatDistribution(1.0, 8.0);
    uniform_real_distribution<double> serviceDistribution(1.0, 6.0);

    file << fixed << setprecision(4);

    for (int i = 0; i < 100; i++) {
        file << iatDistribution(generator) << ","
             << serviceDistribution(generator) << "\n";
    }

    file.close();

    cout << "\nSample input file created at: data/sample_input.csv\n";
    cout << "Open it in Excel to view or edit the IAT and service time values.\n";
}

void displayMenu() {
    displayHeader("BANK QUEUE SIMULATION MENU");

    cout << "1. View input screen\n";
    cout << "2. Generate random simulation\n";
    cout << "3. Import IAT and service times from CSV\n";
    cout << "4. View simulation output table\n";
    cout << "5. View queue statistics\n";
    cout << "6. Export results and statistics to CSV\n";
    cout << "7. Create sample Excel/CSV input file\n";
    cout << "8. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    int numberOfCustomers = 100;
    double minIAT = 1.0;
    double maxIAT = 8.0;
    double minService = 1.0;
    double maxService = 6.0;
    unsigned int seed = 12345;

    vector<Customer> customers;
    QueueStatistics stats{};

    QueueSimulator simulator(
        numberOfCustomers,
        minIAT,
        maxIAT,
        minService,
        maxService,
        seed
    );

    int choice;

    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                displayInputScreen(
                    numberOfCustomers,
                    minIAT,
                    maxIAT,
                    minService,
                    maxService,
                    seed
                );
                clearInput();
                pauseScreen();
                break;

            case 2:
                customers = simulator.generateRandomSimulation();
                stats = simulator.calculateStatistics(customers);
                cout << "\nRandom simulation generated successfully for 100 customers.\n";
                clearInput();
                pauseScreen();
                break;

            case 3: {
                vector<double> importedIAT;
                vector<double> importedService;

                bool imported = CSVHandler::importInputData(
                    "data/sample_input.csv",
                    importedIAT,
                    importedService
                );

                if (!imported) {
                    cout << "\nFailed to import file. Make sure data/sample_input.csv exists.\n";
                } else if (importedIAT.size() < 100 || importedService.size() < 100) {
                    cout << "\nCSV file must contain at least 100 rows of IAT and service times.\n";
                } else {
                    customers = simulator.runSimulationFromInput(importedIAT, importedService);
                    stats = simulator.calculateStatistics(customers);
                    cout << "\nSimulation imported from CSV and processed successfully.\n";
                }

                clearInput();
                pauseScreen();
                break;
            }

            case 4:
                if (customers.empty()) {
                    cout << "\nNo simulation has been run yet.\n";
                } else {
                    displaySimulationTable(customers);
                }

                clearInput();
                pauseScreen();
                break;

            case 5:
                if (customers.empty()) {
                    cout << "\nNo simulation has been run yet.\n";
                } else {
                    displayStatistics(stats);
                }

                clearInput();
                pauseScreen();
                break;

            case 6:
                if (customers.empty()) {
                    cout << "\nNo simulation has been run yet.\n";
                } else {
                    CSVHandler::exportSimulationResults(
                        customers,
                        "data/simulation_results.csv"
                    );

                    CSVHandler::exportQueueStatistics(
                        stats,
                        "data/queue_statistics.csv"
                    );

                    cout << "\nExport complete.\n";
                    cout << "Simulation results saved to: data/simulation_results.csv\n";
                    cout << "Queue statistics saved to: data/queue_statistics.csv\n";
                }

                clearInput();
                pauseScreen();
                break;

            case 7:
                createSampleInputFile();
                clearInput();
                pauseScreen();
                break;

            case 8:
                cout << "\nExiting simulation.\n";
                break;

            default:
                cout << "\nInvalid choice. Please select from 1 to 8.\n";
                clearInput();
                pauseScreen();
        }

    } while (choice != 8);

    return 0;
}