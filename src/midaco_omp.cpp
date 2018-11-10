#include "midaco_omp.hpp"

#include <midaco_core.h>
#include <omp.h>

MidacoSolution solve_midaco_omp(const IGOProblem<double>* problem, const MidacoOMPParameters& params,
    std::function<bool(const double*)> external_stop)
{
  MidacoSolution solution;

  long int o,n,ni,m,me,maxeval,maxtime,printeval,save2file,iflag,istop;
  long int liw,lrw,lpf,i,iw[5000],p=1; double rw[20000],pf[20000];
  double   f[10],g[1000],x[1000],xl[1000],xu[1000],param[13];
  char key[] = "MIDACO_LIMITED_VERSION___[CREATIVE_COMMONS_BY-NC-ND_LICENSE]";

  o  = 1; /* Number of objectives                          */
  n  = problem->GetDimension(); /* Number of variables (in total)                */
  ni = 0; /* Number of integer variables (0 <= ni <= n)    */
  m  = problem->GetConstraintsNumber(); /* Number of constraints (in total)              */
  me = 0; /* Number of equality constraints (0 <= me <= m) */

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

  p = params.numThreads; /* Serial = 1, Parallel = 2,3,4,... */

  /*****************************************************************/
  /*****************************************************************/
  /*****************************************************************/
  /* Preparations for Parallelization */
  double *xxx,*fff,*ggg;
  omp_set_num_threads(p);
  /* Allocate arrays for parallelization */
  fff = (double *) malloc((p*o)*sizeof(double));
  ggg = (double *) malloc((p*m)*sizeof(double));
  xxx = (double *) malloc((p*n)*sizeof(double));
  /* Store starting point x in xxx array */
  for(int c=0; c<p; c++){ for( i=0; i<n; i++){ xxx[c*n+i] = x[i]; }}
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
  midaco_print(1,printeval,save2file,&iflag,&istop,&*f,&*g,&*x,
    &*xl,&*xu,o,n,ni,m,me,&*rw,&*pf,maxeval,maxtime,&*param,p,&*key);
  int n_evals = 0;
  while(istop==0) /*~~~ Start of the reverse communication loop ~~~*/
  {
    /* Evaluate block of 'P' iterates in parallel */
    #pragma omp parallel for
    for (int c=0; c<p; c++)
    {
      for (int i = 0; i < problem->GetConstraintsNumber(); i++)
        ggg[c*m + i] = problem->Calculate(xxx + c*n, i);
      fff[c*o] = problem->Calculate(xxx + c*n, m);
      if (external_stop(xxx + c*n))
      #pragma omp atomic write
        istop = 1;
    }
    n_evals += p;
    /* Call MIDACO */
    midaco(&p,&o,&n,&ni,&m,&me,&*xxx,&*fff,&*ggg,&*xl,&*xu,&iflag,
           &istop,&*param,&*rw,&lrw,&*iw,&liw,&*pf,&lpf,&*key);
    /* Call MIDACO printing routine */
    midaco_print(2,printeval,save2file,&iflag,&istop,&*fff,&*ggg,&*xxx,
      &*xl,&*xu,o,n,ni,m,me,&*rw,&*pf,maxeval,maxtime,&*param,p,&*key);
  } /*~~~End of the reverse communication loop ~~~*/
  /*****************************************************************/
  solution.optValues = std::vector<double>(ggg, ggg + m);
  solution.optValues.push_back(*fff);
  solution.optPoint = std::vector<double>(xxx, xxx + n);
  solution.calcCounters = std::vector<int>(m + 1, n_evals);

  return solution;
}
