find . -name "*.h" -o -name "*.cc" | grep -v thirdparty | xargs python ./test/cpplint.py --filter=-legal/copyright,-readability/streams --counting=detailed --linelength=150 --root=c/src

cd ./build

if [ $# -gt 0 ] 
then cmake -DCMAKE_BUILD_TYPE=$1 ..
else cmake ..
fi

make
