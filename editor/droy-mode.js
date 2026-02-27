// Droy Language Mode for CodeMirror
// ==================================

CodeMirror.defineMode("droy", function() {
    
    // Keywords
    var keywords = {
        "set": true, "~s": true,
        "ret": true, "~r": true,
        "em": true, "~e": true,
        "text": true, "txt": true, "t": true,
        "fe": true, "f": true, "for": true,
        "sty": true, "pkg": true, "media": true,
        "link": true, "a-link": true, "yoex--links": true,
        "link-go": true, "create-link": true, "open-link": true,
        "api": true, "id": true,
        "block": true, "key": true
    };
    
    // Commands
    var commands = {
        "*/employment": true,
        "*/Running": true,
        "*/pressure": true,
        "*/lock": true
    };
    
    // Special variables
    var specialVars = {
        "@si": true, "@ui": true, "@yui": true, "@pop": true, "@abc": true
    };
    
    // Operators
    var operators = /[+\-=*/]/;
    
    function tokenBase(stream, state) {
        // Whitespace
        if (stream.eatSpace()) {
            return null;
        }
        
        // Comments
        if (stream.match("//")) {
            stream.skipToEnd();
            return "comment";
        }
        
        if (stream.match("/*")) {
            state.inComment = true;
            return "comment";
        }
        
        if (state.inComment) {
            if (stream.match("*/")) {
                state.inComment = false;
            } else {
                stream.next();
            }
            return "comment";
        }
        
        // Strings
        if (stream.match('"')) {
            state.inString = true;
            return "string";
        }
        
        if (stream.match("'")) {
            state.inStringSingle = true;
            return "string";
        }
        
        if (state.inString) {
            if (stream.match('"')) {
                state.inString = false;
            } else {
                stream.next();
            }
            return "string";
        }
        
        if (state.inStringSingle) {
            if (stream.match("'")) {
                state.inStringSingle = false;
            } else {
                stream.next();
            }
            return "string";
        }
        
        // Commands (*/command)
        if (stream.match("*/")) {
            stream.eatWhile(/[a-zA-Z]/);
            var cmd = stream.current();
            if (commands[cmd]) {
                return "command";
            }
            return "error";
        }
        
        // Special variables (@si, @ui, etc.)
        if (stream.match("@")) {
            stream.eatWhile(/[a-zA-Z0-9_]/);
            var varName = stream.current();
            if (specialVars[varName]) {
                return "special-var";
            }
            return "variable";
        }
        
        // Shorthand operators (~s, ~r, ~e)
        if (stream.match("~")) {
            var c = stream.next();
            if (c === 's' || c === 'r' || c === 'e') {
                return "keyword";
            }
            return "error";
        }
        
        // Numbers
        if (stream.match(/^[0-9]+(\.[0-9]+)?/)) {
            return "number";
        }
        
        // Keywords and identifiers
        if (stream.match(/^[a-zA-Z][a-zA-Z0-9_-]*/)) {
            var word = stream.current();
            if (keywords[word]) {
                return "keyword";
            }
            if (word.indexOf("link") !== -1 || word === "api" || word === "id") {
                return "link-keyword";
            }
            return "variable";
        }
        
        // Operators
        if (stream.match(operators)) {
            return "operator";
        }
        
        // Delimiters
        if (stream.match(/[{}()\[\]:;,]/)) {
            return null;
        }
        
        // Default
        stream.next();
        return null;
    }
    
    return {
        startState: function() {
            return {
                inComment: false,
                inString: false,
                inStringSingle: false
            };
        },
        
        token: function(stream, state) {
            return tokenBase(stream, state);
        },
        
        lineComment: "//",
        blockCommentStart: "/*",
        blockCommentEnd: "*/",
        
        electricChars: "{}"
    };
});

// Define MIME type
CodeMirror.defineMIME("text/x-droy", "droy");

// Auto-completion for Droy
CodeMirror.registerHelper("hint", "droy", function(editor) {
    var cur = editor.getCursor();
    var token = editor.getTokenAt(cur);
    var word = token.string;
    
    var keywords = [
        "set", "~s", "ret", "~r", "em", "~e",
        "text", "txt", "t", "fe", "f", "for",
        "sty", "pkg", "media",
        "link", "a-link", "yoex--links",
        "link-go", "create-link", "open-link",
        "api", "id", "block", "key",
        "@si", "@ui", "@yui", "@pop", "@abc"
    ];
    
    var commands = [
        "*/employment", "*/Running", "*/pressure", "*/lock"
    ];
    
    var list = keywords.concat(commands).filter(function(kw) {
        return kw.indexOf(word) === 0;
    });
    
    return {
        list: list,
        from: CodeMirror.Pos(cur.line, token.start),
        to: CodeMirror.Pos(cur.line, token.end)
    };
});
