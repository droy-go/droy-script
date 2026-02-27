// Droy Studio - Professional IDE with C/C++ Support
// ==================================================

// File type configurations
const fileTypes = {
    'droy': { mode: 'droy', name: 'Droy', icon: 'fa-code' },
    'c': { mode: 'text/x-csrc', name: 'C', icon: 'fa-c' },
    'h': { mode: 'text/x-csrc', name: 'C Header', icon: 'fa-h' },
    'cpp': { mode: 'text/x-c++src', name: 'C++', icon: 'fa-cplusplus' },
    'hpp': { mode: 'text/x-c++src', name: 'C++ Header', icon: 'fa-h' },
    'js': { mode: 'javascript', name: 'JavaScript', icon: 'fa-js' },
    'json': { mode: 'javascript', name: 'JSON', icon: 'fa-brackets-curly' },
    'md': { mode: 'markdown', name: 'Markdown', icon: 'fa-markdown' },
    'txt': { mode: 'null', name: 'Plain Text', icon: 'fa-file-alt' }
};

// Sample C code template
const cTemplate = `/**
 * @file main.c
 * @brief Main entry point for Droy application
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "droy.h"

/**
 * @brief Main function
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status
 */
int main(int argc, char *argv[]) {
    printf("Droy Language Runtime v1.0.0\\n");
    printf("===========================\\n\\n");
    
    if (argc < 2) {
        printf("Usage: %s <file.droy>\\n", argv[0]);
        return 1;
    }
    
    // Initialize Droy interpreter
    DroyContext *ctx = droy_init();
    if (!ctx) {
        fprintf(stderr, "Failed to initialize Droy context\\n");
        return 1;
    }
    
    // Load and execute file
    int result = droy_execute_file(ctx, argv[1]);
    
    // Cleanup
    droy_cleanup(ctx);
    
    return result;
}
`;

// Sample header template
const hTemplate = `/**
 * @file droy.h
 * @brief Droy Language Public API
 */

#ifndef DROY_H
#define DROY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

/* Type Definitions */
typedef struct DroyContext DroyContext;
typedef struct DroyValue DroyValue;
typedef enum {
    DROY_OK = 0,
    DROY_ERROR_SYNTAX,
    DROY_ERROR_RUNTIME,
    DROY_ERROR_MEMORY,
    DROY_ERROR_FILE
} DroyError;

/* Context Management */
DroyContext* droy_init(void);
void droy_cleanup(DroyContext *ctx);
void droy_reset(DroyContext *ctx);

/* Execution */
int droy_execute(DroyContext *ctx, const char *code);
int droy_execute_file(DroyContext *ctx, const char *filename);

/* Variable Management */
void droy_set_variable(DroyContext *ctx, const char *name, const char *value);
const char* droy_get_variable(DroyContext *ctx, const char *name);

/* Error Handling */
DroyError droy_get_last_error(DroyContext *ctx);
const char* droy_error_string(DroyError error);

#ifdef __cplusplus
}
#endif

#endif /* DROY_H */
`;

// Sample C++ template
const cppTemplate = `/**
 * @file droy_cpp.cpp
 * @brief Droy C++ Interface
 */

#include "droy_cpp.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <map>

namespace droy {

// DroyValue Implementation
DroyValue::DroyValue() : type_(Type::Null) {}

DroyValue::DroyValue(const std::string& str) 
    : type_(Type::String), string_value_(str) {}

DroyValue::DroyValue(int num) 
    : type_(Type::Number), number_value_(num) {}

DroyValue::DroyValue(bool val) 
    : type_(Type::Boolean), bool_value_(val) {}

DroyValue::~DroyValue() = default;

std::string DroyValue::toString() const {
    switch (type_) {
        case Type::String:
            return string_value_;
        case Type::Number:
            return std::to_string(number_value_);
        case Type::Boolean:
            return bool_value_ ? "true" : "false";
        case Type::Null:
            return "null";
        default:
            return "<object>";
    }
}

// DroyContext Implementation
class DroyContext::Impl {
public:
    std::map<std::string, DroyValue> variables;
    std::map<std::string, std::string> links;
    std::vector<std::string> output;
    bool employment = false;
    bool running = false;
    int pressure = 0;
    bool locked = false;
};

DroyContext::DroyContext() : impl_(std::make_unique<Impl>()) {}

DroyContext::~DroyContext() = default;

void DroyContext::setVariable(const std::string& name, const DroyValue& value) {
    impl_->variables[name] = value;
}

DroyValue DroyContext::getVariable(const std::string& name) const {
    auto it = impl_->variables.find(name);
    if (it != impl_->variables.end()) {
        return it->second;
    }
    return DroyValue();
}

int DroyContext::execute(const std::string& code) {
    std::cout << "Executing Droy code..." << std::endl;
    // TODO: Implement actual execution
    return 0;
}

int DroyContext::executeFile(const std::string& filename) {
    std::cout << "Executing file: " << filename << std::endl;
    // TODO: Implement file execution
    return 0;
}

} // namespace droy
`;

