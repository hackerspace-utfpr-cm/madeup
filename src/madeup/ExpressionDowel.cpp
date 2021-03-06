#include "madeup/ExpressionBoolean.h"
#include "madeup/ExpressionClosure.h"
#include "madeup/ExpressionDowel.h"
#include "madeup/ExpressionMesh.h"
#include "madeup/ExpressionNumber.h"
#include "madeup/ExpressionUnit.h"

using namespace td;

namespace madeup {

/* ------------------------------------------------------------------------- */

ExpressionDowel::ExpressionDowel() :
  Expression() {
}

/* ------------------------------------------------------------------------- */

Co<Expression> ExpressionDowel::evaluate(Environment &env) const {
  // Get roundedness.
  Co<ExpressionClosure> max_bend_closure = env["maxBend"];
  if (max_bend_closure.IsNull()) {
    throw MessagedException(getSourceLocation().toAnchor() + ": I expect function dowel to be given a parameter named maxBend, but no maxBend was given.");
  }

  Co<Expression> max_bend_value = max_bend_closure->evaluate(env);
  ExpressionNumber *max_bend_number = dynamic_cast<ExpressionNumber *>(max_bend_value.GetPointer());
  if (!max_bend_number) {
    throw MessagedException(max_bend_value->getSourceLocation().toAnchor() + ": I expect function dowel to be given a numeric maxBend value, but what you gave isn't a number.");
  }
  float max_bend = max_bend_number->toReal();

  // Get twist.
  Co<ExpressionClosure> twist_closure = env["twist"];
  if (twist_closure.IsNull()) {
    throw MessagedException(getSourceLocation().toAnchor() + ": I expect function dowel to be given a parameter named twist, but no twist was given.");
  }

  Co<Expression> twist_value = twist_closure->evaluate(env);
  ExpressionNumber *twist_number = dynamic_cast<ExpressionNumber *>(twist_value.GetPointer());
  if (!twist_number) {
    throw MessagedException(twist_value->getSourceLocation().toAnchor() + ": I expect function dowel to be given a numeric twist value, but what you gave isn't a number.");
  }
  float twist = twist_number->toReal();

  // Emit dowel.
  try {
    Co<Trimesh> trimesh = env.dowel(twist, max_bend);
    return Co<Expression>(new ExpressionMesh(trimesh));
  } catch (MessagedException e) {
    throw UnlocatedException(e.GetMessage());
  }
}

/* ------------------------------------------------------------------------- */

void ExpressionDowel::write(ostream &out) const {
  out << "(dowel)";
}

/* ------------------------------------------------------------------------- */

}
