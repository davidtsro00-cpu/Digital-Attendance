#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <ctime>
#include <sstream>
#include <cstddef> 
#include <cctype>  
#include <algorithm>

using namespace std;

// --- Attendance Status Enum ---
enum class AttendanceStatus { PRESENT, ABSENT, LATE, UNKNOWN };

string statusToString(AttendanceStatus status) {
    switch(status) {
        case AttendanceStatus::PRESENT: return "P";
        case AttendanceStatus::ABSENT: return "A";
        case AttendanceStatus::LATE: return "L";
        default: return "?";
    }
}

AttendanceStatus charToStatus(char c) {
    switch(toupper(c)) {
        case 'P': return AttendanceStatus::PRESENT;
        case 'A': return AttendanceStatus::ABSENT;
        case 'L': return AttendanceStatus::LATE;
        default: return AttendanceStatus::UNKNOWN;
    }
}

// --- Student Class ---
class Student {
private:
    string indexNumber;
    string name;

public:
    Student() {} 
    Student(string idx, string n) : indexNumber(idx), name(n) {}

    string getIndex() const { return indexNumber; }
    string getName() const { return name; }

    void saveToFile(ofstream& outFile) const {
        outFile << indexNumber << "," << name << endl;
    }
    
    void displayFormatted() const {
        cout << left << setw(15) << indexNumber << setw(30) << name << endl;
    }
};

// --- Attendance Record ---
struct AttendanceRecord {
    string studentIndex;
    AttendanceStatus status;
};

// --- Session Class ---
class AttendanceSession {
private:
    string courseCode;
    string date;
    string startTime;
    string duration;
    vector<AttendanceRecord> records;

public:
    AttendanceSession(string code, string dt, string st, string dur)
        : courseCode(code), date(dt), startTime(st), duration(dur) {}

    void addRecord(string index, AttendanceStatus status) {
        AttendanceRecord newRecord;
        newRecord.studentIndex = index;
        newRecord.status = status;
        records.push_back(newRecord);
    }

    string generateFilename() const {
        return "session_" + courseCode + "_" + date + ".csv";
    }

    const vector<AttendanceRecord>& getRecords() const { return records; }
    
    string getCourseCode() const { return courseCode; }
    string getDate() const { return date; }

    void saveToFile() const {
        string filename = generateFilename();
        ofstream outFile(filename.c_str()); 

        if (outFile.is_open()) {
            outFile << "Course Code,Date,Start Time,Duration" << endl;
            outFile << courseCode << "," << date << "," << startTime << "," << duration << endl;
            outFile << endl << "Student Index,Status" << endl;
            
            for (size_t i = 0; i < records.size(); ++i) {
                outFile << records[i].studentIndex << "," << statusToString(records[i].status) << endl;
            }
            outFile.close();
            cout << "\nSuccess! Excel file created: " << filename << endl;
        } else {
            cerr << "Error: Could not create Excel file." << endl;
        }
    }

    void displaySummary() const {
        int present = 0, absent = 0, late = 0;
        
        for (const auto& record : records) {
            if (record.status == AttendanceStatus::PRESENT) present++;
            else if (record.status == AttendanceStatus::ABSENT) absent++;
            else if (record.status == AttendanceStatus::LATE) late++;
        }
        
        cout << "\n--- Attendance Summary ---\n";
        cout << "Present: " << present << "\n";
        cout << "Absent: " << absent << "\n";
        cout << "Late: " << late << "\n";
        cout << "Total: " << present + absent + late << "\n";
        cout << "Attendance Rate: " << fixed << setprecision(1) 
             << (present + late) * 100.0 / (present + absent + late) << "%\n";
    }
};

// --- Helper functions ---
string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "_"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "_"
       << setw(2) << ltm->tm_mday;
    return ss.str();
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "_"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "_"
       << setw(2) << ltm->tm_mday << "_"
       << setw(2) << ltm->tm_hour << "_"
       << setw(2) << ltm->tm_min;
    return ss.str();
}

