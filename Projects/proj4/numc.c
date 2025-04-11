#include "numc.h"
#include <structmember.h>

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject *get_shape(int rows, int cols) {
  if (rows == 1 || cols == 1) {
    return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
  } else {
    return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
  }
}
/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low,
              double high) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    rand_matrix(new_mat, seed, low, high);
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject *self, int rows, int cols, double val) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed)
        return alloc_failed;
    else {
        fill_matrix(new_mat, val);
        ((Matrix61c *)self)->mat = new_mat;
        ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    }
    return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with 1d_list values
 */
int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
    if (rows * cols != PyList_Size(lst)) {
        PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
        return -1;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject *self, PyObject *lst) {
    int rows = PyList_Size(lst);
    if (rows == 0) {
        PyErr_SetString(PyExc_ValueError,
                        "Cannot initialize numc.Matrix with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0))) {
        PyErr_SetString(PyExc_ValueError, "List values not valid");
        return -1;
    } else {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++) {
        if (!PyList_Check(PyList_GetItem(lst, i)) ||
                PyList_Size(PyList_GetItem(lst, i)) != cols) {
            PyErr_SetString(PyExc_ValueError, "List values not valid");
            return -1;
        }
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j,
                PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c *self) {
    deallocate_matrix(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args,
                        PyObject *kwds) {
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    Matrix61c *self = (Matrix61c *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success otherwise -1
 */
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
    /* Generate random matrices */
    if (kwds != NULL) {
        PyObject *rand = PyDict_GetItemString(kwds, "rand");
        if (!rand) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand)) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject *low = PyDict_GetItemString(kwds, "low");
        PyObject *high = PyDict_GetItemString(kwds, "high");
        PyObject *seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low) {
            if (PyFloat_Check(low)) {
                double_low = PyFloat_AsDouble(low);
            } else if (PyLong_Check(low)) {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high) {
            if (PyFloat_Check(high)) {
                double_high = PyFloat_AsDouble(high);
            } else if (PyLong_Check(high)) {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed) {
            if (PyLong_Check(seed)) {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject *rows = NULL;
        PyObject *cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low,
                                 double_high);
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3)
                || PyFloat_Check(arg3))) {
            if (PyLong_Check(arg3)) {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            } else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3)) {
            /* Matrix(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_2d(self, arg1);
        } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/*
 * List of lists representations for matrices
 */
PyObject *Matrix61c_to_list(Matrix61c *self) {
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject *py_lst = NULL;
    if (self->mat->is_1d) {  // If 1D matrix, print as a single list
        py_lst = PyList_New(rows * cols);
        int count = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
                count++;
            }
        }
    } else {  // if 2D, print as nested list
        py_lst = PyList_New(rows);
        for (int i = 0; i < rows; i++) {
            PyList_SetItem(py_lst, i, PyList_New(cols));
            PyObject *curr_row = PyList_GetItem(py_lst, i);
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
            }
        }
    }
    return py_lst;
}

