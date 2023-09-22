#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef size_t usize;
typedef struct Token Token;
typedef struct Parser Parser;
typedef struct BinaryOperation BinaryOperation;
typedef struct Expr Expr;
typedef struct Value Value;

enum ValueType {
  ValueType_Int,
  ValueType_Float,
};

enum BinaryOperator {
  BinaryOperator_Add,
  BinaryOperator_Sub,
  BinaryOperator_Mul,
  BinaryOperator_Div,
};

enum ExprType {
  ExprType_BinaryOperation,
  ExprType_Literal,
};

typedef enum BinaryOperator BinaryOperator;
typedef enum ExprType ExprType;
typedef enum ValueType ValueType;

struct Token {
  char * slice;
  usize len;
};

static Token Token_NONE = (Token) {
  .slice = NULL, .len = 0
};

struct Parser {
  Token src;
  usize i;
};

struct Value {

  ValueType type;

  union {
    int _int;
    double _float;
  };
};

struct BinaryOperation {
  BinaryOperator op;
  Expr * left;
  Expr * right;
};

struct Expr {
  ExprType type;

  union {
    BinaryOperation _binary_operation;
    Value _literal;
  };
};

Value value_add(Value left, Value right);
Value value_sub(Value left, Value right);
Value value_mul(Value left, Value right);
Value value_div(Value left, Value right);
Value binary_operation(BinaryOperation * operation);
Value eval(Expr * expr);
Expr * new_binary_operation(BinaryOperator op, Expr * left, Expr * right);
Expr * new_literal(Value value);
void free_expr(Expr * expr);
bool parser_char(Parser * parser, char expected_char);
bool is_operator(char c);
int get_precedence(char op);
Expr * parse_value(Parser * parser);
Expr * parse_binary(Parser * parser, int min_precedence);
Expr * parse_expr(Parser * parser);
Expr * parse_primary(Parser * parser);
Expr * parse(Parser * parser);
const char * binary_operator_to_string(BinaryOperator op);
void print_expr(Expr * expr);
void print_binary_operation(BinaryOperation * bin_op);
void print_value(Value * value);
