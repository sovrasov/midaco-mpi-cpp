#include <mpi.h>
#include <algorithm>
#include "midaco_mpi.hpp"

#include <midaco_core.h>
#include <omp.h>

MidacoSolution solve_midaco_mpi(const IGOProblem<double>* problem, const MidacoMPIParameters& params,
                                std::function<bool(const double*)> external_stop)
{
    MidacoSolution solution;

    int proc, nprocs;
    MPI_Status status;
    MPI_Comm_rank( MPI_COMM_WORLD, &proc );
    MPI_Comm_size( MPI_COMM_WORLD, &nprocs );

    long int o,n,ni,m,me,maxeval,maxtime,printeval,save2file,iflag,istop;
    long int liw,lrw,lpf,i,iw[5000],p=1; double rw[20000],pf[20000],param[13];
    char key[] = "MIDACO_LIMITED_VERSION___[CREATIVE_COMMONS_BY-NC-ND_LICENSE]";

    o  = 1; /* Number of objectives                          */
    n  = problem->GetDimension(); /* Number of variables (in total)                */
    ni = 0; /* Number of integer variables (0 <= ni <= n)    */
    m  = problem->GetConstraintsNumber(); /* Number of constraints (in total)              */
    me = 0; /* Number of equality constraints (0 <= me <= m) */

    double* f = new double[o*params.numThreads];
    double* g = new double[m*params.numThreads];
    double* x = new double[n*params.numThreads];
    double* xl = new double[n];
    double* xu = new double[n];

    problem->GetBounds(xl, xu);
    std::copy_n(xl, n, x);

    maxeval = params.maxEvals;    /* Maximum number of function evaluation (e.g. 1000000)  */
    maxtime = 60*60*24; /* Maximum time limit in Seconds (e.g. 1 Day = 60*60*24) */
    printeval = 1000; /* Print-Frequency for current best solution (e.g. 1000) */
    save2file = 0;    /* Save SCREEN and SOLUTION to TXT-files [ 0=NO/ 1=YES]  */

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

    long int num_points = params.numThreads * nprocs;
    p = nprocs; // nprocs argument given automatically via command:
                  // "mpirun -np nprocs ./run"

    if (proc == 0)
    {
        double *xxx,*fff,*ggg; int c;
        /* Allocate arrays for parallelization */
        xxx = new double[params.numThreads*p*n];
        fff = new double[params.numThreads*p*o];
        ggg = new double[params.numThreads*p*m];
        /* Store starting point x in xxx array */
        for( c=0; c<p*params.numThreads; c++){ for( i=0; i<n; i++){ xxx[c*n+i] = x[i]; }}
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
                for( i=0; i<params.numThreads; i++)
                  if (external_stop(xxx + (c-1)*n*params.numThreads + i*n))
                    istop = 1;
            }
            MPI_Scatter(xxx, n*params.numThreads, MPI_DOUBLE, x,
              n*params.numThreads, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            #pragma omp parallel for num_threads(params.numThreads)
            for (unsigned t = 0; t < params.numThreads; t++)  {
              for (int i = 0; i < m; i++)
                g[t*m + i] = problem->Calculate(xxx + t*n, i);
              f[t*o] = problem->Calculate(xxx + t*n, m);
              if (external_stop(xxx + t*n))
              #pragma omp atomic write
                istop = 1;
            }

            /* Collect results F & G */
            MPI_Gather(f, o*params.numThreads, MPI_DOUBLE, fff, o*params.numThreads, MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
            MPI_Gather(g, m*params.numThreads, MPI_DOUBLE, ggg, m*params.numThreads, MPI_DOUBLE, 0,
               MPI_COMM_WORLD);

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
        }

        solution.optValues = std::vector<double>(ggg, ggg + m);
        solution.optValues.push_back(*fff);
        solution.optPoint = std::vector<double>(xxx, xxx + n);
        solution.calcCounters = std::vector<int>(m + 1, n_evals);
        delete[] xxx;
        delete[] fff;
        delete[] ggg;
    }
    else
    {
       istop = 0;
       while(istop<=0)
       {
         MPI_Scatter(nullptr, n*params.numThreads, MPI_DOUBLE, x,
           n*params.numThreads, MPI_DOUBLE, 0, MPI_COMM_WORLD);

         #pragma omp parallel for num_threads(params.numThreads)
         for (unsigned t = 0; t < params.numThreads; t++)  {
           for (int i = 0; i < m; i++)
             g[t*m + i] = problem->Calculate(x + t*n, i);
           f[t*o] = problem->Calculate(x + t*n, m);
         }

         MPI_Gather(f, o*params.numThreads, MPI_DOUBLE, nullptr, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
         MPI_Gather(g, m*params.numThreads, MPI_DOUBLE, nullptr, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
         MPI_Recv( &istop,1,MPI_INTEGER,0,4,MPI_COMM_WORLD, &status );
       }
    }

    delete[] f;
    delete[] g;
    delete[] x;
    delete[] xl;
    delete[] xu;

    return solution;
}
