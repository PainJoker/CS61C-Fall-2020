from unittest import TestCase
import numc as nc
import numpy as np


class TestInit(TestCase):
    def test_zero_matrix_creation(self):
        mat = nc.Matrix(2, 2)
        mat_list = nc.to_list(mat)
        expected = [[0, 0], [0, 0]]
        assert np.allclose(np.array(mat_list), np.array(expected))

    def test_1D_matrix_creation(self):
        mat = nc.Matrix(1, 3)
        mat_list = nc.to_list(mat)
        expected = [0, 0, 0]
        assert np.allclose(np.array(mat_list), np.array(expected))

    def test_simple_matrix_creation(self):
        mat = nc.Matrix(2, 2, 1)
        mat_list = nc.to_list(mat)
        expected = [[1, 1], [1, 1]]
        assert np.allclose(np.array(mat_list), np.array(expected))

    def test_matrix_creation_with_list(self):
        mat = nc.Matrix([[1, 2, 3], [4, 5, 6]])
        mat_list = nc.to_list(mat)
        expected = [[1, 2, 3], [4, 5, 6]]
        assert np.allclose(np.array(mat_list), np.array(expected))

    def test_matrix_creation_with_mixed_list(self):
        mat = nc.Matrix(2, 3, [1, 2, 3, 4, 5, 6])
        mat_list = nc.to_list(mat)
        expected = [[1, 2, 3], [4, 5, 6]]
        assert np.allclose(np.array(mat_list), np.array(expected))


class TestAdd(TestCase):
    def test_wrong_args(self):
        mat = nc.Matrix(2, 2)
        with self.assertRaises(TypeError):
            mat + 1
        with self.assertRaises(ValueError):
            mat + nc.Matrix(3, 2)

    def test_small_add(self):
        mat1 = nc.Matrix(2, 2, 1)
        mat2 = nc.Matrix(2, 2, 5)
        result = mat1 + mat2
        expected = nc.Matrix(2, 2, 6)
        assert np.allclose(np.array(nc.to_list(result)), np.array(nc.to_list(expected)))

        mat1 = nc.Matrix([[1, 2, 3], [1, 2, 3]])
        mat2 = nc.Matrix([[4, 5, 6], [7, 8, 9]])
        result = mat1 + mat2
        expected = nc.Matrix([[5, 7, 9], [8, 10, 12]])
        assert np.allclose(np.array(nc.to_list(result)), np.array(nc.to_list(expected)))


class TestSub(TestCase):
    def test_wrong_args(self):
        mat = nc.Matrix(2, 2)
        with self.assertRaises(TypeError):
            mat - 1
        with self.assertRaises(ValueError):
            mat - nc.Matrix(3, 2)

    def test_small_sub(self):
        mat1 = nc.Matrix(2, 2, 1)
        mat2 = nc.Matrix(2, 2, 5)
        result = mat1 - mat2
        expected = nc.Matrix(2, 2, -4)
        assert np.allclose(np.array(nc.to_list(result)), np.array(nc.to_list(expected)))

        mat1 = nc.Matrix([[1, 2, 3], [1, 2, 3]])
        mat2 = nc.Matrix([[4, 5, 6], [7, 8, 9]])
        result = mat1 - mat2
        expected = nc.Matrix([[-3, -3, -3], [-6, -6, -6]])
        assert np.allclose(np.array(nc.to_list(result)), np.array(nc.to_list(expected)))


class TestAbs(TestCase):
    def test_small_abs(self):
        mat = nc.Matrix(2, 2, -1)
        result = abs(mat)
        expected = nc.Matrix(2, 2, 1)
        assert np.allclose(np.array(nc.to_list(result)), np.array(nc.to_list(expected)))


class TestNeg(TestCase):
    def test_small_neg(self):
        mat = nc.Matrix(2, 2, 1)
        result = -mat
        expected = nc.Matrix(2, 2, -1)
        assert np.allclose(np.array(nc.to_list(result)), np.array(nc.to_list(expected)))


class TestMul(TestCase):
    def test_wrong_args(self):
        mat = nc.Matrix(2, 2, 1)
        with self.assertRaises(TypeError):
            mat * 2
        # 2 * mat will incur segmentation falut
        with self.assertRaises(ValueError):
            mat * nc.Matrix(4, 2, 1)
        with self.assertRaises(ValueError):
            nc.Matrix(3, 5, 1) * mat

    def test_small_mul(self):
        mat1 = nc.Matrix(2, 2, 1)
        mat2 = nc.Matrix(2, 2, 1)
        result = mat1 * mat2
        expected = nc.Matrix(2, 2, 2)
        assert np.allclose(np.array(nc.to_list(result)), np.array(nc.to_list(expected)))


class TestPow(TestCase):
    def test_wrong_args(self):
        square_mat = nc.Matrix(2, 2, 1)
        with self.assertRaises(TypeError):
            square_mat**0.5
        with self.assertRaises(ValueError):
            square_mat ** (-5)

        rectangle_mat = nc.Matrix(2, 3, 1)
        with self.assertRaises(ValueError):
            rectangle_mat**2

    def test_small_pow(self):
        mat = nc.Matrix([[5, 7], [10, 15]])
        result = mat**3
        expected = nc.Matrix([[1875, 2765], [3950, 5825]])
        assert np.allclose(np.array(nc.to_list(result)), np.array(nc.to_list(expected)))


class TestInstanceMethod(TestCase):
    def test_set_wrong_args(self):
        mat = nc.Matrix(2, 2, 1)
        with self.assertRaises(TypeError):
            mat.set(5)
            mat.set(2, 1)
            mat.set(1.0, 2.3, 1)
            mat.set(0, 1, nc.Matrix(1, 1, 2))
        with self.assertRaises(IndexError):
            mat.set(2, 2, 0)
            mat.set(1, 2, 0)
            mat.set(2, 1, 0)

    def test_get_wrong_args(self):
        mat = nc.Matrix(2, 2, 1)
        with self.assertRaises(TypeError):
            mat.get(5)
            mat.get(1.0, 2.3, 1)
            mat.get(nc.Matrix(1, 1, 2))
        with self.assertRaises(IndexError):
            mat.get(0, 3)
            mat.get(3, 3)
            mat.get(3, 0)

    def test_small_set_get(self):
        mat = nc.Matrix([[6, 7], [3, 4]])

        mat.set(0, 0, 1)
        assert 1 == mat.get(0, 0)
        mat.set(0, 1, 1)
        assert 1 == mat.get(0, 1)
        mat.set(1, 0, 1)
        assert 1 == mat.get(1, 0)
        mat.set(1, 1, 1)
        assert 1 == mat.get(1, 1)
