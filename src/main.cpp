#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "operation.h"
#include "transactionManager.h"

using namespace std;

bool printInput = false;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Provide input file name as an argument" << endl;
        return -1;
    }

    if (argc > 2 && string(argv[2]) == "p") {
        printInput = true;
    }

    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cerr << "Failed to open the file" << endl;
        return -1;
    }

    vector<Operation> operations;
    int timestamp = 0;
    string line;

    TransactionManager& transactionManager = TransactionManager::getInstance();

    while (getline(infile, line)) {
        if (line.empty()) continue;

        // Get the list of waiting operations
        vector<Operation*> waiting_operations = transactionManager.waiting_operations;

        // Process the waiting operations if the list is not empty
        if (!waiting_operations.empty()) {
            for (Operation* current_operation : waiting_operations) {
                if (current_operation->op_type == OperationType::READ) {
                    // cout << "Executing operation : T" << current_operation->transactionId << " read on " << current_operation->variable << endl;
                    int value = transactionManager.readOperation(current_operation->transactionId, current_operation->variable, timestamp++);
                } else {
                    // cout << "Executing operation : T" << current_operation->transactionId << " write on " << current_operation->variable << endl;
                    transactionManager.writeOperation(current_operation->transactionId, current_operation->variable, current_operation->value, timestamp++);
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
        OperationType type = getOperationType(command);
        string transactionName, var, valueStr, siteId;
        int transactionId = -1, value = 0;
        bool committed;

        switch (type) {
            case BEGIN:
                getline(iss, transactionName, ')');
                transactionId = extractTransactionId(transactionName);
                transactionManager.beginTransaction(transactionId, timestamp++);
                if (printInput) cout << "Begin: T" << transactionId << endl;
                break;

            case READ:
                getline(iss, transactionName, ',');
                getline(iss, var, ')');
                transactionId = extractTransactionId(transactionName);
                value = transactionManager.readOperation(transactionId, var, timestamp++);
                if (printInput) cout << "Read: T" << transactionId << ", Variable: " << var << ", Value: " << value << endl;
                break;

            case WRITE:
                getline(iss, transactionName, ',');
                getline(iss, var, ',');
                getline(iss, valueStr, ')');
                value = stoi(valueStr);
                transactionId = extractTransactionId(transactionName);
                transactionManager.writeOperation(transactionId, var, value, timestamp++);
                if (printInput) cout << "Write: T" << transactionId << ", Variable: " << var << ", Value: " << value << endl;
                break;

            case END:
                getline(iss, transactionName, ')');
                transactionId = extractTransactionId(transactionName);
                committed = transactionManager.endTransaction(transactionId, timestamp++);
                if (printInput) cout << (committed ? "Transaction commits" : "Transaction aborts") << endl;
                break;

            case FAIL:
                getline(iss, siteId, ')');
                transactionManager.failSite(stoi(siteId), timestamp++);
                if (printInput) cout << "Fail Site: " << siteId << endl;
                break;

            case RECOVER:
                getline(iss, siteId, ')');
                transactionManager.recoverSite(stoi(siteId), timestamp++);
                if (printInput) cout << "Recover Site: " << siteId << endl;
                break;

            case DUMP:
                transactionManager.dumpSystemState();
                if (printInput) cout << "Dump" << endl;
                break;

            case UNKNOWN:
            default:
                cerr << "Unknown operation: " << command << endl;
                return -1;
        }
    }

    infile.close();
    return 0;
}
