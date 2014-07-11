find . -name "*.h" -o -name "*.cc" | grep -v thirdparty | xargs python ./test/cpplint.py --filter=-legal/copyright,-readability/streams --counting=detailed --linelength=120 --root=c/src
make