bool isValidTime(const string& time) {
    if (time.length() != 5 || time[2] != ':') return false;
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!isdigit(time[i])) return false;
    }
    int hour = stoi(time.substr(0, 2));
    int minute = stoi(time.substr(3, 2));
    return (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59);
}

bool isStudentExists(const vector<Student>& students, const string& index) {
    for (const auto& student : students) {
        if (student.getIndex() == index) {
            return true;
        }
    }
    return false;
}

void loadStudentsFromFile(vector<Student>& students) {
    ifstream inFile("students.csv");
    if (!inFile) {
        cout << "No existing student records found. Starting fresh.\n";
        return;
    }

    string line;
    int lineNum = 0;
    int loaded = 0;
    while (getline(inFile, line)) {
        lineNum++;
        if (line.empty()) continue;
        
        stringstream ss(line);
        string index, name;
        if (getline(ss, index, ',') && getline(ss, name)) {
            students.push_back(Student(index, name));
            loaded++;
        } else {
            cerr << "Warning: Malformed data in students.csv at line " << lineNum << endl;
        }
    }
    inFile.close();
    cout << "Loaded " << loaded << " students from file.\n";
}

void registerStudent(vector<Student>& students) {
    string index, name;
    cout << "Enter student index: "; 
    cin >> index;
    
    if (isStudentExists(students, index)) {
        cout << "Error: Student with index " << index << " already exists!\n";
        return;
    }
    
    cout << "Enter student name: "; 
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, name);

    Student s(index, name);
    students.push_back(s);

    ofstream outFile("students.csv", ios::app); 
    if (outFile.is_open()) {
        s.saveToFile(outFile);
        outFile.close();
        cout << "Student saved to students.csv\n";
    } else {
        cerr << "Error: Could not open students.csv for writing\n";
    }
}

void viewAllStudents(const vector<Student>& students) {
    if (students.empty()) {
        cout << "No students registered yet.\n";
        return;
    }
    
    cout << "\n--- Registered Students ---\n";
    cout << left << setw(15) << "Index Number" << setw(30) << "Name" << endl;
    cout << string(45, '-') << endl;
    for (size_t i = 0; i < students.size(); ++i) {
        students[i].displayFormatted();
    }
    cout << "Total: " << students.size() << " students\n";
}

void searchStudent(const vector<Student>& students) {
    if (students.empty()) {
        cout << "No students registered yet.\n";
        return;
    }
    
    string searchTerm;
    cout << "Enter student name or index to search: ";
    cin.ignore();
    getline(cin, searchTerm);
    
    // Convert search term to lowercase for case-insensitive search
    string searchLower = searchTerm;
    transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
    
    bool found = false;
    cout << "\nSearch Results:\n";
    cout << left << setw(15) << "Index Number" << setw(30) << "Name" << endl;
    cout << string(45, '-') << endl;
    
    for (const auto& student : students) {
        string nameLower = student.getName();
        string indexLower = student.getIndex();
        transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
        transform(indexLower.begin(), indexLower.end(), indexLower.begin(), ::tolower);
        
        if (nameLower.find(searchLower) != string::npos || 
            indexLower.find(searchLower) != string::npos) {
            student.displayFormatted();
            found = true;
        }
    }
    
    if (!found) {
        cout << "No matching students found.\n";
    }
}

