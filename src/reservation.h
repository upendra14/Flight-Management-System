#include<iostream>
using namespace std;


void reservation(MYSQL* conn, int userID) {
    string flightID, coach;
    int seatsRequested;
    char wantWindowSeatChar;
    bool wantWindowSeat;

    cout << "Enter Flight ID: ";
    cin >> flightID;
    cout << "Enter Coach (e.g., Economy, Business): ";
    cin >> coach;
    cout << "Enter Number of Seats: ";
    cin >> seatsRequested;
    cout << "Do you prefer window seats? (y/n): ";
    cin >> wantWindowSeatChar;
    wantWindowSeat = (wantWindowSeatChar == 'y' || wantWindowSeatChar == 'Y');

    // Fetch base price and available seats
    string query = "SELECT Price, Available_Seats FROM flight WHERE flight_id = '" + flightID + "'";
    if (mysql_query(conn, query.c_str()) != 0) {
        cerr << "Error querying flight: " << mysql_error(conn) << "\n";
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) {
        cout << "Flight not found.\n";
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    double basePrice = atof(row[0]);
    int availableSeats = atoi(row[1]);
    mysql_free_result(res);

    // Check if enough seats are available
    if (availableSeats < seatsRequested) {
        cout << "Not enough seats available in the requested coach.\n";
        return;
    }

    // Fetch available seat numbers
    query = "SELECT seat_number FROM seat_allocation WHERE flightid = '" + flightID + 
            "' AND coach = '" + coach + "' AND seat_status = 'available' LIMIT " + toString(seatsRequested);

    if (mysql_query(conn, query.c_str()) != 0) {
        cerr << "Error querying seat allocation: " << mysql_error(conn) << "\n";
        return;
    }

    res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) < seatsRequested) {
        cout << "Not enough seats available.\n";
        return;
    }

    vector<int> seatNumbers;
    while (MYSQL_ROW seatRow = mysql_fetch_row(res)) {
        seatNumbers.push_back(atoi(seatRow[0]));
    }
    mysql_free_result(res);

    // Start transaction
    mysql_autocommit(conn, false);

    // Insert into reservation and update seat status
    for (int seatNumber=0;seatNumber<seatNumbers.size();seatNumber++) {
        int isWindowSeat = (wantWindowSeat && (seatNumber % 2 == 0));
        double finalPrice = isWindowSeat ? basePrice * 1.1 : basePrice;

        // Insert reservation
        query = "INSERT INTO reservation (userid, flightid, seat_number, coach, reservation_date, base_price, final_price, is_window_seat) "
                "VALUES (" + toString(userID) + ", '" + flightID + "', " + toString(seatNumber) + 
                ", '" + coach + "', NOW(), " + toString(basePrice) + ", " + toString(finalPrice) + 
                ", " + toString(isWindowSeat) + ")";

        if (mysql_query(conn, query.c_str()) != 0) {
            cerr << "Error making reservation: " << mysql_error(conn) << "\n";
            mysql_rollback(conn);
            return;
        }

        // Update seat status
        query = "UPDATE seat_allocation SET seat_status = 'booked' WHERE flightid = '" + flightID + 
                "' AND seat_number = " + toString(seatNumber) + " AND coach = '" + coach + "'";

        if (mysql_query(conn, query.c_str()) != 0) {
            cerr << "Error updating seat allocation: " << mysql_error(conn) << "\n";
            mysql_rollback(conn);
            return;
        }
    }

    // Update available seats in flight table
    query = "UPDATE flight SET Available_Seats = Available_Seats - " + toString(seatsRequested) + 
            " WHERE flight_id = '" + flightID + "'";

    if (mysql_query(conn, query.c_str()) != 0) {
        cerr << "Error updating flight seat count: " << mysql_error(conn) << "\n";
        mysql_rollback(conn);
        return;
    }

    // Commit transaction
    mysql_commit(conn);
    cout << "Reservation successful for " << seatsRequested << " seats.\n";
}


