/**
* @brief
*/

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
        return (val & 1) ? 1 : -1;
    }

    bool Literal::operator==(Literal other) const {
        return val == other.val;
    }

    // Helper functions
    Literal pos(Variable x) {
        return Literal((x.get() << 1) | 1); // Use bitwise shift to create positive literal
    }

    Literal neg(Variable x) {
        return Literal(x.get() << 1); // Use bitwise shift to create negative literal
    }

    Variable var(Literal l) {
        return Variable(l.get() >> 1); // Use bitwise shift to get variable from literal
    }
}