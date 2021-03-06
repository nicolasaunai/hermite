# Copyright (C) 2018 Urbain Vaes
#
# This file is part of hermipy, a python/C++ library for automating the
# Hermite Galerkin method.
#
# hermipy is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# hermipy is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

import hermipy.core as core
import hermite_cpp as hm
import unittest

import numpy as np
import numpy.linalg as la
import scipy.sparse as ss


class TestConversions(unittest.TestCase):

    def test_simple_sparse(self):
        matrix = [[0, 1, 0, 0, 0],
                  [0, 2, 0, 3, 0],
                  [0, 0, 4, 0, 0],
                  [5, 6, 0, 0, 0],
                  [7, 0, 0, 0, 8]]
        matrix = np.array(matrix, dtype=float)
        csr_mat = ss.csr_matrix(matrix)
        spmat = core.convert_to_cpp_sparse(csr_mat)
        cpp_mat = hm.full(spmat)
        self.assertTrue(la.norm(hm.to_numpy(cpp_mat) - matrix) < 1e-10)
