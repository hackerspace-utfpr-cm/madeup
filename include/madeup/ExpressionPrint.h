#ifndef ELEMENTPRINT_H
#define ELEMENTPRINT_H

#include "madeup/Expression.h"

namespace madeup {

/* ------------------------------------------------------------------------- */

class ExpressionPrint : public Expression {
  public:
    ExpressionPrint();

    static bool print(const td::Co<Expression>& value);

    td::Co<Expression> evaluate(Environment &env) const;
    void write(ostream &out) const;

  private:
};

/* ------------------------------------------------------------------------- */

}

#endif