PyObject *Matrix61c_class_to_list(Matrix61c *self, PyObject *args) {
    PyObject *mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
        if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
            PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
            return NULL;
        }
        Matrix61c* mat61c = (Matrix61c*)mat;
        return Matrix61c_to_list(mat61c);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
    {"to_list", (PyCFunction)Matrix61c_class_to_list, METH_VARARGS, "Returns a list representation of numc.Matrix"},
    {NULL, NULL, 0, NULL}
};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject *Matrix61c_repr(PyObject *self) {
    PyObject *py_lst = Matrix61c_to_list((Matrix61c *)self);
    return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/**
 * helper function for NUMBER METHODS.
 */
static matrix* create_matrix(int rows, int cols) {
    matrix *result = NULL;
    int fail_alloc = allocate_matrix(&result, rows, cols);
    if(fail_alloc) {
        deallocate_matrix(result);
        PyErr_SetString(PyExc_RuntimeError, "Memory allocate failed.");
        return NULL;
    }

    return result;
}

static PyObject* wrap_matrix_as_pyobject(matrix* mat) {
    if(!mat) {
        return NULL;
    }

    Matrix61c *rv = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
    rv->mat = mat;
    rv->shape = get_shape(mat->rows, mat->cols);
    return (PyObject *)rv;
}

/*
 * Add the second numc.Matrix (Matrix61c) object to the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_add(Matrix61c* self, PyObject* args) {
    if(!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }

    matrix *mat1 = self->mat;
    matrix *mat2 = ((Matrix61c *)args)->mat;
    matrix *result = create_matrix(mat1->rows, mat1->cols);

    int fail_add = add_matrix(result, mat1, mat2);
    if(fail_add) {
        deallocate_matrix(result);
        if(fail_add == -2) {
            PyErr_SetString(PyExc_ValueError, "+ needs same shape!");
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Add operation failed.");
        }
        return NULL;
    }

    return wrap_matrix_as_pyobject(result);
}

/*
 * Substract the second numc.Matrix (Matrix61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_sub(Matrix61c* self, PyObject* args) {
    if(!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }

    matrix *mat1 = self->mat;
    matrix *mat2 = ((Matrix61c *)args)->mat;
    matrix *result = create_matrix(mat1->rows, mat1->cols);

    int fail_sub = sub_matrix(result, mat1, mat2);
    if(fail_sub) {
        deallocate_matrix(result);
        if(fail_sub == -2) {
            PyErr_SetString(PyExc_ValueError, "- needs same shape!");
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Sub operation failed.");
        }
        return NULL;
    }

    return wrap_matrix_as_pyobject(result);
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second operand
 * can be obtained by casting `args`.
 */
PyObject *Matrix61c_multiply(Matrix61c* self, PyObject *args) {
    if(!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }

    matrix *mat1 = self->mat;
    matrix *mat2 = ((Matrix61c *)args)->mat;
    matrix *result = create_matrix(mat1->rows, mat2->cols);

    int fail_mul = mul_matrix(result, mat1, mat2);
    if(fail_mul) {
        deallocate_matrix(result);
        if(fail_mul == -2) {
            PyErr_SetString(PyExc_ValueError, "mat1's cols must be equal to mat2's rows!");
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Mul operation failed.");
        }
        return NULL;
    }

    return wrap_matrix_as_pyobject(result);
}

/*
 * Negates the given numc.Matrix.
 */
PyObject *Matrix61c_neg(Matrix61c* self) {
    matrix *mat = self->mat;
    matrix *result = create_matrix(mat->rows, mat->cols);

    int fail_neg = neg_matrix(result, mat);
    if(fail_neg) {
        PyErr_SetString(PyExc_RuntimeError, "Neg operation failed.");
        return NULL;
    }

    return wrap_matrix_as_pyobject(result);
}

/*
 * Take the element-wise absolute value of this numc.Matrix.
 */
PyObject *Matrix61c_abs(Matrix61c *self) {
    matrix *mat = self->mat;
    matrix *result = create_matrix(mat->rows, mat->cols);

    int fail_abs = abs_matrix(result, mat);
    if(fail_abs) {
        PyErr_SetString(PyExc_RuntimeError, "Abs operation failed.");
        return NULL;
    }

    return wrap_matrix_as_pyobject(result);
}

/*
 * Raise numc.Matrix (Matrix61c) to the `pow`th power. You can ignore the argument `optional`.
 */
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional) {
    if(!PyLong_Check(pow)) {
        PyErr_SetString(PyExc_TypeError, "pow must of type integer!");
        return NULL;
    }

    long pow_val = PyLong_AsLong(pow);
    if(pow_val < 0) {
        PyErr_SetString(PyExc_ValueError, "pow must greater than 0.");
        return NULL;
    }

    matrix *mat = self->mat;
    matrix *result = create_matrix(mat->rows, mat->cols);
    int fail_pow = pow_matrix(result, mat, pow_val);
    if(fail_pow) {
        deallocate_matrix(result);
        if(fail_pow == -2) {
            PyErr_SetString(PyExc_ValueError, "mat must be square!"); 
        } else {
            PyErr_SetString(PyExc_RuntimeError, "pow operation failed."); 
        }
        return NULL;
    }

    return wrap_matrix_as_pyobject(result);
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods you have
 * define. You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
    .nb_add = (binaryfunc)Matrix61c_add,
    .nb_subtract = (binaryfunc)Matrix61c_sub,
    .nb_multiply = (binaryfunc)Matrix61c_multiply,
    .nb_absolute = (unaryfunc)Matrix61c_abs,
    .nb_negative = (unaryfunc)Matrix61c_neg,
    .nb_power = (ternaryfunc)Matrix61c_pow,
};


