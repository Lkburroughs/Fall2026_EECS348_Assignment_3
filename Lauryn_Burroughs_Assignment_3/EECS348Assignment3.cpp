/*
Name of Program: EECS 348 Assignment 3
Description: C program that manages a priority queue of emails for a CEO.
Input(s): Input file with commanda and Email information
Output(s): Results based on comands in input file, examples include: Emails in the inbox,Next email for
CEO to read,Current count of unread emails
All collaborators: N/A,
Other Code Sources: Chatgpt,Claude
Author: Lauryn Burroughs
Creation date: 10/01/2026
Revision date: 10/01/2026
Revisions: Add Cooments, Add error hanndling for invalid inputs,empty files, etc.
*/
#include <iostream>   // Allows input and output.
#include <fstream>    // Allows reading from files.
#include <sstream>    // Allows string stream processing.
#include <string>     // Allows use of strings.
#include <cstdlib>    // Allows use of atoi().
#include <cctype>     // Allows use of isdigit().
using namespace std;  // Avoids writing std:: before standard library names.

// Represents one email.
class Email {
private:
    string sender;    // Stores the sender category.
    string subject;   // Stores the email subject.
    string date;      // Stores the date as MM-DD-YYYY.
    int rank;         // Stores the sender priority.
    int dateKey;      // Stores date as YYYYMMDD for comparison.
    long sequence;    // Stores arrival order for exact ties.

    // Finds the priority of a sender category.
    static int rankFor(const string& s) {
        if (s == "Boss") return 5;             // Boss has highest priority.
        if (s == "Subordinate") return 4;      // Subordinate is second.
        if (s == "Peer") return 3;             // Peer is third.
        if (s == "ImportantPerson") return 2;  // ImportantPerson is fourth.
        if (s == "OtherPerson") return 1;      // OtherPerson is last.
        return 0;                              // Invalid sender gets no priority.
    }

    // Converts MM-DD-YYYY into YYYYMMDD.
    static int keyFor(const string& d) {
        int mm = atoi(d.substr(0, 2).c_str());  // Gets the month.
        int dd = atoi(d.substr(3, 2).c_str());  // Gets the day.
        int yy = atoi(d.substr(6, 4).c_str());  // Gets the year.
        return yy * 10000 + mm * 100 + dd;      // Creates sortable date number.
    }

public:
    // Creates an empty email.
    Email() : rank(0), dateKey(0), sequence(0) {}

    // Creates an email with all required information.
    Email(const string& s, const string& subj, const string& d, long seq)
        : sender(s),                       // Saves sender category.
          subject(subj),                   // Saves subject.
          date(d),                         // Saves original date.
          rank(rankFor(s)),                // Calculates sender priority.
          dateKey(keyFor(d)),              // Calculates date priority.
          sequence(seq) {}                 // Saves arrival order.

    // Determines which email has higher priority.
    bool higherPriorityThan(const Email& other) const {
        if (rank != other.rank)            // Checks sender priority first.
            return rank > other.rank;      // Higher rank comes first.

        if (dateKey != other.dateKey)      // If senders have equal priority.
            return dateKey > other.dateKey; // Newer email comes first.

        return sequence < other.sequence;  // Earlier arrival breaks exact ties.
    }

    // Displays one email.
    void display() const {
        cout << "Sender: " << sender << "\n";    // Prints sender.
        cout << "Subject: " << subject << "\n";  // Prints subject.
        cout << "Date: " << date << "\n";        // Prints date.
    }
};

// Implements the email priority queue using a MaxHeap.
class MaxHeap {
private:
    Email* data;   // Dynamic array storing emails.
    int size;      // Number of emails currently stored.
    int capacity;  // Maximum current array size.

    // Finds a node's parent.
    static int parent(int i) {
        return (i - 1) / 2;  // Returns parent index.
    }

    // Finds a node's left child.
    static int left(int i) {
        return 2 * i + 1;  // Returns left child index.
    }

    // Finds a node's right child.
    static int right(int i) {
        return 2 * i + 2;  // Returns right child index.
    }

