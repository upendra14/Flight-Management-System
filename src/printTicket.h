#include<iostream>
#include <sstream> 
using namespace std;

template <typename T>
string toString(T value) {
    ostringstream oss;
    oss << value;
    return oss.str();
}
void printTicket(MYSQL* conn) {
    string email, password;

    // Prompt user for email and password
    cout << "Enter your email: ";
    cin >> email;
    cout << "Enter your password: ";
    cin >> password;

    // Authenticate user
    string query = "SELECT * FROM Users WHERE Email = '" + email + "' AND Password = '" + password + "'";
 //Task: verify user input
    if (mysql_query(conn, query.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res && mysql_num_rows(res) > 0) {
            MYSQL_ROW row = mysql_fetch_row(res);
            int userID = atoi(row[0]);
            mysql_free_result(res);

            // Fetch reservation history
          string query = "SELECT r.reservation_id, r.flightid, f.Origin, f.Destination, r.seat_number, r.coach, "
               "r.reservation_date, r.base_price, r.final_price, r.is_window_seat "  // Corrected column name
               "FROM reservation r "
               "JOIN flight f ON r.flightid = f.flight_id "
               "WHERE r.userid = " + toString(userID);

            
            if (mysql_query(conn, query.c_str()) == 0) {
                res = mysql_store_result(conn);
                if (res && mysql_num_rows(res) > 0) {
                    cout << "\nReservation History:\n";
                    cout << "-----------------------------------------------------------\n";
                    cout << "ResID | FlightID | Origin -> Destination | Seat | Coach | Date | Base Price | Final Price | Window Seat\n";
                    cout << "-----------------------------------------------------------\n";

                    MYSQL_ROW historyRow;
                    while ((historyRow = mysql_fetch_row(res))) {
                        cout << historyRow[0] << " | " << historyRow[1] << " | " << historyRow[2] << " -> " 
                             << historyRow[3] << " | " << historyRow[4] << " | " << historyRow[5] << " | " 
                             << historyRow[6] << " | " << historyRow[7] << " | " << historyRow[8] << " | "
                             << (atoi(historyRow[9]) ? "Yes" : "No") << "\n";
                    }
                    cout << "-----------------------------------------------------------\n";
                } else {
                    cout << "No reservations found for this user.\n";
                }
                mysql_free_result(res);
            } else {
                cerr << "Error fetching reservation history: " << mysql_error(conn) << "\n";
            }
        } else {
            cout << "Invalid email or password.\n";
        }
    } else {
        cerr << "Error querying user: " << mysql_error(conn) << "\n";
    }
}

