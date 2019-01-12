#ifndef MATRIXOPERATIONS_HPP
#define MATRIXOPERATIONS_HPP

#include <boost/numeric/ublas/matrix.hpp> // Matrices definitions
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp> // Enable easy way for printing matrices

template <typename T>
boost::numeric::ublas::matrix<T> operator* (
         const boost::numeric::ublas::matrix<T> & a,
         const boost::numeric::ublas::matrix<T> & b)
{
	return boost::numeric::ublas::prod(a, b);
}


template <typename T>
boost::numeric::ublas::vector<T> operator* (
	const boost::numeric::ublas::matrix<T> & a,
	const boost::numeric::ublas::vector<T> & b)
{
		return boost::numeric::ublas::prod(a, b);
}


template <typename T>
boost::numeric::ublas::matrix<T> operator* (
	const boost::numeric::ublas::vector<T> & a,
	const boost::numeric::ublas::matrix<T> & b)
{
	return boost::numeric::ublas::prod(a, b);
}


template <class T>
bool inverse(boost::numeric::ublas::matrix<T> & input)
{
	using namespace boost::numeric::ublas;
	typedef permutation_matrix<std::size_t> pmatrix;

	// create a working copy of the input
	matrix<T> A(input);
	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());

	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if( res != 0 ) return false;

	// create identity matrix of "input"
	input.assign(identity_matrix<T>(A.size1()));

	// backsubstitute to get the inverse
	lu_substitute(A, pm, input);

	return true;
}

#endif // MATRIXOPERATIONS_HPP