// Sample C++ header template
const hppTemplate = `/**
 * @file droy_cpp.hpp
 * @brief Droy C++ Public Interface
 */

#ifndef DROY_CPP_HPP
#define DROY_CPP_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace droy {

/**
 * @brief Value types supported by Droy
 */
enum class ValueType {
    Null,
    String,
    Number,
    Boolean,
    Array,
    Object
};

/**
 * @brief Droy value container
 */
class DroyValue {
public:
    DroyValue();
    explicit DroyValue(const std::string& str);
    explicit DroyValue(int num);
    explicit DroyValue(bool val);
    explicit DroyValue(const std::vector<DroyValue>& arr);
    explicit DroyValue(const std::map<std::string, DroyValue>& obj);
    ~DroyValue();

    // Copy and move
    DroyValue(const DroyValue& other);
    DroyValue(DroyValue&& other) noexcept;
    DroyValue& operator=(const DroyValue& other);
    DroyValue& operator=(DroyValue&& other) noexcept;

    // Type checking
    bool isNull() const;
    bool isString() const;
    bool isNumber() const;
    bool isBoolean() const;
    bool isArray() const;
    bool isObject() const;

    // Value getters
    std::string asString() const;
    int asNumber() const;
    bool asBoolean() const;
    std::vector<DroyValue> asArray() const;
    std::map<std::string, DroyValue> asObject() const;

    // Conversion
    std::string toString() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief Droy execution context
 */
class DroyContext {
public:
    DroyContext();
    ~DroyContext();

    // Disable copy
    DroyContext(const DroyContext&) = delete;
    DroyContext& operator=(const DroyContext&) = delete;

    // Enable move
    DroyContext(DroyContext&&) noexcept;
    DroyContext& operator=(DroyContext&&) noexcept;

    // Variable management
    void setVariable(const std::string& name, const DroyValue& value);
    DroyValue getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    void clearVariables();

    // Execution
    int execute(const std::string& code);
    int executeFile(const std::string& filename);

    // Output
    std::vector<std::string> getOutput() const;
    void clearOutput();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief Create a new Droy context
 * @return Shared pointer to context
 */
std::shared_ptr<DroyContext> createContext();

} // namespace droy

#endif // DROY_CPP_HPP
`;

// IDE State
const ideState = {
    editor: null,
    openFiles: new Map(),
    currentFile: null,
    fileCounter: 1,
    projectPath: null,
    recentFiles: JSON.parse(localStorage.getItem('recentFiles') || '[]'),
    gitBranch: 'main'
};

// Initialize IDE
document.addEventListener('DOMContentLoaded', () => {
    initEditor();
    initEventListeners();
    initFileTree();
    initTerminal();
    loadRecentFiles();
});

