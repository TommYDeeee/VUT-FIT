#include "arg_parser.h"

bool have_interface = false, have_file = false;
string file, interface;

/* 
* Print help
*/
void print_help(){
    printf("Simple ssl monitoring program that prints basic information about SSL connection into STDOUT\n");
    printf("\n");
    printf("BASIC INFORMATION ARE: <timestamp>,<client IP>,<client PORT>,<server IP>,<bytes>,<SNI>,<packets>,<duration sec>\n");
    printf("\n");
    printf("SUPPORTED ARGUMENTS: <-r FILE> <-i INTERFACE>\n");
    printf("FILE must be a valid file and INTERFACE must be a valid network interface\n");
    printf("\n");
    printf("EXAMPLES:\n");
    printf("./sslsniff -r seznam.pcapng");
    printf("./sslsniff -i any");
    printf("\n");
    printf("AUTHOR: Tomas Duris (xduris05)\n");
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
        fprintf(stderr, "WRONG ARGUMENT COUNT\n");
        fprintf(stderr, "To see help start program with 0 arguments\n");
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
                fprintf(stderr, "WRONG ARGUMENT\n");
                fprintf(stderr,"To see help start program with 0 arguments\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}