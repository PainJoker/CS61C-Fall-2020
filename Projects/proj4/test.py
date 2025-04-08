class Matrix:
    rows: 1
    cols: 1

    def __init__(self):
        pass


def matrix_allocate_ref(
    mat: Matrix, src: Matrix, row_offset: int, col_offset: int, rows: int, cols: int
) -> Matrix:
    def is_valid_argument():
        return (
            row_offset > 0
            and col_offset > 0
            and rows > 0
            and cols > 0
            and row_offset + rows <= src.rows
            and col_offset + cols <= src.cols
        )

    if not is_valid_argument():
        return -1
