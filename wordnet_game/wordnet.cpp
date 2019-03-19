
#include "wordnet.h"

#include <random>
#include <iterator>
#include <boost/graph/random.hpp>


template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

std::string random_q(const wnb::wordnet& wn, std::map<std::string, std::string>& q, int n_options){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    for (int i = 0; i<n_options; ++i)
    {
        auto item = boost::random_vertex(wn.wordnet_graph(), gen);
        auto synset = wn.wordnet_graph()[item];
        auto lemma = *select_randomly(synset.words.begin(), synset.words.end());
        if (q.find(lemma) == q.end()) {
            auto found_example = synset.gloss.find(";");
            q[lemma] = found_example != std::string::npos ? synset.gloss.substr(0, found_example) : synset.gloss;
        }
        else {
            --i;
        }
    }

    auto chosen = *select_randomly(q.begin(), q.end());
    return chosen.first;
}
