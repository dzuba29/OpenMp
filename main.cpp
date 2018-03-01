#include <iostream>
#include <vector>
#include <sstream>
#include <omp.h>
#include <random>
#include <chrono>
#include <stdexcept>

std::vector<double> get_random_OpenMp(size_t size);

class Matrix {

public:

  Matrix(size_t rows, size_t cols)
    : m_rows(rows)
    , m_cols(cols)
    , m_data(rows * cols)
  {}


  size_t rows() const {
    return m_rows;
  }

  size_t cols() const {
    return m_cols;
  }

  double &operator()(size_t row, size_t col){
    return m_data[row * m_cols + col];
  }

  const double &operator()(size_t row, size_t col) const{
    return m_data[row * m_cols + col];
  } 

  Matrix operator * (const Matrix& matrix);

  static Matrix rand(size_t rows,size_t cols){

    return Matrix(rows,cols,get_random_OpenMp(rows*cols));

  }

private:

  Matrix(size_t rows, size_t cols,const std::vector<double> &data)
      : m_rows(rows)
      , m_cols(cols)
      , m_data(data)
  {}

  size_t m_rows;
  size_t m_cols;
  std::vector<double> m_data;


 
};



//linear functions

std::string ToString_Linear(const Matrix &matrix){ //outmatrix

  std::stringstream outStream;

  for (size_t i = 0; i < matrix.rows(); ++i) {

    for (size_t j = 0; j < matrix.cols(); ++j) {

      outStream << matrix(i, j) << " ";
    }
    outStream << std::endl;
  }
return outStream.str();

}



Matrix MultLinear(const Matrix &A,const Matrix &B){ //bad multi 


  Matrix C(A.rows(),B.cols());
  if (A.cols() == B.rows()) {
        for (size_t i = 0; i < C.rows(); ++i)
            for (size_t j = 0; j < C.cols(); ++j)
                for (size_t k = 0; k < C.rows(); ++k)
                    C(i,j) += A(i,k) * B(k,j);
  }
    else
        throw std::invalid_argument("wrong dims!");

  return C;
}

Matrix Matrix::operator*(const Matrix& matrix) {
  return MultLinear((*this), matrix);
}


//openmp functions 

std::vector<double> get_random_OpenMp(size_t size){ //get random numbers

  std::vector<double> result(size);

  #pragma omp parallel shared(result)
  {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::normal_distribution<double> dis(1, 2);

    #pragma omp for schedule(static)
    for (size_t i = 0; i < size; i++) {
      result[i] = dis(gen);
    }
  }

  return result;
 

}

int main(){

	
  auto time1 = std::chrono::steady_clock::now(); //start

  Matrix A =Matrix::rand(10,10);
  Matrix B = Matrix::rand(10,10);

  std::cout<<"\nA MATRIX\n" << ToString_Linear(A) << std::endl;
  std::cout<<"\nB MATRIX\n" << ToString_Linear(B) << std::endl;


  
  auto time2 = std::chrono::steady_clock::now(); //end

  auto delta= std::chrono::duration_cast<std::chrono::duration<double>>(time2 - time1);

  std::cout<<delta.count()<<std::endl;

	return 0;
}
//flags -Wall -Wextra -Wpedantic -fopenmp or -openmp