#include <iostream>
#include <mysql.h>
#include<vector>
#include <string>
#include <ctime> // For time and ctime
#include <cstdlib>
#include <windows.h> 
#include <sstream> 

//components
#include "src/viewFlights.h"
#include "src/printTicket.h"
#include "src/cancelTicket.h"
#include "src/insertFlight.h"
#include "src/reservation.h"

using namespace std;

bool askToSignOut() {
    cout << "\nDo you want to Sign Out? (Y/N): ";
    char choice;
    cin >> choice;
    return (choice == 'Y' || choice == 'y'); // If 'Y', sign out
}
bool isValidPassword(const string& password) {
    if (password.length() < 8 || password.length() > 15) {
        cout << "Password must be between 8 to 15 characters long.\n"<<endl;
        return false;
    }

    bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
    const string specialChars = "!@#$%^&*()-+";

    for (int ch=0;ch<password.size();ch++) {
        if (islower(ch)) hasLower = true;
        else if (isupper(ch)) hasUpper = true;
        else if (isdigit(ch)) hasDigit = true;
        else if (specialChars.find(ch) != string::npos) hasSpecial = true;
    }

    if (!hasLower) cout << "Password must contain at least one lowercase letter.\n";
    if (!hasUpper) cout << "Password must contain at least one uppercase letter.\n";
    if (!hasDigit) cout << "Password must contain at least one digit.\n";
    if (!hasSpecial) cout << "Password must contain at least one special character: " << specialChars << "\n"<<endl;

    return hasLower && hasUpper && hasDigit && hasSpecial;
}

// Alternative to stoi for older compilers
int stringToInt(const string &str) {
    stringstream ss(str);
    int value;
    ss >> value;
    return value;
}
void displayWelcomeMessage() {
    cout << "=========================================================" << endl;
    cout << "         Welcome to the Flight Management System         " << endl;
    cout << "=========================================================" << endl;
    cout << endl; // Add some spacing for better readability
}

// Function to authenticate user
bool authenticateUser(MYSQL *conn, string email, string password) {
    string query = "SELECT * FROM Users WHERE Email = '" + email + "' AND Password = '" + password + "'";
    if (mysql_query(conn, query.c_str())) {
        cout << "Error in query: " << mysql_error(conn) << endl;
        return false;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Error in retrieving result: " << mysql_error(conn) << endl;
        return false;
    }

    if (mysql_num_rows(res) > 0) {
        mysql_free_result(res);
        return true;
    }

    mysql_free_result(res);
    return false;
}

int main() {
    MYSQL *conn = mysql_init(NULL);

    // Database connection details
    const char *HOST = "localhost";
    const char *USER = "root";
    const char *PASSW = "updr29"; // Replace with your actual password
    const char *DB = "mydb";

    if (!mysql_real_connect(conn, HOST, USER, PASSW, DB, 3306, NULL, 0)) {
        cout << "Connection failed: " << mysql_error(conn) << endl;
        return 1;
    }

    cout << "Connected successfully!" << endl;

    Sleep(2000);

    bool exit = false;

while (!exit) {
    system("cls");
      displayWelcomeMessage();

    int choice;
    cout << "1 : Register" << endl;
    cout << "2 : Sign In" << endl;
    cout << "0 : Exit" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 1) { // Register
        string userName, email, password;
        char role;

        cin.ignore(); // Clear buffer
        cout << "Enter username: ";
        getline(cin, userName);
//        cout << "Enter email: ";
//        getline(cin, email);
    const string gmailDomain = "@gmail.com";

    // Prompt user for email and validate
    while (true) {
        cout << "Enter your email: ";
        cin >> email;

        // Check if email ends with '@gmail.com'
        if (email.size() >= gmailDomain.size() &&
            email.compare(email.size() - gmailDomain.size(), gmailDomain.size(), gmailDomain) == 0)  {
            break; // Valid Gmail address
        } else {
            cout << "Please enter a valid Gmail address ending with '@gmail.com'.\n";
        }
    }
        
        
        
//        cout << "Enter password: ";
//        getline(cin, password);
    while (true) {
        cout << "Enter your password: ";
        cin >> password;

        if (isValidPassword(password)) {
            break; // Valid password
        } else {
            cout << "Please enter a valid password.\n";
        }
    }
        cout << "Enter role (A for Admin, U for User): ";
        cin >> role;

        string roleStr = (role == 'A' || role == 'a') ? "Admin" : "User";
        string query = "INSERT INTO Users (UserName, UserRole, Email, Password) VALUES ('" +
                       userName + "', '" + roleStr + "', '" + email + "', '" + password + "')";

        if (mysql_query(conn, query.c_str())) {
            cout << "Error registering user: " << mysql_error(conn) << endl;
        } else {
            cout << "User registered successfully!" << endl;
        }
    } else if (choice == 2) { // Sign In
        string email, password;

        cin.ignore(); // Clear buffer
        cout << "Enter email: ";
        getline(cin, email);
        cout << "Enter password: ";
        getline(cin, password);

        if (authenticateUser(conn, email, password)) { // Login
            string roleQuery = "SELECT UserRole, UserID FROM Users WHERE Email = '" + email + "'";
            mysql_query(conn, roleQuery.c_str());
            MYSQL_RES* res = mysql_store_result(conn);
            MYSQL_ROW row = mysql_fetch_row(res);

            string role = row ? row[0] : "User";
            int userID = row ? stringToInt(row[1]) : -1;
            bool loggedIn = true;

            while (loggedIn) {
            	 displayWelcomeMessage();
                if (role == "User") {
                    int userChoice;
                    cout << "1 : View Flights" << endl;
                    cout << "2 : Make Reservation" << endl;
                    cout << "3 : Print Ticket" << endl;
                    cout << "4 : Cancel Ticket" << endl;
                    cout << "Enter your choice: ";
                    cin >> userChoice;

                    if (userChoice == 1) {
                        viewFlights(conn);
                    } else if (userChoice == 2) {
                        reservation(conn, userID);
                    } else if (userChoice == 3) {
                        printTicket(conn);
                    } else if (userChoice == 4) {
                        cancelTicket(conn);
                    }

                    Sleep(2000);
                    cout << "Press Enter to continue...";
                    cin.ignore();
                    cin.get();

                    if (askToSignOut()) {
                        loggedIn = false; // User logs out
                    }
                } else if (role == "Admin") {
                    int adminChoice;
                    cout << "1 : View Flights" << endl;
                    cout << "2 : Insert Flight" << endl;
                    cout << "Enter your choice: ";
                    cin >> adminChoice;

                    if (adminChoice == 1) {
                        viewFlights(conn);
                        Sleep(2000);
                        cout << "Press Enter to continue..." << endl;
                        cin.ignore();
                        cin.get();
                    } else if (adminChoice == 2) {
                        insertFlight(conn);
                        Sleep(2000);
                        cout << "Press Enter to continue..." << endl;
                        cin.ignore();
                        cin.get();
                    }

                    Sleep(2000);
                    cout << "Press Enter to continue...";
                    cin.ignore();
                    cin.get();

                    if (askToSignOut()) {
                        loggedIn = false; // Admin logs out
                    }
                }
            }
            mysql_free_result(res);
        } else {
            cout << "Authentication failed. Please check your credentials." << endl;
        }
        Sleep(2000);
    } else if (choice == 0) { // Exit
        exit = true;
    } else {
        cout << "Invalid choice. Please try again." << endl;
        Sleep(2000);
    }
}
    mysql_close(conn);
    cout << "Connection closed. Goodbye!" << endl;
    return 0;
}
