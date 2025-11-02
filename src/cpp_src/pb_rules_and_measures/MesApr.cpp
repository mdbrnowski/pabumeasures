#include "MesApr.h"

#include "utils/Election.h"
#include "utils/Math.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"
#include "utils/VoterTypes.h"

#include <algorithm>
#include <functional>
#include <iostream> // DEBUG
#include <limits>
#include <numeric>
#include <queue>
#include <ranges>
#include <set>
#include <vector>

#include "ortools/base/init_google.h"
#include "ortools/init/init.h"
#include "ortools/linear_solver/linear_solver.h"

namespace {
struct Candidate {
    int index;
    long double max_payment;

    bool operator>(const Candidate &other) const { return max_payment > other.max_payment; }
};
} // namespace

using namespace operations_research;

std::vector<ProjectEmbedding> mes_apr(const Election &election, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.num_of_voters();
    const auto &projects = election.projects();

    std::vector<ProjectEmbedding> winners;

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> remaining_candidates;

    for (int i = 0; i < projects.size(); i++) {
        remaining_candidates.emplace(i, 0);
    }

    std::vector<long double> budget(n_voters, static_cast<long double>(total_budget) / n_voters);

    std::vector<Candidate> candidates_to_reinsert;
    candidates_to_reinsert.reserve(projects.size());

    while (true) {
        long double min_max_payment = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            const auto &project = projects[current_candidate.index];
            auto previous_max_payment = current_candidate.max_payment;

            if (pbmath::is_greater_than(previous_max_payment, min_max_payment)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment value can only increase
            }

            long double money_behind_project = 0;
            auto approvers = project.approvers();

            for (const auto &approver : approvers) {
                money_behind_project += budget[approver];
            }

            if (pbmath::is_less_than(money_behind_project, project.cost())) {
                continue;
            }

            std::ranges::sort(approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

            long double paid_so_far = 0, denominator = approvers.size();

            for (const auto &approver : approvers) {
                long double max_payment = (static_cast<long double>(project.cost()) - paid_so_far) / denominator;
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment = max_payment;
                    if (pbmath::is_less_than(max_payment, min_max_payment) ||
                        (pbmath::is_equal(max_payment, min_max_payment) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment = max_payment;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (min_max_payment == std::numeric_limits<long double>::max()) { // No more affordable projects
            break;
        }
        winners.push_back(projects[best_candidate.index]);

        for (const auto &approver : projects[best_candidate.index].approvers()) {
            budget[approver] = std::max(0.0L, budget[approver] - min_max_payment);
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(candidate);
        }
        candidates_to_reinsert.clear();
    }

    return winners;
}

long long cost_reduction_for_mes_apr(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.num_of_voters();
    const auto &projects = election.projects();
    const auto &pp = projects[p];
    auto pp_approvers = pp.approvers();
    long long max_price_to_be_chosen = 0;

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> remaining_candidates;

    for (int i = 0; i < projects.size(); i++) {
        remaining_candidates.emplace(i, 0);
    }

    std::vector<long double> budget(n_voters, static_cast<long double>(total_budget) / n_voters);

    std::vector<Candidate> candidates_to_reinsert;
    candidates_to_reinsert.reserve(projects.size());

    while (true) {
        long double min_max_payment = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            const auto &project = projects[current_candidate.index];
            long double previous_max_payment = current_candidate.max_payment;

            if (pbmath::is_greater_than(previous_max_payment, min_max_payment)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment value can only increase
            }

            long double money_behind_project = 0;
            auto approvers = project.approvers();

            for (const auto &approver : approvers) {
                money_behind_project += budget[approver];
            }

            if (pbmath::is_less_than(money_behind_project, project.cost())) {
                continue;
            }

            std::ranges::sort(approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

            long double paid_so_far = 0, denominator = approvers.size();

            for (const auto &approver : approvers) {
                long double max_payment = (static_cast<long double>(project.cost()) - paid_so_far) / denominator;
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment = max_payment;
                    if (pbmath::is_less_than(max_payment, min_max_payment) ||
                        (pbmath::is_equal(max_payment, min_max_payment) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment = max_payment;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (min_max_payment == std::numeric_limits<long double>::max()) { // No more affordable projects
            long double price_to_be_chosen = 0;
            for (const auto &approver : pp_approvers) {
                price_to_be_chosen += budget[approver];
            }
            price_to_be_chosen =
                pbmath::floor(price_to_be_chosen); // todo: if price doesn't have to be long long, change here

            max_price_to_be_chosen = std::max(max_price_to_be_chosen, static_cast<long long>(price_to_be_chosen));

            break;
        }

        if (best_candidate.index == p) {
            return pp.cost();
        }

        auto winner = projects[best_candidate.index];

        { // measure calculation
            std::ranges::sort(pp_approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

            long double price_to_be_chosen = 0, full_participators_number = pp_approvers.size();
            for (const auto &approver : pp_approvers) {
                if (pbmath::is_greater_than(min_max_payment, budget[approver])) { // cannot afford to fully participate
                    price_to_be_chosen += budget[approver];
                    full_participators_number--;
                } else {
                    price_to_be_chosen += full_participators_number * min_max_payment;
                    break;
                }
            }

            long double floored_price_to_be_chosen =
                pbmath::floor(price_to_be_chosen); // todo: if price doesn't have to be long long, change here
            if (pbmath::is_equal(floored_price_to_be_chosen, price_to_be_chosen) &&
                tie_breaking(winner, ProjectEmbedding(floored_price_to_be_chosen, pp.name(), pp_approvers))) {
                floored_price_to_be_chosen--;
            }

            max_price_to_be_chosen =
                std::max(max_price_to_be_chosen, static_cast<long long>(floored_price_to_be_chosen));
        }

        for (const auto &approver : winner.approvers()) {
            budget[approver] = std::max(0.0L, budget[approver] - min_max_payment);
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(candidate);
        }
        candidates_to_reinsert.clear();
    }

    return max_price_to_be_chosen;
}

std::optional<int> optimist_add_for_mes_apr(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.num_of_voters();
    const auto &projects = election.projects();
    const auto &pp = projects[p];
    auto pp_approvers = pp.approvers();
    std::optional<int> min_number_of_added_approvers = std::nullopt;

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> remaining_candidates;

    for (int i = 0; i < projects.size(); i++) {
        remaining_candidates.emplace(i, 0);
    }

    std::vector<long double> budget(n_voters, static_cast<long double>(total_budget) / n_voters);
    std::vector<int> voters(n_voters);
    std::iota(voters.begin(), voters.end(), 0);

    std::vector<Candidate> candidates_to_reinsert;
    candidates_to_reinsert.reserve(projects.size());

    while (true) {
        long double min_max_payment = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            const auto &project = projects[current_candidate.index];
            long double previous_max_payment = current_candidate.max_payment;

            if (pbmath::is_greater_than(previous_max_payment, min_max_payment)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment value can only increase
            }

            long double money_behind_project = 0;
            auto approvers = project.approvers();

            for (const auto &approver : approvers) {
                money_behind_project += budget[approver];
            }

            if (pbmath::is_less_than(money_behind_project, project.cost())) {
                continue;
            }

            std::ranges::sort(approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

            long double paid_so_far = 0, denominator = approvers.size();

            for (const auto &approver : approvers) {
                long double max_payment = (static_cast<long double>(project.cost()) - paid_so_far) / denominator;
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment = max_payment;
                    if (pbmath::is_less_than(max_payment, min_max_payment) ||
                        (pbmath::is_equal(max_payment, min_max_payment) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment = max_payment;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        std::ranges::sort(voters, [&budget](const int a, const int b) { return budget[a] < budget[b]; });
        std::ranges::sort(pp_approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

        if (min_max_payment == std::numeric_limits<long double>::max()) { // No more affordable projects
            long double money_behind_project = 0;
            for (const auto &approver : pp_approvers) {
                money_behind_project += budget[approver];
            }

            int approvers_added = 0;

            int approvers_idx = pp_approvers.size() - 1, voters_idx = voters.size() - 1;
            while (voters_idx >= 0 && pbmath::is_less_than(money_behind_project, pp.cost())) {
                if (approvers_idx >= 0 && pp_approvers[approvers_idx] == voters[voters_idx]) {
                    approvers_idx--;
                    voters_idx--;
                    continue;
                }
                approvers_added++;
                money_behind_project += budget[voters[voters_idx]];
                voters_idx--;
            }

            if (!pbmath::is_less_than(money_behind_project, pp.cost())) {
                min_number_of_added_approvers = pbmath::optional_min(min_number_of_added_approvers, approvers_added);
            }

            break;
        }

        auto winner = projects[best_candidate.index];

        if (winner == pp) {
            return 0;
        }

        { // measure calculation

            std::set<int> pp_approvers_set(pp_approvers.begin(), pp_approvers.end());

            int low = -1, high = n_voters - pp_approvers.size() + 1;
            while (low + 1 < high) {
                int voters_to_be_added = (low + high) / 2;
                auto pp_curr_approvers_set = pp_approvers_set;

                int approvers_idx = pp_approvers.size() - 1, voters_idx = voters.size() - 1;
                for (int voters_already_added = 0; voters_already_added < voters_to_be_added;) {
                    if (approvers_idx >= 0 && pp_approvers[approvers_idx] == voters[voters_idx]) {
                        approvers_idx--;
                        voters_idx--;
                        continue;
                    }
                    pp_curr_approvers_set.insert(voters[voters_idx]);
                    voters_already_added++;
                    voters_idx--;
                }

                long double money_behind_project = 0;
                for (const auto &voter : voters) {
                    if (pp_curr_approvers_set.count(voter) > 0) {
                        money_behind_project += budget[voter];
                    }
                }
                if (pbmath::is_greater_than(pp.cost(), money_behind_project)) {
                    low = voters_to_be_added;
                    continue;
                }

                long double paid_so_far = 0, denominator = pp_curr_approvers_set.size();
                for (const auto &voter : voters) {
                    if (pp_curr_approvers_set.count(voter) == 0) {
                        continue;
                    }
                    auto &approver = voter;

                    long double max_payment = (static_cast<long double>(pp.cost()) - paid_so_far) / denominator;
                    if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                        paid_so_far += budget[approver];
                        denominator--;
                    } else { // from this voter, everyone can fully participate
                        if (pbmath::is_less_than(max_payment, min_max_payment) ||
                            (pbmath::is_equal(max_payment, min_max_payment) &&
                             tie_breaking(ProjectEmbedding(pp.cost(), pp.name(),
                                                           std::vector<int>(pp_curr_approvers_set.begin(),
                                                                            pp_curr_approvers_set.end())),
                                          winner))) {
                            high = voters_to_be_added;
                        } else {
                            low = voters_to_be_added;
                        }
                        break;
                    }
                }
            }

            if (high != n_voters - pp_approvers.size() + 1) {
                min_number_of_added_approvers = pbmath::optional_min(min_number_of_added_approvers, high);
            }
        }

        for (const auto &approver : winner.approvers()) {
            budget[approver] = std::max(0.0L, budget[approver] - min_max_payment);
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(candidate);
        }
        candidates_to_reinsert.clear();
    }

    return min_number_of_added_approvers;
}

std::optional<int> pessimist_add_for_mes_apr(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.num_of_voters();
    const auto &projects = election.projects();
    const auto &pp = projects[p];
    auto pp_approvers = pp.approvers();

    auto allocation = mes_apr(election, tie_breaking);
    if (std::ranges::find(allocation, pp) != allocation.end()) {
        return 0;
    }

    const auto voter_types = calculate_voter_types(election, p, allocation);
    int t = voter_types.size();

    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
    std::vector<const MPVariable *> x_T;
    x_T.reserve(t);
    for (int j = 0; j < t; j++) {
        auto voter_type_count = voter_types[j].first;
        x_T.push_back(solver->MakeIntVar(0, voter_type_count, "x_T_" + std::to_string(j)));
        std::cout << "x_T_" + std::to_string(j) << " : " << voter_type_count << "    " << voter_types[j].second << "\n";
    }

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> remaining_candidates;

    for (int i = 0; i < projects.size(); i++) {
        remaining_candidates.emplace(i, 0);
    }

    std::vector<long double> budget(n_voters, static_cast<long double>(total_budget) / n_voters);

    std::vector<Candidate> candidates_to_reinsert;
    candidates_to_reinsert.reserve(projects.size());

    int round_number = 0;
    while (true) {
        long double min_max_payment = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            const auto &project = projects[current_candidate.index];
            auto previous_max_payment = current_candidate.max_payment;

            if (pbmath::is_greater_than(previous_max_payment, min_max_payment)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment value can only increase
            }

            long double money_behind_project = 0;
            auto approvers = project.approvers();

            for (const auto &approver : approvers) {
                money_behind_project += budget[approver];
            }

            if (pbmath::is_less_than(money_behind_project, project.cost())) {
                continue;
            }

            std::ranges::sort(approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

            long double paid_so_far = 0, denominator = approvers.size();

            for (const auto &approver : approvers) {
                long double max_payment = (static_cast<long double>(project.cost()) - paid_so_far) / denominator;
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment = max_payment;
                    if (pbmath::is_less_than(max_payment, min_max_payment) ||
                        (pbmath::is_equal(max_payment, min_max_payment) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment = max_payment;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        round_number++;

        if (pp.cost() > total_budget) {
            std::cout << "KONEC\n" << pp.cost() << " / " << total_budget;
            break;
        }

        if (min_max_payment == std::numeric_limits<long double>::max()) { // no more affordable projects
            std::cout << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n";
            long double money_behind_project = 0;
            for (const auto &approver : pp_approvers) {
                money_behind_project += budget[approver];
            }

            long double m_i = pp.cost() - money_behind_project;

            // we need a strict inequality; the solver's default precision is 1e-6, so need to exceed that
            m_i = std::min(m_i - 1e-5, m_i * (1 - 1e-5));

            MPConstraint *const c = solver->MakeRowConstraint(-solver->infinity(), m_i);

            for (int j = 0; j < t; j++) {
                auto voter_type_example = voter_types[j].second;
                c->SetCoefficient(x_T[j], budget[voter_type_example]);
            }

            break;
        }

        auto winner = projects[best_candidate.index];
        std::ranges::sort(pp_approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

        { // ILP reduction constraints
            long double paid_so_far = 0, denominator = pp_approvers.size();

            for (const auto &approver : pp_approvers) {
                if (pbmath::is_greater_than(min_max_payment, budget[approver])) {
                    paid_so_far += budget[approver];
                    denominator--;
                } else {
                    paid_so_far += denominator * min_max_payment;
                    break;
                }
            }

            long double m_i = pp.cost() - paid_so_far;

            // todo: what if tie-breaking depends on the number of votes?
            if (tie_breaking(pp, winner)) {
                std::cout << "pp, better!";
                // we need a strict inequality; the solver's default precision is 1e-6, so need to exceed that
                m_i = std::min(m_i - 1e-5, m_i * (1 - 1e-5));
            }
            MPConstraint *const c = solver->MakeRowConstraint(-solver->infinity(), m_i);
            MPConstraint *const c_2 = solver->MakeRowConstraint(0, solver->infinity());
            std::cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n";
            std::cout << "min max payment " << min_max_payment << "\n";

            long double max_used_value = 0;

            for (int j = 0; j < t; j++) {
                auto voter_type_example = voter_types[j].second;
                if (pbmath::is_less_than(budget[voter_type_example], min_max_payment)) {
                    c->SetCoefficient(x_T[j], budget[voter_type_example]);
                } else {
                    c->SetCoefficient(x_T[j], min_max_payment);
                    c_2->SetCoefficient(x_T[j], 1);
                }
                std::cout << "budget: " << budget[voter_type_example] << " "
                          << std::min(budget[voter_type_example], min_max_payment) << '\n';
                max_used_value = std::max(max_used_value, std::min(budget[voter_type_example], min_max_payment));
            }
        }

        for (const auto &approver : winner.approvers()) {
            budget[approver] = std::max(0.0L, budget[approver] - min_max_payment);
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(candidate);
        }
        candidates_to_reinsert.clear();

        total_budget -= winner.cost();
    }

    MPObjective *const objective = solver->MutableObjective();
    for (int j = 0; j < t; j++) {
        objective->SetCoefficient(x_T[j], 1);
    }
    objective->SetMaximization();

    MPSolver::ResultStatus result_status = solver->Solve();

    std::cout << "========== SOLVER SETUP ==========\n";
    std::cout << "Solver name: " << solver->Name() << "\n";
    std::cout << "Solver type: " << solver->ProblemType() << "\n";
    std::cout << "Number of variables: " << solver->NumVariables() << "\n";
    std::cout << "Number of constraints: " << solver->NumConstraints() << "\n\n";

    // Log all variables
    std::cout << "----- Variables -----\n";
    for (const auto *var : solver->variables()) {
        std::cout << "Name: " << var->name() << " | Bounds: [" << var->lb() << ", " << var->ub() << "]"
                  << " | Solution value (if any): " << var->solution_value() << "\n";
    }

    // Log all constraints
    std::cout << "\n----- Constraints -----\n";
    for (const auto *ct : solver->constraints()) {
        std::cout << "Constraint: " << ct->name() << " | Bounds: [" << ct->lb() << ", " << ct->ub() << "]"
                  << "\n  Expression: ";
        for (const auto &term : ct->terms()) {
            std::cout << term.second << "*" << term.first->name() << " + ";
        }
        std::cout << "\b\b \n";
    }

    std::cout << "\n\n\n";

    for (auto [x, y] : voter_types) {
        std::cout << x << " " << y << "\n";
    }

    if (result_status == MPSolver::OPTIMAL) {
        // MIP solver might return something like 1.99999999, so we add 0.1 to be safe
        int result = objective->Value() + 0.1;
        std::cout << objective->Value() << '\n';
        if (result + 1 + pp.num_of_approvers() <= n_voters) {
            std::cout << "res" << result << "\n";
            return result + 1;
        }
    }
    return {};
}

std::optional<int> singleton_add_for_mes_apr(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto projects = election.projects();
    auto budget = election.budget();
    auto n_voters = election.num_of_voters();
    auto original_n_voters = n_voters;

    auto &pp = projects[p];
    auto pp_approvers = pp.approvers();

    auto allocation = mes_apr(election, tie_breaking);
    if (std::ranges::find(allocation, pp) != allocation.end()) {
        return 0;
    }

    if (pp.cost() == budget) {
        return {};
    }

    int minimal_ans =
        pbmath::ceil_div(static_cast<long long>(n_voters - pp_approvers.size()) * pp.cost(), budget - pp.cost());
    while (pp_approvers.size() < minimal_ans) {
        pp_approvers.push_back(n_voters);
        n_voters++;
    }
    pp = ProjectEmbedding(pp.cost(), pp.name(), pp_approvers);

    while (true) {
        auto allocation = mes_apr(Election(budget, n_voters, projects), tie_breaking);
        if (std::ranges::find(allocation, pp) != allocation.end()) {
            return n_voters - original_n_voters;
        }

        pp_approvers.push_back(n_voters);
        n_voters++;
        pp = ProjectEmbedding(pp.cost(), pp.name(), pp_approvers);
    }
}