    // Swaps two emails in the heap.
    void swapAt(int a, int b) {
        Email temp = data[a];  // Temporarily saves first email.
        data[a] = data[b];     // Moves second email into first position.
        data[b] = temp;        // Moves saved email into second position.
    }

    // Increases the size of the dynamic array.
    void grow() {
        int newCapacity = capacity * 2;            // Doubles capacity.
        Email* newData = new Email[newCapacity];   // Creates larger array.

        for (int i = 0; i < size; i++) {           // Loops through old array.
            newData[i] = data[i];                  // Copies each email.
        }

        delete[] data;                             // Deletes old array.
        data = newData;                            // Uses new array.
        capacity = newCapacity;                    // Saves new capacity.
    }

    // Moves an email upward until heap order is correct.
    void siftUp(int index) {
        while (index > 0 &&                        // Stops at root.
               data[index].higherPriorityThan(
                   data[parent(index)])) {         // Checks parent priority.

            swapAt(index, parent(index));          // Swaps with parent.
            index = parent(index);                 // Moves to parent position.
        }
    }

    // Moves an email downward until heap order is correct.
    void siftDown(int index) {
        while (true) {                             // Continues until correctly placed.
            int largest = index;                   // Assumes current node is largest.
            int leftChild = left(index);           // Finds left child.
            int rightChild = right(index);         // Finds right child.

            if (leftChild < size &&                // Makes sure left child exists.
                data[leftChild].higherPriorityThan(
                    data[largest])) {              // Compares left child.
                largest = leftChild;               // Left child becomes best option.
            }

            if (rightChild < size &&               // Makes sure right child exists.
                data[rightChild].higherPriorityThan(
                    data[largest])) {              // Compares right child.
                largest = rightChild;              // Right child becomes best option.
            }

            if (largest == index) {                // Checks if heap is already correct.
                break;                             // Stops the loop.
            }

            swapAt(index, largest);                // Swaps with higher-priority child.
            index = largest;                       // Continues from new position.
        }
    }

public:
    // Creates an empty heap.
    MaxHeap()
        : data(new Email[16]),                     // Creates initial array.
          size(0),                                 // Starts with zero emails.
          capacity(16) {}                          // Initial capacity is 16.

    // Frees dynamically allocated memory.
    ~MaxHeap() {
        delete[] data;                             // Deletes email array.
    }

    MaxHeap(const MaxHeap&) = delete;              // Prevents unsafe copying.
    MaxHeap& operator=(const MaxHeap&) = delete;   // Prevents unsafe assignment.

    // Checks whether the heap is empty.
    bool isEmpty() const {
        return size == 0;                          // True if no emails exist.
    }

    // Returns the number of emails.
    int count() const {
        return size;                               // Returns current size.
    }

    // Adds an email to the heap.
    void insert(const Email& email) {
        if (size == capacity) {                    // Checks if array is full.
            grow();                                // Makes array larger.
        }

        data[size] = email;                        // Places email at end.
        siftUp(size);                              // Restores heap order.
        size++;                                    // Increases number of emails.
    }

    // Returns highest-priority email.
    const Email& peekMax() const {
        return data[0];                            // Root has highest priority.
    }

    // Removes highest-priority email.
    void removeMax() {
        if (size == 0) {                           // Checks for empty heap.
            return;                                // Does nothing if empty.
        }

        data[0] = data[size - 1];                  // Moves last email to root.
        size--;                                    // Removes last position.

        if (size > 0) {                            // Checks if emails remain.
            siftDown(0);                           // Restores heap order.
        }
    }
};

// Controls CEO inbox actions.
class CEOInbox {
private:
    MaxHeap queue;       // Stores emails by priority.
    long nextSequence;   // Tracks email arrival order.

public:
    // Creates an empty inbox.
    CEOInbox() : nextSequence(0) {}

    // Adds an email.
    void addEmail(const string& sender,
                  const string& subject,
                  const string& date) {

        Email email(sender,                        // Uses sender category.
                    subject,                       // Uses subject.
                    date,                          // Uses date.
                    nextSequence++);               // Assigns arrival number.

        queue.insert(email);                       // Inserts email into heap.
    }

