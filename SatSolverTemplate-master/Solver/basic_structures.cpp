#include "basic_structures.hpp"
#include "util/exception.hpp"

namespace sat {

    // Variable class implementation
    Variable::Variable(unsigned val) : val(val) {}

    unsigned Variable::get() const {
        return val;
    }

    bool Variable::operator==(Variable other) const {
        return val == other.val;
    }

    // Literal class implementation
    Literal::Literal(unsigned val) : val(val) {}

    unsigned Literal::get() const {
        return val;
    }

    Literal Literal::negate() const {
        return Literal(val ^ 1);
    }

    short Literal::sign() const {
        // even => negative literal => -1
        // odd  => positive literal => +1
        return (val % 2 == 0) ? -1 : 1;
    }

    bool Literal::operator==(Literal other) const {
        return val == other.val;
    }

    // Helper functions
    Literal pos(Variable x) {
        // positive literal is an odd number: 2*v + 1
        return Literal(2 * x.get() + 1);
    }

    Literal neg(Variable x) {
        // negative literal is an even number: 2*v
        return Literal(2 * x.get());
    }

    Variable var(Literal l) {
        // variable id is literal_id >> 1
        return Variable(l.get() >> 1);
    }
}