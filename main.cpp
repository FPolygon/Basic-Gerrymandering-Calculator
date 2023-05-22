/*  Francis Pagulayan
 UIN: 665900279
 Project 1: Gerrymandering
 Overview: This program is meant to calculate gerrymandering for s specific state in a data set using the custom
 vector implementation header file. This is done by prompting a user for two files; a districts file containing
 a state and it's district along with democrat and republican votes separated by commas, and a eligible voters file
 which contains the eligible voter count for each state. These files are read and the data is stored to ourvectors.
 The user then searches for a state within the data set. If found, the user can ask for stats or to plot the data.
 The stats command will provide the user with the, per state, calculated wasted democrat and republican votes, if the
 state is gerrymanded and if so by who and the efficiency factor as a percent, and the total voter count. The plot
 command creates a text based horizontal histogram of each district's party vote distribution as represented by D's and
 R's in the target state. The exit command will end the program.
*/

#include <iostream>
#include <cmath>       //For ceil()
#include <string>      //For .length()
#include <iomanip>     //For setprecision()
#include <sstream>     //For istringstream
#include <fstream>     //For opening files
#include "ourvector.h" //Included vector implementation header file


using namespace std;


//Struct used to define a state as having attributes of a name and a number amount of eligible voters
struct State {
    string name;
    int eligibleVoters;
};


/* Edits a target into a standard format of full lowercase without whitespace. If passed with
exception as true, after formatting the function will make the first letter of each word capital.
 This is used for states. Has no return value because target is passed by reference and edited. */
void commandFormat(string& target, //Target string to format
                   bool exception) //Exception case used if format needs to have uppercase on the first letter of every word (states)
{
    target.erase(0, target.find_first_not_of(" \t\n\f\v"));   //Removes leading whitespace.
    target.erase(target.find_last_not_of(" \t\n\r\f\v") + 1); //Removes lagging whitespace.
    transform(target.begin(), target.end(), target.begin(), ::tolower); //Converts the string to full lowercase.
    if (exception) {
        target[0] = toupper(target[0]);                //Sets the first letter to uppercase.
        for(unsigned int i = 0; i < target.length(); i++) {
            if (target[i] == ' ') {
                target[i+1] = toupper(target[i+1]);    //Sets first letter after each space to uppercase.
            }
        }
    }
}


/* Separates one string into two strings by removing whitespace between the strings. Strings first and second
 are the destiation variables and are passed by reference to be directly edited. Has no return value. */
void split(string original,  //Original string to split
    string& first,           //First half of string
    string& second,          //Second half of string
    bool exception)          //Exception case used in format function
{
    unsigned int i;
    i = original.find(" ");                 //Sets variable i to the location of the space to split
    commandFormat(original, exception);     //Formats string before split
    first = original.substr(0, i);          //Sets first string
    second = original.substr(i + 1);        //Sets second string, ignoring the space
}


/* Sets up each prompt for command with state of data, if a user has chosen a state, and prompt for command.
 String commandOriginal is passed by reference because the user command is stored in this variable. Therefore,
 there is no return value.  */
void setup(string state,              //Holds what state user searched for
           string& commandOriginal,   //Original input command by user
           bool isLoaded)             //Status of if a data file is loaded into the program
{
    cout << endl << "Data loaded? ";
    if (isLoaded) {
        cout << "Yes" << endl;
    } else {
        cout << "No" << endl;
    }
    cout << "State: " << state << endl << endl;
    cout << "Enter command: ";
    getline(cin, commandOriginal);
    cout << endl << "-----------------------------" << endl;
}


//Function to search if a state is found in the states vector. Returns the index of the state or -1 if not found
int search(ourvector<State> statesVector,  //Vector of all states on file
           string target)                  //Target string to find in districts
{
    commandFormat(target, true);    //Formats target state with exception so it does not become all lowercase
    for (int i = 0; i < statesVector.size(); i++) {           //Iterates through each element of statesVector
        if (statesVector.at(i).name.compare(target) == 0) {   //If the state is found, return it's index
            return i;
        }
    }
    return -1;  //State was not found
}


/*Parses through the first file (district data). Goes line by line and reads
 the data on file and writes vectors to push into the main 3D output vector.
 Returns true if file is able to be opened, false otherwise. */
