#ifndef ELEMENTINVERSETANGENT_H
#define ELEMENTINVERSETANGENT_H

#include "madeup/Expression.h"

namespace madeup {

/* ------------------------------------------------------------------------- */

class ExpressionInverseTangent : public Expression {
  public:
    ExpressionInverseTangent();

    td::Co<Expression> evaluate(Environment &env) const;
    void write(ostream &out) const;

  private:
};

/* ------------------------------------------------------------------------- */

class ExpressionInverseTangent2 : public Expression {
  public:
    ExpressionInverseTangent2();

    td::Co<Expression> evaluate(Environment &env) const;
    void write(ostream &out) const;

  private:
};

/* ------------------------------------------------------------------------- */

}

#endif
