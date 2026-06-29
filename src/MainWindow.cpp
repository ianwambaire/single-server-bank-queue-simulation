#include "MainWindow.h"
#include "QueueSimulator.h"
#include "CSVHandler.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QString>
#include <QFont>
#include <QColor>
#include <QBrush>
#include <QStatusBar>
#include <QFrame>
#include <fstream>
#include <random>
#include <iomanip>

namespace {
QFrame* createKpiCard(const QString& title, QLabel*& valueLabel, const QString& backgroundColor) {
    QFrame *card = new QFrame();
    card->setFrameShape(QFrame::StyledPanel);
    card->setStyleSheet(
        QString("QFrame { background-color: %1; border: 1px solid #d6d6d6; border-radius: 8px; }").arg(backgroundColor)
    );

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(6);

    QLabel *titleLabel = new QLabel(title);
    QFont titleFont;
    titleFont.setPointSize(9);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #1f2937;");

    valueLabel = new QLabel("--");
    QFont valueFont;
    valueFont.setPointSize(16);
    valueFont.setBold(true);
    valueLabel->setFont(valueFont);
    valueLabel->setStyleSheet("color: #111827;");

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);

    return card;
}
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    tabs = new QTabWidget(this);

    configurationTab = new QWidget();
    resultsTab = new QWidget();
    statisticsTab = new QWidget();
    aboutTab = new QWidget();

    setupConfigurationTab();
    setupResultsTab();
    setupStatisticsTab();
    setupAboutTab();

    tabs->addTab(configurationTab, "Configuration");
    tabs->addTab(resultsTab, "Simulation Results");
    tabs->addTab(statisticsTab, "Queue Statistics");
    tabs->addTab(aboutTab, "About");

    setCentralWidget(tabs);
    statusBar()->showMessage("Ready", 3000);
}

void MainWindow::setupConfigurationTab() {
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QGridLayout *formLayout = new QGridLayout();

    QLabel *title = new QLabel("Single-Server Bank Queue Simulation");
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);

    QLabel *subtitle = new QLabel("C++ GUI simulation using uniform inter-arrival and service time distributions");

    customerCountInput = new QSpinBox();
    customerCountInput->setRange(1, 10000);
    customerCountInput->setValue(100);

    minIATInput = new QDoubleSpinBox();
    minIATInput->setRange(0, 1000);
    minIATInput->setValue(1.0);
    minIATInput->setDecimals(2);

    maxIATInput = new QDoubleSpinBox();
    maxIATInput->setRange(0, 1000);
    maxIATInput->setValue(8.0);
    maxIATInput->setDecimals(2);

    minServiceInput = new QDoubleSpinBox();
    minServiceInput->setRange(0, 1000);
    minServiceInput->setValue(1.0);
    minServiceInput->setDecimals(2);

    maxServiceInput = new QDoubleSpinBox();
    maxServiceInput->setRange(0, 1000);
    maxServiceInput->setValue(6.0);
    maxServiceInput->setDecimals(2);

    seedInput = new QLineEdit();
    seedInput->setText("12345");

    formLayout->addWidget(new QLabel("Number of Customers:"), 0, 0);
    formLayout->addWidget(customerCountInput, 0, 1);

    formLayout->addWidget(new QLabel("Minimum Inter-Arrival Time:"), 1, 0);
    formLayout->addWidget(minIATInput, 1, 1);

    formLayout->addWidget(new QLabel("Maximum Inter-Arrival Time:"), 2, 0);
    formLayout->addWidget(maxIATInput, 2, 1);

    formLayout->addWidget(new QLabel("Minimum Service Time:"), 3, 0);
    formLayout->addWidget(minServiceInput, 3, 1);

    formLayout->addWidget(new QLabel("Maximum Service Time:"), 4, 0);
    formLayout->addWidget(maxServiceInput, 4, 1);

    formLayout->addWidget(new QLabel("Random Seed:"), 5, 0);
    formLayout->addWidget(seedInput, 5, 1);

    generateButton = new QPushButton("Generate Random Simulation");
    importButton = new QPushButton("Import CSV and Run Simulation");
    exportButton = new QPushButton("Export Results to CSV");
    sampleButton = new QPushButton("Create Sample CSV Input");
    clearButton = new QPushButton("Clear Simulation");

    QHBoxLayout *buttonLayout1 = new QHBoxLayout();
    buttonLayout1->addWidget(generateButton);
    buttonLayout1->addWidget(importButton);

    QHBoxLayout *buttonLayout2 = new QHBoxLayout();
    buttonLayout2->addWidget(exportButton);
    buttonLayout2->addWidget(sampleButton);
    buttonLayout2->addWidget(clearButton);

    QLabel *note = new QLabel(
        "Default assignment values: 100 customers, IAT Uniform(1, 8), Service Time Uniform(1, 6), one teller, FCFS queue discipline."
    );
    note->setWordWrap(true);

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout1);
    mainLayout->addLayout(buttonLayout2);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(note);
    mainLayout->addStretch();

    configurationTab->setLayout(mainLayout);

    connect(generateButton, &QPushButton::clicked, this, &MainWindow::generateSimulation);
    connect(importButton, &QPushButton::clicked, this, &MainWindow::importCSV);
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportCSV);
    connect(sampleButton, &QPushButton::clicked, this, &MainWindow::createSampleCSV);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearSimulation);
}

