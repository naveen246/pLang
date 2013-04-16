
#include <stdio.h>
#include <ctype.h>
#include <string>


#define MAX_TOK_SIZE 25

enum Token {
    tok_eof = -1,

    tok_def = -2, tok_extern = -3,

    tok_identifier = -4, tok_number = -5
};

static std::string IdentifierStr;
static double NumVal;

static int gettok() {
    int c = getchar();

    while( isspace( c ) ) {
        c = getchar();
    }

    if( isalpha( c ) ) {

        while( isalnum( c ) ) {
            IdentifierStr += c;
            c = getchar();
        }

        if( IdentifierStr == "def" ) {
            return tok_def;
        } 
        else if( IdentifierStr == "extern" ) {
            return tok_extern;
        } 
        else {
            return tok_identifier;
        }
    } 
    else if( isdigit( c ) || c == '.' ) {
        std::string NumStr;

        while( isdigit( c ) || c == '.' ) {
            NumStr += c;
            c = getchar();
        }
        NumVal = strtod( NumStr.c_str(), 0 );
        return tok_number;
    } 
    else if ( c == '#' ) {

        while( c != '\n' && c != '\r' && c != EOF ) {
            c = getchar();
        }
        if( c != EOF ) {
            return gettok();
        }
    } 
    if( c == EOF ) {
        return tok_eof;
    } 
    return c;
}