#ifndef HERMITE_ITERATORS_H
#define HERMITE_ITERATORS_H

#include "hermite/types.hpp"

namespace hermite {

class Vector_iterator {

    protected:

    public:

    unsigned int dim;
    std::ivec multi_index;
    bool full;


    const std::ivec& get() const {
        return multi_index;
    }

    int operator[](int i) {
        return multi_index[i];
    }

    bool isFull() const {
        return full;
    }

    public:

    virtual void increment() = 0;
    Vector_iterator(int dim): dim(dim), multi_index(std::ivec(dim, 0)), full(false) {}

};

class Multi_index_iterator : public Vector_iterator {

    unsigned int sum;

    // Upper bound included (like polynomial degree)
    const unsigned int upper_bound;

    public:

    void increment();
    Multi_index_iterator(unsigned int dim, unsigned int upper_bound):
        Vector_iterator(dim), sum(0), upper_bound(upper_bound) {}
};

class Hyper_cube_iterator : public Vector_iterator {

    // Upper bounds excluded
    const std::ivec upper_bounds;

    public:

    void increment();
    Hyper_cube_iterator(const std::ivec & upper_bounds):
        Vector_iterator(upper_bounds.size()), upper_bounds(upper_bounds) {}
};}

#endif