void MainWindow::setupResultsTab() {
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *title = new QLabel("Simulation Results Table");
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    title->setFont(font);

    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("Search:");
    resultsSearchInput = new QLineEdit();
    resultsSearchInput->setPlaceholderText("Type to filter rows...");
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(resultsSearchInput);

    resultsTable = new QTableWidget();
    resultsTable->setColumnCount(10);

    QStringList headers;
    headers << "Customer"
            << "IAT"
            << "Arrival"
            << "Service"
            << "Start"
            << "Wait"
            << "Departure"
            << "Time in System"
            << "Idle"
            << "Queue Length";

    resultsTable->setHorizontalHeaderLabels(headers);
    resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    resultsTable->setSortingEnabled(true);

    layout->addWidget(title);
    layout->addLayout(filterLayout);
    layout->addWidget(resultsTable);

    resultsTab->setLayout(layout);

    connect(resultsSearchInput, &QLineEdit::textChanged, this, &MainWindow::filterResultsTable);
}

void MainWindow::setupStatisticsTab() {
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *title = new QLabel("Queue Statistics Dashboard");
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    title->setFont(font);

    QGridLayout *kpiLayout = new QGridLayout();
    kpiLayout->setHorizontalSpacing(10);
    kpiLayout->setVerticalSpacing(10);

    QFrame *avgWaitCard = createKpiCard("Average Waiting Time", kpiAverageWaitValue, "#fff7e8");
    QFrame *utilizationCard = createKpiCard("Server Utilization", kpiUtilizationValue, "#e8f5ff");
    QFrame *maxQueueCard = createKpiCard("Maximum Queue Length", kpiMaxQueueValue, "#fdeeee");
    QFrame *probWaitCard = createKpiCard("Probability of Waiting", kpiProbabilityWaitValue, "#eef8ef");

    kpiLayout->addWidget(avgWaitCard, 0, 0);
    kpiLayout->addWidget(utilizationCard, 0, 1);
    kpiLayout->addWidget(maxQueueCard, 1, 0);
    kpiLayout->addWidget(probWaitCard, 1, 1);

    statisticsLayout = new QGridLayout();

    layout->addWidget(title);
    layout->addLayout(kpiLayout);
    layout->addLayout(statisticsLayout);
    layout->addStretch();

    statisticsTab->setLayout(layout);
}

