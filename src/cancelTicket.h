
#include<iostream>
#include <sstream> 
using namespace std;

void cancelTicket(MYSQL *conn) {
    string email, flightID, coach;
    int numSeats;

    cout << "Enter your email: ";
    cin >> email;
    cout << "Enter Flight ID: ";
    cin >> flightID;
    cout << "Enter class (e.g., Economy, Business): ";
    cin >> coach;
    cout << "Enter number of seats to cancel: ";
    cin >> numSeats;

    // Fetch user ID from email
    string userQuery = "SELECT UserID FROM users WHERE Email = '" + email + "'";
    if (mysql_query(conn, userQuery.c_str())) {
        cout << "Error fetching user details: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES *resUser = mysql_store_result(conn);
    MYSQL_ROW rowUser = mysql_fetch_row(resUser);
    if (!rowUser) {
        cout << "No user found with this email!" << endl;
        mysql_free_result(resUser);
        return;
    }
    int userID = atoi(rowUser[0]); // Using atoi for safe conversion
    mysql_free_result(resUser);

    // Fetch seat details and price
    string seatQuery = "SELECT seat_number, base_price FROM reservation WHERE userid = " + toString(userID) +
                       " AND flightid = '" + flightID + "' AND coach = '" + coach + "' LIMIT " + toString(numSeats);

    if (mysql_query(conn, seatQuery.c_str())) {
        cout << "Error fetching seat details: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES *resSeats = mysql_store_result(conn);
    vector<int> seatNumbers;
    double totalPrice = 0.0;
    while (MYSQL_ROW row = mysql_fetch_row(resSeats)) {
        seatNumbers.push_back(atoi(row[0])); // Using atoi for safe conversion
        totalPrice += atof(row[1]); // Using atof for safe conversion
    }
    mysql_free_result(resSeats);

    if (seatNumbers.size() < numSeats) {
        cout << "You don't have enough seats booked in this class!" << endl;
        return;
    }

    // Apply cancellation charge (20% deduction)
    double refundAmount = totalPrice * 0.8;
    cout << "Cancellation successful! You will receive a refund of: " << refundAmount << endl;

    // Delete seats from reservation table
    for (size_t i = 0; i < seatNumbers.size(); ++i) { // Using range-based for loop
        int seatNum = seatNumbers[i];
        string deleteQuery = "DELETE FROM reservation WHERE userid = " + toString(userID) +
                             " AND flightid = '" + flightID + "' AND coach ='"+ coach +"' AND seat_number = " + toString(seatNum);
        if (mysql_query(conn, deleteQuery.c_str())) {
            cout << "Error deleting seat: " << mysql_error(conn) << endl;
            return;
        }

        // Update seat status in seat_allocation table
        string updateSeatQuery = "UPDATE seat_allocation SET seat_status = 'Available' WHERE flightid = '" + flightID +
                                 "' AND seat_number = " + toString(seatNum) + " AND coach = '" + coach + "'";
        if (mysql_query(conn, updateSeatQuery.c_str())) {
            cout << "Error updating seat status: " << mysql_error(conn) << endl;
            return;
        }
    }

    // Update available seats in flight table
    string updateFlightQuery = "UPDATE flight SET Available_Seats = Available_Seats + " + toString(numSeats) +
                               " WHERE flight_id = '" + flightID + "'";
    if (mysql_query(conn, updateFlightQuery.c_str())) {
        cout << "Error updating flight seats: " << mysql_error(conn) << endl;
        return;
    }

    cout << numSeats << " seats successfully canceled. Seat status updated!" << endl;
}

