#include "MesCost.h"

#include "utils/Election.h"
#include "utils/Math.h"
#include "utils/ProjectComparator.h"
#include "utils/ProjectEmbedding.h"
#include "utils/VoterTypes.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>
#include <queue>
#include <ranges>
#include <set>
#include <vector>

#include "ortools/linear_solver/linear_solver.h"

namespace {
struct Candidate {
    int index;
    long double max_payment_by_cost;

    bool operator>(const Candidate &other) const { return max_payment_by_cost > other.max_payment_by_cost; }
};
} // namespace

using namespace operations_research;

std::vector<ProjectEmbedding> mes_cost(const Election &election, const ProjectComparator &tie_breaking) {
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
        long double min_max_payment_by_cost = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            const auto &project = projects[current_candidate.index];
            auto previous_max_payment_by_cost = current_candidate.max_payment_by_cost;

            if (pbmath::is_greater_than(previous_max_payment_by_cost, min_max_payment_by_cost)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment_by_cost value can only increase
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
                long double max_payment_by_cost = max_payment / project.cost();
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment_by_cost = max_payment_by_cost;
                    if (pbmath::is_less_than(max_payment_by_cost, min_max_payment_by_cost) ||
                        (pbmath::is_equal(max_payment_by_cost, min_max_payment_by_cost) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment_by_cost !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment_by_cost = max_payment_by_cost;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (min_max_payment_by_cost == std::numeric_limits<long double>::max()) { // No more affordable projects
            break;
        }
        winners.push_back(projects[best_candidate.index]);

        for (const auto &approver : projects[best_candidate.index].approvers()) {
            budget[approver] =
                std::max(0.0L, budget[approver] - min_max_payment_by_cost * projects[best_candidate.index].cost());
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(candidate);
        }
        candidates_to_reinsert.clear();
    }

    return winners;
}

long long cost_reduction_for_mes_cost(const Election &election, int p, const ProjectComparator &tie_breaking) {
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
        long double min_max_payment_by_cost = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            const auto &project = projects[current_candidate.index];
            auto previous_max_payment_by_cost = current_candidate.max_payment_by_cost;

            if (pbmath::is_greater_than(previous_max_payment_by_cost, min_max_payment_by_cost)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment_by_cost value can only increase
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
                long double max_payment_by_cost = max_payment / project.cost();
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment_by_cost = max_payment_by_cost;
                    if (pbmath::is_less_than(max_payment_by_cost, min_max_payment_by_cost) ||
                        (pbmath::is_equal(max_payment_by_cost, min_max_payment_by_cost) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment_by_cost !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment_by_cost = max_payment_by_cost;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (min_max_payment_by_cost == std::numeric_limits<long double>::max()) { // No more affordable projects
            long double price_to_be_chosen = 0;
            for (const auto &approver : pp_approvers) {
                price_to_be_chosen += budget[approver];
            }
            price_to_be_chosen =
                pbmath::floor(price_to_be_chosen); // todo: if price doesn't have to be long long, change here

            max_price_to_be_chosen = std::max(max_price_to_be_chosen, static_cast<long long>(price_to_be_chosen));

            break;
        }

        auto winner = projects[best_candidate.index];

        if (winner == pp) {
            return pp.cost();
        }

        // todo: try lowering complexity to O(1) per iteration
        { // measure calculation
            std::ranges::sort(pp_approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });
            long long price_l = 0, price_r = pp.cost();
            while (price_l + 1 < price_r) {
                long long price_mid = (price_l + price_r) / 2;
                long double paid_so_far = 0, denominator = pp_approvers.size();

                for (const auto &approver : pp_approvers) {
                    long double max_payment = (static_cast<long double>(price_mid) - paid_so_far) / denominator;
                    long double max_payment_by_cost = max_payment / price_mid;
                    if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                        paid_so_far += budget[approver];
                        denominator--;
                    } else { // from this voter, everyone can fully participate
                        if (pbmath::is_less_than(max_payment_by_cost, min_max_payment_by_cost) ||
                            (pbmath::is_equal(max_payment_by_cost, min_max_payment_by_cost) &&
                             tie_breaking(ProjectEmbedding(price_mid, pp.name(), pp_approvers), winner))) {
                            price_l = price_mid;
                        }
                        break;
                    }
                }
                if (price_l != price_mid) {
                    price_r = price_mid;
                }
            }

            max_price_to_be_chosen = std::max(max_price_to_be_chosen, price_l);
        }

        for (const auto &approver : winner.approvers()) {
            budget[approver] = std::max(0.0L, budget[approver] - min_max_payment_by_cost * winner.cost());
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(candidate);
        }
        candidates_to_reinsert.clear();
    }

    return max_price_to_be_chosen;
}

std::optional<int> optimist_add_for_mes_cost(const Election &election, int p, const ProjectComparator &tie_breaking) {
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
        long double min_max_payment_by_cost = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            const auto &project = projects[current_candidate.index];
            auto previous_max_payment_by_cost = current_candidate.max_payment_by_cost;

            if (pbmath::is_greater_than(previous_max_payment_by_cost, min_max_payment_by_cost)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment_by_cost value can only increase
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
                long double max_payment_by_cost = max_payment / project.cost();
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment_by_cost = max_payment_by_cost;
                    if (pbmath::is_less_than(max_payment_by_cost, min_max_payment_by_cost) ||
                        (pbmath::is_equal(max_payment_by_cost, min_max_payment_by_cost) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment_by_cost !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment_by_cost = max_payment_by_cost;
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

        if (min_max_payment_by_cost == std::numeric_limits<long double>::max()) { // No more affordable projects
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
                    long double max_payment_by_cost = max_payment / pp.cost();
                    if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                        paid_so_far += budget[approver];
                        denominator--;
                    } else { // from this voter, everyone can fully participate
                        if (pbmath::is_less_than(max_payment_by_cost, min_max_payment_by_cost) ||
                            (pbmath::is_equal(max_payment_by_cost, min_max_payment_by_cost) &&
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
            budget[approver] = std::max(0.0L, budget[approver] - min_max_payment_by_cost * winner.cost());
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(candidate);
        }
        candidates_to_reinsert.clear();
    }

    return min_number_of_added_approvers;
}

std::optional<int> pessimist_add_for_mes_cost(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto total_budget = election.budget();
    auto n_voters = election.num_of_voters();
    const auto &projects = election.projects();
    const auto &pp = projects[p];
    auto pp_approvers = pp.approvers();

    auto allocation = mes_cost(election, tie_breaking);
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
    }

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> remaining_candidates;

    for (int i = 0; i < projects.size(); i++) {
        remaining_candidates.emplace(i, 0);
    }

    std::vector<long double> budget(n_voters, static_cast<long double>(total_budget) / n_voters);

    std::vector<Candidate> candidates_to_reinsert;
    candidates_to_reinsert.reserve(projects.size());

    while (true) {
        long double min_max_payment_by_cost = std::numeric_limits<long double>::max();
        Candidate best_candidate;

        while (!remaining_candidates.empty()) {
            auto current_candidate = remaining_candidates.top();
            remaining_candidates.pop();
            const auto &project = projects[current_candidate.index];
            auto previous_max_payment_by_cost = current_candidate.max_payment_by_cost;

            if (pbmath::is_greater_than(previous_max_payment_by_cost, min_max_payment_by_cost)) {
                candidates_to_reinsert.push_back(current_candidate);
                break; // We already selected the best possible - max_payment_by_cost value can only increase
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
                long double max_payment_by_cost = max_payment / project.cost();
                if (pbmath::is_greater_than(max_payment, budget[approver])) { // cannot afford to fully participate
                    paid_so_far += budget[approver];
                    denominator--;
                } else { // from this voter, everyone can fully participate
                    current_candidate.max_payment_by_cost = max_payment_by_cost;
                    if (pbmath::is_less_than(max_payment_by_cost, min_max_payment_by_cost) ||
                        (pbmath::is_equal(max_payment_by_cost, min_max_payment_by_cost) &&
                         tie_breaking(project, projects[best_candidate.index]))) {
                        if (min_max_payment_by_cost !=
                            std::numeric_limits<long double>::max()) { // Not the first "best" candidate
                            candidates_to_reinsert.push_back(best_candidate);
                        }
                        min_max_payment_by_cost = max_payment_by_cost;
                        best_candidate = current_candidate;
                    } else {
                        candidates_to_reinsert.push_back(current_candidate);
                    }
                    break;
                }
            }
        }

        if (pp.cost() > total_budget) {
            break;
        }

        std::ranges::sort(pp_approvers, [&budget](const int a, const int b) { return budget[a] < budget[b]; });

        { // ILP reduction constraints

            if (min_max_payment_by_cost == std::numeric_limits<long double>::max()) { // no more affordable projects
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
            long double min_max_payment = min_max_payment_by_cost * pp.cost();

            long double paid_so_far = 0, denominator = pp_approvers.size();
            bool pp_has_rich_supporters = false;

            for (const auto &approver : pp_approvers) {
                if (pbmath::is_greater_than(min_max_payment, budget[approver])) {
                    paid_so_far += budget[approver];
                    denominator--;
                } else {
                    paid_so_far += denominator * min_max_payment;
                    pp_has_rich_supporters = true;
                    break;
                }
            }

            long double m_i = pp.cost() - paid_so_far;
            long double m_i_strict = std::min(m_i - 1e-5, m_i * (1 - 1e-5));

            // todo: what if tie-breaking depends on the number of votes?
            if (tie_breaking(pp, winner)) {
                // Case 1: pp WINS tie-breaking with current winner, we need a STRICT inequality
                MPConstraint *const c = solver->MakeRowConstraint(-solver->infinity(), m_i_strict);

                for (int j = 0; j < t; j++) {
                    auto voter_type_example = voter_types[j].second;
                    c->SetCoefficient(x_T[j], std::min(min_max_payment, budget[voter_type_example]));
                }
            } else {
                // Case 2: pp DOESN'T WIN tie-breaking with current winner, we need either a STRICT inequality or a
                // WEAK inequality and guarantee max_payment is not less than min_max_payment

                MPConstraint *const c = solver->MakeRowConstraint(-solver->infinity(), m_i);

                const long double M = election.budget() + 1.0;
                MPVariable *const y = solver->MakeBoolVar("case_2_disjunction");

                MPConstraint *const either_this = solver->MakeRowConstraint(-solver->infinity(), m_i_strict);
                either_this->SetCoefficient(y, -M);

                MPConstraint *const or_that =
                    solver->MakeRowConstraint(1 - M - pp_has_rich_supporters, solver->infinity());
                or_that->SetCoefficient(y, -M);

                for (int j = 0; j < t; j++) {
                    auto voter_type_example = voter_types[j].second;
                    long double payment_coeff;
                    if (pbmath::is_greater_than(min_max_payment, budget[voter_type_example])) {
                        payment_coeff = budget[voter_type_example];
                    } else {
                        payment_coeff = min_max_payment;
                        or_that->SetCoefficient(x_T[j], 1);
                    }
                    c->SetCoefficient(x_T[j], payment_coeff);
                    either_this->SetCoefficient(x_T[j], payment_coeff);
                }
            }
        }

        for (const auto &approver : projects[best_candidate.index].approvers()) {
            budget[approver] =
                std::max(0.0L, budget[approver] - min_max_payment_by_cost * projects[best_candidate.index].cost());
        }

        for (auto &candidate : candidates_to_reinsert) {
            remaining_candidates.push(candidate);
        }
        candidates_to_reinsert.clear();

        total_budget -= projects[best_candidate.index].cost();
    }

    MPObjective *const objective = solver->MutableObjective();
    for (int j = 0; j < t; j++) {
        objective->SetCoefficient(x_T[j], 1);
    }
    objective->SetMaximization();

    MPSolver::ResultStatus result_status = solver->Solve();

    if (result_status == MPSolver::OPTIMAL) {
        // MIP solver might return something like 1.99999999, so we add 0.1 to be safe
        int result = objective->Value() + 0.1;
        if (result + 1 + pp.num_of_approvers() <= n_voters) {
            return result + 1;
        }
    }
    return {};
}

std::optional<int> singleton_add_for_mes_cost(const Election &election, int p, const ProjectComparator &tie_breaking) {
    auto projects = election.projects();
    auto budget = election.budget();
    auto n_voters = election.num_of_voters();
    auto original_n_voters = n_voters;

    auto &pp = projects[p];
    auto pp_approvers = pp.approvers();

    auto allocation = mes_cost(election, tie_breaking);
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
        auto allocation = mes_cost(Election(budget, n_voters, projects), tie_breaking);
        if (std::ranges::find(allocation, pp) != allocation.end()) {
            return n_voters - original_n_voters;
        }

        pp_approvers.push_back(n_voters);
        n_voters++;
        pp = ProjectEmbedding(pp.cost(), pp.name(), pp_approvers);
    }
}
