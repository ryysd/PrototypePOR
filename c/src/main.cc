/** 
 *  @file     main.cpp  
 *  @brief    main file 
*/

#include "./common.h"
#include "./reducer/reducer_option.h"
#include "./reducer/reducer_executer.h"

int main(int argc, char** argv) {
  ReducerOption option;

  if (option.Parse(argc, argv)) {
    ReducerExecuter executer(&option);

    executer.Execute();
    executer.DumpResult();
  }
}
