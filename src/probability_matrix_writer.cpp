
#include "probability_matrix_writer.hpp"

#include <iomanip>


ProbabilityMatrixWriter::ProbabilityMatrixWriter(const bool use_stdout_in, const string filename, const double prob_precision_in) : use_stdout(use_stdout_in), prob_precision(prob_precision_in), prob_precision_digits(ceil(-1 * log10(prob_precision))) {

    streambuf * writer_buffer;

    if (use_stdout) {

        writer_buffer = cout.rdbuf();
    
    } else {

        writer_file.open(filename);
        assert(writer_file.is_open());

        writer_buffer = writer_file.rdbuf();
    }

    writer_stream = new ostream(writer_buffer);
    *writer_stream << fixed;
}

ProbabilityMatrixWriter::~ProbabilityMatrixWriter() {

	delete writer_stream;

    if (!use_stdout) {

        writer_file.close();
    }
}

void ProbabilityMatrixWriter::lockWriter() {

	writer_mutex.lock();
}

void ProbabilityMatrixWriter::unlockWriter() {

	writer_mutex.unlock();
}

void ProbabilityMatrixWriter::writeCollapsedProbabilities(const ReadPathProbabilities & read_path_probs) {

    *writer_stream << read_path_probs.readCount() << " " << read_path_probs.noiseProbability();

    for (auto & collapsed_probs: read_path_probs.collapsedProbabilities()) {

        *writer_stream << " " << collapsed_probs.first << ":";

        bool is_first = true;

        for (auto idx: collapsed_probs.second) {

            if (is_first) {

                *writer_stream << idx;
                is_first = false;

            } else {

                *writer_stream << "," << idx;
            }
        }
    }

    *writer_stream << endl;
}

void ProbabilityMatrixWriter::writeReadPathProbabilityCluster(const vector<ReadPathProbabilities> & cluster_probs, const vector<PathInfo> & cluster_paths) {

    assert(!cluster_paths.empty());

    *writer_stream << "#" << endl;
    *writer_stream << setprecision(3);
    *writer_stream << cluster_paths.front().name << "," << cluster_paths.front().length << "," << cluster_paths.front().effective_length;

    for (size_t i = 1; i < cluster_paths.size(); ++i) {

        *writer_stream << " " << cluster_paths.at(i).name << "," << cluster_paths.at(i).length << "," << cluster_paths.at(i).effective_length;
    }

    *writer_stream << endl;

    if (!cluster_probs.empty()) {

        *writer_stream << setprecision(prob_precision_digits);

        for (auto & probs: cluster_probs) {

            assert(probs.probabilities().size() <= cluster_paths.size());
            writeCollapsedProbabilities(probs);
        }
    }
}

