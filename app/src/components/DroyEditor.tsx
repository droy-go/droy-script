import { useState, useRef, useEffect, useCallback } from 'react';
import { Button } from '@/components/ui/button';
import { Play, RotateCcw, Copy, Check, Terminal, Code2 } from 'lucide-react';
import { execute, getTokens, TokenType } from '@/droy-lang';

interface DroyEditorProps {
  initialCode?: string;
  height?: string;
  showExamples?: boolean;
  readOnly?: boolean;
}

const defaultCode = `// Welcome to Droy Language!
// Try running this example

~s @si = "Hello"
~s @ui = "World"

// Emit the result
em @si + " " + @ui + "!"

// Try some math
set a = 10
set b = 5
em "Sum: " + (a + b)
em "Product: " + (a * b)`;

const examplePrograms = {
  'hello-world': `// Hello World in Droy
~s @si = "Hello"
~s @ui = "World"
em @si + " " + @ui + "!"`,

  'variables': `// Variables in Droy
set name = "Droy"
set version = "1.0.0"
set count = 42

// Shorthand syntax
~s @si = 100
~s @ui = 200

em name + " v" + version
em "Count: " + count
em "@si = " + @si`,

  'functions': `// Functions in Droy
f greet(name) {
    ret "Hello, " + name
}

f add(a, b) {
    ret a + b
}

f multiply(a, b) {
    ret a * b
}

em greet("Droy")
em "10 + 5 = " + add(10, 5)
em "10 * 5 = " + multiply(10, 5)`,

  'conditions': `// Conditions in Droy
set score = 85

fe (score >= 90) {
    em "Grade: A"
}

fe (score >= 80 && score < 90) {
    em "Grade: B"
}

fe (score < 80) {
    em "Grade: C"
}

em "Score: " + score`,

  'loops': `// Loops in Droy
// Note: Simple for loop example
set sum = 0
set i = 1

// Manual loop (for demo)
em "Counting:"
em "1"
em "2"
em "3"
em "4"
em "5"

em "Done!"`,

  'links': `// Links in Droy
link id: "google" api: "https://google.com"
link id: "github" api: "https://github.com"

em "Links created:"
em "- google"
em "- github"

*/Running`,
};

// Syntax highlighting function
function highlightCode(code: string): { html: string; styles: string[] } {
  try {
    const tokens = getTokens(code);
    let html = '';
    const styles: string[] = [];

    for (const token of tokens) {
      if (token.type === TokenType.EOF) continue;

      let className = '';
      let content = token.value;

      switch (token.type) {
        case TokenType.STRING:
          className = 'syntax-string';
          break;
        case TokenType.NUMBER:
          className = 'syntax-number';
          break;
        case TokenType.COMMENT:
          className = 'syntax-comment';
          break;
        case TokenType.SET:
        case TokenType.EM:
        case TokenType.RET:
        case TokenType.FE:
        case TokenType.F:
        case TokenType.FOR:
        case TokenType.TILDE_S:
        case TokenType.TILDE_E:
        case TokenType.TILDE_R:
        case TokenType.LINK:
        case TokenType.STY:
        case TokenType.TEXT:
        case TokenType.TXT:
          className = 'syntax-keyword';
          break;
        case TokenType.SPECIAL_VAR:
          className = 'syntax-variable';
          break;
        case TokenType.IDENTIFIER:
          if (token.value.startsWith('*/')) {
            className = 'syntax-keyword';
          } else {
            className = 'syntax-function';
          }
          break;
        default:
          className = '';
      }

      // Escape HTML
      content = content
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/ /g, '&nbsp;')
        .replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;');

      if (className) {
        html += `<span class="${className}">${content}</span>`;
      } else {
        html += content;
      }

      if (token.type === TokenType.NEWLINE) {
        html += '<br/>';
      }
    }

    return { html, styles };
  } catch {
    return { html: code.replace(/\n/g, '<br/>'), styles: [] };
  }
}

