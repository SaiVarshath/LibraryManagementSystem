#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 50
#define MAX_AUTHOR 50
#define DATA_FILE "library.dat"

typedef struct {
    int id;
    char title[MAX_TITLE];
    char author[MAX_AUTHOR];
    int available; // 1 for available, 0 for borrowed
} Book;

void addBook();
void removeBook();
void searchBook();
void displayBooks();
void borrowBook();
void returnBook();
void loadBooks(Book **books, int *size);
void saveBooks(Book *books, int size);
int getMenuChoice();
void clearInputBuffer();

int main() {
    int choice;
    printf("\n=== Library Management System ===\n");

    do {
        choice = getMenuChoice();

        switch(choice) {
            case 1: addBook(); break;
            case 2: removeBook(); break;
            case 3: searchBook(); break;
            case 4: displayBooks(); break;
            case 5: borrowBook(); break;
            case 6: returnBook(); break;
            case 7: printf("Exiting the program. Goodbye!\n"); break;
            default: printf("Invalid choice. Please select between 1-7.\n");
        }
    } while(choice != 7);

    return 0;
}

int getMenuChoice() {
    int choice;
    printf("\nMenu:\n");
    printf("1. Add Book\n");
    printf("2. Remove Book\n");
    printf("3. Search Book\n");
    printf("4. Display All Books\n");
    printf("5. Borrow Book\n");
    printf("6. Return Book\n");
    printf("7. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    clearInputBuffer();
    return choice;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void loadBooks(Book **books, int *size) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) {
        *books = NULL;
        *size = 0;
        return;
    }
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);
    *size = fileSize / sizeof(Book);
    *books = (Book*)malloc((*size) * sizeof(Book));
    if (*books == NULL) {
        printf("Memory allocation failed.\n");
        fclose(fp);
        *size = 0;
        return;
    }
    fread(*books, sizeof(Book), *size, fp);
    fclose(fp);
}

void saveBooks(Book *books, int size) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) {
        printf("Failed to open file for writing.\n");
        return;
    }
    fwrite(books, sizeof(Book), size, fp);
    fclose(fp);
}

void addBook() {
    Book *books = NULL;
    int size = 0;

    loadBooks(&books, &size);

    Book newBook;
    printf("Enter book ID (integer): ");
    scanf("%d", &newBook.id);
    clearInputBuffer();

    // Check for duplicate ID
    for (int i = 0; i < size; i++) {
        if (books[i].id == newBook.id) {
            printf("A book with this ID already exists. Please use a unique ID.\n");
            free(books);
            return;
        }
    }

    printf("Enter book title: ");
    fgets(newBook.title, MAX_TITLE, stdin);
    newBook.title[strcspn(newBook.title, "\n")] = 0; // Remove newline

    printf("Enter book author: ");
    fgets(newBook.author, MAX_AUTHOR, stdin);
    newBook.author[strcspn(newBook.author, "\n")] = 0; // Remove newline

    newBook.available = 1;

    // Reallocate memory to add the new book
    Book *temp = realloc(books, (size + 1) * sizeof(Book));
    if (!temp) {
        printf("Failed to allocate memory.\n");
        free(books);
        return;
    }
    books = temp;
    books[size] = newBook;
    size++;

    saveBooks(books, size);

    printf("Book added successfully.\n");

    free(books);
}

void removeBook() {
    Book *books = NULL;
    int size = 0;

    loadBooks(&books, &size);

    if (size == 0) {
        printf("No books available to remove.\n");
        return;
    }

    int id;
    printf("Enter book ID to remove: ");
    scanf("%d", &id);
    clearInputBuffer();

    int foundIndex = -1;
    for (int i = 0; i < size; i++) {
        if (books[i].id == id) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("Book with ID %d not found.\n", id);
        free(books);
        return;
    }

    for (int i = foundIndex; i < size - 1; i++) {
        books[i] = books[i + 1];
    }

    size--;
    if (size == 0) {
        free(books);
        remove(DATA_FILE);
        printf("Book removed successfully. No more books left.\n");
        return;
    }

    Book *temp = realloc(books, size * sizeof(Book));
    if (temp) {
        books = temp;
    }

    saveBooks(books, size);

    printf("Book removed successfully.\n");

    free(books);
}