bool loadDistricts(ourvector<ourvector<ourvector<int>>>& output, //Output vector where district data is mapped
                   ourvector<State>& statesVector,               //State vector whith list of all states
                   string districtsFileName)                     //Name of districts file
{
    ifstream input(districtsFileName);
    if (!input.is_open()) {
        cout << "Invalid first file, try again." << endl;  //File could not be opened. Check first before init. vars.
        return false;
    }
    string line,       //Used to get each line from file
           word;       //Holds the current word from the line from the file
    int district,      //Holds each district number
        democrat,      //Holds the number of democrat votes per district
        republican,    //Holds the number of republican votes per district
        i = 0;         //Iterates when we move onto a new line/ state to correctly index states
    cout << "Reading: " << districtsFileName << endl;      //File was successfully opened
    while (getline(input, line)) {
        istringstream iss(line);
        getline(iss, word, ',');                //Reads line up to first comma
        statesVector.push_back(State());
        statesVector[i].name = word;            //Since all lines start with the state, we can copy the string directly to the vector
        ourvector<ourvector<int>> stateData;    //Initializes a new 2D int vector. This vector will hold each district vector in a state and go inside the 3d output vector.
        while (getline(iss, word, ',')) {
            if (word.compare("AL") == 0) {      //Changes districts with name AL to 1 since AL indicates only one district.
                word = "1";
            }
            district = stoi(word);              //Sets district number to the integer of the word obtained
            getline(iss, word, ',');
            democrat = stoi(word);              //Sets democrat count to the integer of the word obtained
            getline(iss, word, ',');
            republican = stoi(word);            //Sets republican count to the integer of the word obtained
            ourvector<int> districtData;
            districtData.push_back(district);
            districtData.push_back(democrat);
            districtData.push_back(republican); //Pushes the data obtained from the line into the district data vector in the specific order {district, democrat, republican} so it is easy to access later in the program.
            stateData.push_back(districtData);  //Pushes the whole district data into the respective state vector
        }
        output.push_back(stateData);            //Pushes each state vector into the full output vector.
        cout << "..." << statesVector[i].name << "..." << stateData.size() << " districts total" << endl;  //Repeats data for confirmation
        i++;
    }
    input.close();   //Close file after completion
    cout << endl;
    return true;     //Returns true indicating that the file read was successful
}


/*Parses through the second file (eligible voter data). Goes line by line and reads
 the data on file and writes vectors to push into the main 3D output vector.
 Returns true if file is able to be opened, false otherwise. */
bool loadVoters(string votersFileName,
    ourvector<State>& statesVector)
{
    ifstream input(votersFileName);
    if (!input.is_open()) {                //First check to see if file can be opened before initializing variables.
        cout << "Invalid second file, try again." << endl << endl;
        return false;
    }
    string line,                     //Holds each line of file, gets split by commas.
           word;                     //Holds each word from string line.
    int index;                       //Used to hold index of state in vector once found. If -1, state not found.
    cout << "Reading: " << votersFileName << endl;
    while (getline(input, line)) {
        istringstream iss(line);
        getline(iss, word, ',');     //Get word up to a comma
        index = search(statesVector, word);  //Searches stateVector for the specified state and sets that value to index.
        if (index != -1) {           //If state is found then push eligible voter data into states vector.
            getline(iss, word, ',');
            statesVector[index].eligibleVoters = stoi(word);
            cout << "..." << statesVector[index].name << "..." << statesVector[index].eligibleVoters << " eligible voters" << endl;
        }
    }
    input.close();
    return true;
}

/* Load function is used to load user input files to the program. This is accomplished by populating four vectors with
the state of each district, the actual district number, and the number of democrat and republican votes */
bool load(ourvector<ourvector<ourvector<int>>>& districts, //Vector is populated with {district, dem vote, rep vote}.
          ourvector<State>& states, //File 2 (should be eligible voters).
          string arg) //Argument taken from original command input.
{
    string file1Name,       //Districts file
           file2Name;       //Eligible voters file
    split(arg, file1Name, file2Name, false);            //Splits argument into each file name, no special exception.
    cout << endl;
    if (loadDistricts(districts, states, file1Name)) {  //Nested loop so if the first file does not open, we will not touch the second file.
        if (loadVoters(file2Name, states)) {
            return true;                                //If both files were successfully opened, return true.
        }
    }
    return false;                                       //If either function was not successfully opened, return false.
}


//Prints out the plot for each district.
void draw(int district,     //District number
          int demVotes,     //Number of democrat votes / 100 (how many D's are printed)
          int repVotes)     //Number of republican votes / 100 (how many R's are printed)
{
    cout << "District: " << district << endl;
    for (int i = 0; i < demVotes; i++) {
        cout << "D";
    }
    for (int i = 0; i < repVotes; i++) {
        cout << "R";
    }
    cout << endl;
}