function initEditor() {
    const textarea = document.getElementById('code-editor');
    
    ideState.editor = CodeMirror.fromTextArea(textarea, {
        mode: 'droy',
        theme: 'dracula',
        lineNumbers: true,
        autoCloseBrackets: true,
        matchBrackets: true,
        indentUnit: 4,
        tabSize: 4,
        indentWithTabs: false,
        lineWrapping: false,
        foldGutter: true,
        gutters: ['CodeMirror-linenumbers', 'CodeMirror-foldgutter'],
        extraKeys: {
            'Ctrl-Space': 'autocomplete',
            'Ctrl-S': (cm) => saveCurrentFile(),
            'Ctrl-O': (cm) => openFile(),
            'Ctrl-N': (cm) => newFile(),
            'Ctrl-Z': (cm) => cm.undo(),
            'Ctrl-Y': (cm) => cm.redo(),
            'Ctrl-F': (cm) => cm.execCommand('find'),
            'Ctrl-H': (cm) => cm.execCommand('replace'),
            'F5': (cm) => runCode(),
            'F9': (cm) => buildCode(),
            'F10': (cm) => debugCode(),
            'Tab': (cm) => {
                if (cm.somethingSelected()) {
                    cm.indentSelection('add');
                } else {
                    cm.replaceSelection('    ');
                }
            }
        }
    });
    
    // Update status bar on cursor activity
    ideState.editor.on('cursorActivity', updateStatusBar);
    ideState.editor.on('change', onEditorChange);
}

function initEventListeners() {
    // Header buttons
    document.getElementById('btn-new-file').addEventListener('click', newFile);
    document.getElementById('btn-open-file').addEventListener('click', openFile);
    document.getElementById('btn-save-file').addEventListener('click', saveCurrentFile);
    document.getElementById('btn-undo').addEventListener('click', () => ideState.editor.undo());
    document.getElementById('btn-redo').addEventListener('click', () => ideState.editor.redo());
    
    // Build and run
    document.getElementById('btn-build').addEventListener('click', buildCode);
    document.getElementById('btn-run').addEventListener('click', runCode);
    document.getElementById('btn-debug').addEventListener('click', debugCode);
    
    // Welcome screen
    document.getElementById('welcome-new').addEventListener('click', newFile);
    document.getElementById('welcome-open').addEventListener('click', openFolder);
    
    // Build target selector
    document.getElementById('build-target').addEventListener('change', (e) => {
        const mode = e.target.value === 'droy' ? 'droy' : 
                     e.target.value === 'cpp' || e.target.value === 'hpp' ? 'text/x-c++src' :
                     'text/x-csrc';
        ideState.editor.setOption('mode', mode);
    });
    
    // Sidebar tabs
    document.querySelectorAll('.sidebar-tab').forEach(tab => {
        tab.addEventListener('click', () => {
            document.querySelectorAll('.sidebar-tab').forEach(t => t.classList.remove('active'));
            document.querySelectorAll('.sidebar-panel').forEach(p => p.classList.remove('active'));
            tab.classList.add('active');
            document.getElementById(`${tab.dataset.panel}-panel`).classList.add('active');
        });
    });
    
    // Panel tabs
    document.querySelectorAll('.bottom-panel .panel-tab').forEach(tab => {
        tab.addEventListener('click', () => {
            document.querySelectorAll('.bottom-panel .panel-tab').forEach(t => t.classList.remove('active'));
            document.querySelectorAll('.bottom-panel .panel-pane').forEach(p => p.classList.remove('active'));
            tab.classList.add('active');
            document.getElementById(`${tab.dataset.panel}-panel`).classList.add('active');
        });
    });
    
    // File inputs
    document.getElementById('file-input').addEventListener('change', handleFileSelect);
    document.getElementById('folder-input').addEventListener('change', handleFolderSelect);
    
    // Keyboard shortcuts
    document.addEventListener('keydown', (e) => {
        if (e.ctrlKey || e.metaKey) {
            switch (e.key) {
                case 'n':
                    e.preventDefault();
                    newFile();
                    break;
                case 'o':
                    e.preventDefault();
                    openFile();
                    break;
                case 's':
                    e.preventDefault();
                    saveCurrentFile();
                    break;
            }
        }
    });
}

