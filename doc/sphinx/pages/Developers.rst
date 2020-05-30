Developers
==========

General Guidelines
------------------

* Every C++ class that will be exposed in the Python interface should, in general, be passed around with ``std::shared_ptr``, otherwise it cannot be used as a function argument in Python (`see <https://pybind11.readthedocs.io/en/stable/advanced/smart_ptrs.html>`_).
