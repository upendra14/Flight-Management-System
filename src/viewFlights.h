#include<iostream>
using namespace std;

void viewFlights(MYSQL *conn) {
    string origin, destination;
   
    cin.ignore(); // Clear buffer
    cout << "Enter origin airport: ";
    getline(cin, origin);
    cout << "Enter destination airport: ";
    getline(cin, destination);

    string query = "SELECT * FROM flight WHERE Origin = '" + origin + "' AND Destination = '" + destination + "'";

    if (mysql_query(conn, query.c_str())) {
        cout << "Error retrieving flights: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;

    if (mysql_num_rows(res) == 0) {  // Check if any flights are available
        cout << "No flights available for the given origin and destination." << endl;
        mysql_free_result(res);
        return;
    }

    cout << "Flight Details:" << endl;
    cout << "--------------------------------------------------------------" << endl;
    cout << "Flight ID | Name         | Origin   | Destination | DepTime        | ArrTime        | Total Seats | Available Seats | Price" << endl;
    cout << "--------------------------------------------------------------" << endl;

    while ((row = mysql_fetch_row(res))) {
        for (int i = 0; i < num_fields; i++) {
            cout << (row[i] ? row[i] : "NULL") << " | ";
        }
        cout << endl;
    }

    mysql_free_result(res);
}
