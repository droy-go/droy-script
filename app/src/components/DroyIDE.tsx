import { useState, useRef, useEffect, useCallback } from 'react';
import Editor from '@monaco-editor/react';
import { 
  Play, 
  Save, 
  FilePlus, 
  Trash2, 
  Terminal,
  Cpu,
  FileCode,
  X,
  GitBranch,
  Layout,
  Moon,
  Sun,
  Zap,
  Copy,
  Check,
  RotateCcw
} from 'lucide-react';
import { Button } from '@/components/ui/button';
import { execute } from '@/droy-lang';
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs';
import { ScrollArea } from '@/components/ui/scroll-area';
import { Separator } from '@/components/ui/separator';

// File type definition
interface DroyFile {
  id: string;
  name: string;
  content: string;
  isOpen: boolean;
  isModified: boolean;
}

// Default files
const defaultFiles: DroyFile[] = [
  {
    id: '1',
    name: 'main.droy',
    content: `// Welcome to Droy IDE!
// Write your Droy code here and see live preview

~s @si = "Hello"
~s @ui = "World"

// Emit the result
em @si + " " + @ui + "!"

// Try some math
set a = 10
set b = 5
em "Sum: " + (a + b)
em "Product: " + (a * b)

// Define a function
f greet(name) {
    ret "Hello, " + name + "!"
}

em greet("Droy Developer")`,
    isOpen: true,
    isModified: false,
  },
  {
    id: '2',
    name: 'functions.droy',
    content: `// Functions Demo

f add(a, b) {
    ret a + b
}

f multiply(a, b) {
    ret a * b
}

f factorial(n) {
    fe (n <= 1) {
        ret 1
    }
    ret n * factorial(n - 1)
}

em "5 + 3 = " + add(5, 3)
em "4 * 7 = " + multiply(4, 7)
em "5! = " + factorial(5)`,
    isOpen: false,
    isModified: false,
  },
  {
    id: '3',
    name: 'links.droy',
    content: `// Links Demo

link id: "google" api: "https://google.com"
link id: "github" api: "https://github.com"
link id: "docs" api: "https://docs.droy-lang.org"

em "Links created successfully!"
em "- google"
em "- github" 
em "- docs"

*/Running`,
    isOpen: false,
    isModified: false,
  },
];

