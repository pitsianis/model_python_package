#include <Python.h>
#include <fftw3.h>
#include <stdlib.h>

static PyObject* convolve(PyObject* self, PyObject* args) {
    PyObject *input1_obj, *input2_obj;
    PyObject *result_list;
    Py_ssize_t size1, size2, n;
    double *input1, *input2, *result;

    // Parse Python arguments
    if (!PyArg_ParseTuple(args, "OO", &input1_obj, &input2_obj)) {
        return NULL;
    }

    input1_obj = PySequence_Fast(input1_obj, "First argument must be a sequence");
    input2_obj = PySequence_Fast(input2_obj, "Second argument must be a sequence");

    size1 = PySequence_Fast_GET_SIZE(input1_obj);
    size2 = PySequence_Fast_GET_SIZE(input2_obj);
    n = size1 + size2 - 1;

    input1 = fftw_malloc(sizeof(double) * n);
    input2 = fftw_malloc(sizeof(double) * n);
    result = fftw_malloc(sizeof(double) * n);

    // Zero-pad inputs
    for (Py_ssize_t i = 0; i < n; i++) {
        input1[i] = (i < size1) ? PyFloat_AsDouble(PySequence_Fast_GET_ITEM(input1_obj, i)) : 0.0;
        input2[i] = (i < size2) ? PyFloat_AsDouble(PySequence_Fast_GET_ITEM(input2_obj, i)) : 0.0;
    }

    // Perform FFT-based convolution
    fftw_complex *fft1 = fftw_malloc(sizeof(fftw_complex) * n);
    fftw_complex *fft2 = fftw_malloc(sizeof(fftw_complex) * n);
    fftw_complex *ifft = fftw_malloc(sizeof(fftw_complex) * n);

    fftw_plan forward1 = fftw_plan_dft_r2c_1d(n, input1, fft1, FFTW_ESTIMATE);
    fftw_plan forward2 = fftw_plan_dft_r2c_1d(n, input2, fft2, FFTW_ESTIMATE);
    fftw_plan backward = fftw_plan_dft_c2r_1d(n, ifft, result, FFTW_ESTIMATE);

    fftw_execute(forward1);
    fftw_execute(forward2);

    for (Py_ssize_t i = 0; i < (n / 2 + 1); i++) {
        ifft[i][0] = fft1[i][0] * fft2[i][0] - fft1[i][1] * fft2[i][1];
        ifft[i][1] = fft1[i][0] * fft2[i][1] + fft1[i][1] * fft2[i][0];
    }

    fftw_execute(backward);

    // Normalize result
    for (Py_ssize_t i = 0; i < n; i++) {
        result[i] /= n;
    }

    // Create Python list
    result_list = PyList_New(n);
    for (Py_ssize_t i = 0; i < n; i++) {
        PyList_SET_ITEM(result_list, i, PyFloat_FromDouble(result[i]));
    }

    // Cleanup
    fftw_free(input1);
    fftw_free(input2);
    fftw_free(result);
    fftw_free(fft1);
    fftw_free(fft2);
    fftw_free(ifft);
    fftw_destroy_plan(forward1);
    fftw_destroy_plan(forward2);
    fftw_destroy_plan(backward);

    Py_DECREF(input1_obj);
    Py_DECREF(input2_obj);

    return result_list;
}

static PyMethodDef MyLibMethods[] = {
    {"convolve", convolve, METH_VARARGS, "Compute the convolution of two vectors using FFT"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef mylibmodule = {
    PyModuleDef_HEAD_INIT,
    "mylib",
    "A module for FFT-based convolution",
    -1,
    MyLibMethods
};

PyMODINIT_FUNC PyInit_mylib(void) {
    return PyModule_Create(&mylibmodule);
}
