/* 1. SWIG settings and workarounds */

%module(directors="1", threads="1", allprotected="1") hermes

%feature("director:except") {
    if( $error != NULL ) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch( &ptype, &pvalue, &ptraceback );
        PyErr_Restore( ptype, pvalue, ptraceback );
        PyErr_Print();
        Py_Exit(1);
    }
} 

%feature("autodoc", "1"); // automatic docstrings

%{
// workaround for SWIG < 2.0.5 with GCC >= 4.7
#include <cstddef>
using std::ptrdiff_t;
%}

/* SWIG headers */

%include "stl.i"
%include "std_set.i"
%include "std_multiset.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_multimap.i"
%include "std_vector.i"
%include "std_string.i"
%include "std_list.i"
%include "stdint.i"
%include "std_container.i"
%include "exception.i"
%include "std_iostream.i"
%include "std_shared_ptr.i"

/* SWIG Exceptions */

%inline %{
class RangeError {};
class StopIterator {};
%}

%exception {
 try {
   $action
 }  catch (Swig::DirectorException &e) {
   SWIG_exception(SWIG_RuntimeError, e.getMessage());
 }  catch (const std::exception& e) {
   SWIG_exception(SWIG_RuntimeError, e.what());
 }  catch (const char *e) {
   SWIG_exception(SWIG_RuntimeError, e);
 }
}

/* Exceptions for Python lists and iterators */

#ifdef SWIG_PYTHON3
%exception __next__ {
#else
%exception next {
#endif
  try {
        $action
  }
  catch (StopIterator) {
        PyErr_SetString(PyExc_StopIteration, "End of iterator");
        return NULL;
  }
}

%exception __getitem__ {
  try {
        $action
  }
  catch (RangeError) {
        SWIG_exception(SWIG_IndexError, "Index out of bounds");
        return NULL;
  }

};

/* std::unique_ptr */

namespace std {
  %feature("novaluewrapper") unique_ptr;
  template <typename Type>
  struct unique_ptr {
     typedef Type* pointer;

     explicit unique_ptr( pointer Ptr );
     unique_ptr (unique_ptr&& Right);
     template<class Type2, Class Del2> unique_ptr( unique_ptr<Type2, Del2>&& Right );
     unique_ptr( const unique_ptr& Right) = delete;


     pointer operator-> () const;
     pointer release ();
     void reset (pointer __p=pointer());
     void swap (unique_ptr &__u);
     pointer get () const;
     operator bool () const;

     ~unique_ptr();
  };
}

%define wrap_unique_ptr(Name, Type)
  %template(Name) std::unique_ptr<Type>;
  %newobject std::unique_ptr<Type>::release;

  %typemap(out) std::unique_ptr<Type> %{
    $result = SWIG_NewPointerObj(new $1_ltype(std::move($1)), $&1_descriptor, SWIG_POINTER_OWN);
  %}

%enddef
