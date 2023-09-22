#include "esd.h"

Value value_add(Value left, Value right) {
  switch (left.type) {
    case ValueType_Int:
      switch (right.type) {
        case ValueType_Int:
          return (Value) {
            .type = ValueType_Int, ._int = left._int + right._int
          };
        case ValueType_Float:
          return (Value) {
            .type = ValueType_Float, ._float = left._int + right._float
          };
      }
    case ValueType_Float:
      switch (right.type) {
        case ValueType_Int:
          return (Value) {
            .type = ValueType_Float, ._float = left._float + right._int
          };
        case ValueType_Float:
          return (Value) {
            .type = ValueType_Float, ._float = left._float + right._float
          };
      }
  }
}

Value value_sub(Value left, Value right) {
  switch (left.type) {
    case ValueType_Int:
      switch (right.type) {
        case ValueType_Int:
          return (Value) {
            .type = ValueType_Int, ._int = left._int - right._int
          };
        case ValueType_Float:
          return (Value) {
            .type = ValueType_Float, ._float = left._int - right._float
          };
      }
    case ValueType_Float:
      switch (right.type) {
        case ValueType_Int:
          return (Value) {
            .type = ValueType_Float, ._float = left._float - right._int
          };
        case ValueType_Float:
          return (Value) {
            .type = ValueType_Float, ._float = left._float - right._float
          };
      }
  }
}

Value value_mul(Value left, Value right) {
  switch (left.type) {
    case ValueType_Int:
      switch (right.type) {
        case ValueType_Int:
          return (Value) {
            .type = ValueType_Int, ._int = left._int * right._int
          };
        case ValueType_Float:
          return (Value) {
            .type = ValueType_Float, ._float = left._int * right._float
          };
      }
    case ValueType_Float:
      switch (right.type) {
        case ValueType_Int:
          return (Value) {
            .type = ValueType_Float, ._float = left._float * right._int
          };
        case ValueType_Float:
          return (Value) {
            .type = ValueType_Float, ._float = left._float * right._float
          };
      }
  }
}

Value value_div(Value left, Value right) {
  switch (left.type) {
    case ValueType_Int:
      switch (right.type) {
        case ValueType_Int:
          return (Value) {
            .type = ValueType_Int, ._int = left._int / right._int
          };
        case ValueType_Float:
          return (Value) {
            .type = ValueType_Float, ._float = left._int / right._float
          };
      }
    case ValueType_Float:
      switch (right.type) {
        case ValueType_Int:
          return (Value) {
            .type = ValueType_Float, ._float = left._float / right._int
          };
        case ValueType_Float:
          return (Value) {
            .type = ValueType_Float, ._float = left._float / right._float
          };
      }
  }
}

Value binary_operation(BinaryOperation * operation) {
  switch (operation->op) {
    case BinaryOperator_Add:
      return value_add(eval(operation->left), eval(operation->right));
    case BinaryOperator_Sub:
      return value_sub(eval(operation->left), eval(operation->right));
    case BinaryOperator_Mul:
      return value_mul(eval(operation->left), eval(operation->right));
    case BinaryOperator_Div:
      return value_div(eval(operation->left), eval(operation->right));
  }
}

Value eval(Expr * expr) {
  switch (expr->type) {
    case ExprType_BinaryOperation:
      return binary_operation(&expr->_binary_operation);
    case ExprType_Literal:
      return expr->_literal;
  }
}

Expr * new_binary_operation(BinaryOperator op, Expr * left, Expr * right) {
  Expr * n = (Expr*) malloc(sizeof(Expr));
  n->type = ExprType_BinaryOperation;
  n->_binary_operation = (BinaryOperation) {
    .op = op, .left = left, .right = right
  };
  return n;
}

Expr * new_literal(Value value) {
  Expr * n = (Expr*) malloc(sizeof(Expr));
  n->type = ExprType_Literal;
  n->_literal = value;
  return n;
}

void free_expr(Expr * expr) {
  switch (expr->type) {
    case ExprType_BinaryOperation:
      free_expr(expr->_binary_operation.left);
      free_expr(expr->_binary_operation.right);
      break;
    case ExprType_Literal:
      break;
  }
  free(expr);
}

bool parser_char(Parser * parser, char expected_char) {
  if (parser->src.slice[parser->i] == expected_char) {
    parser->i++; // Consume the expected character
    return true;
  }
  return false;
}