function initFileTree() {
    // Initialize with default structure
    const defaultStructure = {
        name: 'droy-project',
        type: 'folder',
        expanded: true,
        children: [
            {
                name: 'src',
                type: 'folder',
                expanded: true,
                children: [
                    { name: 'main.c', type: 'file' },
                    { name: 'lexer.c', type: 'file' },
                    { name: 'parser.c', type: 'file' },
                    { name: 'interpreter.c', type: 'file' }
                ]
            },
            {
                name: 'include',
                type: 'folder',
                expanded: false,
                children: [
                    { name: 'droy.h', type: 'file' },
                    { name: 'lexer.h', type: 'file' },
                    { name: 'parser.h', type: 'file' }
                ]
            },
            {
                name: 'examples',
                type: 'folder',
                expanded: false,
                children: [
                    { name: 'hello.droy', type: 'file' },
                    { name: 'variables.droy', type: 'file' },
                    { name: 'math.droy', type: 'file' }
                ]
            },
            { name: 'Makefile', type: 'file' },
            { name: 'README.md', type: 'file' }
        ]
    };
    
    renderFileTree(defaultStructure, document.getElementById('file-tree'));
}

function renderFileTree(node, container, level = 0) {
    const item = document.createElement('div');
    item.className = 'tree-item';
    item.style.paddingLeft = `${8 + level * 12}px`;
    
    const icon = document.createElement('i');
    icon.className = node.type === 'folder' ? 
        `fas fa-folder folder-icon ${node.expanded ? 'fa-folder-open' : ''}` : 
        'fas fa-file file-icon';
    item.appendChild(icon);
    
    const name = document.createElement('span');
    name.textContent = node.name;
    item.appendChild(name);
    
    item.addEventListener('click', (e) => {
        e.stopPropagation();
        if (node.type === 'file') {
            openFileFromTree(node.name);
        } else {
            node.expanded = !node.expanded;
            container.innerHTML = '';
            renderFileTree(node, container, level);
        }
    });
    
    container.appendChild(item);
    
    if (node.type === 'folder' && node.expanded && node.children) {
        const childrenContainer = document.createElement('div');
        childrenContainer.className = 'tree-children';
        node.children.forEach(child => {
            renderFileTree(child, childrenContainer, level + 1);
        });
        container.appendChild(childrenContainer);
    }
}

function initTerminal() {
    const terminalInput = document.getElementById('terminal-input');
    const terminalOutput = document.getElementById('terminal-output');
    
    terminalInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter') {
            const command = terminalInput.value;
            appendToTerminal(`$ ${command}`, 'command');
            executeTerminalCommand(command);
            terminalInput.value = '';
        }
    });
    
    // Initial message
    appendToTerminal('Droy Studio Terminal v2.0.0', 'info');
    appendToTerminal('Type "help" for available commands', 'info');
    appendToTerminal('');
}

function appendToTerminal(text, type = 'normal') {
    const terminalOutput = document.getElementById('terminal-output');
    const line = document.createElement('div');
    line.className = `terminal-line ${type}`;
    line.textContent = text;
    terminalOutput.appendChild(line);
    terminalOutput.scrollTop = terminalOutput.scrollHeight;
}

function executeTerminalCommand(command) {
    const parts = command.trim().split(' ');
    const cmd = parts[0].toLowerCase();
    const args = parts.slice(1);
    
    switch (cmd) {
        case 'help':
            appendToTerminal('Available commands:', 'info');
            appendToTerminal('  help     - Show this help message');
            appendToTerminal('  clear    - Clear terminal');
            appendToTerminal('  build    - Build current project');
            appendToTerminal('  run      - Run current file');
            appendToTerminal('  ls       - List files');
            appendToTerminal('  pwd      - Show current directory');
            break;
        case 'clear':
            document.getElementById('terminal-output').innerHTML = '';
            break;
        case 'build':
            buildCode();
            break;
        case 'run':
            runCode();
            break;
        case 'ls':
            appendToTerminal('src/  include/  examples/  Makefile  README.md', 'info');
            break;
        case 'pwd':
            appendToTerminal('/home/user/droy-project', 'info');
            break;
        default:
            if (cmd) {
                appendToTerminal(`Command not found: ${cmd}`, 'error');
            }
    }
}