//Plot districts votes by calculating the percent of democrat and republican votes.
void plot(ourvector<ourvector<ourvector<int>>> districtsVector,   //3D vector of districts and corresponding voter data.
          ourvector<State>stateVector,                            //Vector of states and count of eligible voter population.
          int stateIndex)          //Index of what state is selected. Corresponds to index on stateVector and districtsVector.
{
    int demVotes,           //Real number of democrat votes per district.
        repVotes,           //Real number of republican votes per district.
        total,              //Total number of voters in the district.
        demVotesPercent,    //Percent of democrat votes in district.
        repVotesPercent;    //Percent of repulican votes in district.
    for (int i = 0; i < districtsVector[stateIndex].size(); i++) {
        demVotes = districtsVector[stateIndex][i][1];
        repVotes = districtsVector[stateIndex][i][2];
        total = demVotes + repVotes;
        if (demVotes < 1) {            //Special case in which votes for district is 0, avoids divide by zero.
            demVotesPercent = 0;
        } else {
            demVotesPercent = floor(100 * demVotes / total);        //Percent calculations:
        }
        if (repVotes < 1) {            //Special case in which votes for district is 0, avoids divide by zero.
            repVotesPercent = 0;
        }
        else {
            repVotesPercent = 100 - demVotesPercent;
        }
        draw(i + 1, demVotesPercent, repVotesPercent);   //Sends data to draw function to print the plot.
    }
    cout << endl;
}


/*Check to see if district is being gerrymandered. If so, function returns true, false if not. This
 is calculated by looping through the districts in the state and adding up the votes to get each party total.
 Then we calculate wasted votes and the efficiency gap and check conditions of gerrymandering. */
bool gerrymandered(ourvector<ourvector<ourvector<int>>> districts,   //3D vector of districts and corresponding voter data.
                   ourvector<State> stateVector,  //Vector of all states and count of their eligible voter population.
                   float& wastedDemVotes,         //Wasted democrat votes is calculated and stored.
                   float& wastedRepVotes,         //Wasted republican votes is calculated and stored.
                   float& efficiencyFactor,       //Efficiency factor is calculated and assigned here.
                   int stateIndex)                //Index of what state selected. Corresponds to index on stateVector.
{
    float halfOver = 0;       //Float to hold over half benchmark for winner
    int demVotes = 0,         //Democrat and republican vote totals are initialized and set to 0.
        repVotes = 0,
        totalCast = 0,
        totalCastState = 0;
    for (int i = 0; i < districts[stateIndex].size(); i++) {   //Iterates through all districts in state and adds votes
        demVotes = districts[stateIndex][i][1];
        repVotes = districts[stateIndex][i][2];
        totalCast = demVotes + repVotes;
        halfOver = ceil((demVotes + repVotes)/2)+1;       //Calculating number needed for win condition
        if (demVotes > repVotes) {                        //Democrat win.
            wastedRepVotes = wastedRepVotes + repVotes;   //If Democrat win, all loosing party votes are wasted and vice versa
            wastedDemVotes = wastedDemVotes + demVotes - halfOver;
        } else {                                          //Republican win.
            wastedDemVotes = wastedDemVotes + demVotes;
            wastedRepVotes = wastedRepVotes + repVotes - halfOver;
        }
        totalCastState = totalCastState + totalCast;
    }
    efficiencyFactor = 100 * abs(wastedDemVotes - wastedRepVotes) / totalCastState; //Calculating efficiency factor as percent.
    if (districts[stateIndex].size() < 3) {     //Condition: if a state has less than 3 districts, it cannot be gerrymanded.
        return false;
    }
    if (efficiencyFactor >= 7) {                //Condition: if EF >=7%, then gerrymanded. If < then not gerrymanded.
        return true;
    } else {
        return false;
    }
}

