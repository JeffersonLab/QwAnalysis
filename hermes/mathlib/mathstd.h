#include <matrix.h>

#ifndef __MATHSTD_H

int    Matherror( int Fehlernummer);
void draw (int x1, int y1, int x2, int y2, int col);
double Gauss_Integral ( double x );
double Gauss_Verteilung( double Mittelwert, 
						 double Standardabweichung,
						 double Value );
double M_1Norm(matrix A, int n, int m);
double M_2Norm(matrix A, int n, int m);
vektor M_A_mal_b(vektor y, matrix A, int n, int m, vektor b);
matrix M_A_T_A(matrix B, matrix A, int n, int m);
void   M_AddZeile(matrix A,int n,int m, int z1,int z2, double faktor,int start);
void   M_Ausgabe(char *info, matrix M, int n, int m, int tab, int vorkomma, int nachkomma);
matrix M_Cholesky(matrix A, vektor p, int n);
double M_Determinante(matrix A, int n);
matrix M_Dyade(matrix Ergebnis, vektor u, int dim_u, vektor v, int dim_v);
matrix M_Erweitere(matrix Ergebnis, matrix A, int n, int m, vektor v);
matrix M_Fill(matrix A, double *M, int Zeilen, int Spalten);
double M_FNorm(matrix A, int n, int m);
vektor M_Gauss(vektor Ergebnis, matrix A, int n, vektor b);
matrix M_Init(int Zeilen, int Spalten);
matrix M_Inverse(matrix A,int n);
matrix M_InvertPos(matrix A,int n);
matrix M_Kopiere(matrix Ergebnis, matrix A, int n, int m);
vektor M_Loese(matrix A, int nm, vektor v);
vektor M_LSqHouseholder(matrix A, int m, int n, vektor D);
vektor M_LSqNorm(vektor x, matrix A, int m, int n, vektor b);
matrix M_Multiplikation(matrix Ergebnis, 
			matrix A, int A_Zeilen,int A_Spalten,
			matrix B, int B_Zeilen,int B_Spalten);
int    M_Pivot(matrix A, int n, int m, int Durchgang);
matrix M_RLZerlegung(matrix A, int n, int *ipivot);
double M_SNorm(matrix A, int n, int m);
void   M_SwapS(matrix A, int n, int m, int spalte1, int spalte2,int start);
void   M_SwapZ(matrix A, int n, int m, int zeile1, int zeile2,int start);
vektor M_Spaltenvektor(vektor v, matrix A, int n, int m, int spalte);
matrix M_Transponiert(matrix Ergebnis, matrix A, int oldspalte, int oldzeile);
double M_UNorm(matrix A, int n, int m);		
vektor M_XausCholesky(matrix L, vektor p, int dim, vektor b);
vektor M_XausHouseholder(matrix Q,vektor D, int m, int n, vektor b);
vektor M_XausRL(matrix A, int dim, vektor x, int ipivot[]);
double M_ZNorm(matrix A, int n, int m);

vektor P_Newton (vektor a, int grad, vektor x, vektor y);
double P_NHorner( vektor a, vektor x, int grad, double z);

void   Pl_Plot(double xa,double xe,double h,double (*f)(double x));
int    Pl_System(double Xa, double Xe, double Ya, double Ye, 
		 double mx,double my,
		 int , int, int, int, int);

vektor V_Add(vektor x, vektor b, vektor c,int dimension);
vektor V_Sub(vektor x, vektor b, vektor c,int dimension);
void   V_Ausgabe(char *info,vektor v, int n, int vorkomma, int nachkomma);
double V_Betrag(vektor a, int dimension);
double V_1Norm(vektor a, int dimension);
double V_2Norm(vektor a, int dimension);
vektor V_Einheit(vektor Ergebnis, int k, int dimension);
vektor V_Fill(vektor v, double *dv, int n);
vektor V_Init(int Dimension);
int    V_PlaneXLine( vektor n, vektor p, vektor s, vektor q, vektor result);
double V_SkalarProdukt(vektor a, vektor b, int Dimension);
vektor V_S_Produkt(vektor Ergebnis, double skalar, vektor a, int dimension);
double V_UNorm(vektor a, int dimension);

double X_Timer(void);
int    setdeqdim(int);

#define __MATHSTD_H
#endif

