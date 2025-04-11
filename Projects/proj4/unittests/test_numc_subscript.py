from unittest import TestCase
import numc as nc
import numpy as np


class TestSubscript(TestCase):
    def test_wrong_1d_args(self):
        mat = nc.Matrix(1, 5)

        with self.assertRaises(TypeError):
            mat[1.0]
            mat[nc.Matrix(1)]
            mat[True]
            mat[[1, 2]]
            mat[{"key": "value"}]
            mat[(1, 2)]

        # Test ValueError cases - invalid slice parameters
        with self.assertRaises(ValueError):
            mat[0:5:2]  # Slice with step != 1
            mat[2:1]  # Slice with length < 1
            mat[::-1]  # Negative step

        with self.assertRaises(IndexError):
            mat[5]
            mat[-6]

    def test_1d_int_subscript(self):
        row_values = [1, 2, 3]
        row_mat = nc.Matrix(1, 3)
        for i in range(3):
            row_mat.set(0, i, row_values[i])

        for i in range(3):
            assert row_values[i] == row_mat[i]

        col_values = [4, 5, 6]
        col_mat = nc.Matrix(3, 1)
        for i in range(3):
            col_mat.set(i, 0, col_values[i])

        for i in range(3):
            assert col_values[i] == col_mat[i]

    def test_1d_slice_subscript(self):
        row_mat = nc.Matrix(1, 6)
        for i in range(6):
            row_mat.set(0, i, i + 1)

        assert np.allclose(nc.to_list(row_mat[1:5]), [[2, 3, 4, 5]])
        assert np.allclose(nc.to_list(row_mat[2:]), [[3, 4, 5, 6]])
        assert np.allclose(nc.to_list(row_mat[:3]), [[1, 2, 3]])
        assert np.allclose(nc.to_list(row_mat[:]), [[1, 2, 3, 4, 5, 6]])

        col_mat = nc.Matrix(6, 1)
        for i in range(6):
            col_mat.set(i, 0, i + 1)

        assert np.allclose(nc.to_list(col_mat[1:5]), [[2, 3, 4, 5]])
        assert np.allclose(nc.to_list(col_mat[2:]), [[3, 4, 5, 6]])
        assert np.allclose(nc.to_list(col_mat[:3]), [[1, 2, 3]])
        assert np.allclose(nc.to_list(col_mat[:]), [[1, 2, 3, 4, 5, 6]])

    def test_wrong_2d_args(self):
        mat = nc.Matrix(3, 3)

        with self.assertRaises(TypeError):
            mat[1.0]
            mat[nc.Matrix(1)]
            mat[True]
            mat[[1, 2]]
            mat[{"key": "value"}]
            mat[1.5, 2]
            mat[1, 2.5]
            mat[nc.Matrix(1), 1]
            mat[1, nc.Matrix(1)]

        # Test ValueError cases - invalid slice parameters
        with self.assertRaises(ValueError):
            mat[0:3:2, :]  # First slice with step != 1
            mat[:, 0:3:2]  # Second slice with step != 1
            mat[1:1, :]  # First slice with length < 1
            mat[:, 2:1]  # Second slice with length < 1

        with self.assertRaises(IndexError):
            mat[3, 1]
            mat[1, 3]
            mat[-4, 1]
            mat[1, -4]
            mat[0:5, :]
            mat[:, 0:5]
            mat[-5:, :]
            mat[:, -5:]

    def test_2d_subscript(self):
        mat = nc.Matrix([[1, 2, 3], [4, 5, 6], [7, 8, 9]])

        assert np.allclose(nc.to_list(mat[0]), [[1, 2, 3]])
        assert np.allclose(nc.to_list(mat[1]), [[4, 5, 6]])
        assert np.allclose(nc.to_list(mat[2]), [[7, 8, 9]])

        assert np.allclose(nc.to_list(mat[0:2]), [[1, 2, 3], [4, 5, 6]])
        assert np.allclose(nc.to_list(mat[1:]), [[4, 5, 6], [7, 8, 9]])
        assert np.allclose(nc.to_list(mat[:2]), [[1, 2, 3], [4, 5, 6]])

        assert np.allclose(nc.to_list(mat[0:2, 0:2]), [[1, 2], [4, 5]])
        assert np.allclose(nc.to_list(mat[1:3, 1:3]), [[5, 6], [8, 9]])
        assert np.allclose(nc.to_list(mat[:, :]), [[1, 2, 3], [4, 5, 6], [7, 8, 9]])

        assert np.allclose(nc.to_list(mat[0, 0:2]), [[1, 2]])
        assert np.allclose(nc.to_list(mat[0:2, 1]), [[2, 5]])
        assert np.allclose(nc.to_list(mat[0:2, 1:]), [[2, 3], [5, 6]])
        assert np.allclose(nc.to_list(mat[1:, 0:2]), [[4, 5], [7, 8]])
        assert np.allclose(nc.to_list(mat[:, 2]), [[3, 6, 9]])
        assert np.allclose(nc.to_list(mat[2, :]), [[7, 8, 9]])

        assert mat[0, 0] == 1
        assert mat[1, 1] == 5
        assert mat[2, 2] == 9

        assert mat[0][1] == 2
        assert mat[1][2] == 6

        assert mat[0:1, 0:1] == 1
        assert mat[1:2, 1:2] == 5


