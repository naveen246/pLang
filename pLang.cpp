
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>


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

//AST
class ExprAST {
    virtual ~ExprAST() {}
};

class NumberExprAST : public ExprAST {
    double Val;

    public :
        NumberExprAST( double num ) : Val( num ) {}
};

class VariableExprAST : public ExprAST {
    std::string Name;

    public :
        VariableExprAST( const std::string &name ) : Name( name ) {}
};

class BinaryExprAST : public ExprAST {
    char Op;
    ExprAST *LHS, *RHS;

    public :
        BinaryExprAST( char op, ExprAST *lhs, ExprAST *rhs ) 
                            : Op( op ), LHS( lhs ), RHS( rhs ) {}
};

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<ExprAST*> Args;

    public :
        CallExprAST( const std::string &callee, std::vector<ExprAST*> args ) 
                            : Callee( callee ), Args( args ) {}
};

class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

    public :
        PrototypeAST( const std::string &name, const std::vector<std::string> &args ) 
                            : Name( name ), Args( args ) {}
};

class FunctionAST {
    PrototypeAST *Proto;
    ExprAST *Body;

    public :
        FunctionAST( PrototypeAST *proto, ExprAST *body )
                            : Proto( proto ), Body( body ) {}
};

static int CurTok;
static int getNextToken() {
    return CurTok = gettok();
}

ExprAST *Error( const char *errMsg ) {
    fprintf(stderr, "Error : %s\n", errMsg);
    return 0;
}

PrototypeAST *ErrorP( const char *errMsg ) { 
    Error( errMsg ); 
    return 0;
}

FunctionAST *ErrorF( const char *errMsg ) { 
    Error( errMsg ); 
    return 0;
}

static ExprAST *ParseNumberExpr() {
    ExprAST *Result = new NumberExprAST( NumVal );
    getNextToken();
    return Result;
}

static ExprAST *ParseParenExpr() {
    getNextToken();
    ExprAST *Result = ParseExpression();
    if( ! Result ) return 0;

    if( CurTok != ')' ) {
        return Error( "Expected ')'" )
    }
    getNextToken();
    return Result;
}

static ExprAST *ParseIdentifierExpr() {
    std::string IdName = IdentifierStr;
    getNextToken(); //eat identifier
    if( CurTok != '(' ) {
        return new VariableExprAST( IdName );
    }
    getNextToken(); //eat '('
    std::vector<ExprAST*> Args;
    if( CurTok != ')' ) {
        while( 1 ) {
            ExprAST *Arg = ParseExpression();
            if( ! Arg ) {
                return 0;
            }
            Args.push_back( Arg );
            
            if( CurTok == ')' ) {
                break;
            }

            if( CurTok != ',' ) {
                Error( "Expected ')' or ','" )
            }
            getNextToken();
        }
    }
    getNextToken();     
    return new CallExprAST( IdName, Args );
}

static ExprAST *ParsePrimary() {
    switch( CurTok ) {
        case tok_number     : return ParseNumberExpr();
        case tok_identifier : return ParseIdentifierExpr();
        case '('            : return ParseParenExpr();
        default             : Error( "Expected expression" );
    }
}

std::map<char, int> BinopPrecedence;

static int GetTokPrecedence() {
    if( ! isascii( CurTok ) ) {
        return -1;
    }
    int TokPrec = BinopPrecedence[ CurTok ];
    if( TokPrec <= 0 ) {
        return -1;
    }
    return TokPrec;
}

static ExprAST *ParseExpression() {
    ExprAST *lhs = ParsePrimary();
    if( ! lhs ) {
        return 0;
    }
    return ParseBinOpRHS( 0, lhs );
}

static ExprAST *ParseBinOpRHS( int ExpPrec, ExprAST *lhs ) {
    while( 1 ) {
        int TokPrec = GetTokPrecedence();
        if( TokPrec < ExpPrec ) {
            return lhs;
        }
        int op = CurTok;
        getNextToken();
        ExprAST *rhs = ParsePrimary();
        if( ! rhs ) return 0;

        int NextPrec = GetTokPrecedence();
        if( TokPrec < NextPrec ) {
            rhs = ParseBinOpRHS( TokPrec + 1, rhs );
            if( rhs == 0 ) return 0;
        }
        lhs = new BinaryExprAST( op, lhs, rhs );
    }
}
















int main(){
    BinopPrecedence[ '<' ] = 1;
    BinopPrecedence[ '+' ] = 2;
    BinopPrecedence[ '-' ] = 2;
    BinopPrecedence[ '*' ] = 4;
    return 0;
}