/***********************************************************************/
/*
//     This is an example call of MIDACO 6.0
//     -------------------------------------
//
//     MIDACO solves Multi-Objective Mixed-Integer Non-Linear Problems:
//
//
//      Minimize     F_1(X),... F_O(X)  where X(1,...N-NI)   is CONTINUOUS
//                                      and   X(N-NI+1,...N) is DISCRETE
//
//      subject to   G_j(X)  =  0   (j=1,...ME)      equality constraints
//                   G_j(X) >=  0   (j=ME+1,...M)  inequality constraints
//
//      and bounds   XL <= X <= XU
//
//
//     The problem statement of this example is given below. You can use 
//     this example as template to run your own problem. To do so: Replace 
//     the objective functions 'F' (and in case the constraints 'G') given 
//     here with your own problem and follow the below instruction steps.
*/
/***********************************************************************/
#include "mpi.h"        
#include <stdlib.h>
#include <stdio.h>
/***********************************************************************/
void problem_function( double *F, double *G, double *X );
/***********************************************************************/
extern"C"{ int midaco(long int*,long int*,long int*,long int*,long int*,
                      long int*,double*,double*,double*,double*,double*,
                      long int*,long int*,double*,double*,long int*,
                      long int*,long int*,double*,long int*,char*);}
/***********************************************************************/
extern"C"{ int midaco_print(int,long int,long int,long int*,long int*,double*,
                            double*,double*,double*,double*,long int,long int,
                            long int,long int,long int,double*,double*,
                            long int,long int,double*,long int,char*);}
