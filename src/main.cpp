#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "operation.h"
#include "transactionManager.h"

using namespace std;

int main(int argc, char* argv[]) {
    ifstream infile;
    string line;
    if (argc < 2) {
        cerr << "No input file provided, reading from stdin..." << endl;
    } else {
        infile.open(argv[1]);
        if (!infile.is_open()) {
            cerr << "Failed to open the file" << endl;
            return -1;
        }
    }

    vector<Operation> operations;
    int timestamp = 0;
    TransactionManager& transactionManager = TransactionManager::get_instance();

    // Read from stdin if no file is given
    istream& inputStream = (argc < 2) ? cin : infile;

    while (getline(inputStream, line)) {
        if (line.empty()) continue;

        // Get the list of waiting operations
        vector<Operation*> waiting_operations = transactionManager.waiting_operations;

        // Process the waiting operations if the list is not empty
        if (!waiting_operations.empty()) {
            for (Operation* current_operation : waiting_operations) {
                if (current_operation->op_type == OperationType::READ) {
                    int value = transactionManager.read_operation(current_operation->transactionId, current_operation->variable, timestamp++);
                } else {
                    transactionManager.write_operation(current_operation->transactionId, current_operation->variable, current_operation->value, timestamp++);
                }
            }
            // Clear the waiting operations
            transactionManager.waiting_operations.clear();
        }

        string formattedLine;
        for (char c : line) {
            if (c != ' ') {
                formattedLine += c;
            }
        }
        istringstream iss(formattedLine);
        string command;

        getline(iss, command, '(');
        OperationType type = get_operation_type(command);
        string transactionName, var, valueStr, siteId;
        int transactionId = -1, value = 0;
        bool committed;

        switch (type) {
            case BEGIN:
                getline(iss, transactionName, ')');
                transactionId = extract_transaction_id(transactionName);
                transactionManager.begin_transaction(transactionId, timestamp++);
                break;

            case READ:
                getline(iss, transactionName, ',');
                getline(iss, var, ')');
                transactionId = extract_transaction_id(transactionName);
                value = transactionManager.read_operation(transactionId, var, timestamp++);
                break;

            case WRITE:
                getline(iss, transactionName, ',');
                getline(iss, var, ',');
                getline(iss, valueStr, ')');
                value = stoi(valueStr);
                transactionId = extract_transaction_id(transactionName);
                transactionManager.write_operation(transactionId, var, value, timestamp++);
                break;

            case END:
                getline(iss, transactionName, ')');
                transactionId = extract_transaction_id(transactionName);
                committed = transactionManager.end_transaction(transactionId, timestamp++);
                break;

            case FAIL:
                getline(iss, siteId, ')');
                transactionManager.fail_site(stoi(siteId), timestamp++);
                break;

            case RECOVER:
                getline(iss, siteId, ')');
                transactionManager.recover_site(stoi(siteId), timestamp++);
                break;

            case DUMP:
                transactionManager.dump_system_state();
                break;

            case UNKNOWN:
            default:
                cerr << "Unknown operation: " << command << endl;
                return -1;
        }
    }

    if (infile.is_open()) {
        infile.close();
    }
    return 0;
}
