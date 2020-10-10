#include <string.h>
#include <string>

using namespace std; 
extern bool have_interface;
extern bool have_file;
extern string interface;
extern string file;

void parse_args(int argc, char *argv[]);
void print_help();