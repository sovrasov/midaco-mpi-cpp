#include <mpi.h>
#include "midaco_mpi.hpp"

#include <midaco_core.h>
#include <omp.h>

MidacoSolution solve_midaco_mpi(const IGOProblem<double>* problem, const MidacoMPIParameters& params,
    std::function<bool(const double*)> external_stop)
{
  MidacoSolution solution;

  /* Prepare MPI parallelization */
    int proc, nprocs;
    MPI_Status status;
    MPI_Comm_rank( MPI_COMM_WORLD, &proc );
    MPI_Comm_size( MPI_COMM_WORLD, &nprocs );

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
    n  = problem->GetDimension(); /* Number of variables (in total)                */
    ni = 0; /* Number of integer variables (0 <= ni <= n)    */
    m  = problem->GetConstraintsNumber(); /* Number of constraints (in total)              */
    me = 0; /* Number of equality constraints (0 <= me <= m) */

    /* STEP 1.B: Lower and upper bounds 'xl' & 'xu'
    **********************************************/
    problem->GetBounds(xl, xu);

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
    maxeval = params.maxEvals;    /* Maximum number of function evaluation (e.g. 1000000)  */
    maxtime = 60*60*24; /* Maximum time limit in Seconds (e.g. 1 Day = 60*60*24) */

    /* STEP 2.B: Printing options
    ****************************/
    printeval = 1000; /* Print-Frequency for current best solution (e.g. 1000) */
    save2file = 0;    /* Save SCREEN and SOLUTION to TXT-files [ 0=NO/ 1=YES]  */

    /*****************************************************************/
    /***  Step 3: Choose MIDACO parameters (FOR ADVANCED USERS)    ***/
    /*****************************************************************/

    param[ 0] =  0.0;  /* ACCURACY  */
    param[ 1] =  params.seed;  /* SEED      */
    param[ 2] =  0.0;  /* FSTOP     */
    param[ 3] =  0.0;  /* ALGOSTOP  */
    param[ 4] =  0.0;  /* EVALSTOP  */
    param[ 5] =  params.focus;  /* FOCUS     */
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

    omp_set_num_threads(params.numThreads);
    long int num_points = params.numThreads * nprocs;
    p = nprocs; // nprocs argument given automatically via command:
                  // "mpirun -np nprocs ./run"

    /*****************************************************************/
    /****************  Start MPI Thread Splitting    *****************/
    /*****************************************************************/
    if (proc == 0)
    {
    /*****************************************************************/
    /********************  MPI Master Thread   ***********************/
    /*****************************************************************/
    double *xxx,*fff,*ggg; int c;
    /* Allocate arrays for parallelization */
    xxx = (double *) malloc((params.numThreads*p*n)*sizeof(double));
    fff = (double *) malloc((params.numThreads*p*o)*sizeof(double));
    ggg = (double *) malloc((params.numThreads*p*m)*sizeof(double));
    /* Store starting point x in xxx array */
    for( c=0; c<p*params.numThreads; c++){ for( i=0; i<n; i++){ xxx[c*n+i] = x[i]; }}
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
                 o,n,ni,m,me,&*rw,&*pf,maxeval,maxtime,&*param,num_points,&*key);
    int n_evals = 0;
    while(istop==0) /*~~~ Start of the reverse communication loop ~~~*/
    {
        for (c=2; c<=p; c++) /* Send iterates X for evaluation */
        {
          /* Store variables X in dummy dx and send to MPI slave */
            for( i=0; i<n*params.numThreads; i++){ x[i] = xxx[(c-1)*n*params.numThreads+i]; }
            MPI_Send( &x, n*params.numThreads, MPI_DOUBLE, c-1,1, MPI_COMM_WORLD);
            for( i=0; i<params.numThreads; i++)
              if (external_stop(x + i*n))
                istop = 1;
        }
        #pragma omp parallel for
        for (unsigned t = 0; t < params.numThreads; t++)  {
          for (int i = 0; i < m; i++)
            ggg[t*m + i] = problem->Calculate(xxx + t*n, i);
          fff[t*o] = problem->Calculate(xxx + t*n, m);
          if (external_stop(xxx + t*n))
          #pragma omp atomic write
            istop = 1;
        }

        for (c=2; c<=p; c++) /* Collect results F & G */
        {
            MPI_Recv( &f, o*params.numThreads, MPI_DOUBLE, c-1,2, MPI_COMM_WORLD,&status);
            MPI_Recv( &g, m*params.numThreads, MPI_DOUBLE, c-1,3, MPI_COMM_WORLD,&status);
            for(i=0;i<o*params.numThreads;i++){fff[(c-1)*o*params.numThreads+i]=f[i];}
            for(i=0;i<m*params.numThreads;i++){ggg[(c-1)*m*params.numThreads+i]=g[i];}
        }
        n_evals += p*params.numThreads;
        /* Call MIDACO */
        midaco(&num_points,&o,&n,&ni,&m,&me,&*xxx,&*fff,&*ggg,&*xl,&*xu,&iflag,
               &istop,&*param,&*rw,&lrw,&*iw,&liw,&*pf,&lpf,&*key);
        /* Call MIDACO printing routine */
        midaco_print(2,printeval,save2file,&iflag,&istop,&*fff,&*ggg,&*xxx,&*xl,&*xu,
                     o,n,ni,m,me,&*rw,&*pf,maxeval,maxtime,&*param,num_points,&*key);
        /* Send istop to slave */
        for (c=2; c<=p; c++)
        { MPI_Send( &istop,1, MPI_INTEGER, c-1,4, MPI_COMM_WORLD); }
    } /*~~~End of the reverse communication loop ~~~*/

    solution.optValues = std::vector<double>(ggg, ggg + m);
    solution.optValues.push_back(*fff);
    solution.optPoint = std::vector<double>(xxx, xxx + n);
    solution.calcCounters = std::vector<int>(m + 1, n_evals);
    free(xxx);
    free(fff);
    free(ggg);
    /*****************************************************************/
    /*********************  MPI Slave Thread   ***********************/
    /*****************************************************************/
    }else{
       istop = 0;
       while(istop<=0)
       {
         MPI_Recv( &x, n*params.numThreads, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status );

         #pragma omp parallel for
         for (unsigned t = 0; t < params.numThreads; t++)  {
           for (int i = 0; i < m; i++)
             g[t*m + i] = problem->Calculate(x + t*n, i);
           f[t*o] = problem->Calculate(x + t*n, m);
         }

         MPI_Send( &f, o*params.numThreads, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD );
         MPI_Send( &g, m*params.numThreads, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD );
         MPI_Recv( &istop,1,MPI_INTEGER,0,4,MPI_COMM_WORLD, &status );
       }
    }
    /*****************************************************************/
    //if( istop >= 1 ){ MPI_Finalize(); }
    /*****************************************************************/
    // printf("\n Solution f[0] = %f ", f[0]);
    // printf("\n Solution g[0] = %f ", g[0]);
    // printf("\n Solution x[0] = %f ", x[0]);
    /*****************************************************************/

  return solution;
}
