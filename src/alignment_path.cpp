
#include "alignment_path.hpp"

#include <algorithm>
#include <numeric>

AlignmentPath::AlignmentPath(const int32_t seq_length_in, const double mapq_prob_in, const int32_t score_sum_in, const vector<gbwt::size_type> & ids_in) : seq_length(seq_length_in), mapq_prob(mapq_prob_in), score_sum(score_sum_in), ids(ids_in) {}

AlignmentPath::AlignmentPath(const AlignmentSearchPath & align_path_in, const vector<gbwt::size_type> & ids_in) : seq_length(align_path_in.seq_length), mapq_prob(align_path_in.mapqProb()), score_sum(align_path_in.scoreSum()), ids(ids_in) {}

bool operator==(const AlignmentPath & lhs, const AlignmentPath & rhs) { 

    return (lhs.seq_length == rhs.seq_length && doubleCompare(lhs.mapq_prob, rhs.mapq_prob) && lhs.score_sum == rhs.score_sum && lhs.ids == rhs.ids);
}

bool operator!=(const AlignmentPath & lhs, const AlignmentPath & rhs) { 

    return !(lhs == rhs);
}

ostream & operator<<(ostream & os, const AlignmentPath & align_path) {

    os << align_path.seq_length;
    os << " | " << align_path.mapq_prob;
    os << " | " << align_path.score_sum;
    os << " | (" << align_path.ids << ")";

    return os;
}

ostream & operator<<(ostream & os, const vector<AlignmentPath> & align_path) {

    os << "# " << align_path.size() << endl;

    for (auto & align_path: align_path) {

        os << align_path << endl;
    }

    return os;
}

AlignmentSearchPath::AlignmentSearchPath() {

    path_end_pos = 0;
    seq_start_offset = 0;
    seq_end_offset = 0;
    seq_length = 0;
}

double AlignmentSearchPath::mapqProb() const {

    double prob = 1;

    for (auto & mapq: mapqs) {

        if (mapq > 0) {

            prob *= (1 - phred_to_prob(mapq));

        } else {

            return 1;        
        }
    }

    return (1 - prob);
}

int32_t AlignmentSearchPath::scoreSum() const {

    return accumulate(scores.begin(), scores.end(), 0);
}

bool AlignmentSearchPath::complete() const {

    if (path.empty() || path_end_pos != path.size()) {

        return false;
    }

    assert(search.node == path.back());

    return true;
}

ostream & operator<<(ostream & os, const AlignmentSearchPath & align_search_path) {

    os << "(" << align_search_path.path << ")";
    os << " | " << align_search_path.path_end_pos;
    os << " | " << align_search_path.seq_start_offset;
    os << " | " << align_search_path.seq_end_offset;
    os << " | " << gbwt::Node::id(align_search_path.search.node);
    os << " | " << align_search_path.search.size();
    os << " | " << align_search_path.seq_length;
    os << " | (" << align_search_path.mapqs << ")";
    os << " | (" << align_search_path.scores << ")";

    return os;
}

ostream & operator<<(ostream & os, const vector<AlignmentSearchPath> & align_search_path) {

    os << "# " << align_search_path.size() << endl;

    for (auto & align_search_path: align_search_path) {

        os << align_search_path << endl;
    }

    return os;
}