void markAttendance(vector<Student>& students) {
    if (students.empty()) {
        cout << "Error: No students registered. Please register students first.\n";
        return;
    }

    string code, start, dur;
    string date = getCurrentDate();
    
    cout << "Course Code: "; 
    cin >> code;
    
    do {
        cout << "Start Time (HH:MM): "; 
        cin >> start;
        if (!isValidTime(start)) {
            cout << "Invalid time format. Please use HH:MM (e.g., 09:00)\n";
        }
    } while (!isValidTime(start));
    
    cout << "Duration (e.g., 2 hours): "; 
    cin.ignore();
    getline(cin, dur);

    AttendanceSession session(code, date, start, dur);
    
    cout << "\nMark attendance for each student [P=Present, A=Absent, L=Late]:\n";
    cout << string(50, '-') << endl;
    
    for (size_t i = 0; i < students.size(); ++i) {
        char status;
        do {
            cout << left << setw(30) << students[i].getName() 
                 << " [" << students[i].getIndex() << "] - Status [P/A/L]: ";
            cin >> status;
            status = toupper(status);
        } while (status != 'P' && status != 'A' && status != 'L');
        
        session.addRecord(students[i].getIndex(), charToStatus(status));
    }
    
    session.saveToFile();
    session.displaySummary();
}

void displayReports() {
    string code, date;
    cout << "Course Code: "; 
    cin >> code;
    cout << "Date (YYYY_MM_DD): "; 
    cin >> date;

    string filename = "session_" + code + "_" + date + ".csv";
    ifstream inFile(filename.c_str());

    if (!inFile.is_open()) {
        cout << "Report file not found for " << code << " on " << date << endl;
        return;
    }

    string line;
    // Skip Excel headers
    for(int i = 0; i < 4; ++i) getline(inFile, line); 

    cout << "\n--- Attendance Records for " << code << " on " << date << " ---\n";
    cout << left << setw(15) << "Index" << setw(10) << "Status" << endl;
    cout << string(25, '-') << endl;
    
    string idx, status;
    int present = 0, absent = 0, late = 0;
    
    while (getline(inFile, line)) {
        stringstream ss(line);
        if (getline(ss, idx, ',') && getline(ss, status)) {
            cout << left << setw(15) << idx << setw(10) << status << endl;
            if (status == "P") present++;
            else if (status == "A") absent++;
            else if (status == "L") late++;
        }
    }
    inFile.close();
    
    cout << string(25, '-') << endl;
    cout << "Present: " << present << "  ";
    cout << "Absent: " << absent << "  ";
    cout << "Late: " << late << endl;
    cout << "Total: " << present + absent + late << " students\n";
}

void backupStudentFile() {
    string backupName = "backups/students_backup_" + getCurrentDateTime() + ".csv";
    
    // Create backups directory if it doesn't exist
    #ifdef _WIN32
        system("mkdir backups 2> nul");
    #else
        system("mkdir -p backups");
    #endif
    
    ifstream src("students.csv", ios::binary);
    if (!src) {
        cout << "No student file to backup.\n";
        return;
    }
    
    ofstream dst(backupName.c_str(), ios::binary);
    
    if (src && dst) {
        dst << src.rdbuf();
        cout << "Backup created: " << backupName << endl;
    } else {
        cout << "Failed to create backup.\n";
    }
}

void displayMenu() {
    cout << "\n=== ATTENDANCE MANAGEMENT SYSTEM ===\n";
    cout << "1. Register New Student\n";
    cout << "2. View All Students\n";
    cout << "3. Search Student\n";
    cout << "4. Mark Attendance (Excel)\n";
    cout << "5. View Attendance Report\n";
    cout << "6. Backup Student Data\n";
    cout << "7. Exit\n";
    cout << "------------------------------------\n";
    cout << "Choice: ";
}

int getMenuChoice() {
    int choice;
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number (1-7): ";
    }
    return choice;
}

int main() {
    vector<Student> students;
    loadStudentsFromFile(students);

    int choice = 0;
    while (choice != 7) {
        displayMenu();
        choice = getMenuChoice();

        switch(choice) {
            case 1:
                registerStudent(students);
                break;
            case 2:
                viewAllStudents(students);
                break;
            case 3:
                searchStudent(students);
                break;
            case 4:
                markAttendance(students);
                break;
            case 5:
                displayReports();
                break;
            case 6:
                backupStudentFile();
                break;
            case 7:
                cout << "Thank you for using the Attendance Management System!\n";
                break;
            default:
                cout << "Invalid choice. Please enter 1-7.\n";
        }
    }
    return 0;
}
