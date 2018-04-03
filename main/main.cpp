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

void benchmark(size_t dim, double mulDuration, double runtimeDuration) {
  std::cout.setf(std::ios::fixed);
  std::cout.precision(5);
  std::cout << "║   " << dim << "║   "<< mulDuration << "║   "<< runtimeDuration<< std::endl;
}

void benchmark_to_csv(int threads, size_t dim, double mulDuration, double runtimeDuration) {
  std::cout << threads << "," << dim << "," << mulDuration << "," << runtimeDuration << std::endl;
}

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

//openmp functions 

std::vector<double> get_random_OpenMp(size_t size){ //get random numbers

  std::vector<double> result(size);

  #pragma omp parallel shared(result)
  {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::cauchy_distribution<double>dis(1, 2);

    #pragma omp for schedule(static)
    for (size_t i = 0; i < size; i++) {
      result[i] = dis(gen);
    }
  }

  return result;
 

}


Matrix MultOpenMp(const Matrix &A,const Matrix &B){ 


  Matrix C(A.rows(),B.cols());

  size_t i,j,k;
  double localResult=0;
  
  if (A.cols() == B.rows()) {

    #pragma omp parallel for private(i,j,k,localResult)

        for (i = 0; i < C.rows(); ++i)
        {
            for (j = 0; j < C.cols(); ++j)
            {
                for (k = 0; k < C.rows(); ++k)
                {
                   localResult+= A(i,k) * B(k,j);
                }
                C(i,j)=localResult;
            }
        }
    
  }
    else
        throw std::invalid_argument("wrong dims!");

  return C;
}

Matrix Matrix::operator*(const Matrix& matrix) {
  return MultOpenMp((*this), matrix);
}


int main(int argc, char* argv[]){

  auto startTime = std::chrono::steady_clock::now();


  size_t rows=10;
  size_t cols=10;

  if (argc > 1){
    std::istringstream ss(argv[1]);
    int dim;
    if (!(ss >> dim)){
      throw std::invalid_argument("Wrong ARGV");
    } else {
      rows = dim;
      cols = dim;
    }
  } 

  Matrix A = Matrix::rand(rows,cols);
  Matrix B = Matrix::rand(rows,cols);
  Matrix C(rows,cols);
  auto initTime = std::chrono::steady_clock::now();

  C=A*B;
  
  auto mulTime = std::chrono::steady_clock::now(); //end



  auto mulDuration = std::chrono::duration_cast<std::chrono::duration<double>>(mulTime - initTime);
  auto runtimeDuration = std::chrono::duration_cast<std::chrono::duration<double>>(mulTime - startTime);

  //benchmark(rows, mulDuration.count(), runtimeDuration.count());

  benchmark_to_csv(omp_get_max_threads(), rows, mulDuration.count(), runtimeDuration.count());
  
	return 0;
}
//flags -Wall -Wextra -Wpedantic -fopenmp or -openmp
//#### ssh student-math-03@fujitsu-hpc-02.narfu.ru
//#### Cneltyn2014
