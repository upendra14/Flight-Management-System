#include<iostream>
#include <sstream> 
using namespace std;

void insertFlight(MYSQL *conn) {
    string flight_id, f_name, origin, destination;
    string dep_time, arr_time;
    int total_seats, available_seats;
    double price;

    cin.ignore(); // Clear buffer
    cout << "Enter Flight ID: ";
    getline(cin, flight_id);
    cout << "Enter Flight Name: ";
    getline(cin, f_name);
    cout << "Enter Origin: ";
    getline(cin, origin);
    cout << "Enter Destination: ";
    getline(cin, destination);
    cout << "Enter Departure Time (YYYY-MM-DD HH:MM:SS): ";
    getline(cin, dep_time);
    cout << "Enter Arrival Time (YYYY-MM-DD HH:MM:SS): ";
    getline(cin, arr_time);

    cout << "Enter Total Seats: ";
    cin >> total_seats;
    cout << "Enter Available Seats: ";
    cin >> available_seats;
    cout << "Enter Price: ";
    cin >> price;

    string query = "INSERT INTO flight (flight_id, f_name, Origin, Destination, DepTime, ArrTime, Total_Seats, Available_Seats, Price) "
                   "VALUES ('" + flight_id + "', '" + f_name + "', '" + origin + "', '" + destination + "', '" + dep_time + "', '" + arr_time + "', " +
                   toString(total_seats) + ", " + toString(available_seats) + ", " + toString(price) + ")";

    if (mysql_query(conn, query.c_str())) {
        cout << "Error inserting flight: " << mysql_error(conn) << endl;
    } else {
        cout << "Flight inserted successfully!" << endl;
    }
}
