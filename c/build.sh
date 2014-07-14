project_root=`pwd`
gtest_dir=$project_root/test/gtest-svn
gtest_build_dir=$gtest_dir/build
gtest_libgtest=$gtest_build_dir/libgtest.a
gtest_libgtest_main=$gtest_build_dir/libgtest_main.a

if [ ! -e $gtest_dir ]
then svn checkout http://googletest.googlecode.com/svn/trunk/ $gtest_dir
fi

if [ ! -e $gtest_build_dir ]
then mkdir $gtest_build_dir
fi

if [ ! -e $gtest_libgtest -o ! -e gtest_libgtest_main ]
then
  cd $gtest_build_dir
  cmake ..
  make
fi

cd $project_root
find . -name "*.h" -o -name "*.cc" | grep -v thirdparty | grep -v test | xargs python ./test/cpplint.py --filter=-legal/copyright,-readability/streams --counting=detailed --linelength=150 --root=c/src

cd ./build

export GTEST_DIR=$gtest_dir
if [ $# -gt 0 ] 
then cmake -DCMAKE_BUILD_TYPE=$1 ..
else cmake ..
fi

make
