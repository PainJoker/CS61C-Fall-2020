from unittest import TestCase
import numc as nc


class TestSubscript(TestCase):
    def test_wrong_1d_args(self):
        mat = nc.Matrix(1, 5)  # Create a 1D matrix with 5 elements

        # Test TypeError cases - invalid types
        with self.assertRaises(TypeError):
            mat[1.0]  # Float index

        with self.assertRaises(TypeError):
            mat[nc.Matrix(1)]  # Matrix index

        with self.assertRaises(TypeError):
            mat[True]  # Boolean index

        with self.assertRaises(TypeError):
            mat[[1, 2]]  # List index

        with self.assertRaises(TypeError):
            mat[{"key": "value"}]  # Dict index

        with self.assertRaises(TypeError):
            mat[(1, 2)]  # Tuple index (only valid for 2D)

        # Test ValueError cases - invalid slice parameters
        with self.assertRaises(ValueError):
            mat[0:5:2]  # Slice with step != 1

        with self.assertRaises(ValueError):
            mat[2:1]  # Slice with length < 1

        with self.assertRaises(ValueError):
            mat[::-1]  # Negative step

        # Test IndexError cases - out of bounds indices
        with self.assertRaises(IndexError):
            mat[5]  # Index too high

        with self.assertRaises(IndexError):
            mat[-6]  # Negative index too low

        with self.assertRaises(IndexError):
            mat[0:10]  # Slice end out of range

        with self.assertRaises(IndexError):
            mat[-10:]  # Negative slice start out of range

    def test_wrong_2d_args(self):
        mat = nc.Matrix(3, 3)

        # Test TypeError cases - invalid types
        with self.assertRaises(TypeError):
            mat[1.0]  # Float index

        with self.assertRaises(TypeError):
            mat[nc.Matrix(1)]  # Matrix index

        with self.assertRaises(TypeError):
            mat[True]  # Boolean index

        with self.assertRaises(TypeError):
            mat[[1, 2]]  # List index

        with self.assertRaises(TypeError):
            mat[{"key": "value"}]  # Dict index

        with self.assertRaises(TypeError):
            mat[1.5, 2]  # Tuple with float

        with self.assertRaises(TypeError):
            mat[1, 2.5]  # Tuple with float

        with self.assertRaises(TypeError):
            mat[nc.Matrix(1), 1]  # Tuple with matrix

        with self.assertRaises(TypeError):
            mat[1, nc.Matrix(1)]  # Tuple with matrix

        # Test ValueError cases - invalid slice parameters
        with self.assertRaises(ValueError):
            mat[0:3:2, :]  # First slice with step != 1

        with self.assertRaises(ValueError):
            mat[:, 0:3:2]  # Second slice with step != 1

        with self.assertRaises(ValueError):
            mat[1:1, :]  # First slice with length < 1

        with self.assertRaises(ValueError):
            mat[:, 2:1]  # Second slice with length < 1

        # Test IndexError cases - out of bounds indices
        with self.assertRaises(IndexError):
            mat[3, 1]  # Row index out of range

        with self.assertRaises(IndexError):
            mat[1, 3]  # Column index out of range

        with self.assertRaises(IndexError):
            mat[-4, 1]  # Negative row index out of range

        with self.assertRaises(IndexError):
            mat[1, -4]  # Negative column index out of range

        with self.assertRaises(IndexError):
            mat[0:5, :]  # Slice end out of range

        with self.assertRaises(IndexError):
            mat[:, 0:5]  # Slice end out of range

        with self.assertRaises(IndexError):
            mat[-5:, :]  # Negative slice start out of range

        with self.assertRaises(IndexError):
            mat[:, -5:]  # Negative slice start out of range
