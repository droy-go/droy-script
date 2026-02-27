/**
 * Droy Language - Main Entry Point
 * ================================
 * Command-line interface for the Droy language
 */

#include "../include/droy.h"
#include <getopt.h>

static void print_banner(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                                                          ║\n");
    printf("║     ██████╗ ██████╗  ██████╗ ██╗   ██╗                  ║\n");
    printf("║     ██╔══██╗██╔══██╗██╔═══██╗╚██╗ ██╔╝                  ║\n");
    printf("║     ██║  ██║██████╔╝██║   ██║ ╚████╔╝                   ║\n");
    printf("║     ██║  ██║██╔══██╗██║   ██║  ╚██╔╝                    ║\n");
    printf("║     ██████╔╝██║  ██║╚██████╔╝   ██║                     ║\n");
    printf("║     ╚═════╝ ╚═╝  ╚═╝ ╚═════╝    ╚═╝                     ║\n");
    printf("║                                                          ║\n");
    printf("║          Programming Language v%s                     ║\n", DROY_VERSION);
    printf("║                                                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

static void print_usage(const char* program) {
    printf("Usage: %s [OPTIONS] <file.droy>\n", program);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help          Show this help message\n");
    printf("  -v, --version       Show version information\n");
    printf("  -t, --tokens        Print tokens (lexical analysis)\n");
    printf("  -a, --ast           Print AST (parsing)\n");
    printf("  -r, --run           Run the interpreter (default)\n");
    printf("  -c, --compile       Compile to LLVM IR\n");
    printf("  -o, --output FILE   Output file for compilation\n");
    printf("  -i, --interactive   Interactive REPL mode\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s program.droy              Run a Droy program\n", program);
    printf("  %s -t program.droy           Show tokens\n", program);
    printf("  %s -a program.droy           Show AST\n", program);
    printf("  %s -c -o out.ll program.droy Compile to LLVM IR\n", program);
    printf("  %s -i                        Start REPL\n", program);
}

static void print_tokens(Token* tokens) {
    printf("\n========== TOKENS ==========\n\n");
    
    Token* current = tokens;
    int count = 0;
    
    while (current && current->type != TOKEN_EOF) {
        if (current->type != TOKEN_WHITESPACE && current->type != TOKEN_NEWLINE) {
            printf("[%3d] %-20s | %-15s | L%d:C%d\n", 
                   count++,
                   token_type_to_string(current->type),
                   current->value,
                   current->line,
                   current->column);
        }
        current = current->next;
    }
    
    printf("\nTotal tokens: %d\n", count);
}

static void run_repl(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║              Droy Interactive REPL                       ║\n");
    printf("║         Type 'exit' or press Ctrl+D to quit              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    DroyState* state = state_create();
    char input[4096];
    
    while (1) {
        printf("droy> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        // Remove trailing newline
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }
        
        // Check for exit
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            break;
        }
        
        // Skip empty lines
        if (strlen(input) == 0) {
            continue;
        }
        
        // Add newline for proper parsing
        strcat(input, "\n");
        
        // Tokenize
        Lexer* lexer = lexer_create(input);
        Token* tokens = lexer_tokenize(lexer);
        
        // Parse
        Parser* parser = parser_create(tokens);
        ASTNode* ast = parser_parse(parser);
        
        // Execute
        interpret(state, ast);
        
        // Cleanup
        ast_free(ast);
        parser_destroy(parser);
        token_free(tokens);
        lexer_destroy(lexer);
        
        printf("\n");
    }
    
    printf("\nGoodbye!\n");
    state_destroy(state);
}

static int compile_to_llvm(const char* input_file, const char* output_file) {
    printf("Compiling %s to LLVM IR...\n", input_file);
    
    char* source = read_file(input_file);
    if (!source) {
        return 1;
    }
    
    // Tokenize
    Lexer* lexer = lexer_create(source);
    Token* tokens = lexer_tokenize(lexer);
    
    // Parse
    Parser* parser = parser_create(tokens);
    ASTNode* ast = parser_parse(parser);
    
    // Generate LLVM IR (placeholder - would need actual LLVM C API)
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Could not create output file '%s'\n", output_file);
        free(source);
        return 1;
    }
    
    fprintf(out, "; Droy Language Compiled Output\n");
    fprintf(out, "; Source: %s\n", input_file);
    fprintf(out, "; Generated by Droy Compiler v%s\n\n", DROY_VERSION);
    
    fprintf(out, "; ModuleID = '%s'\n", input_file);
    fprintf(out, "source_filename = \"%s\"\n\n", input_file);
    
    fprintf(out, "; External functions\n");
    fprintf(out, "declare i32 @printf(i8*, ...)\n");
    fprintf(out, "declare i8* @malloc(i64)\n");
    fprintf(out, "declare void @free(i8*)\n\n");
    
    fprintf(out, "; Main function placeholder\n");
    fprintf(out, "define i32 @main() {\n");
    fprintf(out, "entry:\n");
    fprintf(out, "  ; Program would be compiled here\n");
    fprintf(out, "  ret i32 0\n");
    fprintf(out, "}\n");
    
    fclose(out);
    
    printf("Successfully compiled to: %s\n", output_file);
    
    // Cleanup
    ast_free(ast);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
    free(source);
    
    return 0;
}

int main(int argc, char* argv[]) {
    int show_tokens = 0;
    int show_ast = 0;
    int compile_mode = 0;
    int interactive_mode = 0;
    char* output_file = NULL;
    char* input_file = NULL;
    
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"tokens", no_argument, 0, 't'},
        {"ast", no_argument, 0, 'a'},
        {"run", no_argument, 0, 'r'},
        {"compile", no_argument, 0, 'c'},
        {"output", required_argument, 0, 'o'},
        {"interactive", no_argument, 0, 'i'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "hvtarco:i", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_banner();
                print_usage(argv[0]);
                return 0;
                
            case 'v':
                printf("%s version %s\n", DROY_NAME, DROY_VERSION);
                return 0;
                
            case 't':
                show_tokens = 1;
                break;
                
            case 'a':
                show_ast = 1;
                break;
                
            case 'r':
                // Default mode
                break;
                
            case 'c':
                compile_mode = 1;
                break;
                
            case 'o':
                output_file = optarg;
                break;
                
            case 'i':
                interactive_mode = 1;
                break;
                
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Interactive mode
    if (interactive_mode) {
        print_banner();
        run_repl();
        return 0;
    }
    
    // Get input file
    if (optind < argc) {
        input_file = argv[optind];
    }
    
    if (!input_file) {
        print_banner();
        printf("Error: No input file specified\n\n");
        print_usage(argv[0]);
        return 1;
    }
    
    // Compile mode
    if (compile_mode) {
        if (!output_file) {
            output_file = "output.ll";
        }
        return compile_to_llvm(input_file, output_file);
    }
    
    // Normal execution mode
    print_banner();
    
    char* source = read_file(input_file);
    if (!source) {
        return 1;
    }
    
    printf("Loading: %s\n\n", input_file);
    
    // Tokenize
    Lexer* lexer = lexer_create(source);
    Token* tokens = lexer_tokenize(lexer);
    
    if (show_tokens) {
        print_tokens(tokens);
        printf("\n");
    }
    
    // Parse
    Parser* parser = parser_create(tokens);
    ASTNode* ast = parser_parse(parser);
    
    if (show_ast) {
        printf("\n========== AST ==========\n\n");
        ast_print(ast, 0);
        printf("\n");
    }
    
    // Execute
    DroyState* state = state_create();
    int result = interpret(state, ast);
    
    // Cleanup
    state_destroy(state);
    ast_free(ast);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
    free(source);
    
    return result;
}
