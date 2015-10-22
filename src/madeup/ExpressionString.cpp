#include "madeup/ExpressionString.h"

using std::string;

namespace madeup {

/* ------------------------------------------------------------------------- */

ExpressionString::ExpressionString(const std::string &value) :
  ExpressionPrimitive(),
  value(value) {
}

/* ------------------------------------------------------------------------- */

Co<Expression> ExpressionString::evaluate(Environment &env) const {
  return Co<Expression>(new ExpressionString(value));
}

/* ------------------------------------------------------------------------- */

string ExpressionString::getString() const {
  return value;
}

/* ------------------------------------------------------------------------- */

void ExpressionString::write(ostream &out) const {
  out << "(STRING " << value << ")";
}

/* ------------------------------------------------------------------------- */

std::string ExpressionString::toString() const {
  return value;
}

/* ------------------------------------------------------------------------- */

}