// Monaco Editor Droy Language Definition
const droyLanguageDef = {
  defaultToken: '',
  tokenPostfix: '.droy',
  keywords: [
    'set', 'em', 'ret', 'fe', 'f', 'for', 'sty', 'link', 'text', 'txt', 'pkg', 'media',
    'create-link', 'open-link', 'yoex--links'
  ],
  operators: ['+', '-', '*', '/', '=', '>', '<', '=='],
  symbols: /[=><!~?:&|+\-*\/\^%]+/,
  tokenizer: {
    root: [
      [/[a-zA-Z_]\w*/, {
        cases: {
          '@keywords': 'keyword',
          '@default': 'identifier'
        }
      }],
      [/~[ser]/, 'keyword'],
      [/@\w+/, 'variable.special'],
      [/\*\/\w+/, 'keyword'],
      [/[{}()\[\]]/, '@brackets'],
      [/[;,.]/, 'delimiter'],
      [/@symbols/, {
        cases: {
          '@operators': 'operator',
          '@default': ''
        }
      }],
      [/\d+\.?\d*/, 'number'],
      [/"([^"\\]|\\.)*$/, 'string.invalid'],
      [/"/, 'string', '@string'],
      [/'([^'\\]|\\.)*$/, 'string.invalid'],
      [/'/, 'string', '@string_single'],
      [/\/\/.*$/, 'comment'],
    ],
    string: [
      [/[^\\"]+/, 'string'],
      [/"/, 'string', '@pop']
    ],
    string_single: [
      [/[^\\']+/, 'string'],
      [/'/, 'string', '@pop']
    ]
  }
};

// Monaco Editor Theme
const droyTheme = {
  base: 'vs-dark' as const,
  inherit: true,
  rules: [
    { token: 'keyword', foreground: '#ff6b35', fontStyle: 'bold' },
    { token: 'variable.special', foreground: '#79c0ff' },
    { token: 'string', foreground: '#7ee787' },
    { token: 'number', foreground: '#79c0ff' },
    { token: 'comment', foreground: '#8b949e', fontStyle: 'italic' },
    { token: 'operator', foreground: '#ff6b35' },
    { token: 'identifier', foreground: '#d2a8ff' },
    { token: 'delimiter', foreground: '#c9d1d9' },
  ],
  colors: {
    'editor.background': '#0d1117',
    'editor.foreground': '#c9d1d9',
    'editor.lineHighlightBackground': '#161b22',
    'editor.selectionBackground': '#264f78',
    'editor.inactiveSelectionBackground': '#264f7855',
  }
};

export default function DroyIDE() {
  const [files, setFiles] = useState<DroyFile[]>(defaultFiles);
  const [activeFileId, setActiveFileId] = useState<string>('1');
  const [output, setOutput] = useState<string[]>([]);
  const [terminalOutput, setTerminalOutput] = useState<string[]>([]);
  const [error, setError] = useState<string | null>(null);
  const [isRunning, setIsRunning] = useState(false);
  const [sidebarVisible, setSidebarVisible] = useState(true);
  const [terminalVisible, setTerminalVisible] = useState(true);
  const [activeTab, setActiveTab] = useState<'output' | 'terminal' | 'llvm'>('output');
  const [llvmOutput, setLlvmOutput] = useState<string>('');
  const [copied, setCopied] = useState(false);
  const [isDark, setIsDark] = useState(true);

  const editorRef = useRef<any>(null);
  const terminalEndRef = useRef<HTMLDivElement>(null);

  const activeFile = files.find(f => f.id === activeFileId) || files[0];

  // Monaco Editor Setup
  const handleEditorDidMount = (editor: any, monaco: any) => {
    editorRef.current = editor;
    
    // Register Droy Language
    monaco.languages.register({ id: 'droy' });
    monaco.languages.setMonarchTokensProvider('droy', droyLanguageDef);
    monaco.editor.defineTheme('droy-dark', droyTheme);
    monaco.editor.setTheme('droy-dark');

    // Add keyboard shortcuts
    editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.Enter, () => {
      runCode();
    });

    editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyS, () => {
      saveFile();
    });
  };

  // Auto-scroll terminal
  useEffect(() => {
    terminalEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  }, [terminalOutput, output]);

  // Run Droy Code
  const runCode = useCallback(() => {
    setIsRunning(true);
    setError(null);
    setOutput([]);

    const code = activeFile.content;
    
    try {
      const result = execute(code);
      setOutput(result.output);
      
      // Add to terminal
      setTerminalOutput(prev => [
        ...prev,
        `$ droy run ${activeFile.name}`,
        ...result.output,
        result.error ? `Error: ${result.error}` : '',
        ''
      ]);

      if (result.error) {
        setError(result.error);
      }
    } catch (err) {
      const errorMsg = err instanceof Error ? err.message : String(err);
      setError(errorMsg);
      setTerminalOutput(prev => [
        ...prev,
        `$ droy run ${activeFile.name}`,
        `Error: ${errorMsg}`,
        ''
      ]);
    } finally {
      setIsRunning(false);
    }
  }, [activeFile]);

  // Compile to LLVM (simulated)
  const compileToLLVM = useCallback(() => {
    setActiveTab('llvm');
    setLlvmOutput(`; LLVM IR generated from ${activeFile.name}
; ModuleID = '${activeFile.name}'
source_filename = "${activeFile.name}"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str.hello = private unnamed_addr constant [14 x i8] c"Hello World!\\0A\\00"
@.str.sum = private unnamed_addr constant [8 x i8] c"Sum: %d\\00"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %si = alloca i8*, align 8
  %ui = alloca i8*, align 8
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.hello, i64 0, i64 0), i8** %si
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.hello, i64 0, i64 6), i8** %ui
  
  store i32 10, i32* %a, align 4
  store i32 5, i32* %b, align 4
  
  %0 = load i32, i32* %a, align 4
  %1 = load i32, i32* %b, align 4
  %add = add nsw i32 %0, %1
  
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.sum, i64 0, i64 0), i32 %add)
  
  ret i32 0
}

declare i32 @printf(i8*, ...) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" }
attributes #1 = { "frame-pointer"="all" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"Droy Compiler v1.0.0"}`);
  }, [activeFile]);

  // Save file
  const saveFile = useCallback(() => {
    setFiles(prev => prev.map(f => 
      f.id === activeFileId ? { ...f, isModified: false } : f
    ));
    setTerminalOutput(prev => [
      ...prev,
      `$ save ${activeFile.name}`,
      `File saved successfully.`,
      ''
    ]);
  }, [activeFileId, activeFile]);

  // Update file content
  const updateFileContent = useCallback((content: string) => {
    setFiles(prev => prev.map(f => 
      f.id === activeFileId ? { ...f, content, isModified: true } : f
    ));
  }, [activeFileId]);

  // Create new file
  const createNewFile = useCallback(() => {
    const newId = String(Date.now());
    const newFile: DroyFile = {
      id: newId,
      name: `untitled${files.filter(f => f.name.startsWith('untitled')).length + 1}.droy`,
      content: '// New Droy file\n',
      isOpen: true,
      isModified: false,
    };
    setFiles(prev => [...prev, newFile]);
    setActiveFileId(newId);
  }, [files]);

  // Close file
  const closeFile = useCallback((fileId: string, e: React.MouseEvent) => {
    e.stopPropagation();
    setFiles(prev => prev.map(f => 
      f.id === fileId ? { ...f, isOpen: false } : f
    ));
    
    // Switch to another open file
    const remainingOpen = files.filter(f => f.id !== fileId && f.isOpen);
    if (remainingOpen.length > 0) {
      setActiveFileId(remainingOpen[0].id);
    }
  }, [files]);

  // Delete file
  const deleteFile = useCallback((fileId: string, e: React.MouseEvent) => {
    e.stopPropagation();
    setFiles(prev => prev.filter(f => f.id !== fileId));
    if (activeFileId === fileId) {
      const remaining = files.filter(f => f.id !== fileId);
      if (remaining.length > 0) {
        setActiveFileId(remaining[0].id);
      }
    }
  }, [files, activeFileId]);

  // Copy output
  const copyOutput = useCallback(() => {
    navigator.clipboard.writeText(output.join('\n'));
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  }, [output]);

  // Clear terminal
  const clearTerminal = useCallback(() => {
    setTerminalOutput([]);
  }, []);

  return (
    <div className="h-screen flex flex-col bg-[#0d1117] text-[#c9d1d9] font-mono text-sm overflow-hidden">
      {/* Top Bar */}
      <div className="h-12 bg-[#161b22] border-b border-[#30363d] flex items-center justify-between px-4">
        <div className="flex items-center gap-4">
          <div className="flex items-center gap-2">
            <img src="/logo.png" alt="Droy" className="w-6 h-6" />
            <span className="font-bold text-white">Droy IDE</span>
          </div>
          <Separator orientation="vertical" className="h-5 bg-[#30363d]" />
          <div className="flex items-center gap-1">
            <Button 
              variant="ghost" 
              size="sm" 
              onClick={() => setSidebarVisible(!sidebarVisible)}
              className={`h-8 px-2 ${sidebarVisible ? 'text-[#ff6b35]' : 'text-[#8b949e]'}`}
            >
              <Layout className="w-4 h-4" />
            </Button>
            <Button 
              variant="ghost" 
              size="sm" 
              onClick={() => setTerminalVisible(!terminalVisible)}
              className={`h-8 px-2 ${terminalVisible ? 'text-[#ff6b35]' : 'text-[#8b949e]'}`}
            >
              <Terminal className="w-4 h-4" />
            </Button>
          </div>
        </div>

        <div className="flex items-center gap-2">
          <Button
            variant="ghost"
            size="sm"
            onClick={runCode}
            disabled={isRunning}
            className="h-8 bg-[#238636] hover:bg-[#2ea043] text-white"
          >
            <Play className="w-4 h-4 mr-1" />
            Run
          </Button>
          <Button
            variant="ghost"
            size="sm"
            onClick={compileToLLVM}
            className="h-8 text-[#8b949e] hover:text-white hover:bg-[#30363d]"
          >
            <Cpu className="w-4 h-4 mr-1" />
            LLVM
          </Button>
          <Button
            variant="ghost"
            size="sm"
            onClick={saveFile}
            className="h-8 text-[#8b949e] hover:text-white hover:bg-[#30363d]"
          >
            <Save className="w-4 h-4 mr-1" />
            Save
          </Button>
          <Separator orientation="vertical" className="h-5 bg-[#30363d] mx-2" />
          <Button
            variant="ghost"
            size="sm"
            onClick={() => setIsDark(!isDark)}
            className="h-8 text-[#8b949e] hover:text-white"
          >
            {isDark ? <Sun className="w-4 h-4" /> : <Moon className="w-4 h-4" />}
          </Button>
        </div>
      </div>

      {/* Main Content */}
      <div className="flex-1 flex overflow-hidden">
        {/* Sidebar */}
        {sidebarVisible && (
          <div className="w-64 bg-[#0d1117] border-r border-[#30363d] flex flex-col">
            <div className="h-10 flex items-center justify-between px-3 border-b border-[#30363d]">
              <span className="text-xs font-semibold text-[#8b949e] uppercase tracking-wider">Explorer</span>
              <div className="flex items-center gap-1">
                <Button variant="ghost" size="sm" onClick={createNewFile} className="h-6 w-6 p-0 text-[#8b949e] hover:text-white">
                  <FilePlus className="w-4 h-4" />
                </Button>
              </div>
            </div>
            <ScrollArea className="flex-1">
              <div className="p-2">
                <div className="text-xs text-[#8b949e] mb-2 px-2">DROY-PROJECT</div>
                {files.map(file => (
                  <div
                    key={file.id}
                    onClick={() => {
                      setActiveFileId(file.id);
                      setFiles(prev => prev.map(f => 
                        f.id === file.id ? { ...f, isOpen: true } : f
                      ));
                    }}
                    className={`flex items-center justify-between px-2 py-1.5 rounded cursor-pointer group ${
                      activeFileId === file.id 
                        ? 'bg-[#1f6feb] text-white' 
                        : 'text-[#8b949e] hover:bg-[#21262d] hover:text-[#c9d1d9]'
                    }`}
                  >
                    <div className="flex items-center gap-2 overflow-hidden">
                      <FileCode className="w-4 h-4 flex-shrink-0" />
                      <span className="truncate text-sm">{file.name}</span>
                      {file.isModified && <span className="text-[#8b949e]">•</span>}
                    </div>
                    <div className="flex items-center gap-1 opacity-0 group-hover:opacity-100">
                      <Button 
                        variant="ghost" 
                        size="sm" 
                        onClick={(e) => deleteFile(file.id, e)}
                        className="h-5 w-5 p-0 hover:bg-red-500/20 hover:text-red-400"
                      >
                        <Trash2 className="w-3 h-3" />
                      </Button>
                    </div>
                  </div>
                ))}
              </div>
            </ScrollArea>
          </div>
        )}

        {/* Editor Area */}
        <div className="flex-1 flex flex-col min-w-0">
          {/* Tabs */}
          <div className="h-9 bg-[#0d1117] flex items-center overflow-x-auto">
            {files.filter(f => f.isOpen).map(file => (
              <div
                key={file.id}
                onClick={() => setActiveFileId(file.id)}
                className={`flex items-center gap-2 px-3 py-1.5 min-w-[120px] max-w-[200px] cursor-pointer border-r border-[#30363d] ${
                  activeFileId === file.id 
                    ? 'bg-[#0d1117] text-[#c9d1d9] border-t-2 border-t-[#ff6b35]' 
                    : 'bg-[#010409] text-[#8b949e] hover:bg-[#0d1117]'
                }`}
              >
                <FileCode className="w-3.5 h-3.5 flex-shrink-0" />
                <span className="truncate text-xs">{file.name}</span>
                {file.isModified && <span className="text-[#8b949e]">•</span>}
                <Button 
                  variant="ghost" 
                  size="sm" 
                  onClick={(e) => closeFile(file.id, e)}
                  className="h-4 w-4 p-0 ml-auto opacity-60 hover:opacity-100 hover:bg-[#30363d]"
                >
                  <X className="w-3 h-3" />
                </Button>
              </div>
            ))}
          </div>

          {/* Editor */}
          <div className="flex-1 relative">
            <Editor
              height="100%"
              language="droy"
              value={activeFile.content}
              onChange={(value) => updateFileContent(value || '')}
              onMount={handleEditorDidMount}
              options={{
                minimap: { enabled: true },
                fontSize: 14,
                fontFamily: 'JetBrains Mono, monospace',
                lineNumbers: 'on',
                roundedSelection: false,
                scrollBeyondLastLine: false,
                readOnly: false,
                automaticLayout: true,
                padding: { top: 16 },
                folding: true,
                bracketPairColorization: { enabled: true },
                guides: {
                  bracketPairs: true,
                  indentation: true,
                },
                wordWrap: 'on',
                tabSize: 4,
                insertSpaces: true,
                smoothScrolling: true,
                cursorBlinking: 'smooth',
                cursorSmoothCaretAnimation: 'on',
              }}
              theme="droy-dark"
            />
          </div>

          {/* Bottom Panel */}
          {terminalVisible && (
            <div className="h-48 bg-[#0d1117] border-t border-[#30363d] flex flex-col">
              <Tabs value={activeTab} onValueChange={(v) => setActiveTab(v as any)} className="flex-1 flex flex-col">
                <div className="flex items-center justify-between px-4 border-b border-[#30363d]">
                  <TabsList className="bg-transparent h-8">
                    <TabsTrigger value="output" className="text-xs data-[state=active]:text-[#ff6b35] data-[state=active]:border-b-2 data-[state=active]:border-[#ff6b35]">
                      <Zap className="w-3 h-3 mr-1" />
                      Output
                    </TabsTrigger>
                    <TabsTrigger value="terminal" className="text-xs data-[state=active]:text-[#ff6b35] data-[state=active]:border-b-2 data-[state=active]:border-[#ff6b35]">
                      <Terminal className="w-3 h-3 mr-1" />
                      Terminal
                    </TabsTrigger>
                    <TabsTrigger value="llvm" className="text-xs data-[state=active]:text-[#ff6b35] data-[state=active]:border-b-2 data-[state=active]:border-[#ff6b35]">
                      <Cpu className="w-3 h-3 mr-1" />
                      LLVM IR
                    </TabsTrigger>
                  </TabsList>
                  <div className="flex items-center gap-1">
                    {activeTab === 'output' && (
                      <Button variant="ghost" size="sm" onClick={copyOutput} className="h-6 text-[#8b949e] hover:text-white">
                        {copied ? <Check className="w-3 h-3" /> : <Copy className="w-3 h-3" />}
                      </Button>
                    )}
                    <Button variant="ghost" size="sm" onClick={clearTerminal} className="h-6 text-[#8b949e] hover:text-white">
                      <RotateCcw className="w-3 h-3" />
                    </Button>
                  </div>
                </div>

                <div className="flex-1 overflow-auto p-4 font-mono text-xs">
                  <TabsContent value="output" className="m-0 h-full">
                    {output.length === 0 && !error && (
                      <div className="text-[#484f58]">// Run your code to see output</div>
                    )}
                    {output.map((line, i) => (
                      <div key={i} className="text-[#7ee787]">{line}</div>
                    ))}
                    {error && (
                      <div className="mt-2 p-2 bg-[#ff5f56]/10 border border-[#ff5f56]/30 rounded text-[#ff5f56]">
                        {error}
                      </div>
                    )}
                  </TabsContent>

                  <TabsContent value="terminal" className="m-0 h-full">
                    {terminalOutput.length === 0 && (
                      <div className="text-[#484f58]">// Terminal ready</div>
                    )}
                    {terminalOutput.map((line, i) => (
                      <div key={i} className={line.startsWith('$') ? 'text-[#58a6ff]' : line.startsWith('Error') ? 'text-[#ff5f56]' : 'text-[#c9d1d9]'}>
                        {line}
                      </div>
                    ))}
                    <div ref={terminalEndRef} />
                  </TabsContent>

                  <TabsContent value="llvm" className="m-0 h-full">
                    {llvmOutput ? (
                      <pre className="text-[#d2a8ff] whitespace-pre-wrap">{llvmOutput}</pre>
                    ) : (
                      <div className="text-[#484f58]">// Click LLVM button to compile</div>
                    )}
                  </TabsContent>
                </div>
              </Tabs>
            </div>
          )}
        </div>
      </div>

      {/* Status Bar */}
      <div className="h-6 bg-[#161b22] border-t border-[#30363d] flex items-center justify-between px-3 text-[10px]">
        <div className="flex items-center gap-4">
          <div className="flex items-center gap-1">
            <GitBranch className="w-3 h-3" />
            <span>main</span>
          </div>
          <div className="flex items-center gap-1">
            <X className="w-3 h-3 text-[#8b949e]" />
            <span>0 errors</span>
          </div>
          <div className="flex items-center gap-1">
            <AlertIcon className="w-3 h-3 text-[#8b949e]" />
            <span>0 warnings</span>
          </div>
        </div>
        <div className="flex items-center gap-4">
          <span>Droy v1.0.0</span>
          <span>Ln {activeFile.content.split('\n').length}, Col 1</span>
          <span>UTF-8</span>
          <span>Droy</span>
          <div className="flex items-center gap-1">
            <Layout className="w-3 h-3" />
            <span>Prettier</span>
          </div>
        </div>
      </div>
    </div>
  );
}

function AlertIcon({ className }: { className?: string }) {
  return (
    <svg className={className} viewBox="0 0 16 16" fill="currentColor">
      <path fillRule="evenodd" d="M8.22 1.754a.25.25 0 00-.44 0L1.698 13.132a.25.25 0 00.22.368h12.164a.25.25 0 00.22-.368L8.22 1.754zm-1.763-.707c.659-1.234 2.427-1.234 3.086 0l6.082 11.378A1.75 1.75 0 0114.082 15H1.918a1.75 1.75 0 01-1.543-2.575L6.457 1.047zM9 11a1 1 0 11-2 0 1 1 0 012 0zm-.25-5.25a.75.75 0 00-1.5 0v2.5a.75.75 0 001.5 0v-2.5z" />
    </svg>
  );
}