/* INSTANCE METHODS */

/*
 * Given a numc.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val.
 * Return None in Python (this is different from returning null).
 */
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject* args) {
    PyObject *row_obj = NULL;
    PyObject *col_obj = NULL;
    PyObject *val_obj = NULL;

    if(!PyArg_UnpackTuple(args, "args", 3, 3, &row_obj, &col_obj, &val_obj)) {
        PyErr_SetString(PyExc_TypeError, "set() require exactly 3 arguments (row, col, value).");
        return NULL;
    }

    if(!PyLong_CheckExact(row_obj) || !PyLong_CheckExact(col_obj)) {
        PyErr_SetString(PyExc_TypeError, "row and col indices must of type integer.");
        return NULL;
    }

    if(!PyLong_CheckExact(val_obj) && !PyFloat_Check(val_obj)) {
        PyErr_SetString(PyExc_TypeError, "value must be numeric (int or float).");
        return NULL;
    }

    int row = (int)PyLong_AsLong(row_obj);
    int col = (int)PyLong_AsLong(col_obj);
    double val = PyLong_CheckExact(val_obj) ? (double)PyLong_AS_LONG(val_obj) : PyFloat_AS_DOUBLE(val_obj);
    if(row < 0 || row >= self->mat->rows || col < 0 || col >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Matrix indices out of range.");
        return NULL;
    }

    set(self->mat, row, col, val);

    Py_RETURN_NONE;
}

/*
 * Given a numc.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject* args) {
    PyObject *row_obj = NULL;
    PyObject *col_obj = NULL;

    if(!PyArg_UnpackTuple(args, "args", 2, 2, &row_obj, &col_obj)) {
        PyErr_SetString(PyExc_TypeError, "get() require exactly 2 arguments (row, col).");
        return NULL;
    }

    if(!PyLong_CheckExact(row_obj) || !PyLong_CheckExact(col_obj)) {
        PyErr_SetString(PyExc_TypeError, "row and col indices must of type integer.");
        return NULL;
    }

    int row = (int)PyLong_AsLong(row_obj);
    int col = (int)PyLong_AsLong(col_obj);
    if(row < 0 || row >= self->mat->rows || col < 0 || col >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Matrix indices out of range.");
        return NULL;
    }
    
    double val = get(self->mat, row, col);
    return PyFloat_FromDouble(val);
}


/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and Matrix61c_set_value
 * as "set"
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
    {"set", (PyCFunction)Matrix61c_set_value, METH_VARARGS, 
        "Set self's entry at the ith row and jth column to val"
    }, 
    {"get", (PyCFunction)Matrix61c_get_value, METH_VARARGS, 
        "Get value at ith row and jth column."
    }, 
    {NULL, NULL, 0, NULL}
};

/* INDEXING */
/**
 * Helper function for 1D/2D matrix subscript.
 */
static int parse_slice(PyObject* key, Py_ssize_t length, 
    Py_ssize_t* start, Py_ssize_t* slicelength) {

    Py_ssize_t stop, step;
    int fail_slice = PySlice_GetIndicesEx(key, length, start, &stop, &step, slicelength);
    if(fail_slice) {
        PyErr_SetString(PyExc_RuntimeError, "Slice unpack failed.");
        return -1;
    }
    if(step != 1) {
        PyErr_SetString(PyExc_ValueError, "Slice step must be 1.");
        return -2;
    }
    if(slicelength < 1) {
        PyErr_SetString(PyExc_ValueError, "Slice length must be at least 1.");
        return -3;
    }

    return 0;
}

void safe_allocate_matrix_ref(matrix** p, matrix* parent, int row_start, 
    int col_start, int rows, int cols){
    if(allocate_matrix_ref(p, parent, row_start, col_start, rows, cols)) {
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed.");
        deallocate_matrix(*p);
        return NULL;
    }
}

