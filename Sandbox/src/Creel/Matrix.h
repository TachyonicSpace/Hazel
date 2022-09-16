#pragma once
#include "ActivationFunctions.h"
#include <initializer_list>
#include <math.h>


//maps an input range to an output range
static float map(float in, float inMin, float inMax, float outMin, float outMax)
{
	float slope = (outMax - outMin) / (inMax - inMin);
	return outMin + slope * (in - inMin);
}

namespace AI
{
	class Matrix
	{
	public:
		Matrix()
		{
			rows = 0;
			cols = 0; 
			total = 0;
			this->data = std::vector<std::vector<float>>();
		}

		Matrix(int rows, int cols)
		{
			this->rows = rows;
			this->cols = cols;
			total = rows * cols;
			auto matrixCols = std::vector<float>(cols);
			data = std::vector<std::vector<float>>(rows);
			for (int i = 0; i < rows; i++)
				data[i] = matrixCols;


			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					this->operator()(r, c) = 0;
				}
			}
		}

		~Matrix()
		{
			
		}

		Matrix(const std::vector<std::vector<float>> a)
		{
			data = a;
			rows = a.size();
			cols = a[0].size();
			total = rows * cols;
		}

		Matrix(const Matrix& a)
		{

			this->rows = a.rows;
			this->cols = a.cols;
			this->total = a.total;
			auto matrixCols = std::vector<float>(cols);
			data = std::vector<std::vector<float>>(rows);
			for (int i = 0; i < rows; i++)
				data[i] = matrixCols;


			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					this->operator()(r, c) = a(r, c);
				}
			}

		}

		Matrix(const std::initializer_list<std::initializer_list<float>> a)
		{
			this->rows = a.size();
			this->cols = 0;
			for(auto l:a)
				this->cols = __max(this->cols, l.size());
			this->total = rows*cols;
			auto matrixCols = std::vector<float>(cols);
			data = std::vector<std::vector<float>>(rows);
			for (int i = 0; i < rows; i++)
				data[i] = matrixCols;


			int c = 0, r = 0;
			for (auto la : a)
			{
				for (auto l : la)
				{
					operator()(r, c) = l;

					c++;
				}
				r++;
				c = 0;
			}
		}

		float& operator()(int row, int col)
		{
			int index = col + row * (cols);
			if (index >= total || index < 0)
				throw "indicies must not be larger than size of the marix";


			return data[row][col];
		}

		float operator()(int row, int col) const
		{

			int index = col + row * (cols - 1);
			if (index >= total || index < 0)
				throw "indicies must not be larger than size of the marix";


			return data[row][col];
		}

		Matrix dot(Matrix& b)
		{
			//Matrix a = *this;
			Matrix result(this->rows, b.cols);
			if (this->cols != b.rows)
				throw "Cols of a must equal rows of B";
			
			int sharedDim = this->cols;
			for (int r = 0; r < this->rows; r++)
			{
				for (int c = 0; c < b.cols; c++)
				{//test speed of switching looping over r or c first
					float total = 0.f;
					for (int d = 0; d < sharedDim; d++)
					{
						total += this->operator()(r, d) * b(d, c);
					}
					result(r, c) = total;
				}
			}
			return result;
		}

		bool isEmpty() 
		{
			return total == 0;
		}

		bool isNotEmpty()
		{
			return total != 0;
		}

		Matrix addNoise(float nouseAmount)
		{
			Matrix result(this->rows, this->cols);
			for (int r = 0; r < result.rows; r++)
			{
				for (int c = 0; c < result.cols; c++)
				{
					float tmp = ((float)rand() / (float)RAND_MAX) * 2.f - 1.f;
					result(r, c) = this->operator()(r, c) + nouseAmount * tmp;
				}
			}
			return result;
		}

		Matrix Hadamard(Matrix& a)
		{

			Matrix result(a.rows, a.cols);
			if (this->rows != a.rows || this->cols != a.cols)
				throw "Matrices must have same Dimensions";


			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					result(r, c) = this->operator()(r, c) * a(r, c);
				}
			}

			return result;
		}
		Matrix operator+(Matrix& b)
		{
			Matrix& a = *this;
			Matrix result(a.rows, a.cols);
			if (this->rows != a.rows || this->cols != a.cols)
				throw "Matrices must have same Dimensions";


			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					result(r, c) = a(r, c) + b(r, c);;
				}
			}

			return result;
		}
		Matrix operator-(Matrix& b)
		{
			Matrix& a = *this;
			Matrix result(a.rows, a.cols);
			if (this->rows != b.rows || this->cols != b.cols)
				throw "Matrices must have same Dimensions";


			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					result(r, c) = a(r, c) - b(r, c);;
				}
			}
			return result;
		}

		Matrix subtractBatches(Matrix b)
		{
			Matrix output = *this;
			output.ADD_COL_VEC(b.data[0]);
			for (int i = 1; i < b.cols; i++)
			{
				output.ADD_COL_VEC(b.data[i]);
			}
			return output;
		}

		Matrix operator*(float x)
		{
			Matrix result(rows, cols);
			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					result(r, c) = data[r][c] * x;
				}
			}
			return result;
		}

		void Randomize(int incomingPoints = 1)
		{
			float max = 1 / sqrtf(incomingPoints);
			float range = 2 * max;
			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					this->operator()(r, c) = ((float)rand() / (float)RAND_MAX) * range - max;
				}
			}
		}

		void ADD_ROW_VEC(Matrix* colvec)
		{
			if (this->cols != colvec->cols || colvec->rows != 1)
				throw "Column vector's size is incorrect";

			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					this->operator()(r, c) += colvec->operator()(0, c);
				}
			}
		}

		void ADD_COL_VEC(std::vector<float> colVec)
		{
			if (this->cols != colVec.size())
				throw "Column vector's size is incorrect";

			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					this->operator()(r, c) += colVec[r];
				}
			}
		}

		Matrix ApplyActivationFunction(Activationfunctions f)
		{
			Matrix a(rows, cols);


			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					a(r, c) = activationFunc(this->operator()(r, c), f);
				}
			}
			return a;
		}

		Matrix ApplyFunction(float (*f)(float))
		{
			Matrix a(rows, cols);


			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					a(r, c) = f(this->operator()(r, c));
				}
			}
			return a;
		}

		Matrix ApplyDXFunction(Activationfunctions f)
		{
			Matrix a(rows, cols);


			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					a(r, c) = activationFuncDX(this->operator()(r, c), f);
				}
			}
			return a;
		}

		Matrix T()
		{
			Matrix t(cols, rows);
			for (int r = 0; r < rows; r++)
			{
				for (int c = 0; c < cols; c++)
				{
					t(c, r) = this->operator()(r, c);
				}
			}
			return t;
		}

		std::string toString()
		{
			std::string str = "{\t" + matrixShape() + "\n";
			for (int r = 0; r < rows; r++)
			{
				str += "[";
				for (int c = 0; c < cols; c++)
				{
					str += std::to_string(this->operator()(r, c));
					str += ", ";
				}
				if (str.back() == ' ')
					str = str.substr(0, str.size() - 2);
				str += "]\n";
			}
			return str += "}\n";
		}

		std::string matrixShape()
		{
			return "(" + std::to_string(rows) + ", " + std::to_string(cols) + ")";
		}

		std::vector<int> shape()
		{
			return { rows, cols };
		}

		Matrix PushMatrixIntoRows(Matrix b)
		{
			std::vector<std::vector<float>> resultData = data;
			if (this->cols != b.cols)
				throw "Matrix cols must match";

			for (std::vector<float> row : b.data)
			{
				resultData.push_back(row);
			}
			return Matrix(resultData);
		}

	private:
		std::vector<std::vector<float>> data;
		//float* data = nullptr;
		int rows, cols;
		int total;
	};
}