class TestSetSubscript(TestCase):
    def test_1d_set_element(self):
        row_mat = nc.Matrix(1, 3, 0)
        row_mat[0] = 10
        row_mat[1] = 20
        row_mat[2] = 30
        assert np.allclose(nc.to_list(row_mat), [[10, 20, 30]])

        col_mat = nc.Matrix(3, 1, 0)
        col_mat[0] = 10
        col_mat[1] = 20
        col_mat[2] = 30
        assert np.allclose(nc.to_list(col_mat), [[10, 20, 30]])

    def test_1d_set_slice(self):
        row_mat = nc.Matrix(1, 5, 0)
        row_mat[1:4] = [10, 20, 30]
        assert np.allclose(nc.to_list(row_mat), [[0, 10, 20, 30, 0]])

        col_mat = nc.Matrix(5, 1, 0)
        col_mat[1:4] = [10, 20, 30]
        assert np.allclose(nc.to_list(col_mat), [[0, 10, 20, 30, 0]])

    def test_1d_set_errors(self):
        mat = nc.Matrix(1, 3, 0)

        with self.assertRaises(TypeError):
            mat[0] = "string"
            mat[0] = [1, 2]
            mat[0:2] = 5
            mat[0:2] = "string"

        with self.assertRaises(ValueError):
            mat[0:2] = [1, 2, 3]
            mat[:] = [1]
            mat[0:2] = ["string", 2]
            mat[0:2] = [1, [2]]

    def test_2d_set_element(self):
        mat = nc.Matrix(3, 3, 0)
        mat[0, 0] = 1
        mat[1, 1] = 5
        mat[2, 2] = 9
        assert np.allclose(nc.to_list(mat), [[1, 0, 0], [0, 5, 0], [0, 0, 9]])

    def test_2d_set_row_col(self):
        mat = nc.Matrix(3, 3, 0)

        mat[0, :] = [1, 2, 3]
        assert np.allclose(nc.to_list(mat), [[1, 2, 3], [0, 0, 0], [0, 0, 0]])

        mat[:, 1] = [4, 5, 6]
        assert np.allclose(nc.to_list(mat), [[1, 4, 3], [0, 5, 0], [0, 6, 0]])

    def test_2d_set_submatrix(self):
        mat = nc.Matrix(3, 3, 0)
        mat[0:2, 0:2] = [[1, 2], [3, 4]]
        assert np.allclose(nc.to_list(mat), [[1, 2, 0], [3, 4, 0], [0, 0, 0]])

        mat[:, :] = [[10, 20, 30], [40, 50, 60], [70, 80, 90]]
        assert np.allclose(nc.to_list(mat), [[10, 20, 30], [40, 50, 60], [70, 80, 90]])

    def test_2d_set_errors(self):
        mat = nc.Matrix(3, 3, 0)

        with self.assertRaises(TypeError):
            mat[0, 0] = "string"
            mat[0, 0] = [1, 2]
            mat[0, :] = 5
            mat[0:2, 0:2] = 5

        with self.assertRaises(ValueError):
            mat[0, :] = [1, 2]
            mat[:, 0] = [1, 2, 3, 4]
            mat[0, :] = [1, "string", 3]

        with self.assertRaises(ValueError):
            mat[0:2, 0:2] = [[1, 2]]
            mat[0:2, 0:2] = [[1, 2], [3, 4], [5, 6]]
            mat[0:2, 0:2] = [[1, 2, 3], [4, 5]]

        with self.assertRaises(ValueError):
            mat[0:2, 0:2] = [[1, "string"], [3, 4]]
            mat[0:2, 0:2] = [[1, 2], [nc.Matrix(1), 4]]
