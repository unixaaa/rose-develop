/*
 * Automatic Parallelization using OpenMP 
 *
 * Input: sequential C/C++ code
 * Output: parallelized C/C++ code using OpenMP
 *
 * Algorithm:
 *   Read in semantics specification (formerly array abstraction) files 
 *   Collect all loops with canonical forms
 *     x. Conduct loop normalization
 *     x. Call dependence analysis from Qing's loop transformations
 *     x. Conduct liveness analysis and variable classification
 *     x. Judge if the loop is parallelizable
 *     x. Attach OmpAttribute if it is
 *     x. Insert OpenMP pragma accordingly
 *
 * By Chunhua Liao
 * Nov 3, 2008
 */
#include "rose.h"
// all kinds of analyses needed
#include "autoParSupport.h" 
using namespace std;
using namespace AutoParallelization;
using namespace SageInterface;
int
main (int argc, char *argv[])
{
  vector<string> argvList(argv, argv+argc);
  //Processing debugging and annotation options
  autopar_command_processing(argvList);
  // enable parsing user-defined pragma if enable_diff is true
  // -rose:openmp:parse_only
  if (enable_diff)
    argvList.push_back("-rose:openmp:parse_only");
  SgProject *project = frontend (argvList);
  ROSE_ASSERT (project != NULL);

 // This has to happen before analyses are called.
  // For each loop 
  VariantVector vv (V_SgForStatement); 
  Rose_STL_Container<SgNode*> loops = NodeQuery::queryMemoryPool(vv); 

  // normalize C99 style for (int i= x, ...) to C89 style: int i;  (i=x, ...)
  // Liao, 10/22/2009. Thank Jeff Keasler for spotting this bug
  for (Rose_STL_Container<SgNode*>::iterator iter = loops.begin();
      iter!= loops.end(); iter++ )
  {
    SgForStatement* cur_loop = isSgForStatement(*iter);
    ROSE_ASSERT(cur_loop);
    // skip for (;;) , SgForStatement::get_test_expr() has a buggy assertion.
    SgStatement* test_stmt = cur_loop->get_test();
    if (test_stmt!=NULL && 
        isSgNullStatement(test_stmt))
      continue;

    // skip system header
    if (insideSystemHeader (cur_loop) )
      continue; 
#if 0 // we now always normalize loops, then later undo some normalization 6/22/2016
    // SageInterface::normalizeForLoopInitDeclaration(cur_loop);
    if (keep_c99_loop_init) 
    {
      // 2/29/2016, disable for loop init declaration normalization
      // This is not used . No longer used.
      normalizeForLoopTest(cur_loop);
      normalizeForLoopIncrement(cur_loop);
      ensureBasicBlockAsBodyOfFor(cur_loop);
      constantFolding(cur_loop->get_test());
      constantFolding(cur_loop->get_increment());
    }
    else
#endif
      SageInterface::forLoopNormalization(cur_loop);
  }

  //Prepare liveness analysis etc.
  //TOO much output for analysis debugging info.
  //initialize_analysis (project,enable_debug);   
  initialize_analysis (project, false);   

  // For each source file in the project
    SgFilePtrList & ptr_list = project->get_fileList();
    for (SgFilePtrList::iterator iter = ptr_list.begin(); iter!=ptr_list.end();
        iter++)
   {
     SgFile* sageFile = (*iter);
     SgSourceFile * sfile = isSgSourceFile(sageFile);
     ROSE_ASSERT(sfile);
     SgGlobal *root = sfile->get_globalScope();
     SgDeclarationStatementPtrList& declList = root->get_declarations ();
     bool hasOpenMP= false; // flag to indicate if omp.h is needed in this file

    //For each function body in the scope
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) 
     {
        SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
        if (func == 0)  continue;
        SgFunctionDefinition *defn = func->get_definition();
        if (defn == 0)  continue;
         //ignore functions in system headers, Can keep them to test robustness
        if (defn->get_file_info()->get_filename()!=sageFile->get_file_info()->get_filename())
          continue;
        SgBasicBlock *body = defn->get_body();  
       // For each loop 
        Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(defn,V_SgForStatement); 
        if (loops.size()==0) continue;

#if 0 // Moved to be executed before running liveness analysis.
      // normalize C99 style for (int i= x, ...) to C89 style: int i;  (i=x, ...)
       // Liao, 10/22/2009. Thank Jeff Keasler for spotting this bug
         for (Rose_STL_Container<SgNode*>::iterator iter = loops.begin();
                     iter!= loops.end(); iter++ )
         {
           SgForStatement* cur_loop = isSgForStatement(*iter);
           ROSE_ASSERT(cur_loop);
           SageInterface::normalizeForLoopInitDeclaration(cur_loop);
         }
#endif
        // X. Replace operators with their equivalent counterparts defined 
        // in "inline" annotations
        AstInterfaceImpl faImpl_1(body);
        CPPAstInterface fa_body(&faImpl_1);
        OperatorInlineRewrite()( fa_body, AstNodePtrImpl(body));
         
	 // Pass annotations to arrayInterface and use them to collect 
         // alias info. function info etc.  
         ArrayAnnotation* annot = ArrayAnnotation::get_inst(); 
         ArrayInterface array_interface(*annot);
         array_interface.initialize(fa_body, AstNodePtrImpl(defn));
         array_interface.observe(fa_body);
	
	//FR(06/07/2011): aliasinfo was not set which caused segfault
	LoopTransformInterface::set_aliasInfo(&array_interface);
       
        // X. Loop normalization for all loops within body
        // Merged into SageInterface::forLoopNormalization()
        //midend/programTransformation/loopProcessing/driver/LoopTransformInterface.h
        /* normalize the forloops in C
          i<x is normalized to i<= (x-1)
          i>x is normalized to i>= (x+1)
        
          i++ is normalized to i=i+1
          i-- is normalized to i=i-1
        */
//        NormalizeForLoop(fa_body, AstNodePtrImpl(body));

	for (Rose_STL_Container<SgNode*>::iterator iter = loops.begin(); 
	    iter!= loops.end(); iter++ ) 
        {
          SgNode* current_loop = *iter;
          //X. Parallelize loop one by one
          // getLoopInvariant() will actually check if the loop has canonical forms 
          // which can be handled by dependence analysis
          SgInitializedName* invarname = getLoopInvariant(current_loop);
          if (invarname != NULL)
          {
            bool ret = ParallelizeOutermostLoop(current_loop, &array_interface, annot);
            if (ret) // if at least one loop is parallelized, we set hasOpenMP to be true for the entire file.
              hasOpenMP = true;  
          }
          else // cannot grab loop index from a non-conforming loop, skip parallelization
          {
            if (enable_debug)
              cout<<"Skipping a non-canonical loop at line:"<<current_loop->get_file_info()->get_line()<<"..."<<endl;
            // We should not reset it to false. The last loop may not be parallelizable. But a previous one may be.  
            //hasOpenMP = false;
          }
        }// end for loops
      } // end for-loop for declarations
     // insert omp.h if needed
     if (hasOpenMP && !enable_diff)
     {
       SageInterface::insertHeader("omp.h",PreprocessingInfo::after,false,root);
       if (enable_patch)
         generatePatchFile(sfile); 
     }
     // compare user-defined and compiler-generated OmpAttributes
     if (enable_diff)
       diffUserDefinedAndCompilerGeneratedOpenMP(sfile); 
   } //end for-loop of files

#if 1
  // undo loop normalization
  std::map <SgForStatement* , bool >::iterator iter = trans_records.forLoopInitNormalizationTable.begin();
  for (; iter!= trans_records.forLoopInitNormalizationTable.end(); iter ++) 
  {
    SgForStatement* for_loop = (*iter).first; 
    unnormalizeForLoopInitDeclaration (for_loop);
  }
#endif
  // Qing's loop normalization is not robust enough to pass all tests
  //AstTests::runAllTests(project);
  
  release_analysis();
  //project->unparse();
  return backend (project);
}
