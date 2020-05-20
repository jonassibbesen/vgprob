
#include "path_estimates_writer.hpp"

#include <iomanip>


PathEstimatesWriter::PathEstimatesWriter(const bool use_stdout_in, const string filename) : use_stdout(use_stdout_in) {

    streambuf * writer_buffer;

    if (use_stdout) {

        writer_buffer = cout.rdbuf();
    
    } else {

        writer_file.open(filename);
        assert(writer_file.is_open());

        writer_buffer = writer_file.rdbuf();
    }

    writer_stream = new ostream(writer_buffer);
}

PathEstimatesWriter::~PathEstimatesWriter() {

	delete writer_stream;

    if (!use_stdout) {

        writer_file.close();
    }
}

void PathEstimatesWriter::writeThreadedPathClusterLikelihoods(const vector<vector<PathClusterEstimates> > & threaded_path_cluster_estimates, const uint32_t ploidy) {

    for (uint32_t i = 0; i < ploidy; ++i) {

        *writer_stream << "Name" << i + 1 << "\t";
    }

    *writer_stream << "ClusterID\tMinusLogLikelihood" << endl;

    uint32_t cluster_id = 0;

    for (auto & path_cluster_estimates_thread: threaded_path_cluster_estimates) {

        for (auto & path_cluster_estimates: path_cluster_estimates_thread) {

            ++cluster_id;

            const Likelihoods & likelihoods = path_cluster_estimates.likelihoods;

            assert(likelihoods.is_log);
            assert(likelihoods.groups.size() == likelihoods.likelihoods.size());

            for (size_t i = 0; i < likelihoods.groups.size(); ++i) {

                assert(likelihoods.groups.at(i).size() == ploidy);

                for (auto & path_idx: likelihoods.groups.at(i)) {

                    *writer_stream << path_cluster_estimates.paths.at(path_idx).name << "\t";
                }

                *writer_stream << cluster_id;
                *writer_stream << "\t" << -1 * likelihoods.likelihoods(i);
                *writer_stream << endl;
            }
        }
    }
}

void PathEstimatesWriter::writeThreadedPathClusterAbundances(const vector<vector<PathClusterEstimates> > & threaded_path_cluster_estimates) {

    *writer_stream << "Name\tClusterID\tLength\tEffectiveLength\tIsExpressedConfidence\tMeanGroupExpression\tMeanReadCount\tMeanTPM" << endl;

    double transcript_count_sum = 0;

    for (auto & path_cluster_estimates_thread: threaded_path_cluster_estimates) {

        for (auto & path_cluster_estimates: path_cluster_estimates_thread) {

            for (size_t i = 0; i < path_cluster_estimates.paths.size(); ++i) {

                assert(path_cluster_estimates.paths.size() == path_cluster_estimates.abundances.confidence.cols());
                assert(path_cluster_estimates.paths.size() == path_cluster_estimates.abundances.expression.cols());

                if (path_cluster_estimates.paths.at(i).effective_length > 0) {

                    transcript_count_sum += path_cluster_estimates.abundances.expression(0, i) * path_cluster_estimates.abundances.read_count / path_cluster_estimates.paths.at(i).effective_length;
                }
            }
        }
    }

    uint32_t cluster_id = 0;

    for (auto & path_cluster_estimates_thread: threaded_path_cluster_estimates) {

        for (auto & path_cluster_estimates: path_cluster_estimates_thread) {

            ++cluster_id;

            for (size_t i = 0; i < path_cluster_estimates.paths.size(); ++i) {

                double transcript_count = 0;

                if (path_cluster_estimates.paths.at(i).effective_length > 0) {

                    transcript_count = path_cluster_estimates.abundances.expression(0, i) * path_cluster_estimates.abundances.read_count / path_cluster_estimates.paths.at(i).effective_length;
                }

                *writer_stream << path_cluster_estimates.paths.at(i).name;
                *writer_stream << "\t" << cluster_id;
                *writer_stream << "\t" << path_cluster_estimates.paths.at(i).length;
                *writer_stream << "\t" << path_cluster_estimates.paths.at(i).effective_length;
                *writer_stream << "\t" << path_cluster_estimates.abundances.confidence(0, i);
                *writer_stream << "\t" << path_cluster_estimates.abundances.expression(0, i);
                *writer_stream << "\t" << path_cluster_estimates.abundances.expression(0, i) * path_cluster_estimates.abundances.read_count;
                *writer_stream << "\t" << transcript_count / transcript_count_sum * pow(10, 6);
                *writer_stream << endl;
            }
        }
    }
}
