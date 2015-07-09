#include "madeup/ExpressionClosure.h"
#include "madeup/ExpressionMoveTo.h"
#include "madeup/ExpressionNumber.h"
#include "madeup/ExpressionReal.h"
#include "madeup/ExpressionUnit.h"
#include "twodee/MessagedException.h"

using namespace td;

namespace madeup {

/* ------------------------------------------------------------------------- */

ExpressionMoveTo::ExpressionMoveTo() :
  Expression() {
}

/* ------------------------------------------------------------------------- */

Co<Expression> ExpressionMoveTo::evaluate(Environment &env) const {
  Co<ExpressionClosure> xyz[3] = {
    env["x"],
    env["y"],
    env["z"],
  };
  float v[3];

  if (xyz[0].IsNull()) {
    throw MessagedException(getSourceLocation().toAnchor() + ": Function moveto expects a value named x. No value named x is defined.");
  }

  if (xyz[1].IsNull()) {
    throw MessagedException(getSourceLocation().toAnchor() + ": Function moveto expects a value named y. No value named y is defined.");
  }

  if (xyz[2].IsNull()) {
    throw MessagedException(getSourceLocation().toAnchor() + ": Function moveto expects a value named z. No value named z is defined.");
  }

  for (int i = 0; i < 3; ++i) {
    Co<Expression> vv = xyz[i]->evaluate(env);
    ExpressionNumber *number = dynamic_cast<ExpressionNumber *>(vv.GetPointer());
    if (!number) {
      std::stringstream ss;
      ss << xyz[i]->getSourceLocation().toAnchor() << ": Function moveto expects a numeric " << (char) ('x' + i) << " coordinate. " << xyz[i]->getSource() << " is not a number.";
      throw MessagedException(ss.str());
    }
    v[i] = number->toReal();
  }

  env.moveTo(v[0], v[1], v[2]);

  return Co<Expression>(ExpressionUnit::getSingleton());
}

/* ------------------------------------------------------------------------- */

void ExpressionMoveTo::write(ostream &out) const {
  out << "(moveto x y z)";
}

/* ------------------------------------------------------------------------- */

}
