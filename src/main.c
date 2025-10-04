#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* buffer;
    size_t length;
    size_t input_length;
} InputBuffer;

InputBuffer input_buffer_init() {
    InputBuffer input_buffer;
    input_buffer.buffer = NULL;
    input_buffer.length = 0;
    input_buffer.input_length = 0;
    return input_buffer;
}

ssize_t read_input(InputBuffer* input_buffer) {
    const ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->input_length), stdin);
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = '\0';
    return bytes_read;
}

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_EXIT,
    META_COMMAND_UNKNOWN_COMMAND,
} MetaCommandResult;

MetaCommandResult parse_meta_command(const InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        return META_COMMAND_EXIT;
    }

    return META_COMMAND_UNKNOWN_COMMAND;
}

typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT,
} StatementType;

typedef struct {
    StatementType type;
} Statement;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
} PrepareResult;

PrepareResult prepare_statement(const InputBuffer* input_buffer, Statement* statement) {
    if (strcmp(input_buffer->buffer, "SELECT") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "INSERT", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(const Statement* statement) {
    switch (statement->type) {
        case STATEMENT_SELECT:
            printf("Should select something :/\n");
            break;
        case STATEMENT_INSERT:
            printf("Should insert something :/\n");
            break;
    }
}

int main(void) {
    InputBuffer input_buffer = input_buffer_init();

    while (true) {
        printf("cql> ");
        const ssize_t bytes_read = read_input(&input_buffer);
        assert(bytes_read > 0 && "error reading input");

        if (input_buffer.buffer[0] == '.') {
            switch (parse_meta_command(&input_buffer)) {
                case META_COMMAND_EXIT:
                    exit(EXIT_SUCCESS);
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNKNOWN_COMMAND:
                    printf("Unknown command: %s\n", input_buffer.buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(&input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized statement at start of: '%s'\n", input_buffer.buffer);
                continue;
        }

        execute_statement(&statement);
    }
}
