//
// Created by jianfei on 16-11-11.
//

#include <iostream>
#include <thread>
#include <memory>
#include <mpi.h>
#include <publisher_subscriber.h>
#include "corpus.h"
#include "clock.h"
#include <chrono>
#include "glog/logging.h"
#include <sstream>
#include "atomic_vector.h"

using namespace std;

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    // output all logs to stderr
    FLAGS_stderrthreshold=google::INFO;
    FLAGS_colorlogtostderr=true;

    int provided;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
    LOG_IF(FATAL, provided != MPI_THREAD_MULTIPLE) << "MPI_THREAD_MULTIPLE is not supported";
    int process_id, process_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_size);
    LOG(INFO) << process_id << ' ' << process_size;

//    {
//        bool is_publisher = process_id < 2;
//        bool is_subscriber = process_id >= 1;
//
//        auto on_recv = [&](char *data, int length){
//            LOG(INFO) << process_id << " received " << std::string(data, data+length);
//        };
//
//        PublisherSubscriber<decltype(on_recv)> pubsub(is_subscriber, on_recv);
//        LOG(INFO) << "PubSub started";
//
//        std::this_thread::sleep_for(1s);
//        if (process_id == 0) {
//            string message = "Message from node 0";
//            pubsub.Publish(message.data(), message.size());
//        }
//
//        std::this_thread::sleep_for(1s);
//        if (process_id == 1) {
//            string message = "Message from node 1";
//            pubsub.Publish(message.data(), message.size());
//        }
//
//        pubsub.Barrier();
//    }

//    {
//        // Generate some data
//        int num_docs = 10000;
//        float avg_doc_length = 1000;
//        int vocab_size = 10000;
//        auto corpus = Corpus::Generate(num_docs, avg_doc_length, vocab_size);
//        LOG(INFO) << "Corpus have " << corpus.T << " tokens";
//
//        // Pubsub for cv
//        std::vector<int> cv((size_t)vocab_size);
//        auto on_recv = [&](const char *msg, size_t length) {
//            cv[*((const int*)msg)]++;
//        };
//        PublisherSubscriber<decltype(on_recv)> pubsub(true, on_recv);
//
//        // Another pubsub for cv
//        std::vector<int> cv2((size_t)vocab_size);
//        auto on_recv2 = [&](const char *msg, size_t length) {
//            cv2[*((const int*)msg)]++;
//        };
//        PublisherSubscriber<decltype(on_recv2)> pubsub2(true, on_recv2);
//
//        // Compute via allreduce
//        std::vector<int> local_cv((size_t)vocab_size);
//        std::vector<int> global_cv((size_t)vocab_size);
//        for (auto &doc: corpus.w)
//            for (auto v: doc)
//                local_cv[v]++;
//        MPI_Allreduce(local_cv.data(), global_cv.data(), vocab_size,
//                      MPI_INT, MPI_SUM, MPI_COMM_WORLD);
//
//        // Compute via pubsub
//        Clock clk;
//        for (auto &doc: corpus.w) {
//            for (auto v: doc) {
//                pubsub.Publish((char*)&v, sizeof(v));
//                pubsub2.Publish((char*)&v, sizeof(v));
//            }
//        }
//        pubsub.Barrier();
//        pubsub2.Barrier();
//        LOG(INFO) << "Finished in " << clk.toc() << " seconds. (" << pubsub.GetNumSyncs() << " syncs)";
//
//        // Compare
//        LOG_IF(FATAL, global_cv != cv) << "Incorrect CV";
//        LOG_IF(FATAL, global_cv != cv2) << "Incorrect CV2";
//    }

    {
        AtomicVector<int> v;

        auto PrintVector = [&]() {
            for (int i = 0; i < process_size; i++) {
                if (i == process_id) {
                    auto sess = v.GetSession();
                    auto size = sess.Size();
                    std::ostringstream sout;
                    sout << "Node " << i << " size = " << size;
                    for (int i = 0; i < size; i++)
                        sout << " " << sess.Get(i);
                    LOG(INFO) << sout.str();
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        };

        if (process_id == 0) {
            v.IncreaseSize(5);
            auto sess = v.GetSession();
            sess.Inc(3);
            sess.Inc(2);
            sess.Dec(1);
        }
        v.Barrier();
        PrintVector();

        if (process_id == 1) {
            auto sess = v.GetSession();
            sess.Inc(4);
        }
        v.Barrier();
        PrintVector();
    }

    MPI_Finalize();
}