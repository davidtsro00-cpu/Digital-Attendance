/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Student {
public:
    string indexNumber, name, programme;

    void registerStudent() {
        cout << "Enter Index Number: ";
        cin >> indexNumber;
        cin.ignore();

        cout << "Enter Name: ";
        getline(cin, name);

        cout << "Enter Programme: ";
        getline(cin, programme);

        ofstream file("students.txt", ios::app);
        file << indexNumber << "," << name << "," << programme << endl;
        file.close();

        cout << "Student saved successfully!\n";
    }

    static void viewStudents() {
        ifstream file("students.txt");
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
};

class AttendanceSession {
public:
    string courseCode, date, startTime;
    int duration;

    void createSession() {
        cout << "Enter Course Code: ";
        cin >> courseCode;
        cout << "Enter Date (YYYY-MM-DD): ";
        cin >> date;
        cout << "Enter Start Time: ";
        cin >> startTime;
        cout << "Enter Duration (hours): ";
        cin >> duration;

        cout << "Session created successfully!\n";
    }
};

int main() {
    int choice;
    AttendanceSession session;

    do {
        cout << "\n--- Digital Attendance System ---\n";
        cout << "1. Register Student\n";
        cout << "2. View Students\n";
        cout << "3. Create Session\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            Student s;
            s.registerStudent();
            break;
        }
        case 2:
            Student::viewStudents();
            break;
        case 3:
            session.createSession();
            break;
        case 4:
            cout << "Goodbye!\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }

    } while (choice != 4);

    return 0;
}