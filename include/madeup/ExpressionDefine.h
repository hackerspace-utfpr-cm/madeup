#ifndef ELEMENTDEFINE_H
#define ELEMENTDEFINE_H

#include <set>

#include "madeup/Expression.h"

namespace madeup {

class ExpressionClosure;

/* ------------------------------------------------------------------------- */

class FormalParameter {
  public:
    enum evaluation_mode_t {
      EAGER,
      LAZY
    };

    FormalParameter(const std::string &name,
                    evaluation_mode_t evaluation_mode = EAGER);

    evaluation_mode_t getEvaluationMode() const;
    const std::string &getName() const;
    bool isSplattable() const;
    void splatTo(const std::vector<std::string> &splats);
    const std::vector<std::string> &getSplats() const;
    void setDefaultValue(td::Co<Expression> default_value);
    td::Co<Expression> getDefaultValue() const;
    void write(ostream &out) const;

  private:
    std::string name;
    evaluation_mode_t evaluation_mode;
    std::vector<std::string> splats;
    td::Co<Expression> default_value;
};

/* ------------------------------------------------------------------------- */

std::ostream &operator<<(std::ostream &out, const FormalParameter &parameter);

/* ------------------------------------------------------------------------- */

class ExpressionDefine : public Expression {
  public:
    ExpressionDefine(const std::string &name, td::Co<Expression> body);
    ExpressionDefine(const std::string &name, td::Co<Expression> body, const std::vector<FormalParameter> &formals);
    void addFormal(const std::string &name, FormalParameter::evaluation_mode_t evaluation_mode = FormalParameter::EAGER);
    void addFormal(const FormalParameter &formal);
    const FormalParameter &getFormal(int i);
    unsigned int getArity() const;
    td::Co<Expression> evaluate(Environment &env) const;
    td::Co<Expression> getBody() const;
    void write(ostream &out) const;
    void isDynamicallyScoped(bool enable);
    bool isDynamicallyScoped() const;
    const std::string &getName() const { return name; }
    void predeclare(Environment &env) const;
    std::set<std::string> getFormalNames() const;
    void splat(const std::string &fromID, const std::vector<std::string> &toIDs);

  private:
    std::string name;
    td::Co<Expression> body;
    std::vector<FormalParameter> formals;
    bool is_dynamically_scoped;
};

/* ------------------------------------------------------------------------- */

}

#endif