// Helper function to check if a character is an operator
bool is_operator(char c) {
  return c == '+' || c == '-' || c == '*' || c == '/';
}

// Helper function to get the precedence of an operator
int get_precedence(char op) {
  switch (op) {
    case '+':
    case '-':
      return 1;
    case '*':
    case '/':
      return 2;
    default:
      return 0; // Default precedence for non-operators
  }
}

// Parse a value (literal)
Expr * parse_value(Parser * parser) {
  Value value;
  if (parser->src.slice[parser->i] == '0' && parser->src.slice[parser->i + 1] == 'x') {
    // Handle hexadecimal literals
    sscanf(parser->src.slice + parser->i, "0x%x", &value._int);
    while (isdigit(parser->src.slice[parser->i])) {
      parser->i++;
    }
    value.type = ValueType_Int;
  } else {
    // Handle decimal literals
    sscanf(parser->src.slice + parser->i, "%lf", &value._float);
    while (isdigit(parser->src.slice[parser->i])
           || parser->src.slice[parser->i] == '.') {
      parser->i++;
    }
    value.type = ValueType_Float;
  }
  Expr * expr = new_literal(value);
  return expr;
}

// Parse a binary operation
Expr * parse_binary(Parser * parser, int min_precedence) {
  Expr * left = parse_primary(parser);
  while (is_operator(parser->src.slice[parser->i])) {
    char op = parser->src.slice[parser->i];
    int precedence = get_precedence(op);
    if (precedence < min_precedence) {
      return left;
    }
    parser->i++; // Consume the operator
    Expr * right = parse_binary(parser, precedence + 1);
    BinaryOperator bin_op;
    switch (op) {
      case '+':
        bin_op = BinaryOperator_Add;
        break;
      case '-':
        bin_op = BinaryOperator_Sub;
        break;
      case '*':
        bin_op = BinaryOperator_Mul;
        break;
      case '/':
        bin_op = BinaryOperator_Div;
        break;
      default:
        fprintf(stderr, "Unexpected character. '%c' \n", op);
        exit(1);
    }
    left = new_binary_operation(bin_op, left, right);
  }
  return left;
}

// Parse an expression
Expr * parse_expr(Parser * parser) {
  return parse_binary(parser, 0);
}

// Parse a primary expression (either a value or a subexpression in parentheses)
Expr * parse_primary(Parser * parser) {
  char c = parser->src.slice[parser->i];
  if (isdigit(c) || (c == '-' && isdigit(parser->src.slice[parser->i + 1]))) {
    return parse_value(parser);
  } else if (c == '(') {
    parser->i++; // Consume '('
    Expr * expr = parse_expr(parser);
    if (parser_char(parser, ')')) {
      return expr;
    } else {
      fprintf(stderr, "Error: Expected closing parenthesis\n");
      exit(1);
    }
  } else {
    fprintf(stderr, "Error: Unexpected character '%c'\n", c);
    exit(1);
  }
}

// Main parsing function
Expr * parse(Parser * parser) {
  return parse_expr(parser);
}

// Helper function to print binary operator as a string
const char * binary_operator_to_string(BinaryOperator op) {
  switch (op) {
    case BinaryOperator_Add:
      return "+";
    case BinaryOperator_Sub:
      return "-";
    case BinaryOperator_Mul:
      return "*";
    case BinaryOperator_Div:
      return "/";
  }
}

// Print an Expr in S-expression syntax
void print_expr(Expr * expr) {
  switch (expr->type) {
    case ExprType_BinaryOperation:
      print_binary_operation(&expr->_binary_operation);
      break;
    case ExprType_Literal:
      print_value(&expr->_literal);
      break;
  }
}

// Print a BinaryOperation in S-expression syntax
void print_binary_operation(BinaryOperation * bin_op) {
  printf("(");
  printf("%s", binary_operator_to_string(bin_op->op));
  printf(" ");
  print_expr(bin_op->left);
  printf(" ");
  print_expr(bin_op->right);
  printf(")");
}

// Print a Value in S-expression syntax
void print_value(Value * value) {
  switch (value->type) {
    case ValueType_Int:
      printf("%d", value->_int);
      break;
    case ValueType_Float:
      printf("%lf", value->_float);
      break;
  }
}
