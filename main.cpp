#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <fstream>
#include <limits>
#include <string>

using namespace std;

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
};

double generateUniformDouble(double minValue, double maxValue, mt19937 &generator) {
    uniform_real_distribution<double> distribution(minValue, maxValue);
    return distribution(generator);
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

void pauseScreen() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

vector<Customer> runSimulation(
    int numberOfCustomers,
    double minIAT,
    double maxIAT,
    double minService,
    double maxService,
    unsigned int seed
) {
    vector<Customer> customers;
    mt19937 generator(seed);

    double previousArrivalTime = 0.0;
    double previousDepartureTime = 0.0;

    for (int i = 1; i <= numberOfCustomers; i++) {
        Customer customer;

        customer.customerNo = i;

        customer.randomIAT = generateUniformDouble(0.0, 1.0, generator);
        customer.interArrivalTime = minIAT + customer.randomIAT * (maxIAT - minIAT);

        customer.randomService = generateUniformDouble(0.0, 1.0, generator);
        customer.serviceTime = minService + customer.randomService * (maxService - minService);

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

        customers.push_back(customer);

        previousArrivalTime = customer.arrivalTime;
        previousDepartureTime = customer.departureTime;
    }

    return customers;
}

void displayInputScreen(
    int numberOfCustomers,
    double minIAT,
    double maxIAT,
    double minService,
    double maxService,
    unsigned int seed
) {
    cout << "\n============================================================\n";
    cout << "                  BANK QUEUE SIMULATION\n";
    cout << "                       INPUT SCREEN\n";
    cout << "============================================================\n";
    cout << "Number of customers              : " << numberOfCustomers << "\n";
    cout << "Inter-arrival time distribution  : Uniform("
         << minIAT << ", " << maxIAT << ") minutes\n";
    cout << "Service time distribution        : Uniform("
         << minService << ", " << maxService << ") minutes\n";
    cout << "Random seed used                 : " << seed << "\n";
    cout << "Server type                      : Single bank teller\n";
    cout << "Queue discipline                 : First Come First Served\n";
    cout << "============================================================\n";
}

void displaySimulationTable(const vector<Customer> &customers) {
    cout << "\n====================================================================================================================\n";
    cout << "                                      OUTPUT SCREEN 1: SIMULATION TABLE\n";
    cout << "====================================================================================================================\n";

    cout << left
         << setw(6)  << "No"
         << setw(10) << "RN-IAT"
         << setw(10) << "IAT"
         << setw(10) << "Arrive"
         << setw(10) << "RN-Srv"
         << setw(10) << "Service"
         << setw(12) << "Start"
         << setw(10) << "Wait"
         << setw(12) << "Depart"
         << setw(12) << "InSystem"
         << setw(10) << "Idle"
         << "\n";

    cout << "--------------------------------------------------------------------------------------------------------------------\n";

    cout << fixed << setprecision(2);

    for (const auto &customer : customers) {
        cout << left
             << setw(6)  << customer.customerNo
             << setw(10) << customer.randomIAT
             << setw(10) << customer.interArrivalTime
             << setw(10) << customer.arrivalTime
             << setw(10) << customer.randomService
             << setw(10) << customer.serviceTime
             << setw(12) << customer.serviceStartTime
             << setw(10) << customer.waitingTime
             << setw(12) << customer.departureTime
             << setw(12) << customer.timeInSystem
             << setw(10) << customer.serverIdleTime
             << "\n";
    }

    cout << "====================================================================================================================\n";
}

void displayQueueStatistics(const vector<Customer> &customers) {
    double totalIAT = 0.0;
    double totalServiceTime = 0.0;
    double totalWaitingTime = 0.0;
    double totalTimeInSystem = 0.0;
    double totalIdleTime = 0.0;
    double totalBusyTime = 0.0;

    int customersWhoWaited = 0;
    double longestWaitingTime = 0.0;
    double longestTimeInSystem = 0.0;

    for (const auto &customer : customers) {
        totalIAT += customer.interArrivalTime;
        totalServiceTime += customer.serviceTime;
        totalWaitingTime += customer.waitingTime;
        totalTimeInSystem += customer.timeInSystem;
        totalIdleTime += customer.serverIdleTime;
        totalBusyTime += customer.serviceTime;

        if (customer.waitingTime > 0) {
            customersWhoWaited++;
        }

        if (customer.waitingTime > longestWaitingTime) {
            longestWaitingTime = customer.waitingTime;
        }

        if (customer.timeInSystem > longestTimeInSystem) {
            longestTimeInSystem = customer.timeInSystem;
        }
    }

    int n = customers.size();
    double simulationEndTime = customers.back().departureTime;

    double averageIAT = totalIAT / n;
    double averageServiceTime = totalServiceTime / n;
    double averageWaitingTime = totalWaitingTime / n;
    double averageTimeInSystem = totalTimeInSystem / n;

    double probabilityOfWaiting = static_cast<double>(customersWhoWaited) / n;
    double serverUtilization = totalBusyTime / simulationEndTime;
    double serverIdleProbability = totalIdleTime / simulationEndTime;

    double averageNumberInQueue = totalWaitingTime / simulationEndTime;
    double averageNumberInSystem = totalTimeInSystem / simulationEndTime;
    double arrivalRate = n / simulationEndTime;
    double serviceRate = n / totalServiceTime;

    cout << fixed << setprecision(4);

    cout << "\n============================================================\n";
    cout << "              OUTPUT SCREEN 2: QUEUE STATISTICS\n";
    cout << "============================================================\n";
    cout << "Total number of customers simulated       : " << n << "\n";
    cout << "Total simulation time                     : " << simulationEndTime << " minutes\n";
    cout << "Total service/busy time                   : " << totalBusyTime << " minutes\n";
    cout << "Total server idle time                    : " << totalIdleTime << " minutes\n";
    cout << "------------------------------------------------------------\n";
    cout << "Average inter-arrival time                : " << averageIAT << " minutes\n";
    cout << "Average service time                      : " << averageServiceTime << " minutes\n";
    cout << "Average waiting time in queue             : " << averageWaitingTime << " minutes\n";
    cout << "Average time spent in system              : " << averageTimeInSystem << " minutes\n";
    cout << "------------------------------------------------------------\n";
    cout << "Number of customers who waited            : " << customersWhoWaited << "\n";
    cout << "Probability that a customer waits         : " << probabilityOfWaiting << "\n";
    cout << "Longest waiting time                      : " << longestWaitingTime << " minutes\n";
    cout << "Longest time spent in system              : " << longestTimeInSystem << " minutes\n";
    cout << "------------------------------------------------------------\n";
    cout << "Server utilization                        : " << serverUtilization * 100 << "%\n";
    cout << "Server idle probability                   : " << serverIdleProbability * 100 << "%\n";
    cout << "Average number of customers in queue      : " << averageNumberInQueue << "\n";
    cout << "Average number of customers in system     : " << averageNumberInSystem << "\n";
    cout << "Arrival rate                              : " << arrivalRate << " customers/minute\n";
    cout << "Service rate                              : " << serviceRate << " customers/minute\n";
    cout << "============================================================\n";
}

void exportToCSV(const vector<Customer> &customers, string fileName) {
    ofstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Could not create CSV file.\n";
        return;
    }

    file << "Customer No,"
         << "Random Number IAT,"
         << "Inter Arrival Time,"
         << "Arrival Time,"
         << "Random Number Service,"
         << "Service Time,"
         << "Service Start Time,"
         << "Waiting Time,"
         << "Departure Time,"
         << "Time In System,"
         << "Server Idle Time\n";

    file << fixed << setprecision(4);

    for (const auto &customer : customers) {
        file << customer.customerNo << ","
             << customer.randomIAT << ","
             << customer.interArrivalTime << ","
             << customer.arrivalTime << ","
             << customer.randomService << ","
             << customer.serviceTime << ","
             << customer.serviceStartTime << ","
             << customer.waitingTime << ","
             << customer.departureTime << ","
             << customer.timeInSystem << ","
             << customer.serverIdleTime << "\n";
    }

    file.close();

    cout << "\nCSV file successfully created: " << fileName << "\n";
    cout << "You can open this file in Microsoft Excel, Numbers, or Google Sheets.\n";
}

