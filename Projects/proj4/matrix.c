#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    if(rows < 1 || cols < 1) {
        return -1;  // illegal argument
    }

    matrix *temp = (matrix *)malloc(sizeof(matrix));
    if(!temp) {
        return -2;  // memeory allocation failed
    }
    temp->data = malloc(rows * sizeof(double *));
    if(!temp->data) {
        free(temp);
        return -2;
    }

    for(int i = 0; i < rows; i++) {
        temp->data[i] = (double *)calloc(cols, sizeof(double));
        if(!temp->data[i]) {
            for(int j = 0; j < i; j++) {
                free(temp->data[j]);
            }
            free(temp->data);
            free(temp);
            return -2;
        }
    }
    temp->cols = cols;
    temp->rows = rows;
    temp->parent = NULL;
    temp->is_1d = (1 == rows || 1 == cols) ? 1 : 0;
    temp->ref_cnt = 1;
    *mat = temp;
    return 0;  // allocation success
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    if(!from || !from->data) {
        return -1;  // invalid arguments for p_from
    }

    if(row_offset < 0 || col_offset < 0 || rows <= 0 || cols <= 0) {
        return -1;  // invalid args for offsets
    }
    
    if(row_offset + rows > from->rows || col_offset + cols > from->cols) {
        return -1;  // invalid args for combation
    }

    matrix *temp = (matrix *)malloc(sizeof(matrix));
    if(!temp) {
        return -2;  // memeory allocation failed
    }
    temp->data = (double **)malloc(rows * sizeof(double *));
    if(!temp->data) {
        free(temp);
        return -2;
    }
    for(int i = 0; i < rows; i++) {
        temp->data[i] = from->data[row_offset + i] + col_offset;
    }

    temp->cols = cols;
    temp->rows = rows;
    temp->is_1d = (cols == 1 || rows == 1) ? 1 : 0;
    temp->parent = from;
    temp->ref_cnt = 1;

    from->ref_cnt++;
    *mat = temp;
    return 0;  // allocation success
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    if(!mat) {
        return;
    }

    if(!mat->parent) {
        if(--mat->ref_cnt == 0) {
            for(int i = 0; i < mat->rows; i++) {
                free(mat->data[i]);
            }
            free(mat->data);
            free(mat);
        }
    } else {
        mat->parent->ref_cnt--;
        free(mat->data);
        free(mat);
    }
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    mat->data[row][col] = val;
}

/*
 * Set the matrix to the identity. You may assume `row` and
 * `col` are valid, i.e., it has square shape.
 */
void set_matrix_identity(matrix *mat) {
    for(int i = 0; i < mat->rows; i++) {
        for(int j = 0; j < mat->cols; j++) {
            if(i == j) set(mat, i, i, 1);
            else set(mat, i, j, 0);
        }
    }
}

/*
 * Copy the from to the src. You may assume `row` and
 * `col` are valid, i.e., it has same shape.
 */
void copy_matrix(matrix *dest, matrix *src) {
    for(int i = 0; i < src->rows; i++) {
        for(int j = 0; j < src->cols; j++) {
            set(dest, i, j, get(src, i, j));
        }
    }
}
/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    if(!mat) {
        return;
    }

    for(int i = 0; i < mat->rows; i++) {
        for(int j = 0; j < mat->cols; j++) {
            set(mat, i, j, val);
        }
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    if(!result || !mat1 || !mat2) {
        return -1;  // invalid ref.
    }
    if(mat1->cols != mat2->cols || mat1->rows != mat2->rows ||
        mat1->cols != result->cols || mat1->rows != result->rows) {
        return -2;  // shape is not matched.
    }

    for(int i = 0; i < mat1->rows; i++) {
        for(int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
        }
    }

    return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    if(!result || !mat1 || !mat2) {
        return -1;  // invalid ref.
    }
    if(mat1->cols != mat2->cols || mat1->rows != mat2->rows ||
        mat1->cols != result->cols || mat1->rows != result->rows) {
        return -2;  // shape is not matched.
    }

    for(int i = 0; i < mat1->rows; i++) {
        for(int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
        }
    }

    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    if(!result || !mat1 || !mat2) {
        return -1;  // invalid ref.
    }
    if(mat1->cols != mat2->rows || mat1->rows != result->rows || mat2->cols != result->cols) {
        return -2;  // shape is not matched.
    }

    for(int i = 0; i < result->rows; i++) {
        for(int j = 0; j < result->cols; j++) {
            double product_sum = 0;
            for(int k = 0; k < mat1->cols; k++) {
                product_sum += get(mat1, i, k) * get(mat2, k, j);
            }
            set(result, i, j, product_sum);
        }
    }

    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    if(!mat || !result) {
        return -1;
    }
    if(mat->cols != mat->rows || result->cols != mat->cols || result->rows != mat->rows) {
        return -2;
    }

    matrix *temp = NULL;
    if(allocate_matrix(&temp, result->rows, result->cols) != 0) {
        return -3;  // memory allocation failed
    }

    set_matrix_identity(result);
    for(int i = 0; i < pow; i++) {
        if(mul_matrix(temp, result, mat) != 0) {
            deallocate_matrix(temp);
            return -4;  // muliplication failed.
        }
        copy_matrix(result, temp);
    }
    deallocate_matrix(temp);

    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    if(!result || !mat) {
        return -1;  // invalid ref.
    }
    if(result->rows != mat->rows || result->cols != mat->cols) {
        return -2;  // shape is not matched.
    }

    for(int i = 0; i < result->rows; i++) {
        for(int j = 0; j < result->cols; j++) {
            set(result, i, j, -get(mat, i, j));
        }
    }

    return 0;
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    if(!result || !mat) {
        return -1;  // invalid ref.
    }
    if(result->rows != mat->rows || result->cols != mat->cols) {
        return -2;  // shape is not matched.
    }

    for(int i = 0; i < result->rows; i++) {
        for(int j = 0; j < result->cols; j++) {
            double val = get(mat, i, j);
            set(result, i, j, val > 0 ? val : -val);
        }
    }

    return 0;
}

