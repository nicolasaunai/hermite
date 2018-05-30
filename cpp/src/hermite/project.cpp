#include <iostream>
#include <unordered_map>

#include <boost/math/special_functions/binomial.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

#include "hermite/matrix.hpp"
#include "hermite/iterators.hpp"
#include "hermite/project.hpp"
#include "hermite/templates.hpp"
#include "hermite/types.hpp"
#include "hermite/lib.hpp"
#include "hermite/io.hpp"

using namespace std;
using boost::math::binomial_coefficient;
namespace hermite
{

vec project(const vec & input, u_int dim, const ivec & dirs)
{
    u_int n_polys = input.size();
    u_int dim_sub = dirs.size();
    u_int degree = bissect_degree(dim, n_polys);
    u_int polys_sub = (u_int) binomial_coefficient<double> (degree + dim_sub, dim_sub);
    vec results(polys_sub, 0.);

    Multi_index_iterator m(dim, degree);
    Multi_index_iterator m_sub(dim_sub, degree);

    u_int i;
    unordered_map<u_int, u_int> lin_indices = hash_table(dim_sub, degree);
    for (i = 0, m.reset(); !m.isFull(); i++, m.increment())
    {
        if (isAligned(m.get(), dirs))
        {
            ivec sub_vec = extract(m.get(), dirs);
            u_int lin_index = lin_indices[hash_multi_ind(sub_vec, degree)];
            results[lin_index] = input[i];
        }
    }
    return results;
}

vec project(const vec & input, u_int dim, u_int dir)
{
    ivec dirs(1, dir);
    return project(input, dim, dirs);
}

template<typename M>
M project(const M & input, u_int dim, const ivec & dirs)
{
    u_int n_polys = matrix::size1(input);
    u_int dim_sub = dirs.size();
    u_int degree = bissect_degree(dim, n_polys);
    u_int polys_sub = (u_int) binomial_coefficient<double> (degree + dim_sub, dim_sub);
    M results = matrix::construct<M>(polys_sub, polys_sub);

    Multi_index_iterator m1(dim, degree), m2(dim, degree);
    Multi_index_iterator m_sub(dim_sub, degree);

    u_int i,j;
    unordered_map<u_int, u_int> lin_indices = hash_table(dim_sub, degree);
    for (i = 0, m1.reset(); !m1.isFull(); i++, m1.increment())
    {
        if (isAligned(m1.get(), dirs))
        {

            ivec sub_m1 = extract(m1.get(), dirs);
            u_int ind1 = lin_indices[hash_multi_ind(sub_m1, degree)];
            for (j = 0, m2.reset(); !m2.isFull(); j++, m2.increment())
            {
                if (isAligned(m2.get(), dirs))
                {
                    ivec sub_m2 = extract(m2.get(), dirs);
                    u_int ind2 = lin_indices[hash_multi_ind(sub_m2, degree)];
                    matrix::set(results, ind1, ind2, matrix::get(input, i, j));
                }
            }
        }
    }
    return results;
}

template <typename M>
M project(const M & input, u_int dim, u_int dir)
{
    ivec dirs(1, dir);
    return project(input, dim, dirs);
}

template mat project(const mat & input, u_int dim, u_int dir);
template spmat project(const spmat & input, u_int dim, u_int dir);
template mat project(const mat & input, u_int dim, const ivec & dirs);
template spmat project(const spmat & input, u_int dim, const ivec & dirs);

}