/***********************************************************************/
/************************   MAIN PROGRAM   *****************************/
/***********************************************************************/
int main( int argc, char **argv)
{  
	  /* Prepare MPI parallelization */
      int thread, nthreads;
      MPI_Status status;      
      MPI_Init( &argc, &argv );
      MPI_Comm_rank( MPI_COMM_WORLD, &thread );
      MPI_Comm_size( MPI_COMM_WORLD, &nthreads );

      /* Variable and Workspace Declarations */
      long int o,n,ni,m,me,maxeval,maxtime,printeval,save2file,iflag,istop;
      long int liw,lrw,lpf,i,iw[5000],p=1; double rw[20000],pf[20000];
      double   f[10],g[1000],x[1000],xl[1000],xu[1000],param[13];
      char key[] = "MIDACO_LIMITED_VERSION___[CREATIVE_COMMONS_BY-NC-ND_LICENSE]";

      /*****************************************************************/
      /***  Step 1: Problem definition  ********************************/
      /*****************************************************************/

      /* STEP 1.A: Problem dimensions
      ******************************/
      o  = 1; /* Number of objectives                          */
      n  = 4; /* Number of variables (in total)                */
      ni = 2; /* Number of integer variables (0 <= ni <= n)    */
      m  = 3; /* Number of constraints (in total)              */
      me = 1; /* Number of equality constraints (0 <= me <= m) */

      /* STEP 1.B: Lower and upper bounds 'xl' & 'xu'  
      **********************************************/ 
      for( i=0; i<n; i++)
      { 
         xl[i] = 1.0; 
         xu[i] = 4.0; 
      }

      /* STEP 1.C: Starting point 'x'  
      ******************************/     
      for( i=0; i<n; i++)
      { 
         x[i] = xl[i]; /* Here for example: starting point = lower bounds */
      } 

      /*****************************************************************/
      /***  Step 2: Choose stopping criteria and printing options   ****/
      /*****************************************************************/

      /* STEP 2.A: Stopping criteria 
      *****************************/
      maxeval = 10000;    /* Maximum number of function evaluation (e.g. 1000000)  */
      maxtime = 60*60*24; /* Maximum time limit in Seconds (e.g. 1 Day = 60*60*24) */

      /* STEP 2.B: Printing options  
      ****************************/
      printeval = 1000; /* Print-Frequency for current best solution (e.g. 1000) */
      save2file = 1;    /* Save SCREEN and SOLUTION to TXT-files [ 0=NO/ 1=YES]  */
    
      /*****************************************************************/
      /***  Step 3: Choose MIDACO parameters (FOR ADVANCED USERS)    ***/
      /*****************************************************************/

      param[ 0] =  0.0;  /* ACCURACY  */
      param[ 1] =  0.0;  /* SEED      */
      param[ 2] =  0.0;  /* FSTOP     */
      param[ 3] =  0.0;  /* ALGOSTOP  */
      param[ 4] =  0.0;  /* EVALSTOP  */
      param[ 5] =  0.0;  /* FOCUS     */
      param[ 6] =  0.0;  /* ANTS      */
      param[ 7] =  0.0;  /* KERNEL    */
      param[ 8] =  0.0;  /* ORACLE    */
      param[ 9] =  0.0;  /* PARETOMAX */
      param[10] =  0.0;  /* EPSILON   */
      param[11] =  0.0;  /* BALANCE   */
      param[12] =  0.0;  /* CHARACTER */ 

      /*****************************************************************/
      /***  Step 4: Choose Parallelization Factor    *******************/
      /*****************************************************************/  

      p = nthreads; // nthreads argument given automatically via command:
                    // "mpirun -np nthreads ./run"

      /*****************************************************************/
      /****************  Start MPI Thread Splitting    *****************/
      /*****************************************************************/ 
      if (thread == 0)
      {
      /*****************************************************************/
      /********************  MPI Master Thread   ***********************/
      /*****************************************************************/         
      double *xxx,*fff,*ggg; int c;
      /* Allocate arrays for parallelization */
      xxx = (double *) malloc((p*n)*sizeof(double));
      fff = (double *) malloc((p*o)*sizeof(double));
      ggg = (double *) malloc((p*m)*sizeof(double));
      /* Store starting point x in xxx array */
      for( c=0; c<p; c++){ for( i=0; i<n; i++){ xxx[c*n+i] = x[i]; }} 
      /*****************************************************************/
      /*   
         Call MIDACO by Reverse Communication
      */
      /*****************************************************************/
      /* Workspace length calculation */
      lrw=sizeof(rw)/sizeof(double); 
      lpf=sizeof(pf)/sizeof(double);   
      liw=sizeof(iw)/sizeof(long int);     
      /* Print midaco headline and basic information */
      midaco_print(1,printeval,save2file,&iflag,&istop,&*f,&*g,&*x,&*xl,&*xu,
                   o,n,ni,m,me,&*rw,&*pf,maxeval,maxtime,&*param,p,&*key);
      while(istop==0) /*~~~ Start of the reverse communication loop ~~~*/
      {   
          for (c=2; c<=p; c++) /* Send iterates X for evaluation */
          {     
            /* Store variables X in dummy dx and send to MPI slave */
              for( i=0; i<n; i++){ x[i] = xxx[(c-1)*n+i]; }          
              MPI_Send( &x, n, MPI_DOUBLE, c-1,1, MPI_COMM_WORLD);
          }
          
          problem_function( &*fff, &*ggg, &*xxx); /* Perform one evaluation on Master */

          for (c=2; c<=p; c++) /* Collect results F & G */
          {
              MPI_Recv( &f, o, MPI_DOUBLE, c-1,2, MPI_COMM_WORLD,&status);
              MPI_Recv( &g, m, MPI_DOUBLE, c-1,3, MPI_COMM_WORLD,&status);            
              for(i=0;i<o;i++){fff[(c-1)*o+i]=f[i];}
              for(i=0;i<m;i++){ggg[(c-1)*m+i]=g[i];}             
          }                      
          /* Call MIDACO */
          midaco(&p,&o,&n,&ni,&m,&me,&*xxx,&*fff,&*ggg,&*xl,&*xu,&iflag,
                 &istop,&*param,&*rw,&lrw,&*iw,&liw,&*pf,&lpf,&*key);                  
          /* Call MIDACO printing routine */            
          midaco_print(2,printeval,save2file,&iflag,&istop,&*fff,&*ggg,&*xxx,&*xl,&*xu,
                       o,n,ni,m,me,&*rw,&*pf,maxeval,maxtime,&*param,p,&*key);  
          /* Send istop to slave */
          for (c=2; c<=p; c++)
          { MPI_Send( &istop,1, MPI_INTEGER, c-1,4, MPI_COMM_WORLD); }  

      } /*~~~End of the reverse communication loop ~~~*/ 

      /*****************************************************************/
      /*********************  MPI Slave Thread   ***********************/
      /*****************************************************************/       
      }else{

         istop = 0;    
         while(istop<=0)
         {
           MPI_Recv( &x, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status );

           problem_function( &*f, &*g, &*x);    
           
           MPI_Send( &f, o, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD );
           MPI_Send( &g, m, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD );   
           MPI_Recv( &istop,1,MPI_INTEGER,0,4,MPI_COMM_WORLD, &status );             
         }  
      }
      /*****************************************************************/
      if( istop >= 1 ){ MPI_Finalize(); }  
      /*****************************************************************/
      // printf("\n Solution f[0] = %f ", f[0]);
      // printf("\n Solution g[0] = %f ", g[0]);
      // printf("\n Solution x[0] = %f ", x[0]);            
      /*****************************************************************/      
      return 0;
      /*****************************************************************/
      /***********************  END OF MAIN  ***************************/
      /*****************************************************************/ 
}



/***********************************************************************/
/*********************   OPTIMIZATION PROBLEM   ************************/
/***********************************************************************/
void problem_function( double *f, double *g, double *x )
{
    /* Objective functions F(X) */
    f[0] = (x[0]-1.0)*(x[0]-1.0)
         + (x[1]-2.0)*(x[1]-2.0)
         + (x[2]-3.0)*(x[2]-3.0)
         + (x[3]-4.0)*(x[3]-4.0)
         + 1.23456789;

    /* Equality constraints G(X) = 0 MUST COME FIRST in g[0:me-1] */
    g[0] = x[0] - 1.0; 
    /* Inequality constraints G(X) >= 0 MUST COME SECOND in g[me:m-1] */
    g[1] = x[1] - 1.333333333;
    g[2] = x[2] - 2.666666666;

    /******************************************************************/
    /* Dummy calculation to simulate cpu-time intensive F calculation */
    /******************************************************************/ 
    double dummy = 0.01;
    while( dummy > 1.0e-8 )
    {
      dummy = dummy - (1.0e-8*x[0]);
    }
    f[0] = f[0] + dummy;    
}
/***********************************************************************/
/*************************** END OF FILE *******************************/
/***********************************************************************/

