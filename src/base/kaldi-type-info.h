#include "cppa/cppa.hpp"

#include "matrix/kaldi-vector.h"
#include "matrix/kaldi-matrix.h"

using namespace cppa;

namespace kaldi {
  template<typename Real>
  bool operator==(const Vector<Real>& lhs, const Vector<Real>& rhs) {
    if (lhs.Dim() != rhs.Dim())
      return false;
    
    for (MatrixIndexT i = 0; i < lhs.Dim(); i++) {
      if (lhs(i) != rhs(i))
        return false;
    }

    return true;
  }
  
  template<typename Real>
  bool operator==(const Matrix<Real>& lhs, const Matrix<Real>& rhs) {
    if (lhs.NumRows() != rhs.NumRows() || lhs.NumCols() != rhs.NumCols())
      return false;
    
    for (MatrixIndexT i = 0; i < lhs.NumRows(); i++) {
      for (MatrixIndexT j = 0; j < lhs.NumCols(); j++) {
        if (lhs(i, j) != rhs(i, j))
          return false;
      }
    }

    return true;
  }
  
}

// abstract_uniform_type_info implements all functions of uniform_type_info
// except for serialize() and deserialize() if the template parameter T:
// - does have a default constructor
// - does have a copy constructor
// - does provide operator==

template<typename Real>
class VectorTypeInfo : public util::abstract_uniform_type_info<kaldi::Vector<Real> > {

protected:

  void serialize(const void* ptr, serializer* sink) const {
    // ptr is guaranteed to be a pointer of type Vector
    auto vector_ptr = reinterpret_cast<const kaldi::Vector<Real>*>(ptr);
    sink->write_value(static_cast<uint32_t>(vector_ptr->Dim()));
    sink->write_raw(vector_ptr->SizeInBytes(), vector_ptr->Data());
  }

  void deserialize(void* ptr, deserializer* source) const {
    // ptr is guaranteed to be a pointer of type Vector
    auto vector_ptr = reinterpret_cast<kaldi::Vector<Real>*>(ptr);
    // vector_ptr->clear();
    auto size = source->read<std::uint32_t>();
    vector_ptr->Resize(size, kaldi::kUndefined);
    source->read_raw(vector_ptr->SizeInBytes(), vector_ptr->Data());
  }
    
};


template<typename Real>
class MatrixTypeInfo : public util::abstract_uniform_type_info<kaldi::Matrix<Real> > {

protected:

  void serialize(const void* ptr, serializer* sink) const {
    // ptr is guaranteed to be a pointer of type Matrix
    auto matrix_ptr = reinterpret_cast<const kaldi::Matrix<Real>*>(ptr);
    sink->write_value(static_cast<uint32_t>(matrix_ptr->NumRows()));
    sink->write_value(static_cast<uint32_t>(matrix_ptr->NumCols()));
    sink->write_raw(matrix_ptr->SizeInBytes(), matrix_ptr->Data());
  }

  void deserialize(void* ptr, deserializer* source) const {
    // ptr is guaranteed to be a pointer of type Matrix
    auto matrix_ptr = reinterpret_cast<kaldi::Matrix<Real>*>(ptr);
    // matrix_ptr->clear();
    auto num_rows = source->read<std::uint32_t>();
    auto num_cols = source->read<std::uint32_t>();
    matrix_ptr->Resize(num_rows, num_cols, kaldi::kUndefined);
    source->read_raw(matrix_ptr->SizeInBytes(), matrix_ptr->Data());
  }
    
};

// Announces kaldi types to the libcppa type system.
bool AnnounceKaldiTypes() {
  announce(typeid(kaldi::Vector<float>), 
           std::unique_ptr<uniform_type_info>{new VectorTypeInfo<float>});
  announce(typeid(kaldi::Vector<double>), 
           std::unique_ptr<uniform_type_info>{new VectorTypeInfo<double>});
  announce(typeid(kaldi::Matrix<float>), 
           std::unique_ptr<uniform_type_info>{new MatrixTypeInfo<float>});
  announce(typeid(kaldi::Matrix<double>), 
           std::unique_ptr<uniform_type_info>{new MatrixTypeInfo<double>});
  return true;
}