// C/C++ Mode for CodeMirror
// =========================

CodeMirror.defineMode("c_droy", function(config, parserConfig) {
    const indentUnit = config.indentUnit;
    const statementIndentUnit = parserConfig.statementIndentUnit || indentUnit;
    const dontAlignCalls = parserConfig.dontAlignCalls;
    const keywords = parserConfig.keywords || {};
    const types = parserConfig.types || {};
    const blockKeywords = parserConfig.blockKeywords || {};
    const defKeywords = parserConfig.defKeywords || {};
    const typeFirstDefinitions = parserConfig.typeFirstDefinitions || {};
    const atoms = parserConfig.atoms || {};
    const hooks = parserConfig.hooks || {};
    const multiLineStrings = parserConfig.multiLineStrings;
    const indentStatements = parserConfig.indentStatements !== false;
    const indentSwitch = parserConfig.indentSwitch !== false;
    const namespaceSeparator = parserConfig.namespaceSeparator;
    const isPunctuationChar = parserConfig.isPunctuationChar || /[\[\]{}\(\),;\:\.]/;
    const numberStart = parserConfig.numberStart || /[\d\.]/;
    const number = parserConfig.number || /^(?:0x[a-f\d]+|(?:\d+(?:\.\d*)?|\.\d+)(?:e[-+]?\d+)?)(u|ll?|l|f)?/i;
    const isOperatorChar = parserConfig.isOperatorChar || /[+\-*&%=<>!?|\/]/;
    const isIdentifierChar = parserConfig.isIdentifierChar || /[\w\$_\xa1-\uffff]/;
    const isReservedIdentifier = parserConfig.isReservedIdentifier || false;

    let curPunc;
    let isDefKeyword;

    function tokenBase(stream, state) {
        const ch = stream.next();
        
        if (hooks[ch]) {
            const result = hooks[ch](stream, state);
            if (result !== false) return result;
        }
        
        if (ch == '"' || ch == "'") {
            state.tokenize = tokenString(ch);
            return state.tokenize(stream, state);
        }
        
        if (isPunctuationChar.test(ch)) {
            curPunc = ch;
            return null;
        }
        
        if (numberStart.test(ch)) {
            stream.backUp(1);
            if (stream.match(number)) return "number";
            stream.next();
        }
        
        if (ch == "/") {
            if (stream.eat("*")) {
                state.tokenize = tokenComment;
                return tokenComment(stream, state);
            }
            if (stream.eat("/")) {
                stream.skipToEnd();
                return "comment";
            }
        }
        
        if (isOperatorChar.test(ch)) {
            while (!stream.match(/^\/[*\/]/, false) && stream.eat(isOperatorChar)) {}
            return "operator";
        }
        
        stream.eatWhile(isIdentifierChar);
        
        if (namespaceSeparator) {
            while (ch == namespaceSeparator) {
                stream.eatWhile(isIdentifierChar);
            }
        }
        
        const cur = stream.current();
        
        if (contains(types, cur)) {
            if (contains(typeFirstDefinitions, cur)) isDefKeyword = true;
            return "type";
        }
        
        if (contains(keywords, cur)) {
            if (contains(blockKeywords, cur)) curPunc = "newstatement";
            else if (contains(defKeywords, cur)) isDefKeyword = true;
            return "keyword";
        }
        
        if (contains(atoms, cur)) return "atom";
        
        if (isReservedIdentifier && isReservedIdentifier(cur)) return "keyword";
        
        return "variable";
    }

    function tokenString(quote) {
        return function(stream, state) {
            let escaped = false, next, end = false;
            while ((next = stream.next()) != null) {
                if (next == quote && !escaped) { end = true; break; }
                escaped = !escaped && next == "\\";
            }
            if (end || !(escaped || multiLineStrings))
                state.tokenize = null;
            return "string";
        };
    }

    function tokenComment(stream, state) {
        let maybeEnd = false, ch;
        while (ch = stream.next()) {
            if (ch == "/" && maybeEnd) {
                state.tokenize = null;
                break;
            }
            maybeEnd = (ch == "*");
        }
        return "comment";
    }

    function maybeEOL(stream, state) {
        if (parserConfig.typeFirstDefinitions && stream.eol() && isTopScope(state.context))
            state.typeAtEndOfLine = typeBefore(stream, state, stream.pos);
    }

    // Interface
    return {
        startState: function(basecolumn) {
            return {
                tokenize: null,
                context: new Context((basecolumn || 0) - indentUnit, 0, "top", null, false),
                indented: 0,
                startOfLine: true,
                prevToken: null
            };
        },

        token: function(stream, state) {
            const ctx = state.context;
            if (stream.sol()) {
                if (ctx.align == null) ctx.align = false;
                state.indented = stream.indentation();
                state.startOfLine = true;
            }
            if (stream.eatSpace()) { maybeEOL(stream, state); return null; }
            curPunc = isDefKeyword = null;
            const style = (state.tokenize || tokenBase)(stream, state);
            if (style == "comment" || style == "meta") return style;
            if (ctx.align == null) ctx.align = true;

            if (curPunc == ";" || curPunc == ":" || (curPunc == "," && stream.match(/^\s*(?:\/\/.*)?$/, false)))
                while (state.context.type == "statement") popContext(state);
            else if (curPunc == "{") pushContext(state, stream.column(), "}");
            else if (curPunc == "[") pushContext(state, stream.column(), "]");
            else if (curPunc == "(") pushContext(state, stream.column(), ")");
            else if (curPunc == "}") {
                while (ctx.type == "statement") ctx = popContext(state);
                if (ctx.type == "}") ctx = popContext(state);
                while (ctx.type == "statement") ctx = popContext(state);
            }
            else if (curPunc == ctx.type) popContext(state);
            else if (indentStatements && (((ctx.type == "}" || ctx.type == "top") && curPunc != ";") ||
                                          (ctx.type == "statement" && curPunc == "newstatement")))
                pushContext(state, stream.column(), "statement");
            
            state.startOfLine = false;
            state.prevToken = isDefKeyword ? "def" : style || curPunc;
            maybeEOL(stream, state);
            return style;
        },

        indent: function(state, textAfter) {
            if (state.tokenize != tokenBase && state.tokenize != null || state.typeAtEndOfLine) return CodeMirror.Pass;
            const ctx = state.context, firstChar = textAfter && textAfter.charAt(0);
            if (ctx.type == "statement" && firstChar == "}") ctx = ctx.prev;
            if (parserConfig.dontIndentStatements)
                while (ctx.type == "statement" && parserConfig.dontIndentStatements.test(ctx.info))
                    ctx = ctx.prev;
            if (hooks.indent) {
                const hook = hooks.indent(state, ctx, textAfter, indentUnit);
                if (typeof hook == "number") return hook + (closing ? 0 : indentUnit);
                if (hook) return hook;
            }
            const closing = firstChar == ctx.type;
            let switchBlock = ctx.prev && ctx.prev.info == "switch";
            if (parserConfig.allmanIndentation && /[{(]/.test(firstChar)) {
                while (ctx.type != "top" && ctx.type != "}") ctx = ctx.prev;
                return ctx.indented;
            }
            if (ctx.type == "statement")
                return ctx.indented + (firstChar == "{" ? 0 : statementIndentUnit);
            if (ctx.align && (!dontAlignCalls || ctx.type != ")"))
                return ctx.column + (closing ? 0 : 1);
            if (ctx.type == ")" && !closing)
                return ctx.indented + statementIndentUnit;
            
            return ctx.indented + (closing ? 0 : indentUnit) +
                (!closing && switchBlock && !indentSwitch ? indentUnit : 0);
        },

        electricInput: indentSwitch ? /\s*(?:case\s+.+:|default:|\{|\})$/ : /\s*[{}]$/,
        blockCommentStart: "/*",
        blockCommentEnd: "*/",
        blockCommentContinue: " * ",
        lineComment: "//",
        fold: "brace"
    };
});

function Context(indented, column, type, info, align, prev) {
    this.indented = indented;
    this.column = column;
    this.type = type;
    this.info = info;
    this.align = align;
    this.prev = prev;
}

function pushContext(state, col, type, info) {
    const indent = state.indented;
    if (state.context && state.context.type == "statement" && type != "statement")
        indent = state.context.indented;
    return state.context = new Context(indent, col, type, info, null, state.context);
}

function popContext(state) {
    const t = state.context.type;
    if (t == ")" || t == "]" || t == "}")
        state.indented = state.context.indented;
    return state.context = state.context.prev;
}

function isTopScope(ctx) {
    for (;;) {
        if (ctx.type == "top" || ctx.type == "namespace") return true;
        if (ctx.type == "{" && ctx.prev.info == "namespace") return true;
        ctx = ctx.prev;
        if (!ctx) return false;
    }
}

function typeBefore(stream, state, pos) {
    return false;
}

function contains(obj, item) {
    if (Array.isArray(obj)) {
        return obj.indexOf(item) !== -1;
    } else if (typeof obj === 'object') {
        return obj.hasOwnProperty(item);
    }
    return false;
}

// C Keywords
const cKeywords = "auto break case char const continue default do double else enum extern float for goto if inline int long register restrict return short signed sizeof static struct switch typedef union unsigned void volatile while _Alignas _Alignof _Atomic _Bool _Complex _Generic _Imaginary _Noreturn _Static_assert _Thread_local";

const cTypes = "int long char short double float unsigned signed void bool size_t int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t ptrdiff_t intptr_t uintptr_t";

const cBlockKeywords = "case do else for if switch while struct enum union";

const cDefKeywords = "struct enum union typedef";

const cAtoms = "NULL true false TRUE FALSE";

CodeMirror.defineMIME("text/x-csrc", {
    name: "c_droy",
    keywords: wordsToSet(cKeywords),
    types: wordsToSet(cTypes),
    blockKeywords: wordsToSet(cBlockKeywords),
    defKeywords: wordsToSet(cDefKeywords),
    atoms: wordsToSet(cAtoms),
    modeProps: { fold: ["brace", "include"] }
});

CodeMirror.defineMIME("text/x-c++src", {
    name: "c_droy",
    keywords: wordsToSet(cKeywords + " alignas alignof and and_eq asm bitand bitor catch class compl concept consteval constexpr constinit const_cast co_await co_return co_yield decltype delete dynamic_cast explicit export false friend inline mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected public reflexpr reinterpret_cast requires static_cast template this thread_local throw true try typeid typename using virtual wchar_t xor xor_eq"),
    types: wordsToSet(cTypes + " bool string vector map set unordered_map unordered_set array list deque queue stack pair tuple unique_ptr shared_ptr weak_ptr optional variant any"),
    blockKeywords: wordsToSet(cBlockKeywords + " class namespace try catch"),
    defKeywords: wordsToSet(cDefKeywords + " class namespace template concept"),
    atoms: wordsToSet(cAtoms + " nullptr"),
    modeProps: { fold: ["brace", "include"] }
});

CodeMirror.defineMIME("text/x-chdr", "text/x-csrc");
CodeMirror.defineMIME("text/x-c++hdr", "text/x-c++src");

function wordsToSet(str) {
    const set = {};
    str.split(" ").forEach(word => {
        if (word) set[word] = true;
    });
    return set;
}

// C/C++ Autocompletion
CodeMirror.registerHelper("hint", "c_droy", function(editor) {
    const cur = editor.getCursor();
    const token = editor.getTokenAt(cur);
    const to = CodeMirror.Pos(cur.line, token.end);
    
    if (token.string.match(/^[^\w\$_]$/)) {
        return { list: [], from: cur, to: cur };
    }
    
    const word = token.string;
    const keywords = Object.keys(wordsToSet(cKeywords + " " + cTypes));
    
    const list = keywords.filter(kw => kw.startsWith(word) && kw !== word);
    
    if (list.length === 0) return;
    
    return {
        list: list,
        from: CodeMirror.Pos(cur.line, token.start),
        to: to
    };
});
