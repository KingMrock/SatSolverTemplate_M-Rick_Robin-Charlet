#include <iostream>
#include <fstream>
#include "Solver/Solver.hpp"
#include "Solver/inout.hpp" 

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <cnf-file>" << std::endl;
        return 1;
    }

    try {
        // Read the DIMACS format file
        std::ifstream inFile(argv[1]);
        if (!inFile.is_open()) {
            std::cerr << "c Error: Cannot open file: " << argv[1] << std::endl;
            return 1;
        }

        auto [clauses, numVars] = sat::inout::read_from_dimacs(inFile);
        sat::Solver solver(numVars);

        // Add all clauses
        for (const auto& clauseLits : clauses) {
            if (!solver.addClause(sat::Clause(clauseLits))) {
                std::cout << "UNSAT" << std::endl;
                return 0;
            }
        }

        // Solve the instance
        if (!solver.solve()) {
            std::cout << "UNSAT" << std::endl;
            return 0;
        }

        // For SAT instances, use to_dimacs to print solution
        std::vector<sat::Literal> solution;
        for (unsigned i = 0; i < numVars; ++i) {
            auto val = solver.val(sat::Variable(i));
            if (val == sat::TruthValue::True) {
                solution.push_back(sat::pos(sat::Variable(i)));
            } else {
                solution.push_back(sat::neg(sat::Variable(i)));
            }
        }
        std::cout << sat::inout::to_dimacs(solution);

    } catch (const std::exception& e) {
        std::cerr << "c Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}