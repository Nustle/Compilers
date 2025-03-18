package main

import "fmt"

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func main() {
	x := 6
	for i := 1; i < 10; i++ {
		if i%x == 0 {
			fmt.Println("число", i, "делится на", x)
		} else {
			isDivisor := false
			for j := 1; j < min(x, i); j++ {
				if (i*j)%x == 0 {
					isDivisor = true
					fmt.Println("число", i, "имеет общий делитель с", x)
				}

			}
			if !isDivisor {
				fmt.Println("число", "DEBUG:", i, "не имеет общий делитель с", x)
			}
		}
	}
}