static *Matrix61c_1d_int_subscript(Matrix61c* self, PyObject* key) {
    matrix *mat = self->mat;
    int idx = PyLong_AS_LONG(key);
    int len = (mat->rows == 1) ? mat->cols : mat->rows;
    if(idx < 0 || idx >= len) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return NULL;
    }
    
    double val;
    if(mat->rows == 1) {
        val = get(mat, 0, idx);
    } else {
        val = get(mat, idx, 0);
    }

    return PyFloat_FromDouble(val);
}

static PyObject *Matrix61c_1d_slice_subscript(Matrix61c* self, PyObject* key) {
    matrix *parent = self->mat;
    Py_ssize_t start, slicelength;
    Py_ssize_t length = parent->rows * parent->cols;
    if(parse_slice(key, length, &start, &slicelength)) {
        return NULL;
    }

    matrix *slice = NULL;
    if(parent->rows == 1) {
        safe_allocate_matrix_ref(&slice, parent, 0, start, 1, slicelength);
    } else {
        safe_allocate_matrix_ref(&slice, parent, start, 0, slicelength, 1);
    }

    return wrap_matrix_as_pyobject(slice);
}

    
static *matrix_2d_int_int_subscript(matrix* mat, PyObject* first, PyObject* second) {
    int row = PyLong_AS_LONG(first);
    int col = PyLong_AS_LONG(second);
    if(row < 0 || col < 0 || row >= mat->rows || col >= mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return NULL;
    }

    double val = get(mat, row, col);
    return PyFloat_FromDouble(val);
}

static *matrix_2d_int_slice_subscript(matrix* mat, PyObject* first, PyObject* second) {
    int row = PyLong_AS_LONG(first);
    if(row < 0 || row >= mat->rows) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return NULL;
    }

    Py_ssize_t start, slicelength;
    Py_ssize_t length = mat->cols;
    if(parse_slice(second, length, &start, &slicelength)) {
        return NULL;
    }

    if(slicelength == 1) {
        double val = get(mat, row, start);
        return PyFloat_FromDouble(val);
    }

    matrix *rv = NULL;
    safe_allocate_matrix_ref(&rv, mat, row, start, 1, slicelength);
    return wrap_matrix_as_pyobject(rv);
}

static PyObject *matrix_2d_slice_int_subscript(matrix* mat, PyObject* first, PyObject* second) {
    int col = PyLong_AS_LONG(second);
    if(col < 0 || col >= mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return NULL;
    }

    Py_ssize_t start, slicelength;
    Py_ssize_t length = mat->rows;
    if(parse_slice(first, length, &start, &slicelength)) {
        return NULL;
    }
    
    if(slicelength == 1) {
        double val = get(mat, start, col);
        return PyFloat_FromDouble(val);
    }

    matrix *rv = NULL;
    safe_allocate_matrix_ref(&rv, mat, start, col, slicelength, 1);
    return wrap_matrix_as_pyobject(rv);
}

static *matrix_2d_slice_slice_subscript(matrix* mat, PyObject* first, PyObject* second) {
    Py_ssize_t row_start, row_slicelength;
    Py_ssize_t row_length = mat->rows;
    if(parse_slice(first, row_length, &row_start, &row_slicelength)) {
        return NULL;
    }

    Py_ssize_t col_start, col_slicelength;
    Py_ssize_t col_length = mat->cols;
    if(parse_slice(second, col_length, &col_start, &col_slicelength)) {
        return NULL;
    }

    if(row_slicelength == 1 && col_slicelength == 1) {
        double val = get(mat, row_start, col_start);
        return PyFloat_FromDouble(val);
    }
    
    matrix *rv = NULL;
    safe_allocate_matrix_ref(&rv, mat, row_start, col_start, row_slicelength, col_slicelength);
    return wrap_matrix_as_pyobject(rv);
}

