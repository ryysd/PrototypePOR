default="\033[m"
red="\033[31m"
greeen="\033[32m" 
yellow="\033[33m"
blue="\033[34m"
cyan="\033[36m"

project_root=`pwd`
gtest_dir=$project_root/test/gtest-svn
gtest_build_dir=$gtest_dir/build
gtest_libgtest=$gtest_build_dir/libgtest.a
gtest_libgtest_main=$gtest_build_dir/libgtest_main.a

function print_header() {
  echo "${blue}******************************************************************${default}"
  echo "${blue}* $1  ${white}"
  echo "${blue}******************************************************************${default}"
}

if [ ! -e $gtest_dir ]
then 
  print_header "Checkout Google Test"
  svn checkout http://googletest.googlecode.com/svn/trunk/ $gtest_dir
fi

if [ ! -e $gtest_build_dir ]
then mkdir $gtest_build_dir
fi

if [ ! -e $gtest_libgtest -o ! -e $gtest_libgtest_main ]
then
  print_header "Built Google Test"
  cd $gtest_build_dir
  cmake ..
  make
fi

print_header "Check Coding Style"
cd $project_root
find . -name "*.h" -o -name "*.cc" | grep -v thirdparty | grep -v test | xargs python ./test/cpplint.py --filter=-legal/copyright,-readability/streams --counting=detailed --linelength=150 --root=c/src

print_header "Built binary"
cd ./build
export GTEST_DIR=$gtest_dir
if [ $# -gt 0 ] 
then cmake -DCMAKE_BUILD_TYPE=$1 ..
else cmake ..
fi

make clean
make
