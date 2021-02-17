#include "Python.h"
#include <iostream>

int main()
{
    PyObject* pInt = nullptr;
    Py_Initialize();

    PyRun_SimpleString("print(\"Hello World\")");
    Py_Finalize();

    return 0;
}