    // Displays the next email.
    void next() const {
        if (queue.isEmpty()) {                     // Checks if inbox is empty.
            cout << "No emails to read.\n\n";        // Displays empty message.
            return;                                // Stops function.
        }

        cout << "Next email:\n";                   // Prints heading.
        queue.peekMax().display();                  // Displays highest-priority email.
        cout <<"\n";              
    }

    // Reads and removes next email.
    void read() {
        if (queue.isEmpty()) {                     // Checks if inbox is empty.
            cout << "No emails to read.\n\n";        // Displays empty message.
            return;                                // Stops function.
        }

        queue.removeMax();                         // Removes next email.
    }

    // Displays number of unread emails.
    void count() const {
        cout << "There are "                       // Starts output sentence.
             << queue.count()                      // Prints number of emails.
             << " emails to read.\n\n";              // Finishes output sentence.
    }
};

// Reads and validates commands.
class CommandProcessor {
private:
    CEOInbox& inbox;  // References the CEO inbox.

    // Removes extra spaces around input.
    static string trim(const string& s) {
        size_t first = s.find_first_not_of(" \t\r\n");  // Finds first real character.

        if (first == string::npos) {                     // Checks for empty string.
            return "";                                   // Returns empty string.
        }

        size_t last = s.find_last_not_of(" \t\r\n");    // Finds last real character.
        return s.substr(first, last - first + 1);        // Returns trimmed text.
    }

    // Checks whether sender category is valid.
    static bool validSender(const string& sender) {
        return sender == "Boss" ||                       // Accepts Boss.
               sender == "Subordinate" ||                // Accepts Subordinate.
               sender == "Peer" ||                       // Accepts Peer.
               sender == "ImportantPerson" ||            // Accepts ImportantPerson.
               sender == "OtherPerson";                  // Accepts OtherPerson.
    }

    // Checks whether the date is valid.
    static bool validDate(const string& date) {
        if (date.length() != 10) {                       // Checks exact date length.
            return false;                                // Rejects wrong length.
        }

        if (date[2] != '-' || date[5] != '-') {          // Checks dash positions.
            return false;                                // Rejects wrong format.
        }

        for (int i = 0; i < 10; i++) {                   // Checks every date character.
            if (i == 2 || i == 5) {                      // Skips dash locations.
                continue;                                // Continues loop.
            }

            if (!isdigit(static_cast<unsigned char>(
                    date[i]))) {                         // Checks for digit.
                return false;                            // Rejects non-digit.
            }
        }

        int month = atoi(date.substr(0, 2).c_str());     // Converts month.
        int day = atoi(date.substr(3, 2).c_str());       // Converts day.
        int year = atoi(date.substr(6, 4).c_str());      // Converts year.

        if (month < 1 || month > 12) {                   // Checks month range.
            return false;                                // Rejects invalid month.
        }

        if (year <= 0) {                                 // Checks year.
            return false;                                // Rejects invalid year.
        }

        int daysInMonth;                                 // Stores number of valid days.

        if (month == 2) {                                // Handles February.
            bool leapYear =                              // Determines leap year.
                (year % 400 == 0) ||                     // Years divisible by 400.
                (year % 4 == 0 &&                        // Or divisible by 4.
                 year % 100 != 0);                       // But not divisible by 100.

            daysInMonth = leapYear ? 29 : 28;            // Sets February length.
        }
        else if (month == 4 ||                           // Checks April.
                 month == 6 ||                           // Checks June.
                 month == 9 ||                           // Checks September.
                 month == 11) {                          // Checks November.

            daysInMonth = 30;                            // These months have 30 days.
        }
        else {
            daysInMonth = 31;                            // Remaining months have 31 days.
        }

        return day >= 1 && day <= daysInMonth;           // Validates day.
    }