void MainWindow::setupAboutTab() {
    QVBoxLayout *layout = new QVBoxLayout();

    aboutText = new QTextEdit();
    aboutText->setReadOnly(true);

    aboutText->setText(
        "Project: Bank Queue Simulation using C++ and Qt GUI\n\n"
        "This application simulates a single-server banking system where customers arrive randomly and are served by one teller.\n\n"
        "Simulation Assumptions:\n"
        "- Inter-arrival time follows Uniform(1, 8) minutes by default.\n"
        "- Service time follows Uniform(1, 6) minutes by default.\n"
        "- The system uses one server/teller.\n"
        "- Customers are served using First Come First Served discipline.\n"
        "- The default number of customers is 100.\n\n"
        "Main Features:\n"
        "- Input/configuration screen\n"
        "- Random simulation generation\n"
        "- CSV import from Excel-compatible files\n"
        "- Full simulation results table\n"
        "- Queue statistics dashboard\n"
        "- CSV export for Excel analysis\n\n"
        "Formula Used:\n"
        "Time = minimum + random number × (maximum - minimum)\n\n"
        "This project demonstrates discrete-event simulation and queue performance analysis."
    );

    layout->addWidget(aboutText);
    aboutTab->setLayout(layout);
}

void MainWindow::generateSimulation() {
    int numberOfCustomers = customerCountInput->value();
    double minIAT = minIATInput->value();
    double maxIAT = maxIATInput->value();
    double minService = minServiceInput->value();
    double maxService = maxServiceInput->value();
    unsigned int seed = seedInput->text().toUInt();

    if (maxIAT < minIAT || maxService < minService) {
        QMessageBox::warning(this, "Invalid Input", "Maximum values must be greater than or equal to minimum values.");
        return;
    }

    QueueSimulator simulator(
        numberOfCustomers,
        minIAT,
        maxIAT,
        minService,
        maxService,
        seed
    );

    customers = simulator.generateRandomSimulation();
    stats = simulator.calculateStatistics(customers);

    populateResultsTable();
    populateStatistics();

    statusBar()->showMessage("Random simulation generated successfully.", 5000);
    tabs->setCurrentWidget(resultsTab);
}

void MainWindow::importCSV() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open CSV Input File",
        "",
        "CSV Files (*.csv)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    std::vector<double> importedIAT;
    std::vector<double> importedService;

    bool imported = CSVHandler::importInputData(
        fileName.toStdString(),
        importedIAT,
        importedService
    );

    if (!imported) {
        QMessageBox::warning(this, "Import Failed", "Could not import the selected CSV file.");
        return;
    }

    if (importedIAT.empty() || importedService.empty()) {
        QMessageBox::warning(this, "Invalid CSV", "The CSV file does not contain valid input data.");
        return;
    }

    if (importedIAT.size() != importedService.size()) {
        QMessageBox::warning(this, "Invalid CSV", "Inter-arrival and service time columns must have the same number of rows.");
        return;
    }

    int numberOfCustomers = importedIAT.size();

    QueueSimulator simulator(
        numberOfCustomers,
        minIATInput->value(),
        maxIATInput->value(),
        minServiceInput->value(),
        maxServiceInput->value(),
        seedInput->text().toUInt()
    );

    customers = simulator.runSimulationFromInput(importedIAT, importedService);
    stats = simulator.calculateStatistics(customers);

    populateResultsTable();
    populateStatistics();

    statusBar()->showMessage("CSV data imported and simulation processed successfully.", 5000);
    tabs->setCurrentWidget(resultsTab);
}

void MainWindow::exportCSV() {
    if (customers.empty()) {
        QMessageBox::warning(this, "No Data", "Run or import a simulation before exporting.");
        return;
    }

    QString resultsFile = QFileDialog::getSaveFileName(
        this,
        "Save Simulation Results",
        "simulation_results.csv",
        "CSV Files (*.csv)"
    );

    if (resultsFile.isEmpty()) {
        return;
    }

    CSVHandler::exportSimulationResults(customers, resultsFile.toStdString());

    QString statsFile = QFileDialog::getSaveFileName(
        this,
        "Save Queue Statistics",
        "queue_statistics.csv",
        "CSV Files (*.csv)"
    );

    if (statsFile.isEmpty()) {
        return;
    }

    CSVHandler::exportQueueStatistics(stats, statsFile.toStdString());

    statusBar()->showMessage("Simulation results and queue statistics exported successfully.", 5000);
}

