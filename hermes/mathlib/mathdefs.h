enum
	{
	M_NOMEM = 1,
	M_NOMUL,
	M_PIVOT,
	M_STUETZ
	};
	
extern int merrno;	

#define E(A,i,j) (*(*(A+i)+j))	
#define abs(x) (((x)<0)?-(x):x)

