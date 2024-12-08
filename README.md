# replicated-concurrency-control

Distributed replicated concurrency control and recovery

## Authors
- Nikhil Kommineni
- Sai Preetham Bojja

## Description
This project implements a distributed replicated concurrency control and recovery system. The system is designed to handle concurrent transactions on multiple sites, and to ensure consistency and recover from failures.

## How to run
1. Compile the code using `make`
2. Run the code using `./bin/application.out <config_file> <log_file>`
3. The config file is a yaml file that contains the initial system configuration.
4. The log file is a csv file that contains the system logs.

## How to test
1. Run the test using `make test`

## How to use
1. The system supports the following commands:
    - `begin <transaction_id>`
    - `read <transaction_id> <variable>`
    - `write <transaction_id> <variable> <value>`
    - `commit <transaction_id>`
    - `abort <transaction_id>`
    - `dump`
    - `fail <site_id>`
    - `recover <site_id>`