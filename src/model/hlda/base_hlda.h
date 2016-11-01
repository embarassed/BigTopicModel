//
// Created by jianfei on 8/29/16.
//

#ifndef HLDA_BASEHLDA_H
#define HLDA_BASEHLDA_H

#include <vector>
#include <string>
#include "matrix.h"
#include "tree.h"
#include "xorshift.h"
#include "types.h"
#include "document.h"

class Corpus;

class BaseHLDA {

public:
    BaseHLDA(Corpus &corpus, int L,
             std::vector<double> alpha, std::vector<double> beta, std::vector<double> gamma,
             int num_iters, int mc_samples);

    virtual void Initialize() = 0;

    virtual void Estimate() = 0;

    void Visualize(std::string fileName, int threshold = -1);

protected:
    virtual std::vector<double>
    WordScore(Document &doc, int l, TTopic num_instantiated, TTopic num_collapsed) = 0;

    virtual void InitializeTreeWeight();

    std::string TopWords(int l, int id);

    Tree tree;
    Corpus &corpus;
    int L;
    std::vector<double> alpha;
    double alpha_bar;
    std::vector<double> beta;        // Beta for each layer
    std::vector<double> gamma;
    int num_iters, mc_samples;
    xorshift generator;

    std::vector<Document> docs;

    // For pcs and is
    std::vector<Matrix<float> > phi;        // Depth * V * K
    std::vector<Matrix<float> > log_phi;

    std::vector<Matrix<TCount> > count;

    Matrix<double> log_normalization;

    bool new_topic;
};


#endif //HLDA_BASEHLDA_H