//Displays stats calculated for state.
void stats(ourvector<ourvector<ourvector<int>>> districts,   //3D vector of all voter data. Used to calculate gerrymandering.
           ourvector<State> stateVector,                     //Vector of all states and count of their eligible voter
           int stateIndex)                                          //Vector index of what state is selected.
{
    float wastedDemVotes = 0,    //Wasted party vote counts as float to maintain precisionm when calculating efficiency factor.
          wastedRepVotes = 0,
          efficiencyFactor = 0;
    cout << endl << "Gerrymandered: ";
    if (gerrymandered(districts, stateVector, wastedDemVotes, wastedRepVotes, efficiencyFactor, stateIndex)) { //If the state is gerrymandered.
        cout << "Yes" << endl;
        cout << "Gerrymandered against: ";
        if (wastedDemVotes > wastedRepVotes) {
            cout << "Democrats" << endl;
        } else {
            cout << "Republicans" << endl;
        }
        cout << "Efficiency Factor: " << setprecision(6) << efficiencyFactor << "%" << endl;
    } else {                   //If the state is not gerryandered.
        cout << "No" << endl;
    }
    cout << "Wasted Democratic votes: " << int(wastedDemVotes) << endl;   //Following is printed regardless of GM outcome
    cout << "Wasted Republican votes: " << int(wastedRepVotes) << endl;
    cout << "Eligible voters: " << stateVector[stateIndex].eligibleVoters << endl << endl;
}


//Function to see what command was made and respond appropriately.
void commandCheck(ourvector<ourvector<ourvector<int>>>& districts, //3D vector of district data, used during all of program.
                  ourvector<State>& states,           //Vector of States structs, eligible voter count. Used throughout program
                  string command,                     //Load, search, plot, stats
                  string arg,                         //Argument 1 from user input (Txt file or state)
                  string& loadedState,                //Used in search() to change target state
                  int& stateIndex,                    //Vector index of target state. Edited throughout program.
                  bool& stopCondition,                //Boolean check if app should still be running. Is edited during exit.
                  bool& isDataLoaded)                 //Stores status of if data is loaded.
{
    if (command.compare("load") == 0) {               //Command: Load
        if (!isDataLoaded) {                          //If there is currently no data then attempt to load the files.
            if (load(districts, states, arg)) { //If files were successfully loaded then change dataLoaded status to true.
                isDataLoaded = true;
            }
        } else {
            cout << "Already read data in, exit and start over." << endl;   //If files were previously read, deny another load.
        }
    } else if (command.compare("search") == 0) {        //Command: Search
        if (states.size() == 0) {                       //If the first districts file never loaded, prompt for data.
            cout << "No data loaded, please load data first.";
        } else {                                        //If there is valid states in the states vector then search for state.
            commandFormat(arg, false);
            stateIndex = search(states, arg);
            if (stateIndex != -1) {                     //If state has valid index then make it the loaded state
                loadedState = arg;
            } else {
                cout << "State does not exist, search again.";
            }
        }
    } else if (command.compare("plot") == 0) {           //Command: Plot
        if (!isDataLoaded) {                             //Check if data file is loaded, prompt for file if not.
            cout << "No data loaded, please load data first.";
        } else {
            plot(districts, states, stateIndex);         //Plot function
        }
    } else if (command.compare("stats") == 0) {          //Command: Stats
        if (loadedState.compare("N/A") == 0) {           //Check if there is a target state loaded in. Prompt for state if not.
            cout << "No state indicated, please search for state first.";
        } else if (!isDataLoaded) {                      //Check if data file is loaded, prompt foe file if not.
            cout << "No data loaded, please load data first.";
        } else {
            stats(districts, states, stateIndex);        //Stats function.
        }
    } else if (command.compare("exit") == 0) {           //Command: Exit
        stopCondition = true;
    } else {                                             //Command unrecognized
        cout << "Unrecognized command." << endl;
    }
}

int main() {
    /*3D vector; outer vector holds vectors corresponding to each state.
    Middle vector holds vectors corresponding to each district in the parent state.
    Interior vectors hold {district number, democrat votes, republican votes}*/
    ourvector<ourvector<ourvector<int>>> districts;
    
    ourvector<State> allStates;  //Vector of State structs containing the state and it's eligible voter count
    
    string commandWhole,         //Original command input by user.
           command,              //Command portion of input (load, search, plot, state, exit).
           arg,                  //The rest of the input that isn't the command (text file names, states).
           loadedState = "N/A";  //Holds what state the user searched for.
    
    int stateIndex;              //Index number of the selected state corresponding to the allStates vector.

    bool stopCondition = false,  //Determines whether app will continue or not. Used for exit command.
         dataLoaded = false;     //Holds status of input file collection.
    
    cout << "Welcome to the Gerrymandering App!" << endl;
    while (!stopCondition) //App will keep running indefinitely until prompted with "exit".
    {
        setup(loadedState, commandWhole, dataLoaded);                //Prompts for command.
        split(commandWhole, command, arg, false);                    //Parses command.
        commandCheck(districts, allStates, command, arg, loadedState, stateIndex, stopCondition, dataLoaded); //Checks what to do with command
    }
    return 0;
}