void MainWindow::createSampleCSV() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save Sample CSV Input",
        "sample_input.csv",
        "CSV Files (*.csv)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    std::ofstream file(fileName.toStdString());

    if (!file.is_open()) {
        QMessageBox::warning(this, "File Error", "Could not create sample CSV file.");
        return;
    }

    int numberOfCustomers = customerCountInput->value();
    double minIAT = minIATInput->value();
    double maxIAT = maxIATInput->value();
    double minService = minServiceInput->value();
    double maxService = maxServiceInput->value();

    std::mt19937 generator(seedInput->text().toUInt());
    std::uniform_real_distribution<double> iatDistribution(minIAT, maxIAT);
    std::uniform_real_distribution<double> serviceDistribution(minService, maxService);

    file << "InterArrivalTime,ServiceTime\n";
    file << std::fixed << std::setprecision(4);

    for (int i = 0; i < numberOfCustomers; i++) {
        file << iatDistribution(generator) << ","
             << serviceDistribution(generator) << "\n";
    }

    file.close();

    statusBar()->showMessage("Sample CSV input file created successfully.", 5000);
}

void MainWindow::clearSimulation() {
    customers.clear();
    resultsTable->setRowCount(0);
    resultsSearchInput->clear();
    clearStatistics();

    statusBar()->showMessage("Simulation data cleared successfully.", 5000);
}

void MainWindow::populateResultsTable() {
    resultsTable->setSortingEnabled(false);
    resultsTable->setRowCount(customers.size());

    for (int row = 0; row < static_cast<int>(customers.size()); row++) {
        const Customer& c = customers[row];

        resultsTable->setItem(row, 0, new QTableWidgetItem(QString::number(c.customerNo)));
        resultsTable->setItem(row, 1, new QTableWidgetItem(QString::number(c.interArrivalTime, 'f', 2)));
        resultsTable->setItem(row, 2, new QTableWidgetItem(QString::number(c.arrivalTime, 'f', 2)));
        resultsTable->setItem(row, 3, new QTableWidgetItem(QString::number(c.serviceTime, 'f', 2)));
        resultsTable->setItem(row, 4, new QTableWidgetItem(QString::number(c.serviceStartTime, 'f', 2)));
        QTableWidgetItem *waitItem = new QTableWidgetItem(QString::number(c.waitingTime, 'f', 2));
        if (c.waitingTime > 5.0) {
            waitItem->setBackground(QColor(255, 212, 212));
            waitItem->setForeground(QBrush(QColor(17, 24, 39)));
        } else if (c.waitingTime > 0.0) {
            waitItem->setBackground(QColor(255, 245, 204));
            waitItem->setForeground(QBrush(QColor(17, 24, 39)));
        }
        resultsTable->setItem(row, 5, waitItem);
        resultsTable->setItem(row, 6, new QTableWidgetItem(QString::number(c.departureTime, 'f', 2)));
        resultsTable->setItem(row, 7, new QTableWidgetItem(QString::number(c.timeInSystem, 'f', 2)));
        QTableWidgetItem *idleItem = new QTableWidgetItem(QString::number(c.serverIdleTime, 'f', 2));
        if (c.serverIdleTime > 0.0) {
            idleItem->setBackground(QColor(222, 239, 255));
            idleItem->setForeground(QBrush(QColor(17, 24, 39)));
        }
        resultsTable->setItem(row, 8, idleItem);

        QTableWidgetItem *queueItem = new QTableWidgetItem(QString::number(c.queueLength));
        if (c.queueLength >= 3) {
            queueItem->setBackground(QColor(255, 228, 196));
            queueItem->setForeground(QBrush(QColor(17, 24, 39)));
        }
        resultsTable->setItem(row, 9, queueItem);
    }

    resultsTable->setSortingEnabled(true);
    filterResultsTable(resultsSearchInput->text());
}

