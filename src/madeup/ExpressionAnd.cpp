#include "madeup/ExpressionArray.h"
#include "madeup/ExpressionAnd.h"
#include "madeup/ExpressionBoolean.h"
#include "madeup/ExpressionMesh.h"
#include "twodee/MessagedException.h"

#ifdef USE_IGL
#include "madeup/MeshBoolean.h"
#endif

using namespace td;

namespace madeup {

/* ------------------------------------------------------------------------- */

ExpressionAnd::ExpressionAnd(Co<Expression> left, Co<Expression> right) :
  Expression(),
  left(left),
  right(right) {
}

/* ------------------------------------------------------------------------- */

Co<Expression> ExpressionAnd::evaluate(Environment &env) const {
  return evaluate_helper(left, right, getSource(), getSourceLocation(), env);
}

/* ------------------------------------------------------------------------- */

Co<Expression> ExpressionAnd::evaluate_helper(Co<Expression> l,
                                              Co<Expression> r,
                                              const std::string &source,
                                              const SourceLocation &location,
                                              Environment &env) {
  Co<Expression> l_value = l->evaluate(env);

  // Booleans
  ExpressionBoolean *l_boolean = dynamic_cast<ExpressionBoolean *>(l_value.GetPointer());

  if (l_boolean) {
    if (l_boolean->toBoolean()) {
      Co<Expression> r_value = r->evaluate(env);
      ExpressionBoolean *r_boolean = dynamic_cast<ExpressionBoolean *>(r_value.GetPointer());
      if (r_boolean) {
        return Co<Expression>(new ExpressionBoolean(r_boolean->toBoolean()));
      } else {
        // Right only is an array
        ExpressionArrayReference *r_array = dynamic_cast<ExpressionArrayReference *>(r_value.GetPointer());
        if (r_array) {
          int nitems = r_array->getArray()->getSize();
          Co<ExpressionArray> array(new ExpressionArray(nitems));
          for (int i = 0; i < nitems; ++i) {
            array->setElement(i, evaluate_helper(l_value, (*r_array->getArray())[i], source, location, env));
          }
          return Co<Expression>(new ExpressionArrayReference(array));
        } else {
          throw MessagedException(location.toAnchor() + ": Operator and doesn't know how to join " + l->getSource() + " and " + r->getSource() + ".");
        }
      }
    } else {
      return Co<Expression>(new ExpressionBoolean(false));
    }
  } else {

    Co<Expression> r_value = r->evaluate(env);
    ExpressionBoolean *r_boolean = dynamic_cast<ExpressionBoolean *>(r_value.GetPointer());

    // Both are arrays
    ExpressionArrayReference *l_array = dynamic_cast<ExpressionArrayReference *>(l_value.GetPointer());
    ExpressionArrayReference *r_array = dynamic_cast<ExpressionArrayReference *>(r_value.GetPointer());
    if (l_array && r_array) {
      int nitems = l_array->getArray()->getSize();
      if (nitems == r_array->getArray()->getSize()) {
        Co<ExpressionArray> array(new ExpressionArray(nitems));
        for (int i = 0; i < nitems; ++i) {
          array->setElement(i, evaluate_helper((*l_array->getArray())[i], (*r_array->getArray())[i], source, location, env));
        }
        return Co<Expression>(new ExpressionArrayReference(array));
      } else {
        throw MessagedException(location.toAnchor() + ": Operator and doesn't know how to join arrays of different sizes.");
      }
    } 

    // Left only is an array
    if (l_array && r_boolean) {
      int nitems = l_array->getArray()->getSize();
      Co<ExpressionArray> array(new ExpressionArray(nitems));
      for (int i = 0; i < nitems; ++i) {
        array->setElement(i, evaluate_helper((*l_array->getArray())[i], r_value, source, location, env));
      }
      return Co<Expression>(new ExpressionArrayReference(array));
    }

  }
 
#ifdef USE_IGL
  // Meshes
  Co<Expression> r_value = r->evaluate(env);
  ExpressionMesh *l_mesh = dynamic_cast<ExpressionMesh *>(l_value.GetPointer());
  ExpressionMesh *r_mesh = dynamic_cast<ExpressionMesh *>(r_value.GetPointer());
  if (l_mesh && r_mesh) {
    return MeshBoolean::construct_and_color(l_mesh, r_mesh, env, MeshBoolean::INTERSECTION);
  }
#endif

  throw MessagedException(location.toAnchor() + ": Operator and doesn't know how to join " + l->getSource() + " and " + r->getSource() + ".");
}

/* ------------------------------------------------------------------------- */

void ExpressionAnd::write(ostream &out) const {
  out << "(and ";
  left->write(out);
  out << " ";
  right->write(out);
  out << ")";
}

/* ------------------------------------------------------------------------- */

}
