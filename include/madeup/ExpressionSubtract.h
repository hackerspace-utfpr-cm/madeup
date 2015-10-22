#ifndef EXPRESSIONSUBTRACT_H
#define EXPRESSIONSUBTRACT_H

#include "madeup/Expression.h"

namespace madeup {

/* ------------------------------------------------------------------------- */

class ExpressionSubtract : public Expression {
  public:
    ExpressionSubtract(Co<Expression> left, Co<Expression> right);

    Co<Expression> evaluate(Environment &env) const;
    void write(ostream &out) const;
    static Co<Expression> subtract(Co<Expression> l,
                                   Co<Expression> r,
                                   const std::string &source,
                                   const SourceLocation &location,
                                   Environment &env);

  private:
    Co<Expression> left;
    Co<Expression> right;
};

/* ------------------------------------------------------------------------- */

}

#endif