function newFile() {
    const filename = `untitled-${ideState.fileCounter++}`;
    const fileType = document.getElementById('build-target').value;
    const fullName = `${filename}.${fileType}`;
    
    let content = '';
    switch (fileType) {
        case 'c': content = cTemplate; break;
        case 'h': content = hTemplate; break;
        case 'cpp': content = cppTemplate; break;
        case 'hpp': content = hppTemplate; break;
        default: content = '// New Droy file\n';
    }
    
    ideState.openFiles.set(fullName, {
        content: content,
        modified: false,
        type: fileType
    });
    
    addTab(fullName);
    switchToFile(fullName);
    hideWelcomeScreen();
}

function openFile() {
    document.getElementById('file-input').click();
}

function openFolder() {
    document.getElementById('folder-input').click();
}

function handleFileSelect(event) {
    const file = event.target.files[0];
    if (!file) return;
    
    const reader = new FileReader();
    reader.onload = (e) => {
        const content = e.target.result;
        const filename = file.name;
        const extension = filename.split('.').pop();
        
        ideState.openFiles.set(filename, {
            content: content,
            modified: false,
            type: extension
        });
        
        addTab(filename);
        switchToFile(filename);
        addToRecentFiles(filename);
        hideWelcomeScreen();
    };
    reader.readAsText(file);
}

function handleFolderSelect(event) {
    const files = event.target.files;
    if (files.length === 0) return;
    
    appendToTerminal(`Opened folder with ${files.length} files`, 'success');
    // TODO: Implement folder handling
}

function openFileFromTree(filename) {
    // Simulate opening file from tree
    const templates = {
        'main.c': cTemplate,
        'droy.h': hTemplate,
        'hello.droy': '// Hello World Example\n~s @si = "Hello"\n~s @ui = "World"\nem @si + " " + @ui'
    };
    
    const content = templates[filename] || `// ${filename}`;
    const extension = filename.split('.').pop();
    
    ideState.openFiles.set(filename, {
        content: content,
        modified: false,
        type: extension
    });
    
    addTab(filename);
    switchToFile(filename);
    hideWelcomeScreen();
}

function addTab(filename) {
    const tabsContainer = document.getElementById('tabs');
    
    // Check if tab already exists
    if (tabsContainer.querySelector(`[data-file="${filename}"]`)) {
        return;
    }
    
    const tab = document.createElement('div');
    tab.className = 'tab';
    tab.dataset.file = filename;
    
    const name = document.createElement('span');
    name.className = 'tab-name';
    name.textContent = filename;
    tab.appendChild(name);
    
    const close = document.createElement('span');
    close.className = 'tab-close';
    close.innerHTML = '&times;';
    close.addEventListener('click', (e) => {
        e.stopPropagation();
        closeTab(filename);
    });
    tab.appendChild(close);
    
    tab.addEventListener('click', () => switchToFile(filename));
    
    tabsContainer.appendChild(tab);
}

function switchToFile(filename) {
    // Save current content
    if (ideState.currentFile) {
        const current = ideState.openFiles.get(ideState.currentFile);
        if (current) {
            current.content = ideState.editor.getValue();
        }
    }
    
    // Switch to new file
    ideState.currentFile = filename;
    const file = ideState.openFiles.get(filename);
    
    if (file) {
        // Set editor mode based on file type
        const mode = fileTypes[file.type]?.mode || 'null';
        ideState.editor.setOption('mode', mode);
        ideState.editor.setValue(file.content);
        
        // Update UI
        document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
        const tab = document.querySelector(`.tab[data-file="${filename}"]`);
        if (tab) tab.classList.add('active');
        
        // Update status bar
        document.getElementById('file-language').textContent = fileTypes[file.type]?.name || 'Plain Text';
    }
}

function closeTab(filename) {
    const tabsContainer = document.getElementById('tabs');
    const tab = tabsContainer.querySelector(`[data-file="${filename}"]`);
    if (tab) tab.remove();
    
    ideState.openFiles.delete(filename);
    
    if (ideState.currentFile === filename) {
        const remaining = tabsContainer.querySelectorAll('.tab');
        if (remaining.length > 0) {
            switchToFile(remaining[0].dataset.file);
        } else {
            ideState.currentFile = null;
            showWelcomeScreen();
        }
    }
}

