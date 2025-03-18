package main

import (
	"fmt"
	"go/ast"
	"go/format"
	"go/parser"
	"go/token"
	"os"
)

func deletePrintlnDebug(file *ast.File) {
	ast.Inspect(file, func(node ast.Node) bool {
		if blockStmt, ok := node.(*ast.BlockStmt); ok {
			for i, stmt := range blockStmt.List {
				if exprStmt, okStmt := stmt.(*ast.ExprStmt); okStmt {
					isDebug := false
					if callExpr, okCallExpr := exprStmt.X.(*ast.CallExpr); okCallExpr {
						if funExpr, okFun := callExpr.Fun.(*ast.SelectorExpr); okFun {
							if funExpr.X.(*ast.Ident).Name != "fmt" || funExpr.Sel.Name != "Println" {
								continue
							}
						}
						args := callExpr.Args
						if len(args) == 0 {
							continue
						}
						firstArg := args[0]
						if basicLit, isLit := firstArg.(*ast.BasicLit); isLit {
							if basicLit.Value == "\"DEBUG:\"" {
								isDebug = true
							}
						}
					}
					if isDebug {
						copy(blockStmt.List[i:], blockStmt.List[i+1:])
						blockStmt.List = blockStmt.List[:len(blockStmt.List)-1]
					}
				}
			}
		}
		return true
	})
}

func main() {
	if len(os.Args) != 2 {
		fmt.Printf("usage: astprint <ast_test.go>\n")
		return
	}

	fset := token.NewFileSet()
	outputFile := os.Args[1][:len(os.Args[1])-2]

	if file, err := parser.ParseFile(
		fset,
		os.Args[1],
		nil,
		parser.ParseComments,
	); err == nil {
		out, _ := os.Create(outputFile + "txt")
		ast.Fprint(out, fset, file, nil)
		deletePrintlnDebug(file)
		outProgram, _ := os.Create(outputFile[:len(outputFile)-1] + "_new.go")
		format.Node(outProgram, fset, file)
		fmt.Printf("New program saved in %s\n", outputFile[:len(outputFile)-1]+"_new.go")
	} else {
		fmt.Printf("Error: %v", err)
	}
}
