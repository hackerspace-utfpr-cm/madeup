#ifndef ELEMENTAXIS_H
#define ELEMENTAXIS_H

#include "madeup/Expression.h"
#include "madeup/ExpressionUnit.h"

namespace madeup {

/* ------------------------------------------------------------------------- */

class ExpressionAxis : public Expression {
  public:
    ExpressionAxis() :
      Expression() {
    }

    Co<Expression> Evaluate(Environment& env) {
      Co<ExpressionClosure> x = env["x"];
      Co<ExpressionClosure> y = env["y"];
      Co<ExpressionClosure> z = env["z"];

      if (x.IsNull()) {
        throw MessagedException(GetSourceLocation() + ": Function axis expects a value named x. No x was defined.");
      }

      if (y.IsNull()) {
        throw MessagedException(GetSourceLocation() + ": Function axis expects a value named y. No y was defined.");
      }

      if (z.IsNull()) {
        throw MessagedException(GetSourceLocation() + ": Function axis expects a value named z. No z was defined.");
      }

      Co<Expression> x_value = x->Evaluate(env);
      Co<Expression> y_value = y->Evaluate(env);
      Co<Expression> z_value = z->Evaluate(env);

      double xd;
      ExpressionDecimal *x_double = dynamic_cast<ExpressionDecimal *>(x_value.GetPointer());
      if (x_double) {
        xd = x_double->GetDecimal();
      } else {
        ExpressionInteger *x_int = dynamic_cast<ExpressionInteger *>(x_value.GetPointer());
        if (x_int) {
          xd = x_int->GetInteger();
        } else {
          throw MessagedException(x->GetSourceLocation() + ": Function axis expects its x parameter to be numeric.");
        }
      }

      double yd;
      ExpressionDecimal *y_double = dynamic_cast<ExpressionDecimal *>(y_value.GetPointer());
      if (y_double) {
        yd = y_double->GetDecimal();
      } else {
        ExpressionInteger *y_int = dynamic_cast<ExpressionInteger *>(y_value.GetPointer());
        if (y_int) {
          yd = y_int->GetInteger();
        } else {
          throw MessagedException(y->GetSourceLocation() + ": Function axis expects its y parameter to be numeric.");
        }
      }

      double zd;
      ExpressionDecimal *z_double = dynamic_cast<ExpressionDecimal *>(z_value.GetPointer());
      if (z_double) {
        zd = z_double->GetDecimal();
      } else {
        ExpressionInteger *z_int = dynamic_cast<ExpressionInteger *>(z_value.GetPointer());
        if (z_int) {
          zd = z_int->GetInteger();
        } else {
          throw MessagedException(z->GetSourceLocation() + ": Function axis expects its z parameter to be numeric.");
        }
      }

      env.Axis(xd, yd, zd);
      return Co<Expression>(new ExpressionUnit());
    }

    void Write(ostream& out) const {
      out << "(axis x y z)";
    }

  private:
};

/* ------------------------------------------------------------------------- */

}

#endif
