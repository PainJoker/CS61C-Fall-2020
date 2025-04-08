from setuptools import setup, Extension


def main():
    CFLAGS = [
        "-g",
        "-Wall",
        "-std=c99",
        "-fopenmp",
        "-mavx",
        "-mfma",
        "-pthread",
        "-O3",
    ]
    LDFLAGS = ["-fopenmp"]
    # Use the setup function we imported and set up the modules.
    # You may find this reference helpful: https://docs.python.org/3.6/extending/building.html
    numc_module = Extension(
        name="numc",
        sources=["numc.c", "matrix.c"],
        extra_compile_args=CFLAGS,
        extra_link_args=LDFLAGS,
        language="c",
    )

    setup(
        name="numc",
        version="1.0",
        description="matrix basic operations based on C language.",
        ext_modules=[numc_module],
    )


if __name__ == "__main__":
    main()