    // Processes an EMAIL command.
    void handleEmail(const string& args, int lineNumber) {
        size_t firstComma = args.find(',');              // Finds first comma.
        size_t lastComma = args.rfind(',');              // Finds last comma.

        if (firstComma == string::npos ||                 // Checks first comma.
            lastComma == string::npos ||                  // Checks last comma.
            firstComma == lastComma) {                    // Makes sure two commas exist.

            cerr << "Invalid EMAIL command on line "      // Starts error message.
                 << lineNumber                            // Prints line number.
                 << ".\n\n";                                // Finishes message.
            return;                                       // Rejects command.
        }

        string sender =
            trim(args.substr(0, firstComma));             // Gets sender category.

        string subject =
            trim(args.substr(firstComma + 1,              // Starts after first comma.
                             lastComma - firstComma - 1)); // Gets subject.

        string date =
            trim(args.substr(lastComma + 1));             // Gets date.

        if (!validSender(sender)) {                       // Checks sender category.
            cerr << "Invalid sender category on line "    // Starts error.
                 << lineNumber                            // Prints line number.
                 << ": "                                  // Separates message.
                 << sender                                // Prints bad sender.
                 << "\n\n";                                 // Ends message.
            return;                                       // Rejects email.
        }

        if (subject.empty()) {                            // Checks for missing subject.
            cerr << "Missing subject on line "            // Starts error message.
                 << lineNumber                            // Prints line number.
                 << ".\n\n";                                // Finishes message.
            return;                                       // Rejects email.
        }

        if (!validDate(date)) {                           // Checks date.
            cerr << "Invalid date on line "               // Starts error message.
                 << lineNumber                            // Prints line number.
                 << ": "                                  // Separates message.
                 << date                                  // Prints invalid date.
                 << "\n\n";                                 // Ends message.
            return;                                       // Rejects email.
        }

        inbox.addEmail(sender, subject, date);            // Adds valid email.
    }

public:
    // Connects processor to an inbox.
    explicit CommandProcessor(CEOInbox& in)
        : inbox(in) {}

    // Processes commands from the input file.
    void process(istream& input) {
        string line;                                      // Stores each input line.
        int lineNumber = 0;                               // Tracks current line.
        bool foundCommand = false;                        // Tracks whether file has content.

        while (getline(input, line)) {                     // Reads each line.
            lineNumber++;                                 // Updates line number.
            line = trim(line);                            // Removes extra spaces.

            if (line.empty()) {                           // Checks for empty line.
                continue;                                 // Ignores empty line.
            }

            foundCommand = true;                          // Records non-empty input.

            if (line.compare(0, 6, "EMAIL ") == 0) {      // Checks EMAIL command.
                handleEmail(line.substr(6),               // Sends email information.
                            lineNumber);                   // Sends line number.
            }
            else if (line == "NEXT") {                    // Checks NEXT command.
                inbox.next();                             // Displays next email.
            }
            else if (line == "READ") {                    // Checks READ command.
                inbox.read();                             // Removes next email.
            }
            else if (line == "COUNT") {                   // Checks COUNT command.
                inbox.count();                            // Displays number of emails.
            }
            else {
                cerr << "Invalid command on line "        // Starts error message.
                     << lineNumber                        // Prints line number.
                     << ": "                              // Separates output.
                     << line                              // Prints invalid command.
                     << "\n\n";                             // Ends output.
            }
        }

        if (!foundCommand) {                              // Checks for empty file.
            cout << "Input file is empty.\n\n";             // Displays empty-file message.
        }
    }
};

int main(int argc, char* argv[]) {                          // Starts the program.
    CEOInbox inbox;                                        // Creates the CEO inbox.
    CommandProcessor processor(inbox);                     // Creates command processor.

    if (argc > 2) {                                        // Checks for too many arguments.
        cerr << "Error: Too many arguments provided.\n\n";   // Prints error message.
        cerr << "Usage: ./ceo_email [filename]\n\n";         // Shows correct usage.
        return 1;                                          // Ends program with error.
    }

    if (argc == 2) {                                       // Checks if a filename was given.
        ifstream file(argv[1]);                            // Opens the given file.

        if (!file) {                                       // Checks if file opened correctly.
            cerr << "Error: Could not open file: "         // Starts error message.
                 << argv[1]                                // Prints filename.
                 << "\n\n";                                  // Ends error message.
            return 1;                                      // Stops program.
        }

        processor.process(file);                           // Processes commands from file.
        file.close();                                      // Closes the file.
    }
    else {                                                 // Runs when no filename argument exists.
        processor.process(cin);                            // Reads redirected or typed input.
    }

    return 0;                                              // Ends program successfully.
}