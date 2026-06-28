#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <vector>

#include "Customer.h"
#include "QueueStatistics.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QTabWidget *tabs;

    QWidget *configurationTab;
    QWidget *resultsTab;
    QWidget *statisticsTab;
    QWidget *aboutTab;

    QSpinBox *customerCountInput;
    QDoubleSpinBox *minIATInput;
    QDoubleSpinBox *maxIATInput;
    QDoubleSpinBox *minServiceInput;
    QDoubleSpinBox *maxServiceInput;
    QLineEdit *seedInput;

    QPushButton *generateButton;
    QPushButton *importButton;
    QPushButton *exportButton;
    QPushButton *sampleButton;
    QPushButton *clearButton;

    QTableWidget *resultsTable;
    QGridLayout *statisticsLayout;
    QTextEdit *aboutText;

    std::vector<Customer> customers;
    QueueStatistics stats;

    void setupConfigurationTab();
    void setupResultsTab();
    void setupStatisticsTab();
    void setupAboutTab();

    void populateResultsTable();
    void populateStatistics();
    void clearStatistics();

private slots:
    void generateSimulation();
    void importCSV();
    void exportCSV();
    void createSampleCSV();
    void clearSimulation();

public:
    MainWindow(QWidget *parent = nullptr);
};

#endif