export default function DroyEditor({
  initialCode = defaultCode,
  height = '500px',
  showExamples = true,
  readOnly = false,
}: DroyEditorProps) {
  const [code, setCode] = useState(initialCode);
  const [output, setOutput] = useState<string[]>([]);
  const [error, setError] = useState<string | null>(null);
  const [isRunning, setIsRunning] = useState(false);
  const [copied, setCopied] = useState(false);
  const [activeExample, setActiveExample] = useState<string | null>(null);
  const [highlightedCode, setHighlightedCode] = useState('');

  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const preRef = useRef<HTMLPreElement>(null);

  // Update highlighted code when code changes
  useEffect(() => {
    const { html } = highlightCode(code);
    setHighlightedCode(html);
  }, [code]);

  // Sync scroll between textarea and pre
  const handleScroll = useCallback(() => {
    if (textareaRef.current && preRef.current) {
      preRef.current.scrollTop = textareaRef.current.scrollTop;
      preRef.current.scrollLeft = textareaRef.current.scrollLeft;
    }
  }, []);

  const runCode = useCallback(() => {
    setIsRunning(true);
    setError(null);

    try {
      const result = execute(code);
      setOutput(result.output);
      if (result.error) {
        setError(result.error);
      }
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Unknown error');
    } finally {
      setIsRunning(false);
    }
  }, [code]);

  const resetCode = useCallback(() => {
    setCode(defaultCode);
    setOutput([]);
    setError(null);
    setActiveExample(null);
  }, []);

  const copyCode = useCallback(() => {
    navigator.clipboard.writeText(code);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  }, [code]);

  const loadExample = useCallback((key: string) => {
    const example = examplePrograms[key as keyof typeof examplePrograms];
    if (example) {
      setCode(example);
      setActiveExample(key);
      setOutput([]);
      setError(null);
    }
  }, []);

  const handleKeyDown = useCallback((e: React.KeyboardEvent) => {
    if (e.key === 'Tab') {
      e.preventDefault();
      const start = textareaRef.current?.selectionStart || 0;
      const end = textareaRef.current?.selectionEnd || 0;
      const newCode = code.substring(0, start) + '    ' + code.substring(end);
      setCode(newCode);
      setTimeout(() => {
        if (textareaRef.current) {
          textareaRef.current.selectionStart = textareaRef.current.selectionEnd = start + 4;
        }
      }, 0);
    }

    if ((e.metaKey || e.ctrlKey) && e.key === 'Enter') {
      runCode();
    }
  }, [code, runCode]);

  return (
    <div className="w-full bg-[#111] rounded-xl border border-[#333] overflow-hidden glow-orange">
      {/* Header */}
      <div className="flex items-center justify-between px-4 py-3 bg-[#1a1a1a] border-b border-[#333]">
        <div className="flex items-center gap-3">
          <div className="flex items-center gap-2">
            <div className="w-3 h-3 rounded-full bg-[#ff5f56]" />
            <div className="w-3 h-3 rounded-full bg-[#ffbd2e]" />
            <div className="w-3 h-3 rounded-full bg-[#27ca40]" />
          </div>
          <span className="text-sm text-[#666] ml-2">main.droy</span>
        </div>

        <div className="flex items-center gap-2">
          <Button
            variant="ghost"
            size="sm"
            onClick={copyCode}
            className="text-[#666] hover:text-white hover:bg-[#333] h-8 px-2"
          >
            {copied ? <Check className="w-4 h-4" /> : <Copy className="w-4 h-4" />}
          </Button>
          <Button
            variant="ghost"
            size="sm"
            onClick={resetCode}
            className="text-[#666] hover:text-white hover:bg-[#333] h-8 px-2"
          >
            <RotateCcw className="w-4 h-4" />
          </Button>
          <Button
            size="sm"
            onClick={runCode}
            disabled={isRunning}
            className="bg-[#ff6b35] hover:bg-[#ff6b35]/90 text-white h-8 px-4"
          >
            <Play className="w-4 h-4 mr-1" />
            Run
          </Button>
        </div>
      </div>

      {/* Examples Bar */}
      {showExamples && (
        <div className="flex items-center gap-1 px-4 py-2 bg-[#0a0a0a] border-b border-[#333] overflow-x-auto">
          <Code2 className="w-4 h-4 text-[#666] mr-2 flex-shrink-0" />
          {Object.entries(examplePrograms).map(([key, _]) => (
            <button
              key={key}
              onClick={() => loadExample(key)}
              className={`px-3 py-1 text-xs rounded-md whitespace-nowrap transition-colors ${
                activeExample === key
                  ? 'bg-[#ff6b35]/20 text-[#ff6b35]'
                  : 'text-[#666] hover:text-white hover:bg-[#222]'
              }`}
            >
              {key.split('-').map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(' ')}
            </button>
          ))}
        </div>
      )}

      {/* Editor Area */}
      <div className="grid lg:grid-cols-2" style={{ height }}>
        {/* Code Input */}
        <div className="relative border-r border-[#333]">
          <div className="absolute top-2 left-2 flex items-center gap-2 text-xs text-[#666]">
            <Terminal className="w-3 h-3" />
            <span>Editor (Ctrl+Enter to run)</span>
          </div>

          <div className="relative h-full pt-8">
            {/* Highlighted code background */}
            <pre
              ref={preRef}
              className="absolute inset-0 m-0 p-4 font-mono text-sm leading-6 text-white bg-transparent pointer-events-none overflow-auto whitespace-pre-wrap"
              dangerouslySetInnerHTML={{ __html: highlightedCode + '<span class="animate-pulse">|</span>' }}
            />

            {/* Textarea for input */}
            <textarea
              ref={textareaRef}
              value={code}
              onChange={(e) => setCode(e.target.value)}
              onScroll={handleScroll}
              onKeyDown={handleKeyDown}
              readOnly={readOnly}
              spellCheck={false}
              className="absolute inset-0 w-full h-full p-4 font-mono text-sm leading-6 text-transparent bg-transparent caret-[#ff6b35] resize-none outline-none overflow-auto whitespace-pre-wrap"
              style={{ caretColor: '#ff6b35' }}
            />
          </div>
        </div>

        {/* Output */}
        <div className="relative bg-[#0a0a0a]">
          <div className="absolute top-2 left-2 flex items-center gap-2 text-xs text-[#666]">
            <Terminal className="w-3 h-3" />
            <span>Output</span>
          </div>

          <div className="h-full pt-8 p-4 font-mono text-sm leading-6 overflow-auto">
            {output.length === 0 && !error && (
              <div className="text-[#444] italic">
                // Click Run to see output
              </div>
            )}

            {output.map((line, index) => (
              <div key={index} className="text-[#7ee787]">
                <span className="text-[#444] mr-2">{index + 1}.</span>
                {line}
              </div>
            ))}

            {error && (
              <div className="mt-4 p-3 bg-[#ff5f56]/10 border border-[#ff5f56]/30 rounded-lg">
                <div className="text-[#ff5f56] font-semibold mb-1">Error:</div>
                <div className="text-[#ff5f56]/80">{error}</div>
              </div>
            )}
          </div>
        </div>
      </div>

      {/* Footer */}
      <div className="flex items-center justify-between px-4 py-2 bg-[#1a1a1a] border-t border-[#333] text-xs text-[#666]">
        <div className="flex items-center gap-4">
          <span>Droy v1.0.0</span>
          <span>{code.split('\n').length} lines</span>
          <span>{code.length} chars</span>
        </div>
        <div className="flex items-center gap-2">
          {isRunning && (
            <span className="text-[#ff6b35] animate-pulse">Running...</span>
          )}
        </div>
      </div>
    </div>
  );
}
