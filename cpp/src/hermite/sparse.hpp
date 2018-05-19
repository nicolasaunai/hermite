#ifndef SPARSE_H
#define SPARSE_H

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include "hermite/types.hpp"

namespace bnu = boost::numeric::ublas;
namespace hermite {

mat row_col_val(bnu::compressed_matrix<double, bnu::row_major> input);
bnu::compressed_matrix<double, bnu::row_major> to_spmat(mat input);
mat full(bnu::compressed_matrix<double, bnu::row_major> input);

}

#endif