static *Matrix61c_2d_tuple_subscript(Matrix61c* self, PyObject* key) {
    if(PyTuple_Size(key) != 2) {
        PyErr_SetString(PyExc_TypeError, "Tuple index must have exactly 2 elements.");
        return NULL;
    }

    PyObject *first = PyTuple_GetItem(key, 0);
    PyObject *second = PyTuple_GetItem(key, 1);
    if((!PyLong_CheckExact(first) && !PySlice_Check(first)) ||
        (!PyLong_CheckExact(second) && !PySlice_Check(second))) {
        PyErr_SetString(PyExc_TypeError, "Tuple elments must be of type int or slices.");
        return NULL;
    }
    
    PyObject *rv = NULL;
    if(PyLong_CheckExact(first) && PyLong_CheckExact(second)) {
        rv = matrix_2d_int_int_subscript(self->mat, first, second);
    } else if(PyLong_Check(first) && PySlice_Check(second)) {
        rv = matrix_2d_int_slice_subscript(self->mat, first, second);
    } else if(PySlice_Check(first) && PyLong_Check(second)) {
        rv = matrix_2d_slice_int_subscript(self->mat, first, second);
    } else {
        rv = matrix_2d_slice_slice_subscript(self->mat, first, second);
    }
    
    return rv;
}

static *Matrix61c_2d_slice_subscript(Matrix61c* self, PyObject* key) {
    matrix *mat = self->mat;
    Py_ssize_t start, slicelength;
    Py_ssize_t length = mat->rows;
    if(parse_slice(key, length, &start, &slicelength)) {
        return NULL;
    }

    matrix *rv = NULL;
    safe_allocate_matrix_ref(&rv, mat, start, 0, slicelength, mat->cols);
    return wrap_matrix_as_pyobject(rv);
}

static *Matrix61c_2d_int_subscript(Matrix61c* self, PyObject* key) {
    int idx = PyLong_AS_LONG(key);
    matrix *mat = self->mat;
    if(idx < 0 || idx >= mat->rows) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return NULL;
    }

    matrix *rv = NULL;
    safe_allocate_matrix_ref(&rv, mat, idx, 0, 1, mat->cols);
    return wrap_matrix_as_pyobject(rv);
}

static *Matrix61c_2d_subscript(Matrix61c* self, PyObject* key) {
    if(!PyLong_CheckExact(key) && !PySlice_Check(key) && !PyTuple_Check(key)) {
        PyErr_SetString(PyExc_TypeError, "Index key of mat must of type integer or slice.");
        return NULL;
    }

    PyObject *rv = NULL;
    if(PyTuple_Check(key)) {
        rv = Matrix61c_2d_tuple_subscript(self, key);
    } else if (PySlice_Check(key)) {
        rv = Matrix61c_2d_slice_subscript(self, key);
    } else {
        rv = Matrix61c_2d_int_subscript(self, key);
    }

    return rv;
}

static *Matrix61c_1d_subscript(Matrix61c* self, PyObject* key) {
    if(!PyLong_CheckExact(key) && !PySlice_Check(key)) {
        PyErr_SetString(PyExc_TypeError, "Index key of mat must of type integer or slice.");
        return NULL;
    }

    PyObject *rv = NULL;
    if(PyLong_CheckExact(key)) {
        rv = Matrix61c_1d_int_subscript(self, key);
    } else {
        rv = Matrix61c_1d_slice_subscript(self, key);
    }

    return rv;
}

/*
 * Given a numc.Matrix `self`, index into it with `key`. Return the indexed result.
 */
PyObject *Matrix61c_subscript(Matrix61c* self, PyObject* key) {
    PyObject *rv = NULL;
    if(self->mat->is_1d) {
        rv = Matrix61c_1d_subscript(self, key);
    } else {
        rv = Matrix61c_2d_subscript(self, key);
    }

    return rv;
}

/**
 * Helper function for Matrix61c_set_subscript
 */
static int Matrix61c_set_1d_int_subscript(Matrix61c* self, PyObject* key, PyObject* v) {
    matrix *mat = self->mat;
    int idx = PyLong_AS_LONG(key);
    int len = (mat->rows == 1) ? mat->cols : mat->rows;
    if(idx < 0 || idx >= len) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return -2;
    }

    if(!PyLong_CheckExact(v) && !PyFloat_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "Value must of type int or float.");
        return -3;
    }
    
    double val = PyFloat_AsDouble(v);
    if(mat->rows == 1) {
        set(mat, 0, idx, val);
    } else {
        set(mat, idx, 0, val);
    }
    
    return 0;
}