function saveCurrentFile() {
    if (!ideState.currentFile) return;
    
    const content = ideState.editor.getValue();
    const file = ideState.openFiles.get(ideState.currentFile);
    
    if (file) {
        file.content = content;
        file.modified = false;
        
        // Remove modified indicator
        const tab = document.querySelector(`.tab[data-file="${ideState.currentFile}"]`);
        if (tab) tab.classList.remove('modified');
        
        // Download file
        const blob = new Blob([content], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = ideState.currentFile;
        a.click();
        URL.revokeObjectURL(url);
        
        appendToTerminal(`Saved ${ideState.currentFile}`, 'success');
    }
}

function onEditorChange() {
    if (ideState.currentFile) {
        const file = ideState.openFiles.get(ideState.currentFile);
        if (file) {
            file.modified = true;
            const tab = document.querySelector(`.tab[data-file="${ideState.currentFile}"]`);
            if (tab) tab.classList.add('modified');
        }
    }
}

function buildCode() {
    const target = document.getElementById('build-target').value;
    appendToTerminal(`Building for target: ${target}...`, 'info');
    
    setTimeout(() => {
        appendToTerminal('Build completed successfully!', 'success');
        document.getElementById('build-status').innerHTML = '<i class="fas fa-check-circle"></i> Build Success';
    }, 1500);
}

function runCode() {
    if (!ideState.currentFile) {
        appendToTerminal('No file to run', 'error');
        return;
    }
    
    appendToTerminal(`Running ${ideState.currentFile}...`, 'info');
    
    const file = ideState.openFiles.get(ideState.currentFile);
    if (file && file.type === 'droy') {
        // Simulate Droy execution
        setTimeout(() => {
            appendToTerminal('Hello World!', 'output');
            appendToTerminal('Execution completed', 'success');
        }, 500);
    } else {
        appendToTerminal('Execution completed', 'success');
    }
}

function debugCode() {
    appendToTerminal('Starting debugger...', 'info');
    appendToTerminal('Debugger attached', 'success');
    
    // Switch to debug panel
    document.querySelectorAll('.sidebar-tab').forEach(t => t.classList.remove('active'));
    document.querySelectorAll('.sidebar-panel').forEach(p => p.classList.remove('active'));
    document.querySelector('[data-panel="debug"]').classList.add('active');
    document.getElementById('debug-panel').classList.add('active');
}

function updateStatusBar() {
    const cursor = ideState.editor.getCursor();
    document.getElementById('cursor-pos').textContent = `Ln ${cursor.line + 1}, Col ${cursor.ch + 1}`;
}

function hideWelcomeScreen() {
    document.getElementById('welcome-screen').style.display = 'none';
    document.querySelector('.CodeMirror').style.display = 'block';
    ideState.editor.refresh();
}

function showWelcomeScreen() {
    document.getElementById('welcome-screen').style.display = 'flex';
    document.querySelector('.CodeMirror').style.display = 'none';
}

function addToRecentFiles(filename) {
    ideState.recentFiles = ideState.recentFiles.filter(f => f !== filename);
    ideState.recentFiles.unshift(filename);
    if (ideState.recentFiles.length > 10) {
        ideState.recentFiles.pop();
    }
    localStorage.setItem('recentFiles', JSON.stringify(ideState.recentFiles));
    loadRecentFiles();
}

function loadRecentFiles() {
    const list = document.getElementById('recent-list');
    list.innerHTML = '';
    
    ideState.recentFiles.forEach(file => {
        const li = document.createElement('li');
        li.innerHTML = `<i class="fas fa-file"></i> ${file}`;
        li.addEventListener('click', () => {
            // Simulate opening recent file
            newFile();
        });
        list.appendChild(li);
    });
}

// Export for global access
window.ideState = ideState;
window.newFile = newFile;
window.openFile = openFile;
window.saveCurrentFile = saveCurrentFile;
window.buildCode = buildCode;
window.runCode = runCode;
window.debugCode = debugCode;
