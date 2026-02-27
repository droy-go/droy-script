package logger

import (
	"fmt"

	"github.com/fatih/color"
)

// Info logs an info message
func Info(format string, args ...interface{}) {
	msg := fmt.Sprintf(format, args...)
	fmt.Printf("%s %s\n", color.BlueString("ℹ"), msg)
}

// Success logs a success message
func Success(format string, args ...interface{}) {
	msg := fmt.Sprintf(format, args...)
	fmt.Printf("%s %s\n", color.GreenString("✓"), msg)
}

// Warning logs a warning message
func Warning(format string, args ...interface{}) {
	msg := fmt.Sprintf(format, args...)
	fmt.Printf("%s %s\n", color.YellowString("⚠"), msg)
}

// Error logs an error message
func Error(format string, args ...interface{}) {
	msg := fmt.Sprintf(format, args...)
	fmt.Printf("%s %s\n", color.RedString("✗"), msg)
}

// Progress logs a progress message
func Progress(current, total int, format string, args ...interface{}) {
	msg := fmt.Sprintf(format, args...)
	percent := float64(current) / float64(total) * 100
	bar := renderProgressBar(percent)
	fmt.Printf("\r%s [%s] %d/%d %s", color.CyanString("→"), bar, current, total, msg)
	if current == total {
		fmt.Println()
	}
}

func renderProgressBar(percent float64) string {
	width := 20
	filled := int(percent / 100 * float64(width))
	
	bar := ""
	for i := 0; i < width; i++ {
		if i < filled {
			bar += color.GreenString("█")
		} else {
			bar += color.WhiteString("░")
		}
	}
	
	return bar
}

// Debug logs a debug message (only in debug mode)
func Debug(format string, args ...interface{}) {
	// Only log if DROY_PM_DEBUG is set
	// For now, just ignore
}

// Fatal logs a fatal error and exits
func Fatal(format string, args ...interface{}) {
	Error(format, args...)
	panic(fmt.Sprintf(format, args...))
}
