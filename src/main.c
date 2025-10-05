#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
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

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT,
} StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT,
} PrepareResult;

PrepareResult prepare_statement(const InputBuffer* input_buffer, Statement* statement) {
    if (strcmp(input_buffer->buffer, "SELECT") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "INSERT", 6) == 0) {
        statement->type = STATEMENT_INSERT;

        const int args = sscanf(
            input_buffer->buffer,
            "INSERT %d %s %s",
            &(statement->row_to_insert.id),
            (char*)(&(statement->row_to_insert.username)),
            (char*)(&(statement->row_to_insert.email))
        );

        if (args < 3) {
            return PREPARE_SYNTAX_ERROR;
        }

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

#define SIZE_OF_ATTRIBUTE(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const uint32_t ID_SIZE = SIZE_OF_ATTRIBUTE(Row, id);
const uint32_t USERNAME_SIZE = SIZE_OF_ATTRIBUTE(Row, username);
const uint32_t EMAIL_SIZE = SIZE_OF_ATTRIBUTE(Row, email);

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void row_serialize(const Row* row, void* dest) {
    memcpy(dest + ID_OFFSET, &(row->id), ID_SIZE);
    memcpy(dest + USERNAME_OFFSET, &(row->username), USERNAME_SIZE);
    memcpy(dest + EMAIL_OFFSET, &(row->email), EMAIL_SIZE);
}

void row_deserialize(const void* source, Row* row) {
    memcpy(&(row->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(row->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(row->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

#define TABLE_MAX_PAGES 100
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct {
    size_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

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
            case PREPARE_SYNTAX_ERROR:
                continue;
        }

        execute_statement(&statement);
    }
}
