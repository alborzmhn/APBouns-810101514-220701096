#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

using namespace std;

const string LOCATION = "Location ";
const string NAME = "name";
const string OPENINGTIME = "openingTime";
const string CLOSINGTIME = "closingTime";
const string RANK = "rank";
const string END_PRINT = "---";

struct place_info{
    vector<string> name;
    vector<int> openingTime;
    vector<int> closingTime;
    vector<int> rank;
    vector<int> visited_or_not;
};

const string VISIT_FROM = "Visit from ";
const string UNTIL = " until ";

int find_number_of_places(vector<string> readed_string){
    int num_of_places = readed_string.size() / 4 - 1;
    return num_of_places;
}

vector<int> find_order_of_datas(vector<string> readed_string){
    vector<int> main_order;
    vector<string> desired_order = { NAME,OPENINGTIME,CLOSINGTIME,RANK };
    for (int i=0;i<4;i++){ 
        for (int j = 0;j<4; j++){ 
            if (desired_order[i] == readed_string[j]){
                main_order.push_back(j);
            }
        }
    }
    return main_order;
}

int convert_time_to_minutes(string string_time){
    int minutes = 0;
    minutes = stoi(string_time.substr(0, 2)) * 60 + stoi(string_time.substr(3, 2));
    return minutes;
}

place_info create_struction_for_places(vector<string> input_string){
    place_info place_info;
    int number_of_places = find_number_of_places(input_string);
    vector<int> order_of_informations = find_order_of_datas(input_string);
    for (int i = 1; i <= number_of_places; i++){
        place_info.name.push_back(input_string[order_of_informations[0] + i * 4]);
        place_info.openingTime.push_back(convert_time_to_minutes(input_string[order_of_informations[1] + i * 4]));
        place_info.closingTime.push_back(convert_time_to_minutes(input_string[order_of_informations[2] + i * 4]));
        int rank = stoi(input_string[order_of_informations[3] + i * 4]);
        place_info.rank.push_back(rank);
        place_info.visited_or_not.push_back(0);
    }
    return place_info;
}

place_info read_input_from_file(string input_address){
    vector<string> readed_file;
    string filename = input_address;
    ifstream input_file(filename);
    string line_of_file;
    while (getline(input_file, line_of_file)){
        istringstream line(line_of_file);
        string file_element;
        while (getline(line, file_element, ',')){
            readed_file.push_back(file_element);
        }
    }
    input_file.close();
    place_info place_info = create_struction_for_places(readed_file);
    return place_info;
}

string convert_minutes_to_time(int minutes_from_midnight){
    string time = "";
    int hours = minutes_from_midnight / 60;
    int minutes = minutes_from_midnight % 60;
    if (hours < 10){
        time += "0";
    }
    time += to_string(hours);
    time += ":";
    if (minutes < 10){
        time += "0";
    }
    time += to_string(minutes);
    return time;
}

void print_output(int start, int finish, place_info place_info, int place_index){
    cout << LOCATION <<  place_info.name[place_index] << endl;
    cout << VISIT_FROM << convert_minutes_to_time(start) << UNTIL;
    cout << convert_minutes_to_time(finish) << endl << END_PRINT << endl;
}

bool check_place_is_open_or_not(int& start, int place_index, place_info place_info){
    return (start >= place_info.openingTime[place_index] && start <= place_info.closingTime[place_index]);
}

bool check_place_is_visitable_or_not(int& start, int place_index, place_info place_info){
    return (start + 30 + 15 <= place_info.closingTime[place_index]);
}

int choose_better_place_to_visit(int start, int chosen_place_index, int place_index, place_info place_info){
    if (chosen_place_index == -1){
        return place_index;
    }
    if (start >= place_info.openingTime[place_index]){
        if (place_info.rank[place_index] < place_info.rank[chosen_place_index]){
            return place_index;
        } 
        else{ 
            return chosen_place_index;
        }
    }
    else{
        if (place_info.openingTime[place_index] <= place_info.openingTime[chosen_place_index] && place_info.rank[place_index] < place_info.rank[chosen_place_index]){ 
            return place_index;
        }
        return chosen_place_index;
    }
}

void update_start_and_finish(int& start, int& finish, place_info place_info,int chosen_place_index){
    if (start + 30 < place_info.openingTime[chosen_place_index]){
        start = place_info.openingTime[chosen_place_index];
    }
    else{
        start += 30;
    }
    if (place_info.closingTime[chosen_place_index] - start >= 60){
        finish = start + 60;
    }
    else{
        finish = place_info.closingTime[chosen_place_index];
    }
}

int find_best_place(int &start, int &finish, place_info &place_info){
    int chosen_place_index = -1;
    for (int i = 0; i < place_info.name.size(); i++){
        if (check_place_is_open_or_not(start,i,place_info) && check_place_is_visitable_or_not(start, i, place_info) && place_info.visited_or_not[i] == 0){
            chosen_place_index = choose_better_place_to_visit(start,chosen_place_index, i, place_info);
        }
    }
    if (chosen_place_index == -1){
        for (int i = 0; i < place_info.name.size(); i++){
            if (start < place_info.openingTime[i] && check_place_is_visitable_or_not(start, i, place_info) && place_info.visited_or_not[i] == 0){
                chosen_place_index = choose_better_place_to_visit(start, chosen_place_index, i, place_info);
            }
        }
    }
    if (chosen_place_index >= 0){
        place_info.visited_or_not[chosen_place_index] = 1;
        update_start_and_finish(start, finish, place_info, chosen_place_index);
    }  
    return chosen_place_index;
}

int main(int argc, char* argv[]){
    place_info place_info = read_input_from_file(argv[1]);
    int finnish = 8 * 60, start = 8 * 60 - 30;
    int index_of_place_to_visit = find_best_place(start, finnish, place_info);
    while (index_of_place_to_visit != -1){
        print_output(start, finnish, place_info, index_of_place_to_visit);
        start = finnish;
        index_of_place_to_visit = find_best_place(start, finnish, place_info);
    }
    return 0;
}
