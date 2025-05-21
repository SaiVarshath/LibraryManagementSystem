import java.io.*;
import java.util.*;

public class LibraryManagementSystem {
    static final String DATA_FILE = "library.dat";

    static class Book implements Serializable {
        int id;
        String title;
        String author;
        boolean available;

        public Book(int id, String title, String author) {
            this.id = id;
            this.title = title;
            this.author = author;
            this.available = true;
        }

        @Override
        public String toString() {
            return String.format("ID: %d\nTitle: %s\nAuthor: %s\nStatus: %s\n",
                    id, title, author, available ? "Available" : "Borrowed");
        }
    }

    private List<Book> books;

    public LibraryManagementSystem() {
        books = loadBooks();
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        LibraryManagementSystem lib = new LibraryManagementSystem();

        int choice;
        do {
            System.out.println("\n=== Library Management System ===");
            System.out.println("1. Add Book");
            System.out.println("2. Remove Book");
            System.out.println("3. Search Book");
            System.out.println("4. Display All Books");
            System.out.println("5. Borrow Book");
            System.out.println("6. Return Book");
            System.out.println("7. Exit");
            System.out.print("Enter your choice: ");

            while (!scanner.hasNextInt()) {
                System.out.print("Enter a valid number: ");
                scanner.next();
            }
            choice = scanner.nextInt();
            scanner.nextLine(); // Clear newline

            switch (choice) {
                case 1 -> lib.addBook(scanner);
                case 2 -> lib.removeBook(scanner);
                case 3 -> lib.searchBook(scanner);
                case 4 -> lib.displayBooks();
                case 5 -> lib.borrowBook(scanner);
                case 6 -> lib.returnBook(scanner);
                case 7 -> System.out.println("Exiting... Goodbye!");
                default -> System.out.println("Invalid choice.");
            }
        } while (choice != 7);
        scanner.close();
    }

    void addBook(Scanner scanner) {
        System.out.print("Enter book ID: ");
        int id = scanner.nextInt();
        scanner.nextLine();

        for (Book book : books) {
            if (book.id == id) {
                System.out.println("A book with this ID already exists.");
                return;
            }
        }

        System.out.print("Enter book title: ");
        String title = scanner.nextLine();
        System.out.print("Enter author name: ");
        String author = scanner.nextLine();

        books.add(new Book(id, title, author));
        saveBooks();
        System.out.println("Book added successfully.");
    }

    void removeBook(Scanner scanner) {
        System.out.print("Enter book ID to remove: ");
        int id = scanner.nextInt();
        scanner.nextLine();

        boolean removed = books.removeIf(book -> book.id == id);

        if (removed) {
            saveBooks();
            System.out.println("Book removed successfully.");
        } else {
            System.out.println("Book not found.");
        }
    }

    void displayBooks() {
        if (books.isEmpty()) {
            System.out.println("No books available.");
            return;
        }
        System.out.println("\n--- Library Books ---");
        for (Book book : books) {
            System.out.println(book);
        }
    }

    void searchBook(Scanner scanner) {
        System.out.print("Search by:\n1. Title\n2. Author\nChoice: ");
        int option = scanner.nextInt();
        scanner.nextLine();

        System.out.print("Enter search text: ");
        String query = scanner.nextLine().toLowerCase();

        boolean found = false;
        for (Book book : books) {
            if ((option == 1 && book.title.toLowerCase().contains(query)) ||
                (option == 2 && book.author.toLowerCase().contains(query))) {
                System.out.println(book);
                found = true;
            }
        }

        if (!found) {
            System.out.println("No matching books found.");
        }
    }

    void borrowBook(Scanner scanner) {
        System.out.print("Enter book ID to borrow: ");
        int id = scanner.nextInt();
        scanner.nextLine();

        for (Book book : books) {
            if (book.id == id) {
                if (book.available) {
                    book.available = false;
                    saveBooks();
                    System.out.println("You borrowed: " + book.title);
                } else {
                    System.out.println("Book already borrowed.");
                }
                return;
            }
        }
        System.out.println("Book not found.");
    }

    void returnBook(Scanner scanner) {
        System.out.print("Enter book ID to return: ");
        int id = scanner.nextInt();
        scanner.nextLine();

        for (Book book : books) {
            if (book.id == id) {
                if (!book.available) {
                    book.available = true;
                    saveBooks();
                    System.out.println("Book returned: " + book.title);
                } else {
                    System.out.println("Book was not borrowed.");
                }
                return;
            }
        }
        System.out.println("Book not found.");
    }

    List<Book> loadBooks() {
        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(DATA_FILE))) {
            return (List<Book>) ois.readObject();
        } catch (IOException | ClassNotFoundException e) {
            return new ArrayList<>();
        }
    }

    void saveBooks() {
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(DATA_FILE))) {
            oos.writeObject(books);
        } catch (IOException e) {
            System.out.println("Error saving books.");
        }
    }
}