static int Matrix61c_set_1d_slice_subscript(Matrix61c* self, PyObject* key, PyObject* v) {
    if(!PyList_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "Value must of type list.");
        return -1;
    }

    matrix *mat = self->mat;
    Py_ssize_t start, slicelength;
    Py_ssize_t length = mat->rows * mat->cols;
    int fail_parse = parse_slice(key, length, &start, &slicelength);
    if(fail_parse) {
        return fail_parse;
    }

    Py_ssize_t v_len = PyList_Size(v);
    if(v_len != slicelength) {
        PyErr_SetString(PyExc_ValueError, "Value has the wrong length.");
        return -2;
    }

    for(Py_ssize_t i = 0; i < slicelength; i++) {
        PyObject *item = PyList_GetItem(v, i);
        if(!PyLong_CheckExact(item) && !PyFloat_Check(item)) {
            PyErr_SetString(PyExc_ValueError, "List value must of type int or float.");
            return -2;
        }

        double val = PyFloat_AsDouble(item);
        if(mat->rows == 1) {
            set(mat, 0, start + i, val);
        } else {
            set(mat, start + i, 0, val);
        }
    }

    return 0;
}

static int matrix_set_2d_int_int_subscript(matrix *mat, PyObject *first, PyObject* second, PyObject* v) {
    int row = PyLong_AS_LONG(first);
    int col = PyLong_AS_LONG(second);
    if(row < 0 || col < 0 || row >= mat->rows || col >= mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return -3;
    }
    if(!PyLong_CheckExact(v) && !PyFloat_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "Value must be of type int or float.");
        return -1;
    }

    double val = PyFloat_AsDouble(v);
    set(mat, row, col, val);
    return 0;
}

static int matrix_set_2d_int_slice_subscript(matrix *mat, PyObject *first, PyObject* second, PyObject* v) {
    int row = PyLong_AS_LONG(first);
    if(row < 0 || row >= mat->rows) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return NULL;
    }

    Py_ssize_t start, slicelength;
    Py_ssize_t length = mat->cols;
    int fail_parse = parse_slice(second, length, &start, &slicelength);
    if(fail_parse) {
        return fail_parse;
    }

    if(slicelength == 1) {
        if(!PyLong_CheckExact(v) && !PyFloat_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "Value must be of type int or float.");
            return -1;
        }
        double val = PyFloat_AsDouble(v);
        set(mat, row, start, val);
        return 0;
    }

    if(!PyList_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "Value must be of type list.");
        return -1;
    }
    if(PyList_Size(v) != slicelength) {
        PyErr_SetString(PyExc_ValueError, "Value has the wrong length.");
        return -2;
    }

    for(Py_ssize_t i = 0; i < slicelength; i++) {
        PyObject *item = PyList_GetItem(v, i);
        if(!PyLong_CheckExact(item) && !PyFloat_Check(item)) {
            PyErr_SetString(PyExc_TypeError, "Element of list must of type int or float.");
            return -1;
        }

        double val = PyFloat_AsDouble(item);
        set(mat, row, start + i, val);
    }

    return 0;
}

static int matrix_set_2d_slice_int_subscript(matrix *mat, PyObject *first, PyObject* second, PyObject* v) {
    int col = PyLong_AS_LONG(second);
    if(col < 0 || col >= mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return NULL;
    }

    Py_ssize_t start, slicelength;
    Py_ssize_t length = mat->rows;
    int fail_parse = parse_slice(first, length, &start, &slicelength);
    if(fail_parse) {
        return fail_parse;
    }

    if(slicelength == 1) {
        if(!PyLong_CheckExact(v) && !PyFloat_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "Value must be of type int or float.");
            return -1;
        }
        double val = PyFloat_AsDouble(v);
        set(mat, start, col, val);
        return 0;
    }

    if(!PyList_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "Value must be of type list.");
        return -1;
    }
    if(PyList_Size(v) != slicelength) {
        PyErr_SetString(PyExc_ValueError, "Value has the wrong length.");
        return -2;
    }

    for(Py_ssize_t i = 0; i < slicelength; i++) {
        PyObject *item = PyList_GetItem(v, i);
        if(!PyLong_CheckExact(item) && !PyFloat_Check(item)) {
            PyErr_SetString(PyExc_TypeError, "Element of list must of type int or float.");
            return -1;
        }

        double val = PyFloat_AsDouble(item);
        set(mat, start + i, col, val);
    }

    return 0;
}