void MainWindow::filterResultsTable(const QString& text) {
    QString query = text.trimmed();

    for (int row = 0; row < resultsTable->rowCount(); row++) {
        bool matches = query.isEmpty();

        if (!matches) {
            for (int col = 0; col < resultsTable->columnCount(); col++) {
                QTableWidgetItem *item = resultsTable->item(row, col);
                if (item && item->text().contains(query, Qt::CaseInsensitive)) {
                    matches = true;
                    break;
                }
            }
        }

        resultsTable->setRowHidden(row, !matches);
    }
}

void MainWindow::populateStatistics() {
    clearStatistics();

    kpiAverageWaitValue->setText(QString::number(stats.averageWaitingTime, 'f', 2) + " min");
    kpiUtilizationValue->setText(QString::number(stats.serverUtilization * 100, 'f', 1) + "%");
    kpiMaxQueueValue->setText(QString::number(stats.maxQueueLength));
    kpiProbabilityWaitValue->setText(QString::number(stats.probabilityOfWaiting * 100, 'f', 1) + "%");

    QStringList labels;
    QStringList values;

    labels << "Total Customers"
           << "Customers Who Waited"
           << "Maximum Queue Length"
           << "Total Simulation Time"
           << "Total Service Time"
           << "Total Waiting Time"
           << "Total Idle Time"
           << "Average Inter-Arrival Time"
           << "Average Service Time"
           << "Average Waiting Time"
           << "Average Time in System"
           << "Probability of Waiting"
           << "Server Utilization"
           << "Server Idle Probability"
           << "Average Number in Queue"
           << "Average Number in System"
           << "Arrival Rate"
           << "Service Rate";

    values << QString::number(stats.totalCustomers)
           << QString::number(stats.customersWhoWaited)
           << QString::number(stats.maxQueueLength)
           << QString::number(stats.totalSimulationTime, 'f', 4)
           << QString::number(stats.totalServiceTime, 'f', 4)
           << QString::number(stats.totalWaitingTime, 'f', 4)
           << QString::number(stats.totalIdleTime, 'f', 4)
           << QString::number(stats.averageInterArrivalTime, 'f', 4)
           << QString::number(stats.averageServiceTime, 'f', 4)
           << QString::number(stats.averageWaitingTime, 'f', 4)
           << QString::number(stats.averageTimeInSystem, 'f', 4)
           << QString::number(stats.probabilityOfWaiting, 'f', 4)
           << QString::number(stats.serverUtilization * 100, 'f', 2) + "%"
           << QString::number(stats.serverIdleProbability * 100, 'f', 2) + "%"
           << QString::number(stats.averageNumberInQueue, 'f', 4)
           << QString::number(stats.averageNumberInSystem, 'f', 4)
           << QString::number(stats.arrivalRate, 'f', 4)
           << QString::number(stats.serviceRate, 'f', 4);

    for (int i = 0; i < labels.size(); i++) {
        QLabel *label = new QLabel(labels[i] + ":");
        QLabel *value = new QLabel(values[i]);
        label->setStyleSheet("color: #1f2937;");
        value->setStyleSheet("color: #111827;");

        QFont labelFont;
        labelFont.setBold(true);
        label->setFont(labelFont);

        statisticsLayout->addWidget(label, i, 0);
        statisticsLayout->addWidget(value, i, 1);
    }
}

void MainWindow::clearStatistics() {
    kpiAverageWaitValue->setText("--");
    kpiUtilizationValue->setText("--");
    kpiMaxQueueValue->setText("--");
    kpiProbabilityWaitValue->setText("--");

    while (QLayoutItem *item = statisticsLayout->takeAt(0)) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}