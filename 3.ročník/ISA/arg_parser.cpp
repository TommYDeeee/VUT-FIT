#include "arg_parser.h"

bool have_interface = false, have_file = false;
string file, interface;

/* 
* Print help
*/
void print_help(){
    printf("TODO HELP\n");
}

/*
* Parse command line arguments, get file or interface name or print help
*/
void parse_args(int argc, char *argv[]){
    bool have_parameter_interface = false, have_parameter_file = false;
    if(argc == 1){
        print_help();
        exit(EXIT_SUCCESS);
    } else if  (argc > 5){
        printf("WRONG ARGUMENT COUNT\n");
        exit(EXIT_FAILURE);
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
                printf("WRONG ARGUMENT\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}