void displayMenu() {
    cout << "\n============================================================\n";
    cout << "                  BANK QUEUE SIMULATION MENU\n";
    cout << "============================================================\n";
    cout << "1. View input screen\n";
    cout << "2. View simulation output table\n";
    cout << "3. View queue statistics\n";
    cout << "4. Export simulation table to CSV for Excel\n";
    cout << "5. Exit\n";
    cout << "============================================================\n";
    cout << "Enter your choice: ";
}

int main() {
    cout << "============================================================\n";
    cout << "        DISCRETE EVENT SIMULATION: BANKING SYSTEM\n";
    cout << "============================================================\n";

    int numberOfCustomers = getIntegerInput("Enter number of customers to simulate: ", 1);

    cout << "\nEnter inter-arrival time range.\n";
    double minIAT = getDoubleInput("Minimum inter-arrival time: ", 0);
    double maxIAT = getDoubleInput("Maximum inter-arrival time: ", minIAT);

    cout << "\nEnter service time range.\n";
    double minService = getDoubleInput("Minimum service time: ", 0);
    double maxService = getDoubleInput("Maximum service time: ", minService);

    int seedChoice;
    cout << "\nChoose random seed option:\n";
    cout << "1. Use fixed seed for repeatable results\n";
    cout << "2. Use random seed for different results each run\n";
    cout << "Enter choice: ";
    cin >> seedChoice;

    unsigned int seed;

    if (seedChoice == 1) {
        seed = static_cast<unsigned int>(getIntegerInput("Enter seed value, e.g. 12345: ", 1));
    } else {
        random_device rd;
        seed = rd();
    }

    vector<Customer> customers = runSimulation(
        numberOfCustomers,
        minIAT,
        maxIAT,
        minService,
        maxService,
        seed
    );

    clearInput();

    int choice;

    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                displayInputScreen(numberOfCustomers, minIAT, maxIAT, minService, maxService, seed);
                clearInput();
                pauseScreen();
                break;

            case 2:
                displaySimulationTable(customers);
                clearInput();
                pauseScreen();
                break;

            case 3:
                displayQueueStatistics(customers);
                clearInput();
                pauseScreen();
                break;

            case 4:
                exportToCSV(customers, "bank_queue_simulation.csv");
                clearInput();
                pauseScreen();
                break;

            case 5:
                cout << "\nExiting simulation. Thank you.\n";
                break;

            default:
                cout << "\nInvalid choice. Please select from 1 to 5.\n";
                clearInput();
                pauseScreen();
        }

    } while (choice != 5);

    return 0;
}