static int matrix_set_2d_slice_slice_subscript(matrix *mat, PyObject *first, PyObject* second, PyObject* v) {
    Py_ssize_t row_start, row_slicelength;
    Py_ssize_t row_length = mat->rows;
    int row_fail_parse = parse_slice(first, row_length, &row_start, &row_slicelength);
    if(row_fail_parse) {
        return row_fail_parse;
    }

    Py_ssize_t col_start, col_slicelength;
    Py_ssize_t col_length = mat->cols;
    int col_fail_parse = parse_slice(second, col_length, &col_start, &col_slicelength);
    if(col_fail_parse) {
        return col_fail_parse;
    }

    if(row_slicelength == 1 && col_slicelength == 1) {
        if(!PyLong_CheckExact(v) && !PyFloat_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "Value must be of type int or float.");
            return -1;
        }
        double val = PyFloat_AsDouble(v);
        set(mat, row_start, col_start, val);
        return 0;
    }

    if(PyList_Size(v) != row_slicelength) {
        PyErr_SetString(PyExc_ValueError, "Value has wrong number of rows.");
        return -2;
    }

    for(Py_ssize_t i = 0; i < row_slicelength; i++) {
        PyObject *row = PyList_GetItem(v, i);
        
        if(!PyList_Check(row)) {
            PyErr_SetString(PyExc_TypeError, "Elements of list must be of type list.");
            return -1;
        }
        if(PyList_Size(row) != col_slicelength) {
            PyErr_SetString(PyExc_ValueError, "Value has wrong number of columns.");
            return -2;
        }
        
        for(Py_ssize_t j = 0; j < col_slicelength; j++) {
            PyObject *element = PyList_GetItem(row, j);
            
            if(!PyLong_CheckExact(element) && !PyFloat_Check(element)) {
                PyErr_SetString(PyExc_ValueError, "Matrix elements must be of type int or float.");
                return -1;
            }
            
            double val = PyFloat_AsDouble(element);
            set(mat, row_start + i, col_start + j, val);
        }
    }

    return 0;
}

static int Matrix61x_set_2d_tuple_subscript(Matrix61c* self, PyObject* key, PyObject* v) {
    if(PyTuple_Size(key) != 2) {
        PyErr_SetString(PyExc_TypeError, "Tuple index must have exactly 2 elements.");
        return -1;
    }

    PyObject *first = PyTuple_GetItem(key, 0);
    PyObject *second = PyTuple_GetItem(key, 1);
    if((!PyLong_CheckExact(first) && !PySlice_Check(first)) ||
        (!PyLong_CheckExact(second) && !PySlice_Check(second))) {
        PyErr_SetString(PyExc_TypeError, "Tuple elments must be of type int or slices.");
        return -1;
    }

    if(PyLong_CheckExact(first) && PyLong_CheckExact(second)) {
        return matrix_set_2d_int_int_subscript(self->mat, first, second, v);
    } else if(PyLong_Check(first) && PySlice_Check(second)) {
        return matrix_set_2d_int_slice_subscript(self->mat, first, second, v);
    } else if(PySlice_Check(first) && PyLong_Check(second)) {
        return matrix_set_2d_slice_int_subscript(self->mat, first, second, v);
    } else {
        return matrix_set_2d_slice_slice_subscript(self->mat, first, second, v);
    }
}

static int Matrix61x_set_2d_slice_subscript(Matrix61c* self, PyObject* key, PyObject* v) {
    matrix *mat = self->mat;
    Py_ssize_t start, slicelength;
    Py_ssize_t length = mat->rows;
    int fail_parse = parse_slice(key, length, &start, &slicelength);
    if(fail_parse) {
        return fail_parse;
    }

    if(!PyList_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "Value must of type list.");
        return -1;
    }

    if(PyList_Size(v) != slicelength) {
        PyErr_SetString(PyExc_ValueError, "Value has the wrong length.");
        return -2;
    }

    for(Py_ssize_t i = 0; i < slicelength; i++) {
        PyObject *item = PyList_GetItem(v, i);
        if(!PyList_Check(item)) {
            PyErr_SetString(PyExc_ValueError, "Elements of list must of type list.");
            return -2;
        }
        if(PyList_Size(item) != mat->cols) {
            PyErr_SetString(PyExc_ValueError, "Value has the wrong length.");
            return -2;
        }

        for(Py_ssize_t j = 0; j < mat->cols; j++) {
            PyObject *element  = PyList_GetItem(item, j);
            if(!PyLong_CheckExact(element) && !PyFloat_Check(element)) {
                PyErr_SetString(PyExc_ValueError, "List element must be of type int or float.");
                return -2;
            }

            double val = PyFloat_AsDouble(element);
            set(mat, start + i, j, val);
        }
    }

    return 0;
}