//void reservation(MYSQL* conn, int userID) {
//	
//    string flightID, coach;
//    int seatsRequested;
//    char wantWindowSeatChar;
//    bool wantWindowSeat;
//
//    // Input flight and seat details
//    cout << "Enter Flight ID: ";
//    cin >> flightID;
//    cout << "Enter Coach (e.g., Economy, Business): ";
//    cin >> coach;
//    cout << "Enter Number of Seats: ";
//    cin >> seatsRequested;
//    cout << "Do you prefer window seats? (y/n): ";
//    cin >> wantWindowSeatChar;
//    wantWindowSeat = (wantWindowSeatChar == 'y' || wantWindowSeatChar == 'Y');
//
//    // Fetch base price for the flight
//    string query = "SELECT Price FROM flight WHERE flight_id = '" + flightID + "'";
//    if (mysql_query(conn, query.c_str()) == 0) {
//        MYSQL_RES* res = mysql_store_result(conn);
//        if (res && mysql_num_rows(res) > 0) {
//            MYSQL_ROW row = mysql_fetch_row(res);
//            double basePrice = atof(row[0]);  // Fetch base price as double
//            mysql_free_result(res);
//
//            // Check available seats in seat_allocation
//            query = "SELECT seat_number FROM seat_allocation WHERE flightid = '" + flightID + 
//                    "' AND coach = '" + coach + "' AND seat_status = 'available' LIMIT " + toString(seatsRequested);
//
//            if (mysql_query(conn, query.c_str()) == 0) {
//                res = mysql_store_result(conn);
//                if (res && mysql_num_rows(res) == seatsRequested) {
//                    cout << "Seats are available. Proceeding with reservation...\n";
//
//                    vector<int> seatNumbers;
//                    MYSQL_ROW seatRow;
//                    while ((seatRow = mysql_fetch_row(res))) {
//                        seatNumbers.push_back(atoi(seatRow[0]));
//                    }
//                    mysql_free_result(res);
//
//                    // Insert reservations and update seat status
//                    for (size_t i = 0; i < seatNumbers.size(); ++i) {
//                        int seatNumber = seatNumbers[i];
//                        int isWindowSeat = (wantWindowSeat && (seatNumber % 2 == 0));  // Example logic: even-numbered seats are window seats
//                        double finalPrice = isWindowSeat ? basePrice * 1.1 : basePrice;  // Adjusted to double for precision
//
//                        // Insert into reservation table
//                        query = "INSERT INTO reservation (userid, flightid, seat_number, coach, reservation_date, base_price, final_price, is_window_seat) "
//                                "VALUES (" + toString(userID) + ", '" + flightID + "', " + toString(seatNumber) + 
//                                ", '" + coach + "', NOW(), " + toString(basePrice) + ", " + toString(finalPrice) + 
//                                ", " + toString(isWindowSeat) + ")";
//                        if (mysql_query(conn, query.c_str()) != 0) {
//                            cerr << "Error making reservation: " << mysql_error(conn) << "\n";
//                            return;
//                        }
//
//                        // Update seat status in seat_allocation
//                        query = "UPDATE seat_allocation SET seat_status = 'booked' WHERE flightid = '" + flightID + 
//                                "' AND seat_number = " + toString(seatNumber) + " AND coach = '" + coach + "'";
//                        if (mysql_query(conn, query.c_str()) != 0) {
//                            cerr << "Error updating seat allocation: " << mysql_error(conn) << "\n";
//                            return;
//                        }
//                    }
//                    cout << "Reservation successful for " << seatsRequested << " seats.\n";
//                } else {
//                    cout << "Not enough seats available in the requested coach.\n";
//                }
//            } else {
//                cerr << "Error querying seat allocation: " << mysql_error(conn) << "\n";
//            }
//        } else {
//            cout << "Flight not found.\n";
//        }
//    } else {
//        cerr << "Error querying flight base price: " << mysql_error(conn) << "\n";
//    }
//}
//
//
//
//
//
////
////void reservation(MYSQL* conn, int userID) {
////    string flightID, coach;
////    int seatsRequested;
////
////    // Input flight and seat details
////    cout << "Enter Flight ID: ";
////    cin >> flightID;
////    cout << "Enter Coach (e.g., Economy, Business): ";
////    cin >> coach;
////    cout << "Enter Number of Seats: ";
////    cin >> seatsRequested;
////
////    // Fetch base price for the flight
////    string query = "SELECT Price FROM flight WHERE flight_id = '" + flightID + "'";
////    if (mysql_query(conn, query.c_str()) == 0) {
////        MYSQL_RES* res = mysql_store_result(conn);
////        if (res && mysql_num_rows(res) > 0) {
////            MYSQL_ROW row = mysql_fetch_row(res);
////            double basePrice = atof(row[0]);  // Fetch base price as double
////            mysql_free_result(res);
////
////            // Check available seats in seat_allocation
////            query = "SELECT seat_number FROM seat_allocation WHERE flightid = '" + flightID + 
////                    "' AND coach = '" + coach + "' AND seat_status = 'available' LIMIT " + toString(seatsRequested);
////
////            if (mysql_query(conn, query.c_str()) == 0) {
////                res = mysql_store_result(conn);
////                if (res && mysql_num_rows(res) == seatsRequested) {
////                    cout << "Seats are available. Proceeding with reservation...\n";
////
////                    vector<int> seatNumbers;
////                    MYSQL_ROW seatRow;
////                    while ((seatRow = mysql_fetch_row(res))) {
////                        seatNumbers.push_back(atoi(seatRow[0]));
////                    }
////                    mysql_free_result(res);
////
////                    // Insert reservations and update seat status
////                    for (size_t i = 0; i < seatNumbers.size(); ++i) {
////                        int seatNumber = seatNumbers[i];
////                        int isWindowSeat = (seatNumber % 2 == 0);  // Example logic: even-numbered seats are window seats
////                        double finalPrice = isWindowSeat ? basePrice * 1.1 : basePrice;  // Adjusted to double for precision
////
////                        // Insert into reservation table
////                        query = "INSERT INTO reservation (userid, flightid, seat_number, coach, reservation_date, base_price, final_price, isWindowSeat) "
////                                "VALUES (" + toString(userID) + ", '" + flightID + "', " + toString(seatNumber) + 
////                                ", '" + coach + "', NOW(), " + toString(basePrice) + ", " + toString(finalPrice) + 
////                                ", " + toString(isWindowSeat) + ")";
////                        if (mysql_query(conn, query.c_str()) != 0) {
////                            cerr << "Error making reservation: " << mysql_error(conn) << "\n";
////                            return;
////                        }
////
////                        // Update seat status in seat_allocation
////                        query = "UPDATE seat_allocation SET seat_status = 'booked' WHERE flightid = '" + flightID + 
////                                "' AND seat_number = " + toString(seatNumber) + " AND coach = '" + coach + "'";
////                        if (mysql_query(conn, query.c_str()) != 0) {
////                            cerr << "Error updating seat allocation: " << mysql_error(conn) << "\n";
////                            return;
////                        }
////                    }
////                    cout << "Reservation successful for " << seatsRequested << " seats.\n";
////                } else {
////                    cout << "Not enough seats available in the requested coach.\n";
////                }
////            } else {
////                cerr << "Error querying seat allocation: " << mysql_error(conn) << "\n";
////            }
////        } else {
////            cout << "Flight not found.\n";
////        }
////    } else {
////        cerr << "Error querying flight base price: " << mysql_error(conn) << "\n";
////    }
////}