void searchBook() {
    Book *books = NULL;
    int size = 0;

    loadBooks(&books, &size);

    if (size == 0) {
        printf("No books available in the library.\n");
        return;
    }

    int option;
    printf("Search by:\n1. Title\n2. Author\nEnter choice: ");
    scanf("%d", &option);
    clearInputBuffer();

    char query[MAX_TITLE];
    if (option == 1) {
        printf("Enter title to search: ");
    } else if (option == 2) {
        printf("Enter author to search: ");
    } else {
        printf("Invalid option.\n");
        free(books);
        return;
    }
    fgets(query, MAX_TITLE, stdin);
    query[strcspn(query, "\n")] = 0;

    int found = 0;
    for (int i = 0; i < size; i++) {
        if ((option == 1 && strcasestr(books[i].title, query)) ||
            (option == 2 && strcasestr(books[i].author, query))) {
            printf("\nID: %d\nTitle: %s\nAuthor: %s\nStatus: %s\n",
                   books[i].id, books[i].title, books[i].author,
                   books[i].available ? "Available" : "Borrowed");
            found = 1;
        }
    }
    if (!found) {
        printf("No matching books found.\n");
    }

    free(books);
}

// Case insensitive substring search helper for older C (since strcasestr is POSIX, define it here)
char *strcasestr(const char *haystack, const char *needle) {
    if (!*needle)
        return (char *)haystack;
    for (; *haystack; haystack++) {
        if (tolower((unsigned char)*haystack) == tolower((unsigned char)*needle)) {
            const char *h, *n;
            for (h = haystack, n = needle; *h && *n; h++, n++) {
                if (tolower((unsigned char)*h) != tolower((unsigned char)*n))
                    break;
            }
            if (!*n)
                return (char *)haystack;
        }
    }
    return NULL;
}

void displayBooks() {
    Book *books = NULL;
    int size = 0;

    loadBooks(&books, &size);

    if (size == 0) {
        printf("No books available in the library.\n");
        return;
    }

    printf("\nAll Books in Library:\n");
    printf("-------------------------------------------------\n");
    printf("%-5s %-30s %-20s %-10s\n", "ID", "Title", "Author", "Status");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < size; i++) {
        printf("%-5d %-30s %-20s %-10s\n",
               books[i].id,
               books[i].title,
               books[i].author,
               books[i].available ? "Available" : "Borrowed");
    }
    printf("-------------------------------------------------\n");

    free(books);
}

void borrowBook() {
    Book *books = NULL;
    int size = 0;

    loadBooks(&books, &size);

    if (size == 0) {
        printf("No books available in the library.\n");
        return;
    }

    int id;
    printf("Enter book ID to borrow: ");
    scanf("%d", &id);
    clearInputBuffer();

    int found = 0;
    for (int i = 0; i < size; i++) {
        if (books[i].id == id) {
            found = 1;
            if (books[i].available) {
                books[i].available = 0;
                saveBooks(books, size);
                printf("You have successfully borrowed '%s'.\n", books[i].title);
            } else {
                printf("Sorry, the book '%s' is already borrowed.\n", books[i].title);
            }
            break;
        }
    }

    if (!found) {
        printf("Book with ID %d not found.\n", id);
    }

    free(books);
}

void returnBook() {
    Book *books = NULL;
    int size = 0;

    loadBooks(&books, &size);

    if (size == 0) {
        printf("No books available in the library.\n");
        return;
    }

    int id;
    printf("Enter book ID to return: ");
    scanf("%d", &id);
    clearInputBuffer();

    int found = 0;
    for (int i = 0; i < size; i++) {
        if (books[i].id == id) {
            found = 1;
            if (!books[i].available) {
                books[i].available = 1;
                saveBooks(books, size);
                printf("You have successfully returned '%s'.\n", books[i].title);
            } else {
                printf("The book '%s' was not borrowed.\n", books[i].title);
            }
            break;
        }
    }

    if (!found) {
        printf("Book with ID %d not found.\n", id);
    }

    free(books);
}
