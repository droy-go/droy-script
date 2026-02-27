# Droy Studio - Professional IDE

Droy Studio is a professional integrated development environment for Droy Language and C/C++ development.

## Features

### Multi-Language Support
- **Droy Language** - Full syntax highlighting and IntelliSense
- **C/C++** - Complete C11 and C++17 support
- **Headers** - `.h` and `.hpp` file support

### Code Editor
- Syntax highlighting for all supported languages
- Auto-completion with intelligent suggestions
- Code folding and bracket matching
- Find and replace with regex support
- Multiple cursors and selections
- Line numbers and error indicators

### File Management
- Project explorer with folder navigation
- Multiple file tabs
- Recent files list
- File search across project
- Drag and drop file operations

### Build System
- Integrated build system
- Support for Make and CMake
- Error parsing and navigation
- Build output panel

### Debugging
- Integrated debugger interface
- Breakpoint management
- Variable inspection
- Call stack view
- Step-through debugging

### Terminal
- Built-in terminal
- Command history
- Project-specific commands
- Build and run shortcuts

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+N` | New File |
| `Ctrl+O` | Open File |
| `Ctrl+S` | Save File |
| `Ctrl+Z` | Undo |
| `Ctrl+Y` | Redo |
| `Ctrl+F` | Find |
| `Ctrl+H` | Replace |
| `Ctrl+Space` | Auto-complete |
| `F5` | Run |
| `F9` | Build |
| `F10` | Debug |

## Usage

### Opening the Editor
```bash
# Open editor directly
open editor/index.html

# Or serve with a local server
cd editor
python -m http.server 8080
# Then open http://localhost:8080
```

### Creating a New File
1. Click "New File" button or press `Ctrl+N`
2. Select file type from dropdown (Droy, C, C++, etc.)
3. Start coding!

### Building and Running
1. Select build target from dropdown
2. Press `F9` to build
3. Press `F5` to run

### Debugging
1. Set breakpoints by clicking line numbers
2. Press `F10` to start debugging
3. Use debug controls to step through code

## File Types

| Extension | Language | Mode |
|-----------|----------|------|
| `.droy` | Droy | droy |
| `.c` | C | text/x-csrc |
| `.h` | C Header | text/x-csrc |
| `.cpp` | C++ | text/x-c++src |
| `.hpp` | C++ Header | text/x-c++src |

## Integration with Droy Runtime

The editor integrates seamlessly with the Droy Runtime for live UI preview:

1. Write Droy code in the editor
2. Switch to Runtime tab for live preview
3. See changes in real-time

## Project Structure

```
editor/
├── index.html          # Main editor interface
├── editor.css          # Editor styles
├── editor.js           # Editor logic
├── droy-mode.js        # Droy language mode for CodeMirror
├── c-mode.js           # C/C++ language mode
└── README.md           # This file
```

## Browser Compatibility

- Chrome 80+
- Firefox 75+
- Safari 13+
- Edge 80+

## License

MIT License - See LICENSE file for details
