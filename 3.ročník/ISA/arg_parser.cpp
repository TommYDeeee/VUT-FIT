#include "arg_parser.h"

bool have_interface = false, have_file = false;
string file, interface;

void print_help(){
    cout << "TODO HELP";
}

void parse_args(int argc, char *argv[]){
    bool have_parameter_interface = false, have_parameter_file = false;
    if(argc == 1){
        print_help();
        exit(0);
    } else if  (argc > 5){
        cout << "WRONG ARGUMENT COUNT";
        exit(1);
    } 
    else {
        for (int i = 1; i < argc; i++){
            if(have_parameter_interface){
                interface = argv[i];
                have_parameter_interface = false;
                continue;
            }
            if(have_parameter_file){
                file = argv[i];
                have_parameter_file = false;
                continue;
            }
            if(strcmp(argv[i], "-r") == 0){
                have_parameter_file = true;
                have_file = true;
                continue;
            } else if (strcmp(argv[i], "-i") == 0){
                have_parameter_interface = true;
                have_interface = true;
                continue;
            } else {
                cout << "WRONG ARGUMENT";
                exit(1);
            }
        }
    }
}