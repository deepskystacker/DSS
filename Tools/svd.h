/******************************************************************************
 *
 * File:           svd.h
 *
 * Created:        11/05/2004
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Header for svd.c
 *
 * Description:    Contains function declarations and descriptions.
 *
 * Revisions:      12/1/2005 PS Added svd_lsq() for least squares fitting via 
 *                 SVD.
 *
 *****************************************************************************/

#if !defined(_SVD_H)
#define _SVD_H

extern int svd_verbose;

/** Performs singular value decomposition for a dense matrix.
 * Borrowed from EISPACK (1972-1973).
 *
 * The input matrix A is presented as  A = U.W.V'.
 *
 * @param A Input matrix A [0..m-1][0..n-1]; output matrix U
 * @param n Number of columns
 * @param m Number of rows
 * @param w Ouput vector [0..n-1] that presents diagonal matrix W 
 * @param V output matrix V [0..n-1][0..n-1] (not transposed)
 */
void svd(double** a, int n, int m, double* w, double** v);

/** Performs sorting of SVD results in order of decreasing singular values.
 *
 * @param A Input-output matrix U [0..m-1][0..n-1]
 * @param n Number of columns
 * @param m Number of rows
 * @param w Input-ouput vector [0..n-1] that presents diagonal matrix W 
 * @param V Input-output matrix V [0..n-1][0..n-1] (not transposed)
 *
 * This function does the work but has downside that it requires temporal 
 * storage equal to the main storage. This may be  a problem for some large
 * applications, but one does not usually use dense SVD in such cases.
 */
void svd_sort(double** A, int n, int m, double* w, double** V);

/** Least squares fitting via singular value decomposition.
 * (Also solves under-determined problems.)
 *
 * With A = U * W * V^T, the solution of 
 *
 * (1) A * x = z 
 *
 * that minimizes the cost function
 *
 * (2) Q = (A * x - z)^T * (A * x - z)
 *
 * ("in the least squares sense") is given by 
 *
 * (3) x = (A^T * A)^-1 * A^T * z = V * W^-1 * U^T * z.
 *
 * It is used in the case if parameter std == NULL.
 *
 * If the data has known covariance S, the cost function modifies as
 *
 * (4) Q = (A * x - z)^T * S^-1 * (A * x - z),
 *
 * which can be rewritten as
 *
 * (5) Q = (A' * x - z')^T * (A' * x - z'),
 *
 * where 
 *
 * (6) A' = S^(-1/2) * A, z' = S^(-1/2) * z, 
 *
 * so that the solution x that minimizes (4) is given by
 *
 * x = V' * W'^-1 * U'^T * z',
 *
 * where V', W' and U' represent SVD of the modified matrix A': 
 * A' = U' * W' * V'^T.
 *
 * In the case when S is diagonal, (6) is equivalent to dividing i-th row of 
 * the system (1) by S(i,i)^(1/2), or by standard deviation of i-th
 * measurement. This approach is used in the case if parameter std != NULL.
 *
 * @param A Matrix A of the system (1) [0..nj-1][0..ni-1]
 * @param ni Number of columns
 * @param nj Number of rows
 * @param z Right-hand of the system (1) [0..nj-1]
 * @param std Vector of standard deviations for each row [0..nj-1] or NULL
 * @param w Singular values of the modified matrix A' = S^(-1/2) * A [0..nj-1]
 * @param sol Solution of the modified system (1),(4) [0..ni-1]
 *
 * Note: A gets overwritten during call to lsq().
 */
void svd_lsq(double** A, int ni, int nj, double* z, double* std, double* w, double* sol);

#endif