static int Matrix61x_set_2d_int_subscript(Matrix61c* self, PyObject* key, PyObject* v) {
    matrix *mat = self->mat;
    int idx = PyLong_AS_LONG(key);
    if(idx < 0 || idx >= mat->rows) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return -1;
    }

    if(!PyList_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "Value must of type list.");
        return -1;
    }

    Py_ssize_t v_len =PyList_Size(v);
    if(v_len != mat->cols) {
        PyErr_SetString(PyExc_ValueError, "Value has the wrong length.");
        return -2;
    }

    for(Py_ssize_t i = 0; i < mat->cols; i++) {
        PyObject *item = PyList_GetItem(v, i);
        if(!PyLong_CheckExact(item) && !PyFloat_Check(item)) {
            PyErr_SetString(PyExc_ValueError, "List value must of type int or float.");
            return -2;
        }

        double val = PyFloat_AsDouble(item);
        set(mat, idx, i, val);
    }

    return 0;
}

int Matrix61c_set_1d_subscript(Matrix61c* self, PyObject* key, PyObject* v) {
    if(!PyLong_CheckExact(key) && !PySlice_Check(key)) {
        PyErr_SetString(PyExc_TypeError, "Index key of mat must of type integer or slice.");
        return -1;
    }
    
    if(PyLong_CheckExact(key)) {
        return Matrix61c_set_1d_int_subscript(self, key, v);
    } else {
        return Matrix61c_set_1d_slice_subscript(self, key, v);
    }
}

int Matrix61c_set_2d_subscript(Matrix61c* self, PyObject* key, PyObject* v) {
    if(!PyLong_CheckExact(key) && !PySlice_Check(key) && !PyTuple_Check(key)) {
        PyErr_SetString(PyExc_TypeError, "Index key of mat must of type integer or slice.");
        return -1;
    }
    
    if(PyTuple_Check(key)) {
        return Matrix61x_set_2d_tuple_subscript(self, key, v);
    } else if(PySlice_Check(key)) {
        return Matrix61x_set_2d_slice_subscript(self, key, v);
    } else {
        return Matrix61x_set_2d_int_subscript(self, key, v);
    }
}
/*
 * Given a numc.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
 */
int Matrix61c_set_subscript(Matrix61c* self, PyObject *key, PyObject *v) {
    if(self->mat->is_1d) {
        return Matrix61c_set_1d_subscript(self, key, v);
    } else {
        return Matrix61c_set_2d_subscript(self, key, v);
    }
    return 0;
}

PyMappingMethods Matrix61c_mapping = {
    NULL,
    (binaryfunc) Matrix61c_subscript,
    (objobjargproc) Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
    {
        "shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0,
        "(rows, cols)"
    },
    {NULL}  /* Sentinel */
};

PyTypeObject Matrix61cType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "numc.Matrix",
    .tp_basicsize = sizeof(Matrix61c),
    .tp_dealloc = (destructor)Matrix61c_dealloc,
    .tp_repr = (reprfunc)Matrix61c_repr,
    .tp_as_number = &Matrix61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,
    .tp_doc = "numc.Matrix objects",
    .tp_methods = Matrix61c_methods,
    .tp_members = Matrix61c_members,
    .tp_as_mapping = &Matrix61c_mapping,
    .tp_init = (initproc)Matrix61c_init,
    .tp_new = Matrix61c_new
};


struct PyModuleDef numcmodule = {
    PyModuleDef_HEAD_INIT,
    "numc",
    "Numc matrix operations",
    -1,
    Matrix61c_class_methods
};

/* Initialize the numc module */
PyMODINIT_FUNC PyInit_numc(void) {
    PyObject* m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&numcmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&Matrix61cType);
    printf("CS61C Fall 2020 Project 4: numc imported!\n");
    fflush(stdout);
    return m;
}