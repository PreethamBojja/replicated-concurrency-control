#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "operation.h"

using namespace std;

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

    //TODO : Handle bad input scenarios and spaces
    while (getline(infile, line)) {
        if (line.empty()) continue;

        istringstream iss(line);
        string command;

        getline(iss, command, '(');
        OperationType type = getOperationType(command);
        string transactionName, var, valueStr, siteId;
        int transactionId = -1, value = 0;

        switch (type) {
            case BEGIN:
                getline(iss, transactionName, ')');
                transactionId = extractTransactionId(transactionName);
                operations.emplace_back(BEGIN, transactionId, "", 0, timestamp++);
                if (printInput) cout << "Begin: T" << transactionId << endl;
                break;

            case READ:
                getline(iss, transactionName, ',');
                getline(iss, var, ')');
                transactionId = extractTransactionId(transactionName);
                operations.emplace_back(READ, transactionId, var, 0, timestamp++);
                if (printInput) cout << "Read: T" << transactionId << ", Variable:" << var << endl;
                break;

            case WRITE:
                getline(iss, transactionName, ',');
                getline(iss, var, ',');
                getline(iss, valueStr, ')');
                value = stoi(valueStr);
                transactionId = extractTransactionId(transactionName);
                operations.emplace_back(WRITE, transactionId, var, value, timestamp++);
                if (printInput) cout << "Write: T" << transactionId << ", Variable:" << var << ", Value:" << value << endl;
                break;

            case END:
                getline(iss, transactionName, ')');
                transactionId = extractTransactionId(transactionName);
                operations.emplace_back(END, transactionId, "", 0, timestamp++);
                if (printInput) cout << "End: T" << transactionId << endl;
                break;

            case FAIL:
                getline(iss, siteId, ')');
                operations.emplace_back(FAIL, -1, siteId, 0, timestamp++);
                if (printInput) cout << "Fail Site: " << siteId << endl;
                break;

            case RECOVER:
                getline(iss, siteId, ')');
                operations.emplace_back(RECOVER, -1, siteId, 0, timestamp++);
                if (printInput) cout << "Recover Site: " << siteId << endl;
                break;

            case DUMP:
                operations.emplace_back(DUMP, -1, "", 0, timestamp++);
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
