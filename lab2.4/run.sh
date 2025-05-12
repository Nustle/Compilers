if [ ! -d "cmake-build-debug" ]; then
    mkdir cmake-build-debug
fi

cd cmake-build-debug
cmake ..
make

if [ $? -ne 0 ]; then
    echo "Сборка завершилась с ошибкой."
    exit 1
fi

./lab2_4 ../example/program.txt ../output/ast.json
cat ../output/ast.json | jq . > ../output/ast_pretty.json
rm ../output/ast.json
mv ../output/ast_pretty.json ../output/ast.json