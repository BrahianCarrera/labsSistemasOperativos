package main

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"time"
)

func actAsChild() {
	data, err := io.ReadAll(os.Stdin)
	if err != nil {
		fmt.Println("Error reading stdin:", err)
		return
	}

	segments := strings.SplitN(string(data), "---\n", 2)
	if len(segments) != 2 {
		fmt.Println("Invalid input structure.")
		return
	}

	subMatrix := parseMatrix(segments[0])
	matrixB := parseMatrix(segments[1])
	output := performSequentialMult(subMatrix, matrixB)
	fmt.Print(formatMatrix(output))
}

func readMatrixFromFile(path string) [][]float64 {
	file, err := os.Open(path)
	if err != nil {
		fmt.Println("Unable to open file:", err)
		return nil
	}
	defer file.Close()

	var matrix [][]float64
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		tokens := strings.Fields(scanner.Text())
		var row []float64
		for _, t := range tokens {
			val, err := strconv.ParseFloat(t, 64)
			if err == nil {
				row = append(row, val)
			}
		}
		matrix = append(matrix, row)
	}
	return matrix
}

func validateMultiplication(a, b [][]float64) error {
	if len(a[0]) != len(b) {
		return errors.New("incompatible dimensions: columns of A must match rows of B")
	}
	return nil
}

func performSequentialMult(a, b [][]float64) [][]float64 {
	m, n := len(a), len(b[0])
	p := len(a[0])
	result := make([][]float64, m)
	for i := 0; i < m; i++ {
		result[i] = make([]float64, n)
		for j := 0; j < n; j++ {
			sum := 0.0
			for k := 0; k < p; k++ {
				sum += a[i][k] * b[k][j]
			}
			result[i][j] = sum
		}
	}
	return result
}

func formatMatrix(matrix [][]float64) string {
	var builder strings.Builder
	for _, row := range matrix {
		for j, val := range row {
			builder.WriteString(fmt.Sprintf("%f", val))
			if j < len(row)-1 {
				builder.WriteString(" ")
			}
		}
		builder.WriteString("\n")
	}
	return builder.String()
}

func parseMatrix(raw string) [][]float64 {
	lines := strings.Split(strings.TrimSpace(raw), "\n")
	var matrix [][]float64
	for _, line := range lines {
		tokens := strings.Fields(line)
		var row []float64
		for _, token := range tokens {
			val, err := strconv.ParseFloat(token, 64)
			if err == nil {
				row = append(row, val)
			}
		}
		matrix = append(matrix, row)
	}
	return matrix
}

func divideMatrixRows(a [][]float64, parts int) [][][]float64 {
	total := len(a)
	chunk := total / parts
	remainder := total % parts
	var divided [][][]float64
	begin := 0

	for i := 0; i < parts; i++ {
		end := begin + chunk
		if i < remainder {
			end++
		}
		divided = append(divided, a[begin:end])
		begin = end
	}
	return divided
}

func performParallelMult(a, b [][]float64, workers int) ([][]float64, time.Duration) {

	aChunks := divideMatrixRows(a, workers)
	bData := formatMatrix(b)
	var combined [][]float64

	type result struct {
		data [][]float64
		err  error
	}

	results := make(chan result, workers)

	// Medir solo el calculo de la matriz
	start := time.Now()

	for _, part := range aChunks {
		go func(part [][]float64) {
			cmd := exec.Command(os.Args[0], "--child")

			inPipe, err := cmd.StdinPipe()
			if err != nil {
				results <- result{nil, fmt.Errorf("stdin error: %w", err)}
				return
			}

			outPipe, err := cmd.StdoutPipe()
			if err != nil {
				results <- result{nil, fmt.Errorf("stdout error: %w", err)}
				return
			}

			if err := cmd.Start(); err != nil {
				results <- result{nil, fmt.Errorf("start error: %w", err)}
				return
			}

			go func() {
				defer inPipe.Close()
				data := formatMatrix(part) + "---\n" + bData
				inPipe.Write([]byte(data))
			}()

			output, err := io.ReadAll(outPipe)
			if err != nil {
				results <- result{nil, fmt.Errorf("read error: %w", err)}
				return
			}

			cmd.Wait()
			section := parseMatrix(string(output))
			results <- result{section, nil}
		}(part)
	}

	for i := 0; i < workers; i++ {
		res := <-results
		if res.err != nil {
			fmt.Println("Error from worker:", res.err)
			time_error := time.Since(start)
			return nil, time_error
		}
		combined = append(combined, res.data...)
	}

	elapsed := time.Since(start)

	return combined, elapsed
}

func askWorkers(limit int) int {
	fmt.Print("Enter number of processes for parallel execution: ")
	reader := bufio.NewReader(os.Stdin)
	text, _ := reader.ReadString('\n')
	text = strings.TrimSpace(text)

	n, err := strconv.Atoi(text)
	if err != nil || n < 1 {
		fmt.Println("Invalid input. Using 1 process.")
		return 1
	}

	if n > limit {
		fmt.Printf("Too many processes for available rows. Using %d instead.\n", limit)
		return limit
	}
	return n
}

func main() {
	if len(os.Args) > 1 && os.Args[1] == "--child" {
		actAsChild()
		return
	}

	a := readMatrixFromFile("../ArchivosTXT/A.txt")
	b := readMatrixFromFile("../ArchivosTXT/B.txt")

	if err := validateMultiplication(a, b); err != nil {
		fmt.Println("Matrix validation error:", err)
		return
	}

	// Ejecutar secuencial
	startSeq := time.Now()
	_ = performSequentialMult(a, b)
	timeSeq := time.Since(startSeq)

	// ejecutar paralelo
	processes := askWorkers(len(a))
	_, timePar := performParallelMult(a, b, processes)

	SpeedUp := timeSeq.Seconds() / timePar.Seconds()

	fmt.Printf("\nSequential time: %f seconds\n", timeSeq.Seconds())
	fmt.Printf("Parallel time (%d processes): %f seconds \n", processes, timePar.Seconds())
	fmt.Printf("Speedup: %fX \n", SpeedUp)
}
